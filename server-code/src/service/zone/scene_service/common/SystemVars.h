#ifndef SYSTEMVARS_H
#define SYSTEMVARS_H

#include "BaseCode.h"
#include "DBRecord.h"

//系统全局变量

export_lua enum SYSTEMVAR_KEYIDX {
    SYSTEMVAR_BEGIN        = 0,
    SYSTEMVAR_SERVER_START = 1, //开服时间,data0=开服时间戳,data1=最后一次合服时间戳

    SYSTEMVAR_MERGE_END = 1000, //这后面的数据,合服时不保留

    SYSTEMVAR_NOT_SAVE = 10000, //这后面的都不会回写数据库,

};
class CPlayer;
export_lua class CSystemVar : public NoncopyableT<CSystemVar>
{
public:
    CSystemVar();
    explicit CSystemVar(CDBRecordPtr&& pRecord);
    ~CSystemVar();

    export_lua uint32_t GetIdx() const;
    export_lua uint64_t GetData(uint32_t nIdx) const;
    export_lua const std::string& GetStr(uint32_t nIdx) const;

    export_lua void AddData(uint32_t nIdx, int64_t nVal, bool bUpdate = false, bool bSync = false);
    export_lua void SetData(uint32_t nIdx, uint64_t nVal, bool bUpdate = false, bool bSync = false);
    export_lua void SetStr(uint32_t nIdx, const std::string& strVal, bool bUpdate = false, bool bSync = false);
    export_lua void Broadcast();
    export_lua void SendToPlayer(CPlayer* pPlayer);
    export_lua void Save();
    export_lua void ClearDirty();
    export_lua void DeleteRecord();

private:
    uint32_t    m_nIdx    = 0;
    uint64_t    m_Data[4] = {0, 0, 0, 0};
    std::string m_szStrData[4];

    CDBRecordPtr m_pRecord;
    OBJECTHEAP_DECLARATION(s_heap);
};

export_lua class CSystemVarSet : public NoncopyableT<CSystemVarSet>
{
    CSystemVarSet();

public:
    CreateNewImpl(CSystemVarSet);

public:
    ~CSystemVarSet();

    bool       Init();
    export_lua CSystemVar* QueryVar(uint32_t nIdx, bool bCreateNew = false);
    export_lua CSystemVar* CreateVar(uint32_t nIdx);
    export_lua void        DeleteVar(uint32_t nIdx);

    void            OnSystemVarChange(class CNetworkMessage* pMsg);
    export_lua void SyncToClient(CPlayer* pPlayer);

private:
    std::unordered_map<uint32_t, std::unique_ptr<CSystemVar> > m_setData;
};
#endif /* SYSTEMVARS_H */
