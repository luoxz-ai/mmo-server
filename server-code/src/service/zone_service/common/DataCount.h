#ifndef DATACOUNT_H
#define DATACOUNT_H

#include "BaseCode.h"
#include "DBRecord.h"
#include "T_GameDataMap.h"
#include "config/Cfg_DataCountLimit.pb.h"
#include "gamedb.h"
//计数及各种每日次数限制
export_lua enum DATA_ACC_TYPE {
    DATA_ACC_SYSTEM      = 0, //其他的计数？？？
    DATA_ACC_MONSTERKILL = 1, //杀怪计数 keyidx=怪物类型id
    DATA_ACC_USEITEM     = 2, //使用物品计数 keyidx=物品类型id
    DATA_ACC_TASKID      = 3, //完成任务计数 keyidx=任务类型id
    DATA_ACC_BUYITEM     = 4, //购买物品计数 keyidx=物品类型id

};

export_lua enum {
    DATA_ACC_SYSTEM_LOGINDAY        = 1, //累计登陆天数
    DATA_ACC_SYSTEM_TXTATR_GLOBAL   = 2, //累计使用小喇叭次数
    DATA_ACC_SYSTEM_KILLPLAYER      = 3, //击杀玩家数量
    DATA_ACC_SYSTEM_BEKILL_BYPLAYER = 4, //被玩家杀死次数
    DATA_ACC_SYSTEM_KILL_MONSTER    = 5, //累计击杀BOSS数量
    DATA_ACC_SYSTEM_KILL_BOSS       = 6, //累计击杀BOSS数量
};

class CDataCountLimit : public NoncopyableT<CDataCountLimit>
{
    CDataCountLimit() {}
    bool Init(const Cfg_DataCountLimit_Row& row)
    {
        m_nType      = row.type();
        m_nKeyIdx    = row.keyidx();
        m_nMaxCount  = row.max_count();
        m_nResetTime = row.reset_time();
        return true;
    }

public:
    CreateNewImpl(CDataCountLimit);

public:
    using PB_T = Cfg_DataCountLimit;
    virtual ~CDataCountLimit() {}

public:
    uint64_t GetID() const { return CDataCountLimit::MakeID(m_nType, m_nKeyIdx); }
    uint32_t GetType() const { return m_nType; }
    uint32_t GetKeyIdx() const { return m_nKeyIdx; }
    uint32_t GetMaxCount() const { return m_nMaxCount; }
    uint32_t GetResetTime() const { return m_nResetTime; }

public:
    static uint64_t MakeID(uint32_t nType, uint32_t nKeyIdx) { return ((uint64_t)(nType) << 32) | (uint64_t)(nKeyIdx); }

protected:
    uint32_t m_nType;
    uint32_t m_nKeyIdx;
    uint32_t m_nMaxCount;
    uint32_t m_nResetTime;
};

DEFINE_GAMEMAPDATA(CDataCountLimitSet, CDataCountLimit);

class CPlayer;

export_lua class CDataCount
{
public:
    CDataCount(CPlayer* pPlayer, CDBRecordPtr&& pRecord);
    ~CDataCount();

    export_lua uint32_t GetType() const;
    export_lua uint32_t GetIdx() const;
    export_lua uint64_t GetDataNum();
    export_lua uint32_t GetLastResetTime() const;

    export_lua uint64_t AddData(uint32_t nVal, bool bUpdate);
    export_lua uint64_t SetData(uint64_t nVal, bool bUpdate);

    export_lua void Sync();
    export_lua void Save();
    export_lua void DeleteRecord();

private:
    CPlayer*     m_pOwner = nullptr;
    CDBRecordPtr m_pRecord;

    OBJECTHEAP_DECLARATION(s_heap);
};

export_lua class CDataCountSet : public NoncopyableT<CDataCountSet>
{
    CDataCountSet();

public:
    CreateNewImpl(CDataCountSet);

public:
    ~CDataCountSet();

    bool            Init(CPlayer* pPlayer);
    export_lua void SyncAll();
    export_lua void Save();

    export_lua uint64_t GetCount(uint32_t nType, uint32_t nIdx);
    export_lua uint64_t GetMaxCount(uint32_t nType, uint32_t nIdx);
    export_lua uint64_t AddCount(uint32_t nType, uint32_t nIdx, uint64_t nVal, bool bUpdate = false);
    export_lua uint64_t _AddCount(uint32_t nType, uint32_t nIdx, uint64_t nVal, bool bUpdate = false);
    export_lua uint64_t SetCount(uint32_t nType, uint32_t nIdx, uint32_t nVal, bool bUpdate = false);
    export_lua void     DeleteCount(uint32_t nType, uint32_t nIdx);

private:
    void CreateData(uint32_t nType, uint32_t nIdx, uint32_t nVal);

private:
    CPlayer* m_pOwner = nullptr;

    std::unordered_map<uint64_t, std::unique_ptr<CDataCount> > m_setDataMap;
};
#endif /* DATACOUNT_H */
