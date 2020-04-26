#ifndef COOLDOWN_H
#define COOLDOWN_H

#include "BaseCode.h"
#include "DBRecord.h"
#include "gamedb.h"

//冷却
export_lua enum COOLDOWN_TYPE {
    COLLDOWN_TYPE_COMMON = 0,
    COOLDOWN_TYPE_ITEM   = 1, //物品,keyidx=物品CD编号
    COOLDOWN_TYPE_SKILL  = 2, //技能,keyidx=技能CD编号
    COOLDOWN_TYPE_TALK   = 3, //聊天,keyidx=频道号

};

export_lua class ICoolDown
{
public:
    ICoolDown() {}
    virtual ~ICoolDown() {}
    export_lua virtual uint64_t GetExpireTime() const                                                  = 0;
    export_lua virtual bool     StartCoolDown(uint32_t nMSec, bool bUpdate = false, bool bSync = true) = 0;
    export_lua virtual bool     ClearCoolDown(bool bUpdate = false, bool bSync = true)                 = 0;

public:
    OBJECTHEAP_DECLARATION(s_heap);
};

export_lua class CCoolDown : public ICoolDown
{
public:
    CCoolDown() {}
    virtual ~CCoolDown() {}
    virtual uint64_t GetExpireTime() const { return m_nTimeStamp; }
    virtual bool     StartCoolDown(uint32_t nMSec, bool bUpdate = false, bool bSync = true)
    {
        m_nTimeStamp = TimeGetMillisecond() + nMSec;
        return true;
    }
    virtual bool ClearCoolDown(bool bUpdate = false, bool bSync = true)
    {
        m_nTimeStamp = 0;
        return true;
    }

private:
    uint64_t m_nTimeStamp;
};

class CPlayer;
export_lua class CPlayerCoolDown : public ICoolDown
{
public:
    CPlayerCoolDown(CPlayer* pPlayer, CDBRecordPtr&& pRecord);
    virtual ~CPlayerCoolDown();

    uint32_t         GetType() const;
    uint32_t         GetIdx() const;
    virtual uint64_t GetExpireTime() const;
    virtual bool     StartCoolDown(uint32_t nMSec, bool bUpdate = false, bool bSync = true);
    virtual bool     ClearCoolDown(bool bUpdate = false, bool bSync = true);

    void Sync();
    void Save();
    void DeleteRecord();

private:
    CPlayer*     m_pOwner = nullptr;
    CDBRecordPtr m_pRecord;
};

export_lua class CCoolDownSet : public NoncopyableT<CCoolDownSet>
{
protected:
    CCoolDownSet();

public:
    CreateNewImpl(CCoolDownSet);

public:
    virtual ~CCoolDownSet();

    bool                    Init();
    export_lua virtual void SyncAll() {}
    export_lua virtual void Save() {}

    export_lua virtual bool IsCoolDown(uint32_t nType, uint32_t nIdx);
    export_lua virtual bool IsCoolDown(uint32_t nType, uint32_t nIdx, uint32_t now);
    export_lua virtual bool StartCoolDown(uint32_t nType,
                                          uint32_t nIdx,
                                          uint32_t nMSec,
                                          bool     bUpdate = false,
                                          bool     bSync   = true);
    export_lua virtual bool ClearCoolDown(uint32_t nType, uint32_t nIdx, bool bUpdate = false, bool bSync = true);

protected:
    virtual ICoolDown* CreateData(uint32_t nType, uint32_t nIdx, uint32_t nMSec);

protected:
    std::unordered_map<uint64_t, std::unique_ptr<ICoolDown> > m_setDataMap;

    OBJECTHEAP_DECLARATION(s_heap);
};

export_lua class CPlayerCoolDownSet : public CCoolDownSet
{
    CPlayerCoolDownSet();

public:
    CreateNewImpl(CPlayerCoolDownSet);

public:
    ~CPlayerCoolDownSet();

    bool         Init(CPlayer* pPlayer);
    virtual void SyncAll() override;
    virtual void Save() override;

protected:
    virtual ICoolDown* CreateData(uint32_t nType, uint32_t nIdx, uint32_t nMSec) override;

protected:
    CPlayer* m_pOwner = nullptr;
};
#endif /* COOLDOWN_H */
