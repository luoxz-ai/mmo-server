#ifndef DBFIELD_H
#define DBFIELD_H

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "BaseCode.h"
#include "CallStackDumper.h"
#include "MemoryHeap.h"
#include "fmt/format.h"
#include "mysql/mysql.h"

enum DB_FIELD_TYPES
{
	DB_FIELD_TYPE_UNSUPPORED,
	DB_FIELD_TYPE_TINY,
	DB_FIELD_TYPE_TINY_UNSIGNED,
	DB_FIELD_TYPE_SHORT,
	DB_FIELD_TYPE_SHORT_UNSIGNED,
	DB_FIELD_TYPE_LONG,
	DB_FIELD_TYPE_LONG_UNSIGNED,
	DB_FIELD_TYPE_LONGLONG,
	DB_FIELD_TYPE_LONGLONG_UNSIGNED,
	DB_FIELD_TYPE_FLOAT,
	DB_FIELD_TYPE_DOUBLE,
	DB_FIELD_TYPE_VARCHAR,
	DB_FIELD_TYPE_BLOB,
};

struct CDBFieldInfo
{
	CDBFieldInfo() {}
	virtual ~CDBFieldInfo() {}

	virtual const char*	   GetTableName() const = 0;
	virtual const char*	   GetFieldName() const = 0;
	virtual DB_FIELD_TYPES GetFieldType() const = 0;
	virtual uint32_t	   GetFieldIdx() const	= 0;
	virtual bool		   IsPriKey() const		= 0;
};

class CDBFieldInfoList
{
public:
	CDBFieldInfoList() {}
	virtual ~CDBFieldInfoList() {}
	virtual const CDBFieldInfo* operator[](size_t idx) const = 0;
	virtual const CDBFieldInfo* get(size_t idx) const		 = 0;
	virtual size_t				size() const				 = 0;
};
using CDBFieldInfoListPtr = std::shared_ptr<CDBFieldInfoList>;

struct MYSQL_FIELD_COPY : public CDBFieldInfo
{
	std::string			m_name;		  /* Name of column */
	std::string			m_table;	  /* Table of column if column was a field */
	enum DB_FIELD_TYPES m_field_type; /* Type of field. See mysql_com.h for types */
	bool				m_isPriKey;
	uint32_t			m_idx;
	MYSQL_FIELD_COPY(MYSQL_FIELD* pField, uint32_t idx)
	{
		m_name	   = (pField->name ? pField->name : "");
		m_table	   = (pField->table ? pField->table : "");
		m_isPriKey = IS_PRI_KEY(pField->flags);
		m_idx	   = idx;
		switch(pField->type)
		{
			case MYSQL_TYPE_TINY:
			{
				if(pField->flags & UNSIGNED_FLAG)
				{
					m_field_type = DB_FIELD_TYPE_TINY_UNSIGNED;
				}
				else
				{
					m_field_type = DB_FIELD_TYPE_TINY;
				}
			}
			break;
			case MYSQL_TYPE_SHORT:
			{
				if(pField->flags & UNSIGNED_FLAG)
				{
					m_field_type = DB_FIELD_TYPE_SHORT_UNSIGNED;
				}
				else
				{
					m_field_type = DB_FIELD_TYPE_SHORT;
				}
			}
			break;
			case MYSQL_TYPE_LONG:
			{
				if(pField->flags & UNSIGNED_FLAG)
				{
					m_field_type = DB_FIELD_TYPE_LONG_UNSIGNED;
				}
				else
				{
					m_field_type = DB_FIELD_TYPE_LONG;
				}
			}
			break;
			case MYSQL_TYPE_LONGLONG:
			{
				if(pField->flags & UNSIGNED_FLAG)
				{
					m_field_type = DB_FIELD_TYPE_LONGLONG_UNSIGNED;
				}
				else
				{
					m_field_type = DB_FIELD_TYPE_LONGLONG;
				}
			}
			break;
			case MYSQL_TYPE_FLOAT:
			{
				m_field_type = DB_FIELD_TYPE_FLOAT;
			}
			break;
			case MYSQL_TYPE_DOUBLE:
			{
				m_field_type = DB_FIELD_TYPE_DOUBLE;
			}
			break;
			case MYSQL_TYPE_VAR_STRING:
			case MYSQL_TYPE_STRING:
			{
				m_field_type = DB_FIELD_TYPE_VARCHAR;
			}
			break;
			case MYSQL_TYPE_BLOB:
			{
				m_field_type = DB_FIELD_TYPE_BLOB;
			}
			break;
			default:
			{
			}
			break;
		}
	}

	virtual const char*	   GetTableName() const override { return m_table.c_str(); }
	virtual const char*	   GetFieldName() const override { return m_name.c_str(); }
	virtual DB_FIELD_TYPES GetFieldType() const override { return m_field_type; }
	virtual bool		   IsPriKey() const override { return m_isPriKey; }
	virtual uint32_t	   GetFieldIdx() const override { return m_idx; }
};

class CMysqlFieldInfoList : public CDBFieldInfoList
{
public:
	CMysqlFieldInfoList(MYSQL_RES* res);
	~CMysqlFieldInfoList();

	virtual const CDBFieldInfo* operator[](size_t idx) const { return m_FieldInfos[idx]; }
	virtual const CDBFieldInfo* get(size_t idx) const { return m_FieldInfos[idx]; }
	virtual size_t				size() const { return m_FieldInfos.size(); }

protected:
	std::vector<CDBFieldInfo*> m_FieldInfos;
};

template<typename T, size_t FIELD_IDX>
struct CDDLFieldInfo : public CDBFieldInfo
{
	virtual const char*	   GetTableName() const override { return T::table_name; }
	virtual const char*	   GetFieldName() const override { return T::field_name[FIELD_IDX]; }
	virtual DB_FIELD_TYPES GetFieldType() const override { return T::field_type_enum_list[FIELD_IDX]; }
	virtual bool		   IsPriKey() const override { return T::pri_key_idx[FIELD_IDX] == true; }
	virtual uint32_t	   GetFieldIdx() const override { return FIELD_IDX; }
};

template<typename T>
class CDDLFieldInfoList : public CDBFieldInfoList
{
public:
	static constexpr const size_t size_fields = sizeOfArray(T::field_name);
	CDDLFieldInfoList()
		: CDDLFieldInfoList(std::make_index_sequence<size_fields>())
	{
	}
	template<size_t... index>
	CDDLFieldInfoList(std::index_sequence<index...>)
		: m_FieldInfos{new CDDLFieldInfo<T, index>()...}
	{
	}

	~CDDLFieldInfoList()
	{
		for(auto& v: m_FieldInfos)
		{
			SAFE_DELETE(v);
		}
	}

	virtual const CDBFieldInfo* operator[](size_t idx) const { return m_FieldInfos[idx]; }
	virtual const CDBFieldInfo* get(size_t idx) const { return m_FieldInfos[idx]; }
	virtual size_t				size() const { return size_fields; }

protected:
	CDBFieldInfo* m_FieldInfos[size_fields];
};

const char* const EMPTY_STR = "";

class CDBRecord;
class CDBField
{
public:
	CDBField();
	CDBField(CDBRecord* pDBRecord, const CDBFieldInfo* pFieldInfo, char* pVal, unsigned long len);

	CDBField& operator=(const CDBField&) = delete;
	CDBField(const CDBField&)			 = delete;

	bool IsString() const;
	void ClearDirty();

public:
	operator const std::string&() const
	{
		if(CheckType<std::string>())
		{
			return std::get<std::string>(m_Val);
		}
		static std::string s_Empty;
		return s_Empty;
	}

	template<class T>
	T get() const
	{
		if constexpr(std::is_same<T, char*>::value)
		{
			if(CheckType<std::string>())
			{
				return const_cast<char*>(std::get<std::string>(m_Val).c_str());
			}
			return const_cast<char*>(EMPTY_STR);
		}
		else if constexpr(std::is_same<T, const char*>::value)
		{
			if(CheckType<std::string>())
			{
				return std::get<std::string>(m_Val).c_str();
			}
			return EMPTY_STR;
		}
		else if constexpr(std::is_same<T, const std::string&>::value)
		{
			if(CheckType<std::string>())
			{
				return std::get<std::string>(m_Val).c_str();
			}
			return EMPTY_STR;
		}
		else
		{
			if(CheckType<T>())
			{
				return std::get<T>(m_Val);
			}
			else
			{
				return T();
			}
		}
	}

	template<class T>
	operator T() const
	{
		return get<T>();
	}

	template<class T>
	CDBField& set(T v)
	{
		if(CanModify() == true)
		{
			if(CheckType<T>())
			{
				if(std::get<T>(m_Val) == v)
					return *this;

				m_Val = v;
				MakeDirty();
			}
		}

		return *this;
	}

	CDBField& set(const char* v) { return set<std::string>(v); }
	CDBField& set(char* v) { return set<std::string>(v); }

	template<class T>
	CDBField& operator=(T&& v)
	{
		return set(std::forward<T>(v));
	}

	template<class T>
	bool operator==(T&& v) const
	{
		return this->operator T() == v;
	}

	uint32_t GetType() const { return (m_pFieldInfo == nullptr) ? 0 : m_pFieldInfo->GetFieldType(); }

	template<class T>
	bool CheckType() const
	{
		__ENTER_FUNCTION
		if(m_pFieldInfo == nullptr)
			return false;

		switch(m_pFieldInfo->GetFieldType())
		{
			case DB_FIELD_TYPE_TINY_UNSIGNED:
			{
				if(std::is_same<typename std::decay<T>::type, uint8_t>::value)
					return true;
			}
			break;
			case DB_FIELD_TYPE_TINY:
			{
				if(std::is_same<typename std::decay<T>::type, int8_t>::value)
					return true;
			}
			break;
			case DB_FIELD_TYPE_SHORT_UNSIGNED:
			{
				if(std::is_same<typename std::decay<T>::type, uint16_t>::value)
					return true;
			}
			break;
			case DB_FIELD_TYPE_SHORT:
			{
				if(std::is_same<typename std::decay<T>::type, int16_t>::value)
					return true;
			}
			break;
			case DB_FIELD_TYPE_LONG_UNSIGNED:
			{
				if(std::is_same<typename std::decay<T>::type, uint32_t>::value)
					return true;
			}
			break;
			case DB_FIELD_TYPE_LONG:
			{
				if(std::is_same<typename std::decay<T>::type, int32_t>::value)
					return true;
			}
			break;
			case DB_FIELD_TYPE_LONGLONG_UNSIGNED:
			{
				if(std::is_same<typename std::decay<T>::type, uint64_t>::value)
					return true;
			}
			break;
			case DB_FIELD_TYPE_LONGLONG:
			{
				if(std::is_same<typename std::decay<T>::type, int64_t>::value)
					return true;
			}
			break;
			case DB_FIELD_TYPE_FLOAT:
			{
				if(std::is_same<typename std::decay<T>::type, float>::value)
					return true;
			}
			break;
			case DB_FIELD_TYPE_DOUBLE:
			{
				if(std::is_same<typename std::decay<T>::type, double>::value)
					return true;
			}
			break;
			case DB_FIELD_TYPE_VARCHAR:
			case DB_FIELD_TYPE_BLOB:
			{
				if(std::is_same<typename std::decay<T>::type, std::string>::value || std::is_same<typename std::decay<T>::type, const char*>::value ||
				   std::is_same<typename std::decay<T>::type, char*>::value)
					return true;
			}
			break;
			default:
			{
			}
			break;
		}

		__LEAVE_FUNCTION
#ifdef DEBUG
		DumpStack(CallFrameMap(1));
#endif

		LOGDBERROR("Field {}.{} get/set type error", m_pFieldInfo->GetTableName(), m_pFieldInfo->GetFieldName());
		return false;
	}

	std::string			GetValString() const;
	void				SetGetValStringFunc(std::function<std::string()> func) { m_funcGetValString = func; }
	bool				CanModify() const;
	bool				IsDirty() const;
	void				MakeDirty();
	const CDBFieldInfo* GetFieldInfo() const { return m_pFieldInfo; }

public:
	MEMORYHEAP_DECLARATION(s_Heap);

private:
	CDBRecord*																										   m_pDBRecord;
	const CDBFieldInfo*																								   m_pFieldInfo;
	std::variant<int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, float, double, std::string> m_Val;
	bool																											   m_bDirty;
	std::function<std::string()>																					   m_funcGetValString;
};

#endif /* DBFIELD_H */
