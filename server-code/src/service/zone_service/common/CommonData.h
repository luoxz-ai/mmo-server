#ifndef COMMONDATA_H
#define COMMONDATA_H

#include "BaseCode.h"
#include "DBRecord.h"
#include "gamedb.h"

//一些不想单独写数据表的数据

export_lua enum COMMON_DATA_KEYIDX {
    COMMON_DATA_BEGIN = 0,

    //给客户端保存数据用的
    COMMON_DATA_CLIENT_BEGIN = 100,
    COMMON_DATA_CLIENT_END   = 200,

    COMMON_DATA_AUTO_SYNC = 1000, //小于1000的都需要自动同步到前端

    COMMON_DATA_NOT_SYNC_CLIENT = 10000, //这后面的，login时都不会同步到前端
    COMMON_DATA_CONTINUELOGIN,           //累计登录 data0=更新时间戳 data1=连续登录次数

};

class CPlayer;

export_lua class CCommonData
{
public:
    CCommonData(CPlayer* pPlayer);
    CCommonData(CPlayer* pPlayer, CDBRecordPtr&& pRecord);
    ~CCommonData();

    export_lua uint32_t GetIdx() const;
    export_lua uint64_t GetData(uint32_t nIdx) const;

    export_lua uint64_t AddData(uint32_t nIdx, uint64_t nVal, bool bUpdate = false, bool bSync = false);
    export_lua void     SetData(uint32_t nIdx, uint64_t nVal, bool bUpdate = false, bool bSync = false);
    export_lua void     Sync();
    export_lua void     Save();
    export_lua void     DeleteRecord();

private:
    CPlayer*     m_pOwner = nullptr;
    CDBRecordPtr m_pRecord;

    OBJECTHEAP_DECLARATION(s_heap);
};

export_lua class CCommonDataSet : public NoncopyableT<CCommonDataSet>
{
    CCommonDataSet();
public:
    CreateNewImpl(CCommonDataSet);
public:
    ~CCommonDataSet();
    

    bool            Init(CPlayer* pPlayer);
    export_lua void SyncAll();
    export_lua void Save();
    export_lua CCommonData* QueryData(uint32_t nIdx, bool bCreateNew = false);
    export_lua CCommonData* CreateData(uint32_t nIdx);
    export_lua void         DeleteData(uint32_t nIdx);

private:
    CPlayer*                                   m_pOwner = nullptr;
    std::unordered_map<uint32_t, std::unique_ptr<CCommonData> > m_setData;
};
#endif /* COMMONDATA_H */
