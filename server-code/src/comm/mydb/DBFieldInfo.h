#ifndef DBFIELDINFO_H
#define DBFIELDINFO_H

#include <memory>
#include <vector>

#include "BaseCode.h"
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

    virtual const char*    GetTableName() const = 0;
    virtual const char*    GetFieldName() const = 0;
    virtual DB_FIELD_TYPES GetFieldType() const = 0;
    virtual uint32_t       GetFieldIdx() const  = 0;
    virtual bool           IsPriKey() const     = 0;
};

class CDBFieldInfoList
{
public:
    CDBFieldInfoList() {}
    virtual ~CDBFieldInfoList() {}
    virtual const CDBFieldInfo* operator[](size_t idx) const = 0;
    virtual const CDBFieldInfo* get(size_t idx) const        = 0;
    virtual size_t              size() const                 = 0;
};
using CDBFieldInfoListPtr = std::shared_ptr<CDBFieldInfoList>;

struct MYSQL_FIELD_COPY : public CDBFieldInfo
{
    std::string         m_name;       /* Name of column */
    std::string         m_table;      /* Table of column if column was a field */
    enum DB_FIELD_TYPES m_field_type; /* Type of field. See mysql_com.h for types */
    bool                m_isPriKey;
    uint32_t            m_idx;
    MYSQL_FIELD_COPY(MYSQL_FIELD* pField, uint32_t idx)
    {
        m_name     = (pField->name ? pField->name : "");
        m_table    = (pField->table ? pField->table : "");
        m_isPriKey = IS_PRI_KEY(pField->flags);
        m_idx      = idx;
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

    virtual const char*    GetTableName() const override { return m_table.c_str(); }
    virtual const char*    GetFieldName() const override { return m_name.c_str(); }
    virtual DB_FIELD_TYPES GetFieldType() const override { return m_field_type; }
    virtual bool           IsPriKey() const override { return m_isPriKey; }
    virtual uint32_t       GetFieldIdx() const override { return m_idx; }
};

class CMysqlFieldInfoList : public CDBFieldInfoList
{
public:
    CMysqlFieldInfoList(MYSQL_RES* res);
    ~CMysqlFieldInfoList();

    virtual const CDBFieldInfo* operator[](size_t idx) const
    {
        CHECKF(idx < size());
        return m_FieldInfos[idx].get();
    }
    virtual const CDBFieldInfo* get(size_t idx) const
    {
        CHECKF(idx < size());
        return m_FieldInfos[idx].get();
    }
    virtual size_t size() const { return m_FieldInfos.size(); }

protected:
    std::vector<std::unique_ptr<CDBFieldInfo> > m_FieldInfos;
};

template<typename T, size_t FIELD_IDX>
struct DBFieldHelp
{
    enum 
    {
        IDX_FIELD_NAME = 0,
        IDX_FIELD_TYPE = 1,
        IDX_FIELD_PRIKEY = 2,
    };
    static constexpr const char*    GetTableName() { return T::table_name(); }
    static constexpr const char*    GetFieldName() { return std::get<IDX_FIELD_NAME>( std::get<FIELD_IDX>(T::field_info()) ); }
    static constexpr DB_FIELD_TYPES GetFieldType() { return std::get<IDX_FIELD_TYPE>( std::get<FIELD_IDX>(T::field_info()) ); }
    static constexpr bool           IsPriKey()  { return std::get<IDX_FIELD_PRIKEY>( std::get<FIELD_IDX>(T::field_info()) ); }
    static constexpr uint32_t       GetFieldIdx()  { return FIELD_IDX; }
};

template<typename T, size_t FIELD_IDX>
struct CDDLFieldInfo : public CDBFieldInfo
{
    virtual const char*    GetTableName() const override { return DBFieldHelp<T,FIELD_IDX>::GetTableName(); }
    virtual const char*    GetFieldName() const override { return DBFieldHelp<T,FIELD_IDX>::GetFieldName(); }
    virtual DB_FIELD_TYPES GetFieldType() const override { return DBFieldHelp<T,FIELD_IDX>::GetFieldType(); }
    virtual bool           IsPriKey() const override { return DBFieldHelp<T,FIELD_IDX>::IsPriKey(); }
    virtual uint32_t       GetFieldIdx() const override { return FIELD_IDX; }
};

template<typename T>
class CDDLFieldInfoList : public CDBFieldInfoList
{
public:
    static constexpr const size_t size_fields = T::field_count();
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
    virtual size_t              size() const { return size_fields; }

protected:
    CDBFieldInfo* m_FieldInfos[size_fields];
};





#endif /* DBFIELDINFO_H */
