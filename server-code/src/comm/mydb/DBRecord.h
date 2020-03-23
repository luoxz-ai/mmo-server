#ifndef DBRECORD_H
#define DBRECORD_H

#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include <mysql/mysql.h>

#include "BaseCode.h"
#include "DBField.h"
#include "MemoryHeap.h"
#include "dynamic_bitset.h"

class CDBRecord;
class CMysqlConnection;

class CDBRecord
{
public:
    CDBRecord(CMysqlConnection*   pMysqlConnection,
              CDBFieldInfoListPtr pMysqlFieldInfo,
              bool                bCanModify,
              MYSQL_ROW           row,
              unsigned long*      lengths);
    ~CDBRecord();

    CDBField& Field(size_t n)
    {
        if(n < m_FieldsByIdx.size())
            return *(m_FieldsByIdx[n]);

        // log error
        LOGDBERROR("CDBRecord {} didn't have Field By Index {}", m_TableName.c_str(), n);

        static CDBField empty;
        return empty;
    }

    CDBField& Field(const std::string& name)
    {
        auto itFind = m_FieldsByName.find(name);
        if(itFind != m_FieldsByName.end())
            return *(itFind->second);

        // log error
        LOGDBERROR("CDBRecord {} didn't have Field By Name {}", m_TableName.c_str(), name.c_str());

        static CDBField empty;
        return empty;
    }
    bool              CanModify() const { return m_bCanModify; }
    bool              Update(bool bSync = true);
    void              ClearDirty();
    void              DeleteRecord(bool bSync = true);
    CMysqlConnection* _GetMysqlConnection() const { return m_pMysqlConnection; }
    bool              IsDirty() const;
    bool              IsDirty(uint32_t idx) const;
    void              MakeDirty(uint32_t idx);

private:
    std::string BuildDeleteSQL();
    std::string BuildUpdateSQL();
    std::string BuildInsertSQL();

public:
    MEMORYHEAP_DECLARATION(s_Heap);

private:
    CMysqlConnection*                          m_pMysqlConnection;
    CDBFieldInfoListPtr                        m_pDBFieldInfo;
    bool                                       m_bCanModify;
    std::vector<CDBField*>                     m_FieldsByIdx;
    std::unordered_map<std::string, CDBField*> m_FieldsByName;
    std::string                                m_TableName;
    std::string                                m_strPriKeyBuf;
    int32_t                                    m_nPriKeyIdx;
    bool                                       m_bNeedCreateFirst;
    dynamic_bitset<uint32_t>                   m_setDirty;
};

using CDBRecordPtr = std::unique_ptr<CDBRecord>;

#endif /* DBRECORD_H */
