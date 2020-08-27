#include "ActorStatus.h"

#include "Actor.h"
#include "IStatus.h"
#include "ZoneService.h"
CActorStatus::CActorStatus() {}

CActorStatus::~CActorStatus()
{
    __ENTER_FUNCTION
    for(auto it = m_setStatusByType.begin(); it != m_setStatusByType.end(); it++)
    {
        SAFE_DELETE(it->second);
    }
    m_setStatusByType.clear();
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
    auto pResult = pDB->QueryT<TBLD_STATUS, TBLD_STATUS::USERID>(m_pOwner->GetID());
    if(pResult)
    {
        for(size_t i = 0; i < pResult->get_num_row(); i++)
        {
            auto     row     = pResult->fetch_row(true);
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

CStatus* CActorStatus::QueryStatusByType(uint16_t idStatusType) const
{
    __ENTER_FUNCTION
    auto itFind = m_setStatusByType.find(idStatusType);
    if(itFind == m_setStatusByType.end())
        return nullptr;
    return itFind->second;
    __LEAVE_FUNCTION
    return nullptr;
}

void CActorStatus::_AddStatus(CStatus* pStatus)
{
    __ENTER_FUNCTION
    m_setStatusByType[pStatus->GetTypeID()] = pStatus;
    __LEAVE_FUNCTION
}

void CActorStatus::_RemoveStatus(CStatus* pStatus)
{
    __ENTER_FUNCTION
    m_setStatusByType.erase(pStatus->GetTypeID());
    __LEAVE_FUNCTION
}

void CActorStatus::Stop()
{
    __ENTER_FUNCTION
    for(auto it = m_setStatusByType.begin(); it != m_setStatusByType.end(); it++)
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
    for(auto it = m_setStatusByType.begin(); it != m_setStatusByType.end(); it++)
    {
        CStatus* pStatus = it->second;
        auto     pInfo   = msg.add_statuslist();
        pInfo->set_statusid(pStatus->GetID());
        pInfo->set_statustype(pStatus->GetTypeID());
        pInfo->set_statuslev(pStatus->GetLevel());
        pInfo->set_power(pStatus->GetPower());
        pInfo->set_sec(pStatus->GetSecs());
        pInfo->set_times(pStatus->GetTimes());
        pInfo->set_laststamp(pStatus->GetLastTimeStamp());
        pInfo->set_idcaster(pStatus->GetCasterID());
        pInfo->set_ispause(pStatus->IsPaused());
    }

    pActor->SendMsg(msg);
    __LEAVE_FUNCTION
}

void CActorStatus::FillStatusMsg(SC_STATUS_LIST& status_msg)
{
    __ENTER_FUNCTION
    for(auto it = m_setStatusByType.begin(); it != m_setStatusByType.end(); it++)
    {
        CStatus* pStatus = it->second;
        auto     pInfo   = status_msg.add_status_list();
        pInfo->set_statusid(pStatus->GetID());
        pInfo->set_statustype(pStatus->GetTypeID());
        pInfo->set_statuslev(pStatus->GetLevel());
        pInfo->set_power(pStatus->GetPower());
        pInfo->set_idcaster(pStatus->GetCasterID());
    }
    __LEAVE_FUNCTION
}

bool CActorStatus::AttachStatus(const AttachStatusInfo& info)
{
    __ENTER_FUNCTION
    CStatus* pStatus = QueryStatusByType(info.id_status_type);
    if(pStatus)
    {
        // STATUSFLAG_OVERRIDE_LEV		= 0x0001,		//高等级可以覆盖低等级
        // STATUSFLAG_OVERLAP = 0x0002,		//允许叠加， 时间形的叠加时间， 数值型的叠加数值
        if(HasFlag(info.flag, STATUSFLAG_OVERLAP))
        {
            if(info.expire_type == STATUSEXPIRETYPE_TIME)
            {
                if(info.times > 0)
                {
                    //叠加次数
                    pStatus->AddTimes(info.times);
                }
                else
                {
                    //叠加时间
                    pStatus->AddSecs(info.secs);
                }
            }
            else if(info.expire_type == STATUSEXPIRETYPE_POINT)
            {
                //叠加数值
                pStatus->SetPower(pStatus->GetPower() + info.power);
            }
        }
        //检查是否可以覆盖
        else if(HasFlag(info.flag, STATUSFLAG_OVERRIDE_LEV) && info.lev >= pStatus->GetLevel())
        {
            pStatus->ChangeData(info);
        }
        else
        {
            return false;
        }
    }
    else
    {
        //创建一个新的
        pStatus = CStatus::CreateNew(m_pOwner, info);
        _AddStatus(pStatus);
    }

    pStatus->OnAttach();
    pStatus->SendStatus();
    if(pStatus->Type() && pStatus->Type()->GetAttribChangeList().empty() == false)
    {
        m_pOwner->RecalcAttrib(false);
    }

    return true;
    __LEAVE_FUNCTION
    return false;
}


bool CActorStatus::AttachStatus(uint32_t idStatus, OBJID idCaster)
{
    __ENTER_FUNCTION
    const CStatusType* pType = StatusTypeSet()->QueryObj(idStatus);
    CHECKF(pType);
    AttachStatus(pType->CloneInfo());

    return true;
    __LEAVE_FUNCTION
    return false;
}


bool CActorStatus::DetachStatus(uint16_t idStatusType)
{
    __ENTER_FUNCTION
    auto itFind = m_setStatusByType.find(idStatusType);
    if(itFind == m_setStatusByType.end())
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
    for(auto it = m_setStatusByType.begin(); it != m_setStatusByType.end();)
    {
        CStatus* pStatus = it->second;
        if(pStatus->GetTypeID() == nStatusType)
        {
            pStatus->OnDeatch();
            SAFE_DELETE(pStatus);
            it = m_setStatusByType.erase(it);
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
    for(auto it = m_setStatusByType.begin(); it != m_setStatusByType.end();)
    {
        CStatus* pStatus = it->second;
        if(HasFlag(pStatus->GetFlag(), nStatusFlag) == bHave)
        {
            pStatus->OnDeatch();
            SAFE_DELETE(pStatus);
            it = m_setStatusByType.erase(it);
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
    auto it = std::find_if(m_setStatusByType.begin(), m_setStatusByType.end(), [nStatusType](const auto& pair_val) -> bool {
        return pair_val.second->GetTypeID() == nStatusType;
    });
    return it != m_setStatusByType.end();
    __LEAVE_FUNCTION
    return false;
}

bool CActorStatus::TestStatusByFlag(uint32_t nFlag) const
{
    __ENTER_FUNCTION
    auto it = std::find_if(m_setStatusByType.begin(), m_setStatusByType.end(), [nFlag](const auto& pair_val) -> bool {
        return HasFlag(pair_val.second->GetFlag(), nFlag);
    });
    return it != m_setStatusByType.end();
    __LEAVE_FUNCTION
    return false;
}

void CActorStatus::OnMove()
{
    __ENTER_FUNCTION
    OnEventDetach(&CStatus::OnMove, STATUSFLAG_DEATCH_MOVE);
    __LEAVE_FUNCTION
}

void CActorStatus::OnSkill(uint32_t idSkill)
{
    __ENTER_FUNCTION
    OnEventDetach(&CStatus::OnSkill, STATUSFLAG_DEATCH_SKILL, idSkill);
    __LEAVE_FUNCTION
}

void CActorStatus::OnAttack(CActor* pTarget, uint32_t idSkill, int32_t nDamage)
{
    __ENTER_FUNCTION
    OnEventDetach(&CStatus::OnAttack, STATUSFLAG_DEATCH_ATTACK, pTarget, idSkill, nDamage);
    __LEAVE_FUNCTION
}

void CActorStatus::OnDead(CActor* pKiller)
{
    __ENTER_FUNCTION
    OnEventDeatchExcept(&CStatus::OnDead, STATUSFLAG_EXCEPT_DEATCH_DEAD, pKiller);
    __LEAVE_FUNCTION
}

void CActorStatus::OnBeAttack(CActor* pAttacker, int32_t nDamage)
{
    __ENTER_FUNCTION
    OnEventDetach(&CStatus::OnBeAttack, STATUSFLAG_DEATCH_BEATTACK, pAttacker, nDamage);
    __LEAVE_FUNCTION
}

void CActorStatus::OnLeaveMap()
{
    __ENTER_FUNCTION
    OnEventDetach(&CStatus::OnLeaveMap, STATUSFLAG_DEATCH_LEAVEMAP);
    __LEAVE_FUNCTION
}

void CActorStatus::OnLogin()
{
    __ENTER_FUNCTION
    for(auto it = m_setStatusByType.begin(); it != m_setStatusByType.end(); it++)
    {
        CStatus* pStatus = it->second;
        pStatus->OnLogin();
    }
    __LEAVE_FUNCTION
}

void CActorStatus::OnLogout()
{
    __ENTER_FUNCTION
    for(auto it = m_setStatusByType.begin(); it != m_setStatusByType.end();)
    {
        CStatus* pStatus = it->second;
        if(HasFlag(pStatus->GetFlag(), STATUSFLAG_DEATCH_OFFLINE) == true)
        {
            pStatus->OnDeatch();
            SAFE_DELETE(pStatus);
            it = m_setStatusByType.erase(it);
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
    for(auto it = m_setStatusByType.begin(); it != m_setStatusByType.end(); it++)
    {
        CStatus* pStatus = it->second;
        pStatus->SaveInfo();
    }
    __LEAVE_FUNCTION
}
