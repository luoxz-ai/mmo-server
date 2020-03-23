#include "SkillType.h"

#include "Actor.h"
bool CSkillType::CanAttackActor(CActor* pAttacker, CActor* pTarget) const
{
    CHECKF(pTarget);
    if(pTarget->IsMonster() && HasFlag(GetDamageFlag(), STF_TARGET_MONSTER) == false)
        return false;
    if(pTarget->IsPlayer() && HasFlag(GetDamageFlag(), STF_TARGET_PLAYER) == false)
        return false;
    if(pTarget->IsPet() && HasFlag(GetDamageFlag(), STF_TARGET_PET) == false)
        return false;
    if(pTarget->IsDead() && HasFlag(GetDamageFlag(), STF_TARGET_DEAD) == false)
        return false;

    if(pAttacker)
    {
        if(pAttacker->IsDead())
            return false;
        if(pAttacker->IsEnemy(pTarget) == false && HasFlag(GetDamageFlag(), STF_TARGET_FRIEND) == false)
            return false;
    }

    return true;
}
