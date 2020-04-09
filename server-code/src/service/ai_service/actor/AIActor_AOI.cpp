#include "AIActor.h"
#include "AIMonster.h"
#include "AIPlayer.h"
#include "AIScene.h"
#include "AIService.h"

//////////////////////////////////////////////////////////////////////
void CAIActor::AddToViewList(CSceneObject* pActor)
{
    CSceneObject::AddToViewList(pActor);

	//如果自己是怪物
	if(GetActorType() == ACT_MONSTER)
	{
		CastTo<CAIMonster>()->SetIsAISleep(false);
	}

}

//////////////////////////////////////////////////////////////////////
void CAIActor::ClearViewList(bool bSendMsgToSelf)
{
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
}

bool CAIActor::IsNeedAddToBroadCastSet(CSceneObject* pActor)
{
	//特殊怪物需要将其他怪物加入自己视野的,这里处理	
    if(GetActorType() == ACT_MONSTER)
    {
        CAIMonster* pMonster = this->CastTo<CAIMonster>();
		// 只看到敌人单位
        return pMonster->IsEnemy(pActor);
    }
    else
    {
        // player need add all to view list
        return true;
    }
}

void CAIActor::OnAOIProcess_ActorRemoveFromAOI(const BROADCAST_SET& setBCActorDel,
                                            BROADCAST_SET&       setBCActor,
                                            int32_t              nCanReserveDelCount,
                                            uint32_t             view_range_out_square)
{
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
				if(distance_square < view_range_out_square)	// 在脱离视野半径内的，不需要离开广播区域)
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
}

//////////////////////////////////////////////////////////////////////////
//
void CAIActor::OnAOIProcess_ActorAddToAOI(BROADCAST_SET& setBCActorAdd, const ACTOR_MAP& mapAllViewActor)
{
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
}
