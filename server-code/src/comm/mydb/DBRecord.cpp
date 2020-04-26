#include "DBRecord.h"

#include "MysqlConnection.h"

OBJECTHEAP_IMPLEMENTATION(CDBField, s_Heap);
OBJECTHEAP_IMPLEMENTATION(CDBRecord, s_Heap);

//////////////////////////////////////////////////////////////////////////

CDBRecord::CDBRecord(CMysqlConnection*   pMysqlConnection,
                     CDBFieldInfoListPtr pDBFieldInfo,
                     bool                bCanModify,
                     MYSQL_ROW           row,
                     unsigned long*      lengths)
    : m_pMysqlConnection(pMysqlConnection)
    , m_pDBFieldInfo(pDBFieldInfo)
    , m_bCanModify(bCanModify)
    , m_nPriKeyIdx(-1)
    , m_bNeedCreateFirst(false)
{
    __ENTER_FUNCTION
    //通过row 转换为CMysqlField
    m_FieldsByIdx.reserve(m_pDBFieldInfo->size());
    for(size_t i = 0; i < m_pDBFieldInfo->size(); i++)
    {
        auto      ref_field_info_ptr = m_pDBFieldInfo->get(i);
        CDBField* pMysqlField        = nullptr;
        if(row == nullptr)
        {
            //没有数据的,那就是Make出来的
            m_bNeedCreateFirst = true;
            pMysqlField        = new CDBField(this, ref_field_info_ptr, nullptr, 0);
            m_FieldsByIdx.push_back(pMysqlField);
            m_FieldsByName[ref_field_info_ptr->GetFieldName()] = pMysqlField;
            if(ref_field_info_ptr->IsPriKey())
            {
                m_nPriKeyIdx = i;
                m_TableName  = ref_field_info_ptr->GetTableName();
            }
        }
        else
        {
            //有数据
            pMysqlField = new CDBField(this, ref_field_info_ptr, row[i], lengths[i]);
            m_FieldsByIdx.push_back(pMysqlField);
            m_FieldsByName[ref_field_info_ptr->GetFieldName()] = pMysqlField;
            if(CanModify())
            {
                if(ref_field_info_ptr->IsPriKey())
                {
                    if(m_nPriKeyIdx == -1)
                        m_nPriKeyIdx = i;
                    m_TableName = ref_field_info_ptr->GetTableName();
                    if(m_strPriKeyBuf.empty() == false)
                        m_strPriKeyBuf += ",";

                    m_strPriKeyBuf += ref_field_info_ptr->GetFieldName();
                    m_strPriKeyBuf += "=";
                    m_strPriKeyBuf += pMysqlField->GetValString();
                }
            }
        }
    }
    __LEAVE_FUNCTION
}

CDBRecord::~CDBRecord()
{
    __ENTER_FUNCTION
    for(CDBField* pField: m_FieldsByIdx)
    {
        delete pField;
    }
    m_FieldsByIdx.clear();
    m_FieldsByName.clear();
    __LEAVE_FUNCTION
}

bool CDBRecord::Update(bool bSync)
{
    __ENTER_FUNCTION
    if(CanModify() == false)
        return false;
    if(IsDirty() == false)
        return false;

    if(m_bNeedCreateFirst)
    {
        std::string sql = BuildInsertSQL();
        if(sql.empty())
        {
            // log error
            LOGDBERROR("BuildInsertSQL fail:{}", m_TableName.c_str());
            return false;
        }
        uint64_t insert_id = m_pMysqlConnection->Insert(sql);
        if(insert_id == static_cast<uint64_t>(-1))
        {
            return false;
        }
        if(m_nPriKeyIdx == 0)
        {
            CDBField& field = *(m_FieldsByIdx[m_nPriKeyIdx]);
            if(insert_id != 0)
            {
                if(field.GetType() == DB_FIELD_TYPE_LONG)
                    field = static_cast<uint32_t>(insert_id);
                else
                    field = insert_id;
            }

            m_strPriKeyBuf += field.GetFieldInfo()->GetFieldName();
            m_strPriKeyBuf += "=";
            m_strPriKeyBuf += field.GetValString();
        }
        else
        {
            // rebuildPriKeyBuf
            m_strPriKeyBuf.clear();
            for(size_t i = 0; i < m_FieldsByIdx.size(); i++)
            {
                auto&       pField     = m_FieldsByIdx[i];
                const auto& pFieldInfo = pField->GetFieldInfo();
                if(pFieldInfo->IsPriKey())
                {

                    if(m_strPriKeyBuf.empty() == false)
                        m_strPriKeyBuf += ",";

                    m_strPriKeyBuf += pFieldInfo->GetFieldName();
                    m_strPriKeyBuf += "=";
                    m_strPriKeyBuf += pField->GetValString();
                }
            }
        }

        ClearDirty();

        m_bNeedCreateFirst = false;
    }
    else
    {
        std::string sql = BuildUpdateSQL();
        if(sql.empty())
        {
            // log error
            LOGDBERROR("BuildUpdateSQL fail:{}", m_TableName.c_str());
            return false;
        }

        if(bSync)
        {
            m_pMysqlConnection->Update(sql);
            return true;
        }
        else
        {
            m_pMysqlConnection->AsyncExec(sql);
        }
        ClearDirty();
    }
    return true;
    __LEAVE_FUNCTION
    return false;
}

void CDBRecord::ClearDirty()
{
    __ENTER_FUNCTION
    m_setDirty.clear();
    __LEAVE_FUNCTION
}

void CDBRecord::DeleteRecord(bool bSync)
{
    __ENTER_FUNCTION
    if(CanModify() == false)
        return;

    std::string sql = BuildDeleteSQL();
    if(sql.empty())
    {
        // log error
        LOGDBERROR("BuildDeleteSQL fail:{}", m_TableName.c_str());
        return;
    }

    if(bSync)
    {
        if(m_pMysqlConnection->Update(sql) != 1)
        {
            // log error
            LOGDBERROR("DBRecord Update fail:{}", sql.c_str());
        }
    }
    else
    {
        m_pMysqlConnection->AsyncExec(sql);
    }
    __LEAVE_FUNCTION
}

std::string CDBRecord::BuildDeleteSQL()
{
    __ENTER_FUNCTION
    std::string szKeyNameBuf;
    std::string szKeyValBuf;
    for(size_t i = 0; i < m_FieldsByIdx.size(); i++)
    {
        CDBField* pField             = m_FieldsByIdx[i];
        auto      ref_field_info_ptr = (*m_pDBFieldInfo)[i];
        if(IsDirty(i) == false)
        {
            continue;
        }
        else
        {
            if(szKeyNameBuf.empty() == false)
            {
                szKeyNameBuf += ",";
                szKeyValBuf += ",";
            }

            szKeyNameBuf += ref_field_info_ptr->GetFieldName();
            if(pField->IsString())
            {
                szKeyValBuf += "'";
                szKeyValBuf += pField->GetValString();
                szKeyValBuf += "'";
            }
            else
            {
                szKeyValBuf += pField->GetValString();
            }
        }
    }
    if(szKeyNameBuf.empty() || szKeyValBuf.empty() || m_TableName.empty())
        return std::string();
    else
        return std::string("DELETE FROM ") + m_TableName + " WHERE " + szKeyNameBuf + "=" + szKeyValBuf + " LIMIT 1";
    __LEAVE_FUNCTION
    return std::string();
}

std::string CDBRecord::BuildUpdateSQL()
{
    __ENTER_FUNCTION
    std::string szKeyBuf;
    for(size_t i = 0; i < m_FieldsByIdx.size(); i++)
    {
        CDBField* pField = m_FieldsByIdx[i];
        if(IsDirty(i) == false)
        {
            continue;
        }
        else
        {
            auto ref_field_info_ptr = (*m_pDBFieldInfo)[i];
            if(szKeyBuf.empty() == false)
                szKeyBuf += ",";

            szKeyBuf += ref_field_info_ptr->GetFieldName();
            szKeyBuf += "=";
            szKeyBuf += pField->GetValString();
        }
    }
    if(szKeyBuf.empty() || m_strPriKeyBuf.empty() || m_TableName.empty())
        return std::string();
    else
        return std::string("UPDATE ") + m_TableName + " SET " + szKeyBuf + " WHERE " + m_strPriKeyBuf + " LIMIT 1";
    __LEAVE_FUNCTION
    return std::string();
}

std::string CDBRecord::BuildInsertSQL()
{
    __ENTER_FUNCTION
    std::string szKeyNameBuf;
    std::string szKeyValBuf;
    for(size_t i = 0; i < m_FieldsByIdx.size(); i++)
    {
        CDBField* pField             = m_FieldsByIdx[i];
        auto      ref_field_info_ptr = (*m_pDBFieldInfo)[i];
        if(IsDirty(i) == false)
        {
            continue;
        }
        else
        {
            if(szKeyNameBuf.empty() == false)
            {
                szKeyNameBuf += ",";
                szKeyValBuf += ",";
            }

            szKeyNameBuf += ref_field_info_ptr->GetFieldName();
            szKeyValBuf += pField->GetValString();
        }
    }
    if(szKeyNameBuf.empty() || szKeyValBuf.empty() || m_TableName.empty())
        return std::string();
    else
        return std::string("INSERT INTO ") + m_TableName + "(" + szKeyNameBuf + ") VALUES ( " + szKeyValBuf + ")";
    __LEAVE_FUNCTION
    return std::string();
}

bool CDBRecord::IsDirty() const
{
    return m_setDirty.any();
}

bool CDBRecord::IsDirty(uint32_t idx) const
{
    return m_setDirty.test(idx);
}

void CDBRecord::MakeDirty(uint32_t idx)
{
    m_setDirty.set(idx, true);
}