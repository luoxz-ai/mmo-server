#include "ActorStatus.h"

#include "Actor.h"
#include "IStatus.h"
#include "ZoneService.h"
CActorStatus::CActorStatus() {}

CActorStatus::~CActorStatus()
{
	__ENTER_FUNCTION
	for(auto it = m_setStatus.begin(); it != m_setStatus.end(); it++)
	{
		SAFE_DELETE(it->second);
	}
	m_setStatus.clear();
	__LEAVE_FUNCTION
}

bool CActorStatus::Init(CActor* pActor)
{
	__ENTER_FUNCTION
	CHECKF(pActor);
	m_pOwner = pActor;
	if(pActor->IsPlayer() == false)
		return true;
	auto pDB = ZoneService()->GetGameDB(m_pOwner->GetWorldID());
	CHECKF(pDB);
	auto pResult = pDB->Query(TBLD_STATUS::table_name, fmt::format(FMT_STRING("SELECT * FROM {} WHERE userid={}"), TBLD_STATUS::table_name, m_pOwner->GetID()));
	if(pResult)
	{
		for(size_t i = 0; i < pResult->get_num_row(); i++)
		{
			auto	 row	 = pResult->fetch_row(true);
			CStatus* pStatus = CStatus::CreateNew(m_pOwner, std::move(row));
			if(pStatus)
			{
				_AddStatus(pStatus);
			}
			else
			{
				// logerror
				LOGERROR("LoadStatus Error, User:{}", m_pOwner->GetID());
			}
		}
	}
	return true;
	__LEAVE_FUNCTION
	return false;
}

CStatus* CActorStatus::QueryStatus(uint16_t idStatus) const
{
	__ENTER_FUNCTION
	auto itFind = m_setStatus.find(idStatus);
	if(itFind == m_setStatus.end())
		return nullptr;
	return itFind->second;
	__LEAVE_FUNCTION
	return nullptr;
}

void CActorStatus::_AddStatus(CStatus* pStatus)
{
	__ENTER_FUNCTION
	m_setStatus[pStatus->GetStatusTypeID()] = pStatus;
	__LEAVE_FUNCTION
}

void CActorStatus::_RemoveStatus(CStatus* pStatus)
{
	__ENTER_FUNCTION
	m_setStatus.erase(pStatus->GetStatusTypeID());
	__LEAVE_FUNCTION
}

void CActorStatus::Stop()
{
	__ENTER_FUNCTION
	for(auto it = m_setStatus.begin(); it != m_setStatus.end(); it++)
	{
		CStatus* pStatus = it->second;
		pStatus->ClearEvent();
	}
	__LEAVE_FUNCTION
}

void CActorStatus::SyncTo(CActor* pActor)
{
	__ENTER_FUNCTION
	CHECK(pActor);
	SC_STATUS_INFO msg;
	msg.set_actor_id(m_pOwner->GetID());
	for(auto it = m_setStatus.begin(); it != m_setStatus.end(); it++)
	{
		CStatus* pStatus = it->second;
		auto	 pInfo	 = msg.add_statuslist();
		pInfo->set_statusid(pStatus->GetStatusTypeID());
		pInfo->set_statuslev(pStatus->GetLevel());
		pInfo->set_power(pStatus->GetPower());
		pInfo->set_sec(pStatus->GetSecs());
		pInfo->set_times(pStatus->GetTimes());
		pInfo->set_laststamp(pStatus->GetLastTimeStamp());
		pInfo->set_idcaster(pStatus->GetCasterID());
		pInfo->set_ispause(pStatus->IsPaused());
	}

	pActor->SendMessage(CMD_SC_STATUS_INFO, msg);
	__LEAVE_FUNCTION
}

void CActorStatus::FillStatusMsg(SC_STATUS_LIST& status_msg)
{
	__ENTER_FUNCTION
	for(auto it = m_setStatus.begin(); it != m_setStatus.end(); it++)
	{
		CStatus* pStatus = it->second;
		status_msg.add_status_typeid_list(pStatus->GetStatusTypeID());
		status_msg.add_status_lev_list(pStatus->GetLevel());
	}
	__LEAVE_FUNCTION
}

bool CActorStatus::AttachStatus(uint16_t idStatusType, UCHAR ucLev, OBJID idCaster, uint32_t nPower, uint32_t nSecs, uint32_t nTimes)
{
	__ENTER_FUNCTION
	CStatusType* pNewStatusType = StatusTypeSet()->QueryObj(CStatusType::MakeID(idStatusType, ucLev));
	CHECKF(pNewStatusType);
	CStatus* pStatus = QueryStatus(idStatusType);
	if(pStatus)
	{
		// STATUSFLAG_OVERRIDE_LEV		= 0x0001,		//高等级可以覆盖低等级
		// STATUSFLAG_OVERLAP = 0x0002,		//允许叠加， 时间形的叠加时间， 数值型的叠加数值
		if(HasFlag(pNewStatusType->GetFlag(), STATUSFLAG_OVERLAP))
		{
			if(pNewStatusType->GetExpireType() == STATUSEXPIRETYPE_TIME)
			{
				if(pNewStatusType->GetTimes() > 0)
				{
					//叠加次数
					pStatus->AddTimes(pNewStatusType->GetTimes());
				}
				else
				{
					//叠加时间
					pStatus->AddSecs(pNewStatusType->GetSecs());
				}
			}
			else if(pNewStatusType->GetExpireType() == STATUSEXPIRETYPE_POINT)
			{
				//叠加数值
				pStatus->SetPower(pStatus->GetPower() + nPower);
			}
		}
		//检查是否可以覆盖
		else if(HasFlag(pNewStatusType->GetFlag(), STATUSFLAG_OVERRIDE_LEV) && ucLev >= pStatus->GetLevel())
		{
			pStatus->ChangeData(ucLev, nPower, nSecs, nTimes, idCaster);
		}
		else
		{
			return false;
		}
	}
	else
	{
		//创建一个新的
		pStatus = CStatus::CreateNew(m_pOwner, idStatusType, ucLev, idCaster, nPower, nSecs, nTimes);
		_AddStatus(pStatus);
	}

	pStatus->OnAttach();
	pStatus->SendStatus();
	if(pStatus->Type()->GetAttribChangeList().empty() == false)
	{
		m_pOwner->RecalcAttrib(false);
	}

	return true;
	__LEAVE_FUNCTION
	return false;
}

bool CActorStatus::DetachStatus(uint16_t idStatusType)
{
	__ENTER_FUNCTION
	auto itFind = m_setStatus.find(idStatusType);
	if(itFind == m_setStatus.end())
		return false;

	CStatus* pStatus = itFind->second;
	pStatus->OnDeatch();
	_RemoveStatus(pStatus);

	SAFE_DELETE(pStatus);
	return true;
	__LEAVE_FUNCTION
	return false;
}

bool CActorStatus::DetachStatusByType(uint32_t nStatusType)
{
	__ENTER_FUNCTION
	for(auto it = m_setStatus.begin(); it != m_setStatus.end();)
	{
		CStatus* pStatus = it->second;
		if(pStatus->GetType() == nStatusType)
		{
			pStatus->OnDeatch();
			SAFE_DELETE(pStatus);
			it = m_setStatus.erase(it);
		}
		else
		{
			it++;
		}
	}
	return true;
	__LEAVE_FUNCTION
	return false;
}

bool CActorStatus::DetachStatusByFlag(uint32_t nStatusFlag, bool bHave)
{
	__ENTER_FUNCTION
	for(auto it = m_setStatus.begin(); it != m_setStatus.end();)
	{
		CStatus* pStatus = it->second;
		if(HasFlag(pStatus->GetFlag(), nStatusFlag) == bHave)
		{
			pStatus->OnDeatch();
			SAFE_DELETE(pStatus);
			it = m_setStatus.erase(it);
		}
		else
		{
			it++;
		}
	}
	return true;
	__LEAVE_FUNCTION
	return false;
}

bool CActorStatus::TestStatusByType(uint32_t nStatusType) const
{
	__ENTER_FUNCTION
	auto it = std::find_if(m_setStatus.begin(), m_setStatus.end(),
						   [nStatusType](const std::pair<const short unsigned int, CStatus*>& pair_val) -> bool { return pair_val.second->GetType() == nStatusType; });
	return it != m_setStatus.end();
	__LEAVE_FUNCTION
	return false;
}

bool CActorStatus::TestStatusByFlag(uint32_t nFlag) const
{
	__ENTER_FUNCTION
	auto it = std::find_if(m_setStatus.begin(), m_setStatus.end(),
						   [nFlag](const std::pair<const short unsigned int, CStatus*>& pair_val) -> bool { return HasFlag(pair_val.second->GetFlag(), nFlag); });
	return it != m_setStatus.end();
	__LEAVE_FUNCTION
	return false;
}

void CActorStatus::OnMove()
{
	__ENTER_FUNCTION
	for(auto it = m_setStatus.begin(); it != m_setStatus.end();)
	{
		CStatus* pStatus = it->second;
		if(HasFlag(pStatus->GetFlag(), STATUSFLAG_DEATCH_MOVE) == true || it->second->OnMove())
		{
			pStatus->OnDeatch();
			SAFE_DELETE(pStatus);
			it = m_setStatus.erase(it);
		}
		else
		{
			it++;
		}
	}
	__LEAVE_FUNCTION
}

void CActorStatus::OnSkill(uint32_t idSkill)
{
	__ENTER_FUNCTION
	for(auto it = m_setStatus.begin(); it != m_setStatus.end();)
	{
		CStatus* pStatus = it->second;
		if(HasFlag(pStatus->GetFlag(), STATUSFLAG_DEATCH_SKILL) == true || it->second->OnSkill(idSkill))
		{
			pStatus->OnDeatch();
			SAFE_DELETE(pStatus);
			it = m_setStatus.erase(it);
		}
		else
		{
			it++;
		}
	}
	__LEAVE_FUNCTION
}

void CActorStatus::OnAttack(CActor* pTarget, uint32_t idSkill, int32_t nDamage)
{
	__ENTER_FUNCTION
	for(auto it = m_setStatus.begin(); it != m_setStatus.end();)
	{
		CStatus* pStatus = it->second;
		if(it->second->OnAttack(pTarget, idSkill, nDamage))
		{
			pStatus->OnDeatch();
			SAFE_DELETE(pStatus);
			it = m_setStatus.erase(it);
		}
		else
		{
			it++;
		}
	}
	__LEAVE_FUNCTION
}

void CActorStatus::OnDead(CActor* pKiller)
{
	__ENTER_FUNCTION
	for(auto it = m_setStatus.begin(); it != m_setStatus.end();)
	{
		CStatus* pStatus = it->second;
		if(HasFlag(pStatus->GetFlag(), STATUSFLAG_UNDEATCH_DEAD) == false || it->second->OnDead(pKiller))
		{
			pStatus->OnDeatch();
			SAFE_DELETE(pStatus);
			it = m_setStatus.erase(it);
		}
		else
		{
			it++;
		}
	}
	__LEAVE_FUNCTION
}

void CActorStatus::OnBeAttack(CActor* pAttacker, int32_t nDamage)
{
	__ENTER_FUNCTION
	for(auto it = m_setStatus.begin(); it != m_setStatus.end();)
	{
		CStatus* pStatus = it->second;
		if(HasFlag(pStatus->GetFlag(), STATUSFLAG_DEATCH_BEATTACK) == true || it->second->OnBeAttack(pAttacker, nDamage))
		{
			pStatus->OnDeatch();
			SAFE_DELETE(pStatus);
			it = m_setStatus.erase(it);
		}
		else
		{
			it++;
		}
	}
	__LEAVE_FUNCTION
}

void CActorStatus::OnLeaveMap()
{
	__ENTER_FUNCTION
	for(auto it = m_setStatus.begin(); it != m_setStatus.end();)
	{
		CStatus* pStatus = it->second;
		if(HasFlag(pStatus->GetFlag(), STATUSFLAG_DEATCH_LEAVEMAP) == true || it->second->OnLeaveMap())
		{
			pStatus->OnDeatch();
			SAFE_DELETE(pStatus);
			it = m_setStatus.erase(it);
		}
		else
		{
			it++;
		}
	}

	__LEAVE_FUNCTION
}

void CActorStatus::OnLogin()
{
	__ENTER_FUNCTION
	for(auto it = m_setStatus.begin(); it != m_setStatus.end(); it++)
	{
		CStatus* pStatus = it->second;
		pStatus->OnLogin();
	}
	__LEAVE_FUNCTION
}

void CActorStatus::OnLogout()
{
	__ENTER_FUNCTION
	for(auto it = m_setStatus.begin(); it != m_setStatus.end();)
	{
		CStatus* pStatus = it->second;
		if(HasFlag(pStatus->GetFlag(), STATUSFLAG_DEATCH_OFFLINE) == true)
		{
			pStatus->OnDeatch();
			SAFE_DELETE(pStatus);
			it = m_setStatus.erase(it);
		}
		else
		{
			pStatus->OnLogout();
			it++;
		}
	}
	__LEAVE_FUNCTION
}

void CActorStatus::SaveInfo()
{
	__ENTER_FUNCTION
	for(auto it = m_setStatus.begin(); it != m_setStatus.end(); it++)
	{
		CStatus* pStatus = it->second;
		pStatus->SaveInfo();
	}
	__LEAVE_FUNCTION
}

void CActorStatus::ProcessAttrib(CActorAttrib& attrib)
{
	__ENTER_FUNCTION
	CActorAttribCalc calc;
	for(auto it = m_setStatus.begin(); it != m_setStatus.end(); it++)
	{
		CStatus*	pStatus = it->second;
		const auto& refList = pStatus->Type()->GetAttribChangeList();
		for(const auto& v: refList)
		{
			calc += v;
		}
	}
	attrib.StoreTmp(calc);
	__LEAVE_FUNCTION
}