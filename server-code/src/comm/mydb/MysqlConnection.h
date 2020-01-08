#pragma once

#include <memory>
#include <unordered_map>
#include "mysql/mysql.h"
#include "Thread.h"

#include "DBRecord.h"

class CMysqlConnection;
class CMysqlStmt;
class CMysqlResult;

using MYSQL_PTR = std::unique_ptr<MYSQL, decltype(&mysql_close)>;
using MYSQL_STMT_PTR = std::unique_ptr<MYSQL_STMT, decltype(&mysql_stmt_close)>;
using MYSQL_RES_PTR = std::unique_ptr<MYSQL_RES, decltype(&mysql_free_result)>;

class CMysqlResult
{
public:
	CMysqlResult(CMysqlConnection* pMysqlConnection, MYSQL_RES* res, CDBFieldInfoListPtr infolist_ptr);
	~CMysqlResult();

	CDBFieldInfoListPtr GetFieldInfo() const;
	size_t get_num_row();

	CDBRecordPtr fetch_row(bool bModify = true);
private:
	CMysqlConnection* m_pMysqlConnection;
	MYSQL_RES_PTR m_MySqlResult;
	CDBFieldInfoListPtr m_pFieldInfoList;
};
using CMysqlResultPtr = std::unique_ptr<CMysqlResult>;

//not thread safe
class CMysqlConnection
{
public:
	CMysqlConnection();

	~CMysqlConnection();

	bool Connect(const std::string& host, const std::string& user, const std::string& password,
			const std::string& db, unsigned int port, unsigned long client_flag = 0, bool bCreateAsync = false);
	CMysqlStmt*   Prepare(const std::string& s);
	CMysqlResultPtr UnionQuery(const std::string& query);
	CMysqlResultPtr Query(const std::string& table_name, const std::string& query = "");
	uint64_t Update(const std::string& query);
	uint64_t Insert(const std::string& query);
	bool SyncExec(const std::string& s);
	void AsyncExec(const std::string& s);
	void JoinAsyncThreadFinish();
	CDBRecordPtr MakeRecord(const std::string& table_name);


	CDBFieldInfoListPtr QueryFieldInfo(const std::string& s, MYSQL_RES* res);
	void _AddFieldInfo(const std::string& s, CDBFieldInfoListPtr ptr);
	MYSQL* _GetHandle() const { return m_pHandle.get(); }
	bool EscapeString(char* pszDst, const char* pszSrc, int nLen);

	template<class T>
	bool CheckTable()
	{
	__ENTER_FUNCTION
		auto ptr = MakeRecord(T::table_name);
		auto pFieldInfo_MYSQL= QueryFieldInfo(T::table_name, nullptr);
		if(pFieldInfo_MYSQL == nullptr)
		{
			LOGFATAL("GameDB Check Error, table:{} not find in MYSQL", T::table_name);
			return false;
		}
		auto pFieldInfo_DDL = new CDDLFieldInfoList<T>();
		for(size_t i = 0; i < pFieldInfo_DDL->size_fields; i++)
		{
			const CDBFieldInfo* pInfo_DDL = pFieldInfo_DDL->get(i);
			const CDBFieldInfo* pInfo_MYSQL = pFieldInfo_MYSQL->get(i);
			if(pFieldInfo_MYSQL == nullptr || pInfo_DDL == nullptr || pInfo_MYSQL == nullptr ||
				pInfo_DDL->GetFieldType() != pInfo_MYSQL->GetFieldType())
			{
				LOGFATAL("GameDB Check Error, table:{}, field:{} fieldname:{}", T::table_name, i, pInfo_DDL->GetFieldName());
				return false;
			}
		}
		return true;
	__LEAVE_FUNCTION
		return false;
	}
private:
	uint64_t GetInsertID();
	uint64_t GetAffectedRows();
	bool MoreResults();
	bool NextResult();
	CMysqlResultPtr UseResult(const std::string& s);
private:
	MYSQL_PTR m_pHandle;
	MYSQL_PTR m_pAsyncHandle;
	std::unordered_map<std::string, CDBFieldInfoListPtr> m_MysqlFieldInfoCache{};
	std::unique_ptr<CWorkerThread> m_AsyncThread{};
	MPSCQueue<std::string*> m_MessageQueue;

	std::mutex m_Mutex;
};

class CMysqlStmt
{
public:
	CMysqlStmt() 
		: m_pMysqlStmt(nullptr, mysql_stmt_close), m_ParamsCount(0)
	{}

	CMysqlStmt(MYSQL_STMT* stmt)
		: m_pMysqlStmt(stmt, mysql_stmt_close)
		, m_ParamsCount(mysql_stmt_param_count(stmt))
		, m_Params(new MYSQL_BIND[m_ParamsCount]())
	{
	}

	CMysqlStmt(CMysqlStmt&& rht)
		: m_pMysqlStmt(rht.m_pMysqlStmt.release(), mysql_stmt_close)
		, m_ParamsCount(rht.m_ParamsCount)
		, m_Params(rht.m_Params.release())
	{
	}

	~CMysqlStmt()
	{
	}



public:
	operator bool() 
	{
		return !!m_pMysqlStmt;
	}


	void _BindParam(int i, enum_field_types buffer_type, void* buffer, int buffer_length, my_bool* is_null, long unsigned int* length)
	{
		MYSQL_BIND& b = m_Params[i];
		b.buffer_type = buffer_type;
		b.buffer = (char*)buffer;
		b.buffer_length = buffer_length;
		b.is_null = is_null;
		b.length = length;
	}

	void BindParam(int i, const int& x)
	{
		_BindParam(i, MYSQL_TYPE_LONG, (char*)&x, 0, 0, 0);
	}

	void BindParam(int i, const long long& x)
	{
		_BindParam(i, MYSQL_TYPE_LONGLONG, (char*)&x, 0, 0, 0);
	}

	void BindParam(int i, const std::string& x)
	{
		_BindParam(i, MYSQL_TYPE_STRING, (char*)x.c_str(), x.size(), 0, &(m_Params[i].buffer_length));
	}



	template <class... Args>
	void Execute(Args&& ... args)
	{
		_Execute(0, std::forward<Args>(args)...);
		mysql_stmt_bind_param(m_pMysqlStmt.get(), m_Params.get());
//		mysql_stmt_bind_result(m_pMysqlStmt.get(), m_Result.get());
		mysql_stmt_execute(m_pMysqlStmt.get());
	}

	/*void store_result()
	{
		mysql_stmt_store_result(m_pMysqlStmt.get());
		mysql_stmt_fetch(m_pMysqlStmt.get());

	}
	void free_result()
	{
		mysql_stmt_free_result(m_pMysqlStmt.get());
	}*/

	template <class T, class... Args>
	void _Execute(int n, const T& x, Args&& ... args)
	{
		BindParam(n, x);
		_Execute(n + 1, std::forward<Args>(args)...);
	}

	// base case
	void _Execute(int n) {}

private:
	MYSQL_STMT_PTR m_pMysqlStmt;
	size_t m_ParamsCount;
	std::unique_ptr<MYSQL_BIND []> m_Params;
};







