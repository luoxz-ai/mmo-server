#include "DBField.h"

#include "DBRecord.h"
#include "MysqlConnection.h"

CDBField::CDBField(CDBRecord* pDBRecord, const CDBFieldInfo* pFieldInfo, char* pVal, unsigned long len)
    : m_pDBRecord(pDBRecord)
    , m_pFieldInfo(pFieldInfo)
{
    __ENTER_FUNCTION
    switch(m_pFieldInfo->GetFieldType())
    {

        case DB_FIELD_TYPE_TINY_UNSIGNED:
        {
            if(pVal == nullptr || len == 0)
                m_Val = uint8_t(0);
            else
                m_Val = uint8_t(std::stoul(pVal));
        }
        break;
        case DB_FIELD_TYPE_TINY:
        {
            if(pVal == nullptr || len == 0)
                m_Val = int8_t(0);
            else
                m_Val = int8_t(std::stol(pVal));
        }
        break;
        case DB_FIELD_TYPE_SHORT_UNSIGNED:
        {
            if(pVal == nullptr || len == 0)
                m_Val = uint16_t(0);
            else
                m_Val = uint16_t(std::stoul(pVal));
        }
        break;
        case DB_FIELD_TYPE_SHORT:
        {
            if(pVal == nullptr || len == 0)
                m_Val = int16_t(0);
            else
                m_Val = int16_t(std::stol(pVal));
        }
        break;
        case DB_FIELD_TYPE_LONG_UNSIGNED:
        {
            if(pVal == nullptr || len == 0)
                m_Val = uint32_t(0);
            else
                m_Val = uint32_t(std::stoul(pVal));
        }
        break;
        case DB_FIELD_TYPE_LONG:
        {
            if(pVal == nullptr || len == 0)
                m_Val = int32_t(0);
            else
                m_Val = int32_t(std::stol(pVal));
        }
        break;
        case DB_FIELD_TYPE_LONGLONG_UNSIGNED:
        {
            if(pVal == nullptr || len == 0)
                m_Val = uint64_t(0);
            else
                m_Val = uint64_t(std::stoull(pVal));
        }
        break;
        case DB_FIELD_TYPE_LONGLONG:
        {
            if(pVal == nullptr || len == 0)
                m_Val = int64_t(0);
            else
                m_Val = int64_t(std::stoll(pVal));
        }
        break;
        case DB_FIELD_TYPE_FLOAT:
        {
            if(pVal == nullptr || len == 0)
                m_Val = float(0.0f);
            else
                m_Val = float(std::stof(pVal));
        }
        break;
        case DB_FIELD_TYPE_DOUBLE:
        {
            if(pVal == nullptr || len == 0)
                m_Val = double(0.0);
            else
                m_Val = double(std::stod(pVal));
        }
        break;
        case DB_FIELD_TYPE_VARCHAR:
        case DB_FIELD_TYPE_BLOB:
        {
            if(pVal == nullptr || len == 0)
                m_Val = std::string("");
            else
                m_Val = std::string(pVal, len);
        }
        break;
        default:
        {
#ifdef DEBUG
            DumpStack(CallFrameMap(1));
#endif
            LOGDBERROR("mysql field:{}.{} use unknow type", m_pFieldInfo->GetTableName(), m_pFieldInfo->GetFieldName());
        }
        break;
    }

    __LEAVE_FUNCTION
}

CDBField::CDBField()
    : m_pDBRecord(nullptr)
    , m_pFieldInfo(nullptr)
{
}

bool CDBField::IsString() const
{
    return (m_pFieldInfo->GetFieldType() == DB_FIELD_TYPE_VARCHAR);
}

std::string CDBField::GetValString() const
{
    __ENTER_FUNCTION
    if(m_pFieldInfo == nullptr)
        return std::string();
    if(m_funcGetValString)
    {
        return "'" + m_funcGetValString() + "'";
    }

    switch(m_pFieldInfo->GetFieldType())
    {
        case DB_FIELD_TYPE_TINY_UNSIGNED:
        {
            return std::to_string(std::get<uint8_t>(m_Val));
        }
        break;
        case DB_FIELD_TYPE_TINY:
        {
            return std::to_string(std::get<int8_t>(m_Val));
        }
        break;
        case DB_FIELD_TYPE_SHORT_UNSIGNED:
        {
            return std::to_string(std::get<uint16_t>(m_Val));
        }
        break;
        case DB_FIELD_TYPE_SHORT:
        {
            return std::to_string(std::get<int16_t>(m_Val));
        }
        break;
        case DB_FIELD_TYPE_LONG_UNSIGNED:
        {
            return std::to_string(std::get<uint32_t>(m_Val));
        }
        break;
        case DB_FIELD_TYPE_LONG:
        {
            return std::to_string(std::get<int32_t>(m_Val));
        }
        break;
        case DB_FIELD_TYPE_LONGLONG_UNSIGNED:
        {
            return std::to_string(std::get<uint64_t>(m_Val));
        }
        break;
        case DB_FIELD_TYPE_LONGLONG:
        {
            return std::to_string(std::get<int64_t>(m_Val));
        }
        break;
        case DB_FIELD_TYPE_FLOAT:
        {
            return std::to_string(std::get<float>(m_Val));
        }
        break;
        case DB_FIELD_TYPE_DOUBLE:
        {
            return std::to_string(std::get<double>(m_Val));
        }
        break;
        case DB_FIELD_TYPE_VARCHAR:
        case DB_FIELD_TYPE_BLOB:
        {
            std::string             tmp    = std::get<std::string>(m_Val);
            std::unique_ptr<char[]> szBuff = std::make_unique<char[]>(tmp.size() * 2 + 1);
            mysql_real_escape_string(m_pDBRecord->_GetMysqlConnection()->_GetHandle(),
                                     szBuff.get(),
                                     tmp.c_str(),
                                     tmp.size());

            return "'" + std::string(szBuff.get()) + "'";
        }
        break;
        default:
        {
#ifdef DEBUG
            DumpStack(CallFrameMap(1));
#endif
            LOGDBERROR("mysql field:{}.{} use unknow type", m_pFieldInfo->GetTableName(), m_pFieldInfo->GetFieldName());
            return std::string();
        }
        break;
    }
    __LEAVE_FUNCTION
    return std::string();
}

bool CDBField::CanModify() const
{
    if(m_pDBRecord)
        return m_pDBRecord->CanModify();
    return false;
}

void CDBField::MakeDirty()
{
    m_pDBRecord->MakeDirty(m_pFieldInfo->GetFieldIdx());
}

bool CDBField::IsDirty() const
{
    if(m_pDBRecord && m_pFieldInfo)
        return m_pDBRecord->IsDirty(m_pFieldInfo->GetFieldIdx());
    return false;
}

int32_t CDBField::GetFieldIndex() const
{
    return m_pFieldInfo ? m_pFieldInfo->GetFieldIdx() : -1;
}

const char* CDBField::GetFieldName() const
{
    return m_pFieldInfo ? m_pFieldInfo->GetFieldName() : "";
}

CMysqlFieldInfoList::CMysqlFieldInfoList(MYSQL_RES* res)
{
    int32_t nFields = mysql_num_fields(res);
    for(uint32_t i = 0; i < nFields; i++)
    {
        m_FieldInfos.push_back(std::make_unique<MYSQL_FIELD_COPY>(mysql_fetch_field(res), i));
    }
}

CMysqlFieldInfoList::~CMysqlFieldInfoList() {}
