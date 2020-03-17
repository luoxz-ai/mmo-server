//
// 数据库记录的集。
// 以map容器存储数据
//
//////////////////////////////////////////////////////////////////////

#ifndef T_GAMEDATAMAP_H
#define T_GAMEDATAMAP_H

#include "BaseCode.h"
#include "MysqlConnection.h"
#include "ProtobuffUtil.h"
#include "Singleton.h"
#include "type_traits_ext.h"

//////////////////////////////////////////////////////////////////////
// 说明：需要GetID()函数支持    另外，还需要这种形式的函数支持：
// void Init(CDBRecordPtr pRes);

namespace details
{
CAT_CLASS_HAS_MEMBER(GetIDFromPBRow);
}; // namespace details
template<typename T>
struct hasFunc_GetIDFromPBRow : bool_type<details::has_GetIDFromPBRow<T>::value>
{
};

template<class T>
class CGameDataMap
{
	std::string GET_NAME() { return demangle(typeid(T).name()); }

	CGameDataMap() {}

public:
	using KEY_T		 = typename std::result_of<decltype (&T::GetID)(T)>::type;
	using MAP_SET_T	 = std::unordered_map<KEY_T, T*>;
	using MAP_ITER_T = typename MAP_SET_T::const_iterator;
	CREATE_NEW_IMPL(CGameDataMap<T>);
	~CGameDataMap() { Clear(); }

public:
	void Clear()
	{
		for(auto& pair_val: m_setData)
		{
			SAFE_DELETE(pair_val.second);
		}
		m_setData.clear();
		LOGDEBUG("Clear {} Succ.", GET_NAME());
	}

	bool Init(CMysqlConnection* pDb, const char* table_name, const char* szSQL)
	{
		auto result_ptr = pDb->Query(table_name, szSQL);
		if(result_ptr)
		{
			for(size_t i = 0; i < result_ptr->get_num_row(); i++)
			{
				auto db_record_ptr = result_ptr->fetch_row(false);
				T*	 pData		   = T::CreateNew(std::move(db_record_ptr));
				if(pData == nullptr)
				{
					return false;
				}

				this->AddObj(pData);
			}
		}
		LOGDEBUG("Init {} Succ.", GET_NAME());
		return true;
	}
	bool Reload(CMysqlConnection* pDb, const char* table_name, const char* szSQL)
	{
		Clear();
		return Init(pDb, table_name, szSQL);
	}

	template<class PBRow_T>
	auto _LoadFromPB(const PBRow_T& row)
	{
		if constexpr(hasFunc_GetIDFromPBRow<T>::value)
		{
			KEY_T key	= T::GetIDFromPBRow(row);
			T*	  pData = QueryObj(key);
			if(pData)
			{
				pData->Init(row);
			}
			else
			{
				T* pData = T::CreateNew(row);
				if(pData == nullptr)
				{
					return false;
				}

				AddObj(pData);
			}
			return true;
		}
		else
		{
			T* pData = T::CreateNew(row);
			if(pData == nullptr)
			{
				return false;
			}

			AddObj(pData);
			return true;
		}
	}

	auto Init(const char* szFileName)
	{
		using PB_T = typename T::PB_T;
		PB_T cfg;
		if(pb_util::LoadFromBinaryFile(szFileName, cfg) == false)
		{
			LOGERROR("InitFromFile {} Fail.", szFileName);
			return false;
		}
		for(const auto& iter: cfg.rows())
		{
			_LoadFromPB(iter);
		}
		LOGDEBUG("Init {} Succ.", szFileName);
		return true;
	}
	bool Reload(const char* szFileName, bool bClear)
	{
		if(bClear)
			Clear();
		return Init(szFileName);
	}

	void AddObj(T* pData)
	{
		auto it_find = m_setData.find(pData->GetID());
		if(it_find != m_setData.end())
		{
			LOGWARNING("AddObj twice {}, id:{}", GET_NAME(), pData->GetID());
		}
		else
		{
			m_setData[pData->GetID()] = pData;
		}
	}

	T* QueryObj(KEY_T id)
	{
		auto it_find = m_setData.find(id);
		if(it_find != m_setData.end())
		{
			return it_find->second;
		}
		return nullptr;
	}

	class Iterator
	{
	private: // create by parent class
		Iterator(const MAP_SET_T& refSet)
			: m_refSet(refSet)
			, m_iter(m_refSet.begin())
		{
		}

	public: // application
		Iterator()				  = delete;
		Iterator(const Iterator&) = default;
		void MoveNext() { m_iter++; }

		bool HasMore() const { return m_iter != m_refSet.end(); }

		T* PeekVal() const { return m_iter->second; }

		friend class CGameDataMap<T>;

	protected:
		const MAP_SET_T& m_refSet;
		MAP_ITER_T		 m_iter;
	};
	Iterator GetIter() const { return Iterator(m_setData); }

private:
	MAP_SET_T m_setData;
};

template<class T>
class CGameMultiDataMap
{
	std::string GET_NAME() { return demangle(typeid(T).name()); }
	CGameMultiDataMap() {}

public:
	using KEY_T		 = typename std::result_of<decltype (&T::GetID)(T)>::type;
	using MAP_SET_T	 = std::unordered_multimap<KEY_T, T*>;
	using MAP_ITER_T = typename MAP_SET_T::const_iterator;

	class Iterator
	{
	private: // create by parent class
		Iterator(const MAP_SET_T& refSet)
			: m_itBeg(refSet.begin())
			, m_itEnd(refSet.end()())
		{
		}
		Iterator(MAP_ITER_T it_beg, MAP_ITER_T it_end)
			: m_itBeg(it_beg)
			, m_itEnd(it_end)
		{
		}

	public: // application
		Iterator()				  = delete;
		Iterator(const Iterator&) = default;
		void MoveNext() { m_itBeg++; }

		bool HasMore() const { return m_itBeg != m_itEnd; }

		T* PeekVal() const { return m_itBeg->second; }

		friend class CGameMultiDataMap<T>;

	protected:
		MAP_ITER_T m_itBeg;
		MAP_ITER_T m_itEnd;
	};

public:
	CREATE_NEW_IMPL(CGameMultiDataMap<T>);
	~CGameMultiDataMap() { Clear(); }

public:
	void Clear()
	{
		for(auto& pair_val: m_setData)
		{
			SAFE_DELETE(pair_val.second);
		}
		m_setData.clear();
		LOGDEBUG("Clear {} Succ.", GET_NAME());
	}

	bool Init(CMysqlConnection* pDb, const char* table_name, const char* szSQL)
	{
		auto result_ptr = pDb->Query(table_name, szSQL);
		if(result_ptr)
		{
			for(size_t i = 0; i < result_ptr->get_num_row(); i++)
			{
				auto db_record_ptr = result_ptr->fetch_row(false);
				T*	 pData		   = T::CreateNew(std::move(db_record_ptr));
				if(pData == nullptr)
				{
					return false;
				}

				this->AddObj(pData);
			}
			LOGDEBUG("Init {} Succ.", GET_NAME());
		}
		return true;
	}
	bool Reload(CMysqlConnection* pDb, const char* table_name, const char* szSQL)
	{
		Clear();
		return Init(pDb, table_name, szSQL);
	}

	bool Init(const char* szFileName)
	{
		using PB_T = typename T::PB_T;
		PB_T cfg;
		if(pb_util::LoadFromBinaryFile(szFileName, cfg) == false)
		{
			LOGERROR("InitFromFile {} Fail.", szFileName);
			return false;
		}

		for(const auto& iter: cfg.rows())
		{
			T* pData = T::CreateNew(iter);
			if(pData == nullptr)
			{
				return false;
			}

			this->AddObj(pData);
		}
		LOGDEBUG("Init {} Succ.", szFileName);
		return true;
	}
	bool Reload(const char* szFileName)
	{
		Clear();
		return Init(szFileName);
	}

	void AddObj(T* pData) { m_setData.insert(std::make_pair(pData->GetID(), pData)); }

	Iterator QueryObj(KEY_T id)
	{
		auto it_find = m_setData.equal_range(id);
		if(it_find.first != it_find.second && it_find.first != m_setData.end())
		{
			return Iterator(it_find.first, it_find.second);
		}
		return Iterator(m_setData.end(), m_setData.end());
	}

	Iterator GetIter() const { return Iterator(m_setData); }

private:
	MAP_SET_T m_setData;
};

#endif // T_GAMEDATAMAP_H
