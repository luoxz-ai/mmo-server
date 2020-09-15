#include "AIActor.h"
#include "AIActorManager.h"
#include "AIMonster.h"
#include "AIPlayer.h"
#include "AIScene.h"
#include "AIService.h"

class NEED_ADD_TO_BROADCASTSET_T
{
public:
    NEED_ADD_TO_BROADCASTSET_T()
    {
        m_DataMap[ACT_MONSTER][ACT_MONSTER] = [](auto self, auto target) {
            bool bSamePhase = self->GetPhaseID() == target->GetPhaseID();
            return bSamePhase && self->IsEnemy(target);
        };
        m_DataMap[ACT_MONSTER][ACT_PET] = [](CSceneObject* self, CSceneObject* target) {
            return self->GetPhaseID() == target->GetPhaseID() || self->GetPhaseID() == target->GetOwnerID();
        };
        m_DataMap[ACT_MONSTER][ACT_BULLET] = [](auto self, auto target) {
            return self->GetPhaseID() == target->GetPhaseID() || self->GetPhaseID() == target->GetOwnerID();
        };
        m_DataMap[ACT_MONSTER][ACT_PLAYER] = [](CSceneObject* self, CSceneObject* target) {
            return self->GetPhaseID() == target->GetPhaseID() || self->GetID() == target->GetPhaseID();
        };
    }
    bool test(CSceneObject* pSelfActor, CSceneObject* pTargetActor) const
    {
        __ENTER_FUNCTION
        uint32_t key1 = pSelfActor->GetActorType();
        uint32_t key2 = pTargetActor->GetActorType();
        if(pSelfActor->GetActorType() > pTargetActor->GetActorType())
        {
            key2 = pSelfActor->GetActorType();
            key1 = pTargetActor->GetActorType();
        }
        const auto& func = m_DataMap[key1][key2];
        if(func)
            return func(pSelfActor, pTargetActor);
        else
            return false;
        __LEAVE_FUNCTION
        return false;
    }

private:
    std::function<bool(CSceneObject*, CSceneObject*)> m_DataMap[ACT_MAX][ACT_MAX];
} const NEED_ADD_TO_BROADCASTSET;

//////////////////////////////////////////////////////////////////////
void CAIActor::AddToViewList(CSceneObject* pActor)
{
    __ENTER_FUNCTION
    CSceneObject::AddToViewList(pActor);

    //如果自己是怪物
    if(IsMonster())
    {
        CastTo<CAIMonster>()->SetIsAISleep(false);
    }

    __LEAVE_FUNCTION
}

//////////////////////////////////////////////////////////////////////
void CAIActor::ClearViewList(bool bSendMsgToSelf)
{
    __ENTER_FUNCTION
    for(uint64_t id: m_ViewActors)
    {
        // 通知对方自己消失
        CAIActor* pActor = AIActorManager()->QueryActor(id);
        if(pActor)
        {
            pActor->RemoveFromViewList(this, GetID(), true);
        }
    }
    //发送删除包

    m_ViewActorsByType.clear();
    m_ViewActors.clear();

    __LEAVE_FUNCTION
}

bool CAIActor::IsNeedAddToBroadCastSet(CSceneObject* pActor)
{
    __ENTER_FUNCTION
    return NEED_ADD_TO_BROADCASTSET.test(this, pActor);

    __LEAVE_FUNCTION
    return false;
}

void CAIActor::OnAOIProcess_ActorRemoveFromAOI(const BROADCAST_SET& setBCActorDel,
                                               BROADCAST_SET&       setBCActor,
                                               int32_t              nCanReserveDelCount,
                                               uint32_t             view_range_out_square)
{
    __ENTER_FUNCTION
    for(auto it = setBCActorDel.begin(); it != setBCActorDel.end(); it++)
    {
        // 如果对方还在脱离视野范围内，则不删除
        uint64_t  id     = *it;
        CAIActor* pActor = AIActorManager()->QueryActor(id);
        if(pActor == nullptr)
        {
            //如果对方已经消失,则移除
            RemoveFromViewList(nullptr, id, false);
            continue;
        }
        if(nCanReserveDelCount > 0)
        {
            if(view_range_out_square > 0)
            {
                uint32_t distance_square = GameMath::simpleDistance(GetPos(), pActor->GetPos());
                if(distance_square < view_range_out_square) // 在脱离视野半径内的，不需要离开广播区域)
                {
                    nCanReserveDelCount--;
                    setBCActor.insert(id);
                    continue;
                }
            }
        }

        // 对方已脱离视野

        // 通知自己对方消失
        //不需要从自己的m_ViewActors移除,因为等下会一次性移除,
        //为了减少发送次数,发送给自己的移除消息一次性发送
        RemoveFromViewList(pActor, id, false);
        // 通知对方自己消失,
        pActor->RemoveFromViewList(this, this->GetID(), true);
    }

    __LEAVE_FUNCTION
}

//////////////////////////////////////////////////////////////////////////
//
void CAIActor::OnAOIProcess_ActorAddToAOI(BROADCAST_SET& setBCActorAdd, const ACTOR_MAP& mapAllViewActor)
{
    __ENTER_FUNCTION
    for(auto it = setBCActorAdd.begin(); it != setBCActorAdd.end(); it++)
    {
        auto itr = mapAllViewActor.find(*it);
        if(itr == mapAllViewActor.end())
        {
            it = setBCActorAdd.erase(it);
            continue;
        }

        CAIActor* pActor = static_cast<CAIActor*>(itr->second);

        // 通知自己,需要发送对方的信息给自己
        AddToViewList(pActor);

        // 通知目标,不要发送自己的信息给对方,后面会统一广播
        pActor->AddToViewList(this);
    }
    __LEAVE_FUNCTION
}
