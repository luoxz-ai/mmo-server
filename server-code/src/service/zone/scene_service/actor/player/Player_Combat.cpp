#include "ActorManager.h"
#include "Phase.h"
#include "Player.h"

bool CPlayer::CanDamage(CActor* pTarget) const
{
    __ENTER_FUNCTION
    CHECKF(pTarget);
    CHECKF(pTarget->GetCurrentScene());
    CHECKF(pTarget->GetCurrentScene() == GetCurrentScene());

    if(this == pTarget)
        return false;

    CPlayer* pTargetPlayer = nullptr;
    if(pTarget->IsMonster())
    {
        if(pTarget->GetOwnerID() != 0)
            pTargetPlayer = ActorManager()->QueryPlayer(pTarget->GetOwnerID());
        else
            return true;
    }
    else if(pTarget->IsPet())
    {
        pTargetPlayer = ActorManager()->QueryPlayer(pTarget->GetOwnerID());
    }
    if(pTarget->IsPlayer())
    {
        pTargetPlayer = pTarget->CastTo<CPlayer>();
    }

    if(pTargetPlayer)
    {
        //根据pk规则
        if(GetCurrentScene()->GetMap()->IsPvPDisable(pTarget->GetPos().x, pTarget->GetPos().y) == true)
            return false;
        if(GetCurrentScene()->GetMap()->IsPvPDisable(GetPos().x, GetPos().y) == true)
            return false;

        //新手保护
        constexpr uint32_t NEWIBLE_PK_PROTECTED_LEV = 40;
        if(GetCurrentScene()->GetMap()->HasMapFlag(MAPFLAG_DISABLE_PKPROTECTED) == false && pTargetPlayer->GetLev() < NEWIBLE_PK_PROTECTED_LEV)
            return false;

        switch(GetPKMode())
        {
            case PKMODE_PEACE:
            {
                return false;
            }
            break;
            case PKMODE_REDNAME:
            {
                return pTargetPlayer->GetStatus()->TestStatusByType(STATUSTYPE_CRIME) || pTargetPlayer->GetPKVal() >= RED_NAME_PKVAL;
            }
            break;
            case PKMODE_TEAM:
            {
                return (GetTeamID() == 0 || pTargetPlayer->GetTeamID() != GetTeamID());
            }
            break;
            case PKMODE_GUILD:
            {
                return (GetGuildID() == 0 || pTargetPlayer->GetGuildID() != GetGuildID());
            }
            break;
            case PKMODE_CAMP:
            {
                return (pTargetPlayer->GetCampID() != GetCampID());
            }
            break;
            case PKMODE_ALL:
            {
                return true;
            }
            break;
        }
    }

    return true;
    __LEAVE_FUNCTION
    return false;
}

void CPlayer::BeKillBy(CActor* pAttacker)
{
    CActor::BeKillBy(pAttacker);

    if(pAttacker && pAttacker->IsPlayer())
    {
        CPlayer* pPlayer = pAttacker->CastTo<CPlayer>();
        if(GetCurrentScene()->IsPvPFree(GetPos().x, GetPos().y) == false)
        {
            //计算PK值
            pPlayer->GetDataCountSet()->AddCount(DATA_ACC_SYSTEM, DATA_ACC_SYSTEM_KILLPLAYER, 1, UPDATE_FALSE);
            pPlayer->AddProperty(PROP_PKVAL, ADD_PKVAL_PERKILL, SYNC_ALL_DELAY);
        }
    }

    GetDataCountSet()->AddCount(DATA_ACC_SYSTEM, DATA_ACC_SYSTEM_BEKILL_BYPLAYER, 1, UPDATE_FALSE);
}

bool CPlayer::IsEnemy(CSceneObject* pTarget) const
{
    __ENTER_FUNCTION
    if(this == pTarget)
        return false;
    CActor* pTargetActor = static_cast<CActor*>(pTarget);
    if(pTargetActor->IsMonster())
    {
        if(pTargetActor->GetOwnerID() == GetID())
            return false;

        return pTargetActor->GetCampID() != GetCampID();
    }
    else if(pTargetActor->IsPet())
    {
        if(pTargetActor->GetOwnerID() == GetID())
            return false;

        return pTargetActor->GetCampID() != GetCampID();
    }
    else if(pTargetActor->IsPlayer())
    {
        return pTargetActor->GetCampID() != GetCampID();
    }
    __LEAVE_FUNCTION
    return false;
}