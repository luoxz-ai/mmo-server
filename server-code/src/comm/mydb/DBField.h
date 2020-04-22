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

#include "DBFieldInfo.h"


const char* const EMPTY_STR = "";

class CDBRecord;
class CDBField
{
public:
    CDBField();
    CDBField(CDBRecord* pDBRecord, const CDBFieldInfo* pFieldInfo, char* pVal, unsigned long len);

    CDBField& operator=(const CDBField&) = delete;
    CDBField(const CDBField&)            = delete;

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

        if(CheckFieldType<T>(m_pFieldInfo->GetFieldType()) == true)
            return true;

        __LEAVE_FUNCTION

        LOGDBERROR("Field {}.{} get/set type error", m_pFieldInfo->GetTableName(), m_pFieldInfo->GetFieldName());
        return false;
    }

    std::string         GetValString() const;
    void                SetGetValStringFunc(std::function<std::string()> func) { m_funcGetValString = func; }
    bool                CanModify() const;
    bool                IsDirty() const;
    void                MakeDirty();
    const CDBFieldInfo* GetFieldInfo() const { return m_pFieldInfo; }
    int32_t             GetFieldIndex() const;
    const char*         GetFieldName() const;


private:
    template<class T>
    static constexpr bool CheckFieldType(uint32_t field_type)
    {
        switch(field_type)
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
                if(std::is_same<typename std::decay<T>::type, std::string>::value ||
                   std::is_same<typename std::decay<T>::type, const char*>::value ||
                   std::is_same<typename std::decay<T>::type, char*>::value)
                    return true;
            }
            break;
            default:
            {
            }
            break;
        }
        return false;
    }
public:
    MEMORYHEAP_DECLARATION(s_Heap);

private:
    CDBRecord*          m_pDBRecord;
    const CDBFieldInfo* m_pFieldInfo;
    std::variant<int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, float, double, std::string>
                                 m_Val;
    bool                         m_bDirty;
    std::function<std::string()> m_funcGetValString;
};

#endif /* DBFIELD_H */
