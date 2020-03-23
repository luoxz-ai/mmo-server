#ifndef ISTATUS_H
#define ISTATUS_H

#include "ActorAttrib.h"
#include "BaseCode.h"
#include "DBRecord.h"
#include "EventEntry.h"
#include "T_GameDataMap.h"
#include "config/Cfg_Status.pb.h"
struct ST_STATUS_INFO
{
    uint16_t idStatusType; // 状态编码(0-100)保留给基础类型
    uint8_t  ucLevel;      // 状态等级
    int32_t  nPower;       //
    int32_t  nSecs;        // 持续时间(或多次作用时间间隔)
    int32_t  nTimes;       // 作用次数
    uint32_t tLastStamp;   // 最后一次作用的时间戳
    OBJID    idCaster;     // 施加此状态的角色
    uint8_t  bPause;       // 状态：0 = 正常， 1 = 暂停
};

export_lua enum StatusType {
    STATUSTYPE_NORMAL    = 1,
    STATUSTYPE_CRIME     = 2, //犯罪
    STATUSTYPE_DEAD      = 3, //死亡
    STATUSTYPE_COMP      = 4, //阵营 power=阵营id
    STATUSTYPE_TRANSFORM = 5, //变形 power=变形后的模型ID
    STATUSTYPE_DAMAGE    = 6, //受到伤害 每隔nSecs秒受到nPower的伤害
    STATUSTYPE_HIDE      = 7, //隐形

};

export_lua enum StatusExpireType {
    STATUSEXPIRETYPE_TIME  = 0, //默认时间计时销毁
    STATUSEXPIRETYPE_POINT = 1, // POWER为0时销毁
    STATUSEXPIRETYPE_NEVER = 2, //必须手动/达到条件销毁
};

export_lua enum StatusFlag {
    STATUSFLAG_NONE         = 0x0000, //默认不可叠加,不可覆盖
    STATUSFLAG_OVERRIDE_LEV = 0x0001, //高等级可以覆盖低等级
    STATUSFLAG_OVERLAP = 0x0002, //不考虑等级, 直接叠加， 时间形的叠加时间， 数值型的叠加数值
    STATUSFLAG_PAUSE_ATTACH = 0x0008, //附加时默认暂停

    STATUSFLAG_DISABLE_MOVE     = 0x0010, //禁止移动
    STATUSFLAG_DISABLE_BEATTACK = 0x0020, //禁止被攻击
    STATUSFLAG_DISABLE_ATTACK   = 0x0040, //禁止攻击
    STATUSFLAG_BREAK_SKILL      = 0x0080, //附加状态时打断目标的技能释放

    STATUSFLAG_UNDEATCH_DEAD   = 0x0100, //死亡不会自动移除
    STATUSFLAG_DEATCH_MOVE     = 0x0200, //移动自动移除
    STATUSFLAG_DEATCH_BEATTACK = 0x0400, //被攻击自动移除
    STATUSFLAG_DEATCH_SKILL    = 0x0800, //使用技能自动移除
    STATUSFLAG_DEATCH_ATTACK   = 0x1000, //攻击他人自动移除
    STATUSFLAG_DEATCH_LEAVEMAP = 0x2000, //离开地图自动移除
    STATUSFLAG_DEATCH_OFFLINE  = 0x4000, //离线自动移除

    STATUSFLAG_OFFLINE_PAUSE = 0x00010000, //下线自动暂停
    STATUSFLAG_ONLINE_RESUME = 0x00020000, //上线自动恢复
    STATUSFLAG_ALLOW_PAUSE   = 0x00040000, //允许手动暂停

};

class CStatusType
{
    CStatusType() {}

public:
    ~CStatusType() {}
    CREATE_NEW_IMPL(CStatusType);
    bool Init(const Cfg_Status_Row& row)
    {
        m_Data = row;
        for(int32_t i = 0; i < m_Data.attrib_change_list_size(); i++)
        {
            m_AttribChangeList.push_back(CActorAttribChange{m_Data.attrib_change_list(i)});
        }
        return true;
    }

    using PB_T = Cfg_Status;
    static uint32_t MakeID(uint16_t idStatus, uint8_t ucLevel)
    {
        if(idStatus <= 100)
            return (uint32_t)(idStatus << 16) | (uint32_t)1;
        else
            return (uint32_t)(idStatus << 16) | (uint32_t)ucLevel;
    }
    uint32_t                               GetID() const { return MakeID(GetStatusTypeID(), GetLevel()); }
    uint16_t                               GetStatusTypeID() const { return m_Data.id(); }
    uint8_t                                GetLevel() const { return m_Data.level(); }
    uint32_t                               GetType() const { return m_Data.status_type(); }
    uint32_t                               GetExpireType() const { return m_Data.status_expire_type(); }
    uint32_t                               GetFlag() const { return m_Data.status_flag(); }
    int32_t                                GetPower() const { return m_Data.power(); }
    int32_t                                GetSecs() const { return m_Data.secs(); }
    int32_t                                GetTimes() const { return m_Data.times(); }
    int32_t                                GetMaxTimes() const { return m_Data.max_times(); }
    int32_t                                GetMaxSecs() const { return m_Data.max_secs(); }
    OBJID                                  GetScirptID() const { return m_Data.scriptid(); }
    const std::vector<CActorAttribChange>& GetAttribChangeList() const { return m_AttribChangeList; }

private:
    Cfg_Status_Row                  m_Data;
    std::vector<CActorAttribChange> m_AttribChangeList;
};

typedef CGameDataMap<CStatusType> CStatusTypeSet;

class CActor;
export_lua class CStatus
{
public:
    CStatus();
    virtual ~CStatus();

public:
    CREATE_NEW_IMPL(CStatus);
    bool Init(CActor* pOwner, CDBRecordPtr&& pRow);
    bool Init(CActor*  pOwner,
              uint16_t idStatusType,
              uint8_t  ucLevel,
              OBJID    idCaster,
              uint32_t nPower,
              uint32_t nSecs,
              uint32_t nTimes);

public:
    unsigned long Release()
    {
        delete this;
        return 0;
    }
    export_lua CActor* GetOwner() const { return m_pOwner; }
    export_lua CStatusType* Type() const { return m_pType; }

    export_lua bool     IsValid() const;
    export_lua uint32_t GetType() const { return m_pType->GetType(); }
    export_lua uint16_t GetStatusTypeID() const { return m_info.idStatusType; }
    export_lua int32_t  GetPower() const { return m_info.nPower; }
    export_lua OBJID    GetCasterID() const { return m_info.idCaster; }
    export_lua uint8_t  GetLevel() const { return m_info.ucLevel; }
    export_lua int32_t  GetSecs() const { return m_info.nSecs; }
    export_lua int32_t  GetTimes() const { return m_info.nTimes; }
    export_lua uint32_t GetLastTimeStamp() const { return m_info.tLastStamp; }
    export_lua uint32_t GetFlag() const { return m_pType->GetFlag(); }
    export_lua int32_t  GetRemainTime() const;

    export_lua void SetPower(int32_t nPower) { m_info.nPower = nPower; }
    export_lua void AddSecs(int32_t nSecs);
    export_lua void AddTimes(int32_t nTimes);
    export_lua bool ChangeData(uint8_t ucLevel, int32_t nPower, int32_t nSecs, int32_t nTimes, OBJID idCaster);

    export_lua bool IsPaused() const { return m_info.bPause == TRUE; }
    export_lua void Pause(bool bSynchro = true);
    export_lua void Resume(bool bSynchro = true);
    export_lua void SaveInfo();
    export_lua void SendStatus();

public:
    export_lua bool ScheduleEvent(time_t tIntervalMS = 0);
    export_lua void CancelEvent();
    export_lua void ClearEvent();
    export_lua void ProcessEvent();

public:
    export_lua void OnAttach();
    export_lua void OnDeatch();
    export_lua bool OnMove();
    export_lua bool OnSkill(uint32_t idSkill);
    export_lua bool OnAttack(CActor* pTarget, uint32_t idSkill, int32_t nDamage);
    export_lua bool OnBeAttack(CActor* pAttacker, int32_t nDamage);
    export_lua bool OnDead(CActor* pKiller);
    export_lua bool OnLeaveMap();
    void            OnLogin();
    void            OnLogout();

private:
    void OnEffect();

protected:
    CActor*        m_pOwner = nullptr;
    CStatusType*   m_pType  = nullptr;
    ST_STATUS_INFO m_info;
    CDBRecordPtr   m_pRecord;

    CEventEntryPtr m_StatusEvent;

public:
    MEMORYHEAP_DECLARATION(s_heap)
};
#endif /* ISTATUS_H */
