#include "IStatus.h"

#include "Actor.h"
#include "ZoneService.h"
#include "gamedb.h"

MEMORYHEAP_IMPLEMENTATION(CStatus, s_heap);
CStatus::CStatus() {}

CStatus::~CStatus() {}

bool CStatus::Init(CActor* pOwner, uint16_t idStatusType, UCHAR ucLevel, OBJID idCaster, uint32_t nPower, uint32_t nSecs, uint32_t nTimes)
{
	__ENTER_FUNCTION
	CStatusType* pStatusType = StatusTypeSet()->QueryObj(CStatusType::MakeID(idStatusType, ucLevel));
	CHECKF(pStatusType);
	m_pType = pStatusType;

	m_pOwner			= pOwner;
	m_info.idStatusType = idStatusType;
	m_info.ucLevel		= nPower;
	m_info.nPower		= nPower;
	m_info.nSecs		= nSecs;
	m_info.nTimes		= nTimes;
	if(pStatusType->GetExpireType() == STATUSEXPIRETYPE_TIME)
		m_info.tLastStamp = TimeGetSecond();
	else
		m_info.tLastStamp = 0;

	m_info.idCaster = idCaster;
	m_info.bPause	= bit_test(pStatusType->GetFlag(), STATUSFLAG_PAUSE_ATTACH) ? TRUE : FALSE;
	return true;
	__LEAVE_FUNCTION
	return false;
}

bool CStatus::Init(CActor* pOwner, CDBRecordPtr&& pRow)
{
	__ENTER_FUNCTION
	uint32_t idStatusType = pRow->Field(TBLD_STATUS::TYPEID);
	uint32_t ucLevel	  = pRow->Field(TBLD_STATUS::LEV);

	CStatusType* pStatusType = StatusTypeSet()->QueryObj(CStatusType::MakeID(idStatusType, ucLevel));
	CHECKF(pStatusType);
	m_pType = pStatusType;

	m_pOwner = pOwner;
	m_pRecord.reset(pRow.release());

	m_info.idStatusType = idStatusType;
	m_info.ucLevel		= ucLevel;
	m_info.nPower		= pRow->Field(TBLD_STATUS::POWER);
	m_info.nSecs		= pRow->Field(TBLD_STATUS::SECS);
	m_info.nTimes		= pRow->Field(TBLD_STATUS::TIMES);
	m_info.tLastStamp	= pRow->Field(TBLD_STATUS::LASTSTAMP);

	m_info.idCaster = pRow->Field(TBLD_STATUS::CASTERID);
	m_info.bPause	= pRow->Field(TBLD_STATUS::PAUSE);

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
	if(m_pType->GetExpireType() == STATUSEXPIRETYPE_TIME)
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
	else if(m_pType->GetExpireType() == STATUSEXPIRETYPE_POINT)
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

int CStatus::GetRemainTime() const
{
	__ENTER_FUNCTION
	if(GetTimes() > 0)
	{
		time_t now		 = TimeGetSecond();
		time_t nextStamp = GetLastTimeStamp() + GetSecs();
		if(nextStamp > now)
			return GetTimes() * GetSecs() + (nextStamp - now);
		else
			return GetTimes() * GetSecs();
	}
	else
	{
		time_t now		 = TimeGetSecond();
		time_t nextStamp = GetLastTimeStamp() + GetSecs();
		if(nextStamp > now)
			return nextStamp - now;
		return 0;
	}
	__LEAVE_FUNCTION
	return 0;
}

void CStatus::AddSecs(int nSecs)
{
	__ENTER_FUNCTION
	Pause(false);

	if(m_pType->GetMaxSecs() - m_info.nSecs > nSecs)
	{
		m_info.nSecs = m_pType->GetMaxSecs();
	}
	else
	{
		m_info.nSecs += nSecs;
	}
	Resume(false);
	__LEAVE_FUNCTION
}

void CStatus::AddTimes(int nTimes)
{
	__ENTER_FUNCTION
	if(m_pType->GetMaxTimes() - m_info.nTimes > nTimes)
	{
		m_info.nTimes = m_pType->GetMaxTimes();
	}
	else
	{
		m_info.nTimes += nTimes;
	}
	__LEAVE_FUNCTION
}

bool CStatus::ChangeData(UCHAR ucLevel, int nPower, int nSecs, int nTimes, OBJID idCaster)
{
	__ENTER_FUNCTION
	m_info.ucLevel	= ucLevel;
	m_info.nPower	= nPower;
	m_info.nSecs	= nSecs;
	m_info.nTimes	= nTimes;
	m_info.idCaster = idCaster;
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

	if(m_info.tLastStamp > 0)
	{
		time_t now		 = TimeGetSecond();
		time_t nextStamp = GetLastTimeStamp() + GetSecs();
		if(nextStamp > now)
		{
			m_info.nSecs = nextStamp - now;
		}
		m_info.tLastStamp = 0;
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

	if(m_info.tLastStamp == 0)
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
		m_pRecord->Field(TBLD_STATUS::LEV)		 = m_info.ucLevel;
		m_pRecord->Field(TBLD_STATUS::POWER)	 = m_info.nPower;
		m_pRecord->Field(TBLD_STATUS::SECS)		 = m_info.nSecs;
		m_pRecord->Field(TBLD_STATUS::TIMES)	 = m_info.nTimes;
		m_pRecord->Field(TBLD_STATUS::LASTSTAMP) = m_info.tLastStamp;
		m_pRecord->Field(TBLD_STATUS::CASTERID)	 = m_info.idCaster;
		m_pRecord->Field(TBLD_STATUS::PAUSE)	 = m_info.bPause;
	}
	else
	{
		//创建
		m_pRecord								 = ZoneService()->GetGameDB(m_pOwner->GetWorldID())->MakeRecord(TBLD_STATUS::table_name);
		m_pRecord->Field(TBLD_STATUS::ID)		 = ZoneService()->CreateUID();
		m_pRecord->Field(TBLD_STATUS::USERID)	 = m_pOwner->GetID();
		m_pRecord->Field(TBLD_STATUS::TYPEID)	 = m_info.idStatusType;
		m_pRecord->Field(TBLD_STATUS::LEV)		 = m_info.ucLevel;
		m_pRecord->Field(TBLD_STATUS::POWER)	 = m_info.nPower;
		m_pRecord->Field(TBLD_STATUS::SECS)		 = m_info.nSecs;
		m_pRecord->Field(TBLD_STATUS::TIMES)	 = m_info.nTimes;
		m_pRecord->Field(TBLD_STATUS::LASTSTAMP) = m_info.tLastStamp;
		m_pRecord->Field(TBLD_STATUS::CASTERID)	 = m_info.idCaster;
		m_pRecord->Field(TBLD_STATUS::PAUSE)	 = m_info.bPause;
	}
	CHECK(m_pRecord->Update());
	__LEAVE_FUNCTION
}

bool CStatus::ScheduleEvent(time_t tIntervalMS /*= 0*/)
{
	__ENTER_FUNCTION
	m_info.tLastStamp = TimeGetSecond();
	return EventManager()->ScheduleEvent(0, std::bind(&CStatus::ProcessEvent, this), tIntervalMS, false, m_StatusEvent);
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
		m_info.nTimes--;
		m_info.nSecs = m_pType->GetSecs();
		ScheduleEvent(GetSecs());
	}
	else
	{
		//最后一次，要销毁了
		m_pOwner->GetStatus()->DetachStatus(GetStatusTypeID());
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

	switch(m_pType->GetType())
	{
		case STATUSTYPE_HIDE:
		{
			m_pOwner->AddHide();
		}
		break;
		default:
			break;
	}

	//执行脚本
	if(m_pType->GetScirptID() != 0)
		ScriptManager()->TryExecScript<void>(m_pType->GetScirptID(), SCB_STATUS_ONATTACH, this);

	SC_STATUS_ACTION msg;
	msg.set_actor_id(m_pOwner->GetID());
	msg.set_action(SC_STATUS_ACTION::STATUS_ATTACH);
	msg.set_statusid(GetStatusTypeID());
	msg.set_statuslev(GetLevel());
	m_pOwner->SendRoomMessage(CMD_SC_STATUS_ACTION, msg);
	__LEAVE_FUNCTION
}

void CStatus::OnDeatch()
{
	__ENTER_FUNCTION
	switch(m_pType->GetType())
	{
		case STATUSTYPE_HIDE:
		{
			m_pOwner->RemoveHide();
		}
		break;
		default:
			break;
	}

	//执行脚本
	if(m_pType->GetScirptID() != 0)
		ScriptManager()->TryExecScript<void>(m_pType->GetScirptID(), SCB_STATUS_ONDETACH, this);

	SC_STATUS_ACTION msg;
	msg.set_actor_id(m_pOwner->GetID());
	msg.set_action(SC_STATUS_ACTION::STATUS_DETACH);
	msg.set_statusid(GetStatusTypeID());
	msg.set_statuslev(GetLevel());
	m_pOwner->SendRoomMessage(CMD_SC_STATUS_ACTION, msg);
	__LEAVE_FUNCTION
}

bool CStatus::OnMove()
{
	__ENTER_FUNCTION
	//执行脚本
	bool bNeedDestory = false;
	if(m_pType->GetScirptID() != 0)
		bNeedDestory = ScriptManager()->TryExecScript<bool>(m_pType->GetScirptID(), SCB_STATUS_ONMOVE, this);

	return bNeedDestory;
	__LEAVE_FUNCTION
	return false;
}

bool CStatus::OnSkill(uint32_t idSkill)
{
	__ENTER_FUNCTION
	//执行脚本
	bool bNeedDestory = false;
	if(m_pType->GetScirptID() != 0)
		bNeedDestory = ScriptManager()->TryExecScript<bool>(m_pType->GetScirptID(), SCB_STATUS_ONSKILL, this, idSkill);

	return bNeedDestory;
	__LEAVE_FUNCTION
	return false;
}

bool CStatus::OnAttack(CActor* pTarget, uint32_t idSkill, int32_t nDamage)
{
	__ENTER_FUNCTION
	//执行脚本
	bool bNeedDestory = false;
	if(m_pType->GetScirptID() != 0)
		bNeedDestory = ScriptManager()->TryExecScript<bool>(m_pType->GetScirptID(), SCB_STATUS_ONATTACK, this, pTarget, idSkill, nDamage);

	return bNeedDestory;

	__LEAVE_FUNCTION
	return false;
}

bool CStatus::OnBeAttack(CActor* pAttacker, int32_t nDamage)
{
	__ENTER_FUNCTION
	//执行脚本
	bool bNeedDestory = false;
	if(m_pType->GetScirptID() != 0)
		bNeedDestory = ScriptManager()->TryExecScript<bool>(m_pType->GetScirptID(), SCB_STATUS_ONBEATTACK, this, pAttacker, nDamage);

	return bNeedDestory;
	__LEAVE_FUNCTION
	return false;
}

bool CStatus::OnDead(CActor* pKiller)
{
	__ENTER_FUNCTION
	bool bNeedDestory = false;
	if(m_pType->GetScirptID() != 0)
		bNeedDestory = ScriptManager()->TryExecScript<bool>(m_pType->GetScirptID(), SCB_STATUS_ONDEAD, this, pKiller);

	return bNeedDestory;
	__LEAVE_FUNCTION
	return false;
}

bool CStatus::OnLeaveMap()
{
	__ENTER_FUNCTION
	//执行脚本
	bool bNeedDestory = false;
	if(m_pType->GetScirptID() != 0)
		bNeedDestory = ScriptManager()->TryExecScript<bool>(m_pType->GetScirptID(), SCB_STATUS_ONLEAVEMAP, this);

	return bNeedDestory;
	__LEAVE_FUNCTION
	return false;
}

void CStatus::OnLogin()
{
	__ENTER_FUNCTION
	//执行脚本
	if(m_pType->GetScirptID() != 0)
		ScriptManager()->TryExecScript<void>(m_pType->GetScirptID(), SCB_STATUS_ONLOGIN, this);

	if(IsPaused() == true)
	{
		if(HasFlag(m_pType->GetFlag(), STATUSFLAG_ONLINE_RESUME))
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
	if(m_pType->GetScirptID() != 0)
		ScriptManager()->TryExecScript<void>(m_pType->GetScirptID(), SCB_STATUS_ONLOGOUT, this);

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
	pInfo->set_statusid(GetStatusTypeID());
	pInfo->set_statuslev(GetLevel());
	pInfo->set_power(GetPower());
	pInfo->set_sec(GetSecs());
	pInfo->set_times(GetTimes());
	pInfo->set_laststamp(GetLastTimeStamp());
	pInfo->set_idcaster(GetCasterID());
	pInfo->set_ispause(IsPaused());
	m_pOwner->SendMessage(CMD_SC_STATUS_INFO, status_msg);

	__LEAVE_FUNCTION
}

void CStatus::OnEffect()
{
	__ENTER_FUNCTION
	//执行脚本
	if(m_pType->GetScirptID() != 0)
		ScriptManager()->TryExecScript<void>(m_pType->GetScirptID(), SCB_STATUS_ONEFFECT, this);

	SC_STATUS_ACTION msg;
	msg.set_actor_id(m_pOwner->GetID());
	msg.set_action(SC_STATUS_ACTION::STATUS_EFFECT);
	msg.set_statusid(GetStatusTypeID());
	msg.set_statuslev(GetLevel());
	m_pOwner->SendRoomMessage(CMD_SC_STATUS_ACTION, msg);

	__LEAVE_FUNCTION
}
