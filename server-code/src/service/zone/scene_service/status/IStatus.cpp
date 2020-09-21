#include "IStatus.h"

#include "Actor.h"
#include "SceneService.h"
#include "gamedb.h"

AttachStatusInfo CStatusType::CloneInfo() const
{
    return {.id_status_type = GetTypeID(),
            .lev            = GetLevel(),
            .power          = GetPower(),
            .secs           = GetSecs(),
            .times          = GetTimes(),
            .flag           = GetFlag(),
            .id_status      = GetID(),
            .expire_type    = GetExpireType()};
}

OBJECTHEAP_IMPLEMENTATION(CStatus, s_heap);
CStatus::CStatus() {}

CStatus::~CStatus() {}

bool CStatus::Init(CActor* pOwner, const AttachStatusInfo& info)
{
    __ENTER_FUNCTION
    m_info = info;
    if(info.id_status != 0)
    {
        m_pType = StatusTypeSet()->QueryObj(info.id_status);
        CHECKF(m_pType);
    }

    m_pOwner = pOwner;

    if(info.expire_type == STATUSEXPIRETYPE_TIME)
        m_info.last_timestamp = TimeGetSecond();
    else
        m_info.last_timestamp = 0;

    m_info.pause = bit_test(info.flag, STATUSFLAG_PAUSE_ATTACH) ? TRUE : FALSE;
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CStatus::Init(CActor* pOwner, CDBRecordPtr&& pRow)
{
    __ENTER_FUNCTION
    uint32_t idStatus = pRow->Field(TBLD_STATUS::STATUSID);

    if(idStatus != 0)
    {
        m_pType = StatusTypeSet()->QueryObj(idStatus);
        CHECKF(m_pType);
    }

    m_pOwner = pOwner;
    m_pRecord.reset(pRow.release());

    m_info.id_status_type = pRow->Field(TBLD_STATUS::TYPEID);
    m_info.lev            = pRow->Field(TBLD_STATUS::LEV);
    m_info.power          = pRow->Field(TBLD_STATUS::POWER);
    m_info.secs           = pRow->Field(TBLD_STATUS::SECS);
    m_info.times          = pRow->Field(TBLD_STATUS::TIMES);
    m_info.last_timestamp = pRow->Field(TBLD_STATUS::LASTSTAMP);

    m_info.id_caster = pRow->Field(TBLD_STATUS::CASTERID);
    m_info.pause     = pRow->Field(TBLD_STATUS::PAUSE);
    m_info.id_status = idStatus;

    //如果上线自动启动的
    if(IsPaused() && HasFlag(GetFlag(), STATUSFLAG_ONLINE_RESUME))
    {
        Resume(false);
    }

    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CStatus::IsValid() const
{
    __ENTER_FUNCTION
    if(m_info.expire_type == STATUSEXPIRETYPE_TIME)
    {
        if(GetTimes() > 0)
        {
            return true;
        }
        else
        {
            return TimeGetSecond() > GetLastTimeStamp() + GetSecs();
        }
    }
    else if(m_info.expire_type == STATUSEXPIRETYPE_POINT)
    {
        return GetPower() > 0;
    }
    else
    {
        return true;
    }
    __LEAVE_FUNCTION
    return false;
}

int32_t CStatus::GetRemainTime() const
{
    __ENTER_FUNCTION
    if(GetTimes() > 0)
    {
        time_t now       = TimeGetSecond();
        time_t nextStamp = GetLastTimeStamp() + GetSecs();
        if(nextStamp > now)
            return GetTimes() * GetSecs() + (nextStamp - now);
        else
            return GetTimes() * GetSecs();
    }
    else
    {
        time_t now       = TimeGetSecond();
        time_t nextStamp = GetLastTimeStamp() + GetSecs();
        if(nextStamp > now)
            return nextStamp - now;
        return 0;
    }
    __LEAVE_FUNCTION
    return 0;
}

void CStatus::AddSecs(int32_t nSecs)
{
    __ENTER_FUNCTION
    Pause(false);

    if(m_pType && ((m_pType->GetMaxSecs() - m_info.secs) > nSecs))
    {
        m_info.secs = m_pType->GetMaxSecs();
    }
    else
    {
        m_info.secs += nSecs;
    }
    Resume(false);
    __LEAVE_FUNCTION
}

void CStatus::AddTimes(int32_t nTimes)
{
    __ENTER_FUNCTION
    if(m_pType && m_pType->GetMaxTimes() - m_info.times > nTimes)
    {
        m_info.times = m_pType->GetMaxTimes();
    }
    else
    {
        m_info.times += nTimes;
    }
    __LEAVE_FUNCTION
}

bool CStatus::ChangeData(const AttachStatusInfo& info)
{
    __ENTER_FUNCTION
    OnDeatch();
    m_info = info;
    if(info.id_status != 0)
    {
        m_pType = StatusTypeSet()->QueryObj(info.id_status);
        CHECKF(m_pType);
    }
    else
    {
        m_pType = nullptr;
    }

    Pause(false);
    Resume(false);

    return true;
    __LEAVE_FUNCTION
    return false;
}

void CStatus::Pause(bool bSynchro /*= true*/)
{
    __ENTER_FUNCTION
    if(IsPaused() == true)
        return;

    if(m_info.last_timestamp > 0)
    {
        time_t now       = TimeGetSecond();
        time_t nextStamp = GetLastTimeStamp() + GetSecs();
        if(nextStamp > now)
        {
            m_info.secs = nextStamp - now;
        }
        m_info.last_timestamp = 0;
    }

    if(bSynchro)
        SendStatus();

    __LEAVE_FUNCTION
}

void CStatus::Resume(bool bSynchro /*= true*/)
{
    __ENTER_FUNCTION
    if(IsPaused() == false)
        return;

    if(m_info.last_timestamp == 0)
    {
        ScheduleEvent(GetSecs());
    }

    if(bSynchro)
        SendStatus();
    __LEAVE_FUNCTION
}

void CStatus::SaveInfo()
{
    __ENTER_FUNCTION
    if(m_pRecord)
    {
        m_pRecord->Field(TBLD_STATUS::LEV)       = m_info.lev;
        m_pRecord->Field(TBLD_STATUS::POWER)     = m_info.power;
        m_pRecord->Field(TBLD_STATUS::SECS)      = m_info.secs;
        m_pRecord->Field(TBLD_STATUS::TIMES)     = m_info.times;
        m_pRecord->Field(TBLD_STATUS::LASTSTAMP) = m_info.last_timestamp;
        m_pRecord->Field(TBLD_STATUS::CASTERID)  = m_info.id_caster;
        m_pRecord->Field(TBLD_STATUS::PAUSE)     = m_info.pause;
        m_pRecord->Field(TBLD_STATUS::STATUSID)  = m_info.id_status;
    }
    else
    {
        //创建
        m_pRecord                                = SceneService()->GetGameDB(m_pOwner->GetWorldID())->MakeRecord(TBLD_STATUS::table_name());
        m_pRecord->Field(TBLD_STATUS::ID)        = SceneService()->CreateUID();
        m_pRecord->Field(TBLD_STATUS::USERID)    = m_pOwner->GetID();
        m_pRecord->Field(TBLD_STATUS::STATUSID)  = m_info.id_status;
        m_pRecord->Field(TBLD_STATUS::TYPEID)    = m_info.id_status_type;
        m_pRecord->Field(TBLD_STATUS::LEV)       = m_info.lev;
        m_pRecord->Field(TBLD_STATUS::POWER)     = m_info.power;
        m_pRecord->Field(TBLD_STATUS::SECS)      = m_info.secs;
        m_pRecord->Field(TBLD_STATUS::TIMES)     = m_info.times;
        m_pRecord->Field(TBLD_STATUS::LASTSTAMP) = m_info.last_timestamp;
        m_pRecord->Field(TBLD_STATUS::CASTERID)  = m_info.id_caster;
        m_pRecord->Field(TBLD_STATUS::PAUSE)     = m_info.pause;
    }
    CHECK(m_pRecord->Update());
    __LEAVE_FUNCTION
}

bool CStatus::ScheduleEvent(time_t tIntervalMS /*= 0*/)
{
    __ENTER_FUNCTION
    m_info.last_timestamp = TimeGetSecond();
    CEventEntryCreateParam param;
    param.evType    = 0;
    param.cb        = std::bind(&CStatus::ProcessEvent, this);
    param.tWaitTime = tIntervalMS;
    param.bPersist  = false;
    return EventManager()->ScheduleEvent(param, m_StatusEvent);
    __LEAVE_FUNCTION
    return false;
}

void CStatus::CancelEvent()
{
    __ENTER_FUNCTION
    m_StatusEvent.Cancel();
    __LEAVE_FUNCTION
}

void CStatus::ClearEvent()
{
    __ENTER_FUNCTION
    m_StatusEvent.Clear();
    __LEAVE_FUNCTION
}

void CStatus::ProcessEvent()
{
    __ENTER_FUNCTION
    OnEffect();
    if(GetTimes() > 0)
    {
        //再次激活
        m_info.times--;
        m_info.secs = m_pType->GetSecs();
        ScheduleEvent(GetSecs());
    }
    else
    {
        //最后一次，要销毁了
        m_pOwner->GetStatus()->DetachStatus(GetID());
    }
    __LEAVE_FUNCTION
}

void CStatus::OnAttach()
{
    __ENTER_FUNCTION
    if(HasFlag(GetFlag(), STATUSFLAG_PAUSE_ATTACH) == true)
    {
        Pause(false);
    }

    if(HasFlag(GetFlag(), STATUSFLAG_BREAK_SKILL) == true)
    {
        m_pOwner->GetSkillFSM().BreakIntone();
        m_pOwner->GetSkillFSM().BreakLaunch();
    }

    switch(m_info.id_status_type)
    {
        case STATUSTYPE_HIDE:
        {
            m_pOwner->AddHide();
        }
        break;
        default:
            break;
    }
    if(m_pType)
        m_pOwner->GetAttrib().Store(m_pType->GetAttribChangeList());
    //执行脚本
    if(m_pType && m_pType->GetScirptID() != 0)
        ScriptManager()->TryExecScript<void>(m_pType->GetScirptID(), SCB_STATUS_ONATTACH, this);

    SC_STATUS_ACTION msg;
    msg.set_actor_id(m_pOwner->GetID());
    msg.set_action(SC_STATUS_ACTION::STATUS_ATTACH);
    msg.set_statusid(GetID());
    msg.set_statustype(GetTypeID());
    msg.set_statuslev(GetLevel());
    m_pOwner->SendRoomMessage(msg);
    __LEAVE_FUNCTION
}

void CStatus::OnDeatch()
{
    __ENTER_FUNCTION
    switch(m_info.id_status_type)
    {
        case STATUSTYPE_HIDE:
        {
            m_pOwner->RemoveHide();
        }
        break;
        default:
            break;
    }
    if(m_pType)
        m_pOwner->GetAttrib().Remove(m_pType->GetAttribChangeList());
    //执行脚本
    if(m_pType && m_pType->GetScirptID() != 0)
        ScriptManager()->TryExecScript<void>(m_pType->GetScirptID(), SCB_STATUS_ONDETACH, this);

    SC_STATUS_ACTION msg;
    msg.set_actor_id(m_pOwner->GetID());
    msg.set_action(SC_STATUS_ACTION::STATUS_DETACH);
    msg.set_statusid(GetID());
    msg.set_statustype(GetTypeID());
    msg.set_statuslev(GetLevel());
    m_pOwner->SendRoomMessage(msg);
    __LEAVE_FUNCTION
}

bool CStatus::OnMove()
{
    __ENTER_FUNCTION
    //执行脚本
    bool bNeedDestory = false;
    if(m_pType && m_pType->GetScirptID() != 0)
    {
        bNeedDestory = ScriptManager()->TryExecScript<bool>(m_pType->GetScirptID(), SCB_STATUS_ONMOVE, this);
    }

    return bNeedDestory;
    __LEAVE_FUNCTION
    return false;
}

bool CStatus::OnSkill(uint32_t idSkill)
{
    __ENTER_FUNCTION
    //执行脚本
    bool bNeedDestory = false;
    if(m_pType && m_pType->GetScirptID() != 0)
    {
        bNeedDestory = ScriptManager()->TryExecScript<bool>(m_pType->GetScirptID(), SCB_STATUS_ONSKILL, this, idSkill);
    }

    return bNeedDestory;
    __LEAVE_FUNCTION
    return false;
}

bool CStatus::OnAttack(CActor* pTarget, uint32_t idSkill, int32_t nDamage)
{
    __ENTER_FUNCTION
    //执行脚本
    bool bNeedDestory = false;
    if(m_pType && m_pType->GetScirptID() != 0)
    {
        bNeedDestory = ScriptManager()->TryExecScript<bool>(m_pType->GetScirptID(), SCB_STATUS_ONATTACK, this, pTarget, idSkill, nDamage);
    }

    return bNeedDestory;

    __LEAVE_FUNCTION
    return false;
}

bool CStatus::OnBeAttack(CActor* pAttacker, int32_t nDamage)
{
    __ENTER_FUNCTION
    //执行脚本
    bool bNeedDestory = false;
    if(m_pType && m_pType->GetScirptID() != 0)
    {
        bNeedDestory = ScriptManager()->TryExecScript<bool>(m_pType->GetScirptID(), SCB_STATUS_ONBEATTACK, this, pAttacker, nDamage);
    }

    return bNeedDestory;
    __LEAVE_FUNCTION
    return false;
}

bool CStatus::OnDead(CActor* pKiller)
{
    __ENTER_FUNCTION
    bool bNeedDestory = true;
    if(m_pType && m_pType->GetScirptID() != 0)
    {
        bNeedDestory = ScriptManager()->TryExecScript<bool>(m_pType->GetScirptID(), SCB_STATUS_ONDEAD, this, pKiller);
    }

    return bNeedDestory;
    __LEAVE_FUNCTION
    return true;
}

bool CStatus::OnLeaveMap()
{
    __ENTER_FUNCTION
    //执行脚本
    bool bNeedDestory = false;
    if(m_pType && m_pType->GetScirptID() != 0)
    {
        bNeedDestory = ScriptManager()->TryExecScript<bool>(m_pType->GetScirptID(), SCB_STATUS_ONLEAVEMAP, this);
    }

    return bNeedDestory;
    __LEAVE_FUNCTION
    return false;
}

void CStatus::OnLogin()
{
    __ENTER_FUNCTION
    //执行脚本
    if(m_pType && m_pType->GetScirptID() != 0)
    {
        ScriptManager()->TryExecScript<void>(m_pType->GetScirptID(), SCB_STATUS_ONLOGIN, this);
    }

    if(IsPaused() == true)
    {
        if(HasFlag(m_info.flag, STATUSFLAG_ONLINE_RESUME))
        {
            Resume(false);
        }
    }

    ScheduleEvent(GetSecs());
    __LEAVE_FUNCTION
}

void CStatus::OnLogout()
{
    __ENTER_FUNCTION
    //执行脚本
    if(m_pType && m_pType->GetScirptID() != 0)
    {
        ScriptManager()->TryExecScript<void>(m_pType->GetScirptID(), SCB_STATUS_ONLOGOUT, this);
    }

    CancelEvent();
    __LEAVE_FUNCTION
}

void CStatus::SendStatus()
{
    __ENTER_FUNCTION
    // send room msg
    SC_STATUS_INFO status_msg;
    status_msg.set_actor_id(m_pOwner->GetID());
    auto pInfo = status_msg.add_statuslist();
    pInfo->set_statusid(GetID());
    pInfo->set_statustype(GetTypeID());
    pInfo->set_statuslev(GetLevel());
    pInfo->set_power(GetPower());
    pInfo->set_sec(GetSecs());
    pInfo->set_times(GetTimes());
    pInfo->set_laststamp(GetLastTimeStamp());
    pInfo->set_idcaster(GetCasterID());
    pInfo->set_ispause(IsPaused());
    m_pOwner->SendMsg(status_msg);

    __LEAVE_FUNCTION
}

void CStatus::OnEffect()
{
    __ENTER_FUNCTION
    //执行脚本
    if(m_pType && m_pType->GetScirptID() != 0)
    {
        ScriptManager()->TryExecScript<void>(m_pType->GetScirptID(), SCB_STATUS_ONEFFECT, this);
    }
    SC_STATUS_ACTION msg;
    msg.set_actor_id(m_pOwner->GetID());
    msg.set_action(SC_STATUS_ACTION::STATUS_EFFECT);
    msg.set_statusid(GetID());
    msg.set_statustype(GetTypeID());
    msg.set_statuslev(GetLevel());
    m_pOwner->SendRoomMessage(msg);

    __LEAVE_FUNCTION
}
