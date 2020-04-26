#ifndef SYSTEMVARS_H
#define SYSTEMVARS_H

#include "BaseCode.h"
#include "DBRecord.h"
#include "gamedb.h"

//系统全局变量

enum SYSTEMVAR_KEYIDX
{
    SYSTEMVAR_BEGIN        = 0,
    SYSTEMVAR_SERVER_START = 1, //开服时间,data0=开服时间戳,data1=最后一次合服时间戳

    SYSTEMVAR_MERGE_END = 1000, //这后面的数据,合服时不保留

    SYSTEMVAR_NOT_SAVE = 10000, //这后面的都不会回写数据库,

};
class CSystemVar : public NoncopyableT<CSystemVar>
{
public:
    CSystemVar();
    explicit CSystemVar(CDBRecordPtr&& pRecord);
    ~CSystemVar();

    uint32_t           GetIdx() const;
    uint64_t           GetData(uint32_t nIdx) const;
    const std::string& GetStr(uint32_t nIdx) const;
    void               AddData(uint32_t nIdx, int64_t nVal, bool bUpdate = false, bool bSync = false);
    void               SetData(uint32_t nIdx, uint64_t nVal, bool bUpdate = false, bool bSync = false);
    void               SetStr(uint32_t nIdx, const std::string& strVal, bool bUpdate = false, bool bSync = false);
    void               Broadcast();
    void               Save();
    void               DeleteRecord();

public:
    OBJECTHEAP_DECLARATION(s_heap);

private:
    uint32_t    m_nIdx    = 0;
    uint64_t    m_Data[4] = {0, 0, 0, 0};
    std::string m_szStrData[4];

    CDBRecordPtr m_pRecord;
};

class CSystemVarSet : public NoncopyableT<CSystemVarSet>
{
    CSystemVarSet();
public:
    CreateNewImpl(CSystemVarSet);
public:
    ~CSystemVarSet();

    bool        Init();
    CSystemVar* QueryVar(uint32_t nIdx, bool bCreateNew = false);
    CSystemVar* CreateVar(uint32_t nIdx);
    void        DeleteVar(uint32_t nIdx);

    void OnSystemVarChange(class CNetworkMessage* pMsg);

private:
    std::unordered_map<uint32_t, std::unique_ptr<CSystemVar> > m_setData;
};
#endif /* SYSTEMVARS_H */
