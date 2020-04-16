#include "Actor.h"
#include "ActorManager.h"
#include "Monster.h"
#include "Player.h"
#include "Phase.h"
#include "Scene.h"
#include "SceneTree.h"
#include "ZoneService.h"
#include "GameEventDef.h"

//玩家 主位面id = X 时 可以看到 主位面id = X的all， 可以看到 主位面id=自己ID 的all, 可以看到 inTaskList(主位面id)的NPC


class NEED_ADD_TO_BROADCASTSET_T
{
public:
    NEED_ADD_TO_BROADCASTSET_T()
    {
        m_DataMap[ACT_MONSTER][ACT_MONSTER] = [](auto self, auto target) 
        {
            bool bSamePhase = self->GetPhaseID() == target->GetPhaseID();
            return bSamePhase && self->IsEnemy(target);
        };
        m_DataMap[ACT_MONSTER][ACT_PET] = [](CSceneObject* self, CSceneObject* target) 
        {
            return self->GetPhaseID() == target->GetPhaseID() ||
                   self->GetPhaseID() == target->GetOwnerID();
        };
        m_DataMap[ACT_MONSTER][ACT_BULLET] = [](auto self, auto target) 
        {
             return self->GetPhaseID() == target->GetPhaseID() ||
                   self->GetPhaseID() == target->GetOwnerID();
        };
        m_DataMap[ACT_MONSTER][ACT_PLAYER] = [](CSceneObject* self, CSceneObject* target) 
        {
            return self->GetPhaseID() == target->GetPhaseID() ||
                   self->GetID() == target->GetPhaseID();
        };
        m_DataMap[ACT_PET][ACT_PET] = [](CSceneObject* self, CSceneObject* target) 
        {
            return self->GetPhaseID() == target->GetPhaseID();
        };
        m_DataMap[ACT_PET][ACT_BULLET] = [](CSceneObject* self, CSceneObject* target) 
        {
            return self->GetPhaseID() == target->GetPhaseID() ||
                   self->GetOwnerID() == target->GetPhaseID();
        };
        m_DataMap[ACT_PET][ACT_PLAYER] = [](CSceneObject* self, CSceneObject* target) 
        {
            return self->GetPhaseID() == target->GetPhaseID() ||
                   self->GetID() == target->GetPhaseID();
        };
        m_DataMap[ACT_BULLET][ACT_BULLET] = [](CSceneObject* self, CSceneObject* target)
        {
            return self->GetPhaseID() == target->GetPhaseID() ||
                   self->GetOwnerID() == target->GetPhaseID();
        };   
        m_DataMap[ACT_NPC][ACT_PLAYER] = [](CSceneObject* self, CSceneObject* target) -> bool
        {
            CPlayer* pThisPlayer = self->CastTo<CPlayer>();
            CHECKF(pThisPlayer);
            return self->GetPhaseID() == target->GetPhaseID() ||
                   self->GetID() == target->GetPhaseID() ||
                   pThisPlayer->CheckTaskPhase(target->GetPhaseID());
        };
        m_DataMap[ACT_BULLET][ACT_PLAYER] = [](CSceneObject* self, CSceneObject* target) 
        {
            return self->GetPhaseID() == target->GetPhaseID() ||
                   self->GetID() == target->GetPhaseID();
        };
        m_DataMap[ACT_MAPITEM][ACT_PLAYER] = [](CSceneObject* self, CSceneObject* target) 
        {
            return self->GetPhaseID() == target->GetPhaseID() ||
                   self->GetID() == target->GetPhaseID();
        };     
        m_DataMap[ACT_PLAYER][ACT_PLAYER] = [](CSceneObject* self, CSceneObject* target) 
        {
            return self->GetPhaseID() == target->GetPhaseID() ||
                   self->GetID() == target->GetPhaseID();
        };
    }
    bool test(CSceneObject* pSelfActor, CSceneObject* pTargetActor) const
    {
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
    }

private:
    std::function<bool(CSceneObject*, CSceneObject*)> m_DataMap[ACT_MAX][ACT_MAX];
} const NEED_ADD_TO_BROADCASTSET;


void CActor::_AddToAOIRemoveMessage(SC_AOI_REMOVE& removeMsg, OBJID id)
{
    if(GetActorType() == ACT_PLAYER)
    {
        removeMsg.add_idlist(id);
        //为了加速前端处理,以及避免单个包过大,每个包中允许缓冲64个aoi_info
        constexpr int32_t MAX_AOI_SIZE_IN_ONE_PACKET = 64;
        if(removeMsg.idlist_size() > MAX_AOI_SIZE_IN_ONE_PACKET)
        {
            SendMsg(CMD_SC_AOI_REMOVE, removeMsg);
            removeMsg.clear_idlist();
        }
    }
}

void CActor::_TrySendAOIRemoveMessage(const SC_AOI_REMOVE& removeMsg)
{
    if(removeMsg.idlist_size() > 0)
    {
        SendMsg(CMD_SC_AOI_REMOVE, removeMsg);
    }
}

//////////////////////////////////////////////////////////////////////
void CActor::RemoveFromViewList(CSceneObject* pActor, OBJID idActor, bool bErase)
{
    // 通知自己对方消失
    CSceneObject::RemoveFromViewList(pActor, idActor, bErase);

    if(pActor)
    {
        if(pActor->GetActorType() == ACT_PLAYER)
        {
            RemoveDelaySendShowTo(pActor->GetID());
        }
    }
    else
    {
        //延迟发送队列里的也可以不要了
        RemoveDelaySendShowTo(idActor);
    }
}

//////////////////////////////////////////////////////////////////////
void CActor::AddToViewList(CSceneObject* pActor)
{
    CSceneObject::AddToViewList(pActor);

    //如果自己是怪物
    if(IsMonster())
    {
        CastTo<CMonster>()->SetIsAISleep(false);
    }
}

//////////////////////////////////////////////////////////////////////
void CActor::ClearViewList(bool bSendMsgToSelf)
{
    SC_AOI_REMOVE hold_info;
    for(uint64_t id: m_ViewActors)
    {
        // 通知对方自己消失
        CActor* pActor = ActorManager()->QueryActor(id);
        if(pActor)
        {
            pActor->RemoveFromViewList(this, GetID(), true);
            if(bSendMsgToSelf)
                _AddToAOIRemoveMessage(hold_info, pActor->GetID());
        }
    }
    //发送删除包

    SC_AOI_REMOVE ntc_aoiInfo;
    ntc_aoiInfo.set_mapid(GetMapID());
    ntc_aoiInfo.add_idlist(GetID());
    SendRoomMessage(CMD_SC_AOI_REMOVE, ntc_aoiInfo, false);

    if(bSendMsgToSelf)
        _TrySendAOIRemoveMessage(hold_info);

    m_ViewActorsByType.clear();
    m_ViewActors.clear();
    if(m_setDealySendShow.empty() == false)
    {
        GetEventMapRef().Cancel(EVENTID_BROCAST_SHOW);
    }
    m_setDealySendShow.clear();
}

void CActor::AddDelaySendShowTo(OBJID id)
{
    if(m_setDealySendShow.empty() == true)
    {
        auto pEntry = GetEventMapRef().Query(EVENTID_BROCAST_SHOW);
        if(pEntry == nullptr || pEntry->IsCanceled() || pEntry->IsRunning() == false)
        {
            EventManager()->ScheduleEvent(EVENTID_BROCAST_SHOW,
                                          std::bind(&CActor::SendShowToDealyList, this),
                                          0,
                                          false,
                                          GetEventMapRef());
        }
    }
    m_setDealySendShow.insert(id);
}

void CActor::AddDelaySendShowToAllViewPlayer()
{
    if(m_setDealySendShow.empty() == true)
    {
        auto pEntry = GetEventMapRef().Query(EVENTID_BROCAST_SHOW);
        if(pEntry == nullptr || pEntry->IsCanceled() || pEntry->IsRunning() == false)
        {
            EventManager()->ScheduleEvent(EVENTID_BROCAST_SHOW,
                                          std::bind(&CActor::SendShowToDealyList, this),
                                          0,
                                          false,
                                          GetEventMapRef());
        }
    }
    m_setDealySendShow.insert(m_ViewActorsByType[ACT_PLAYER].begin(), m_ViewActorsByType[ACT_PLAYER].end());
}

void CActor::RemoveDelaySendShowTo(OBJID id)
{
    if(m_setDealySendShow.empty())
        return;

    m_setDealySendShow.erase(id);
    if(m_setDealySendShow.empty())
    {
        GetEventMapRef().Cancel(EVENTID_BROCAST_SHOW);
    }
}

void CActor::SendShowToDealyList()
{
    if(m_setDealySendShow.empty())
        return;
    BroadcastShowTo(ZoneService()->IDList2VSMap(m_setDealySendShow, 0));
    m_setDealySendShow.clear();
}

void CActor::SendShowTo(CPlayer* pPlayer)
{
    if(pPlayer == nullptr)
        return;

    SC_AOI_NEW msg;
    MakeShowData(msg);
    pPlayer->SendMsg(CMD_SC_AOI_NEW, msg);
}

bool CActor::IsNeedAddToBroadCastSet(CSceneObject* pActor)
{
    return NEED_ADD_TO_BROADCASTSET.test(this, pActor);
}

bool CActor::IsMustAddToBroadCastSet(CSceneObject* pActor)
{
    CHECKF(pActor);
    //自己召唤出来的,必然可见
    if(pActor->GetOwnerID() == GetID())
    {
        return true;
    }
    //私有位面的,必定可见
    if(pActor->GetPhaseID() != 0 && pActor->GetPhaseID() == GetID())
    {
        return true;
    }

    //必须加入视野的, BOSS怪, 组队成员
    if(pActor->IsMonster())
    {
        CMonster* pMonster = pActor->CastTo<CMonster>();
        CHECKF(pMonster);
        if(pMonster->IsBoss())
        {
            return true;
        }
        
        return false;
    }
    else if(pActor->IsPlayer())
    {
        CPlayer* pPlayer = pActor->CastTo<CPlayer>();
        CHECKF(pPlayer);  
        
        //队友必然可见
        if(IsPlayer())
        {
            CPlayer* pThisPlayer = CastTo<CPlayer>();
            CHECKF(pThisPlayer);  
            return (pThisPlayer->GetTeamID() != 0 && pPlayer->GetTeamID() == pThisPlayer->GetTeamID());
        }

        
    }

    return false;
}

void CActor::OnAOIProcess_ActorRemoveFromAOI(const BROADCAST_SET& setBCActorDel,
                                          BROADCAST_SET&       setBCActor,
                                          int32_t              nCanReserveDelCount,
                                          uint32_t             view_range_out_square)
{
    SC_AOI_REMOVE hold_info;
    hold_info.set_mapid(GetMapID());

    //////////////////////////////////////////////////////////////////////////
    // step4: 需要离开视野的角色Remove
    {
        BROADCAST_SET setBCActorDelPlayer;
        for(auto it = setBCActorDel.begin(); it != setBCActorDel.end(); it++)
        {
            // 如果对方还在脱离视野范围内，则不删除
            uint64_t id     = *it;
            CActor*  pActor = ActorManager()->QueryActor(id);
            if(pActor == nullptr)
            {
                //如果对方已经消失,则移除
                RemoveFromViewList(nullptr, id, false);
                _AddToAOIRemoveMessage(hold_info, id);
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
            _AddToAOIRemoveMessage(hold_info, pActor->GetID());
            // 通知对方自己消失,
            pActor->RemoveFromViewList(this, this->GetID(), true);
            //不发送移除消息,等下一次性广播
            if(pActor->GetActorType() == ACT_PLAYER)
            {
                setBCActorDelPlayer.insert(id);
            }
        }

        //通知Del列表删除自己
        if(setBCActorDelPlayer.empty() == false)
        {

            SC_AOI_REMOVE ntc_aoiInfo;
            ntc_aoiInfo.set_mapid(GetMapID());
            ntc_aoiInfo.add_idlist(GetID());
            
            auto setSocketMap = ZoneService()->IDList2VSMap(setBCActorDelPlayer, 0);
            ZoneService()->SendMsgTo(CMD_SC_AOI_REMOVE, ntc_aoiInfo, setSocketMap);
        }
    }
    //通知自己删除del列表
    _TrySendAOIRemoveMessage(hold_info);
}

void CActor::OnAOIProcess_PosUpdate()
{

    //发送移动同步
    SC_AOI_UPDATE ntc;
    ntc.set_mapid(GetMapID());
    ntc.set_actor_id(GetID());
    ntc.set_posx(GetPosX());
    ntc.set_posy(GetPosY());

    SendRoomMessage(CMD_SC_AOI_UPDATE, ntc);
}

void CActor::OnAOIProcess_ActorAddToAOI(BROADCAST_SET& setBCActorAdd, const ACTOR_MAP& mapAllViewActor)
{
    for(auto it = setBCActorAdd.begin(); it != setBCActorAdd.end();)
    {
        auto itr = mapAllViewActor.find(*it);
        if(itr == mapAllViewActor.end())
        {
            it = setBCActorAdd.erase(it);
            continue;
        }

        CActor* pActor = static_cast<CActor*>(itr->second);

        // 通知自己,需要发送对方的信息给自己
        AddToViewList(pActor);
        if(GetActorType() == ACT_PLAYER)
        {
            static_cast<CActor*>(pActor)->AddDelaySendShowTo(GetID());
        }

        // 通知目标,不要发送自己的信息给对方,后面会统一广播
        pActor->AddToViewList(this);

        //只有Player才需要收到show广播
        if(pActor->GetActorType() != ACT_PLAYER)
        {
            it = setBCActorAdd.erase(it);
        }
        else
        {
            it++;
        }
        
    }

    //合并延迟发送Show的队列一起发送
    if(setBCActorAdd.size() > 0 || m_setDealySendShow.size() > 0)
    {
        if(m_setDealySendShow.empty())
        {
            BroadcastShowTo(ZoneService()->IDList2VSMap(setBCActorAdd, 0));
        }
        else if(setBCActorAdd.empty())
        {
            BroadcastShowTo(ZoneService()->IDList2VSMap(m_setDealySendShow, 0));
            m_setDealySendShow.clear();
            GetEventMapRef().Cancel(EVENTID_BROCAST_SHOW);
        }
        else
        {
            m_setDealySendShow.insert(setBCActorAdd.begin(), setBCActorAdd.end());
            BroadcastShowTo(ZoneService()->IDList2VSMap(m_setDealySendShow, 0));
            m_setDealySendShow.clear();
            GetEventMapRef().Cancel(EVENTID_BROCAST_SHOW);
        }
    }
}