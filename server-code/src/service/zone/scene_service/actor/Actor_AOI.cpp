#include "Actor.h"
#include "ActorManager.h"
#include "GameEventDef.h"
#include "Monster.h"
#include "Phase.h"
#include "Player.h"
#include "Scene.h"
#include "SceneService.h"
#include "SceneTree.h"
#include "msg/zone_service.pb.h"
#include "server_msg/server_side.pb.h"

//玩家 位面id = X 时 可以看到 位面id = X的all， 可以看到 位面id=自己ID 的all, 可以看到 inTaskList(位面id)的NPC

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
        m_DataMap[ACT_PET][ACT_PET] = [](CSceneObject* self, CSceneObject* target) {
            return self->GetPhaseID() == target->GetPhaseID();
        };
        m_DataMap[ACT_PET][ACT_BULLET] = [](CSceneObject* self, CSceneObject* target) {
            return self->GetPhaseID() == target->GetPhaseID() || self->GetOwnerID() == target->GetPhaseID();
        };
        m_DataMap[ACT_PET][ACT_PLAYER] = [](CSceneObject* self, CSceneObject* target) {
            return self->GetPhaseID() == target->GetPhaseID() || self->GetID() == target->GetPhaseID();
        };
        m_DataMap[ACT_BULLET][ACT_BULLET] = [](CSceneObject* self, CSceneObject* target) {
            return self->GetPhaseID() == target->GetPhaseID() || self->GetOwnerID() == target->GetPhaseID();
        };
        m_DataMap[ACT_NPC][ACT_PLAYER] = [](CSceneObject* self, CSceneObject* target) -> bool {
            CPlayer* pThisPlayer = self->CastTo<CPlayer>();
            CHECKF(pThisPlayer);
            return self->GetPhaseID() == target->GetPhaseID() || self->GetID() == target->GetPhaseID() ||
                   pThisPlayer->CheckTaskPhase(target->GetPhaseID());
        };
        m_DataMap[ACT_BULLET][ACT_PLAYER] = [](CSceneObject* self, CSceneObject* target) {
            return self->GetPhaseID() == target->GetPhaseID() || self->GetID() == target->GetPhaseID();
        };
        m_DataMap[ACT_MAPITEM][ACT_PLAYER] = [](CSceneObject* self, CSceneObject* target) {
            return self->GetPhaseID() == target->GetPhaseID() || self->GetID() == target->GetPhaseID();
        };
        m_DataMap[ACT_PLAYER][ACT_PLAYER] = [](CSceneObject* self, CSceneObject* target) {
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
        __LEAVE_FUNCTION
        return false;
    }

private:
    std::function<bool(CSceneObject*, CSceneObject*)> m_DataMap[ACT_MAX][ACT_MAX];
} const NEED_ADD_TO_BROADCASTSET;

void CActor::_AddToAOIRemoveMessage(SC_AOI_REMOVE& removeMsg, OBJID id)
{
    __ENTER_FUNCTION
    if(GetActorType() == ACT_PLAYER)
    {
        removeMsg.add_idlist(id);
        //为了加速前端处理,以及避免单个包过大,每个包中允许缓冲64个aoi_info
        constexpr int32_t MAX_AOI_SIZE_IN_ONE_PACKET = 64;
        if(removeMsg.idlist_size() > MAX_AOI_SIZE_IN_ONE_PACKET)
        {
            SendMsg(removeMsg);
            removeMsg.clear_idlist();
        }
    }

    __LEAVE_FUNCTION
}

void CActor::_TrySendAOIRemoveMessage(const SC_AOI_REMOVE& removeMsg)
{
    __ENTER_FUNCTION
    if(removeMsg.idlist_size() > 0)
    {
        SendMsg(removeMsg);
    }
    __LEAVE_FUNCTION
}

//////////////////////////////////////////////////////////////////////
void CActor::RemoveFromViewList(CSceneObject* pActor, OBJID idActor, bool bErase)
{
    __ENTER_FUNCTION
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
    __LEAVE_FUNCTION
}

//////////////////////////////////////////////////////////////////////
void CActor::AddToViewList(CSceneObject* pActor)
{
    __ENTER_FUNCTION
    CSceneObject::AddToViewList(pActor);

    //如果自己是怪物
    if(IsMonster())
    {
        CastTo<CMonster>()->SetIsAISleep(false);
    }
    __LEAVE_FUNCTION
}

//////////////////////////////////////////////////////////////////////
void CActor::OnBeforeClearViewList(bool bSendMsgToSelf)
{
    __ENTER_FUNCTION
    //发送删除包
    SC_AOI_REMOVE ntc_aoiInfo;
    ntc_aoiInfo.set_scene_idx(GetSceneIdx());
    ntc_aoiInfo.add_idlist(GetID());
    SendRoomMessage(ntc_aoiInfo, false);

    if(bSendMsgToSelf)
    {
        SC_AOI_REMOVE hold_info;
        for(uint64_t id: m_ViewActors)
        {
            _AddToAOIRemoveMessage(hold_info, id);
        }
        _TrySendAOIRemoveMessage(hold_info);
    }

    if(m_setDealySendShow.empty() == false)
    {
        GetEventMapRef().Cancel(EVENTID_BROCAST_SHOW);
    }
    m_setDealySendShow.clear();
    __LEAVE_FUNCTION
}

void CActor::AddDelaySendShowTo(OBJID id)
{
    __ENTER_FUNCTION
    if(m_setDealySendShow.empty() == true)
    {
        auto pEntry = GetEventMapRef().Query(EVENTID_BROCAST_SHOW);
        if(pEntry == nullptr || pEntry->IsCanceled() || pEntry->IsRunning() == false)
        {
            CEventEntryCreateParam param;
            param.evType    = EVENTID_BROCAST_SHOW;
            param.cb        = std::bind(&CActor::SendShowToDealyList, this);
            param.tWaitTime = 0;
            param.bPersist  = false;
            EventManager()->ScheduleEvent(param, GetEventMapRef());
        }
    }
    m_setDealySendShow.insert(id);
    __LEAVE_FUNCTION
}

void CActor::AddDelaySendShowToAllViewPlayer()
{
    __ENTER_FUNCTION
    if(m_setDealySendShow.empty() == true)
    {
        auto pEntry = GetEventMapRef().Query(EVENTID_BROCAST_SHOW);
        if(pEntry == nullptr || pEntry->IsCanceled() || pEntry->IsRunning() == false)
        {
            CEventEntryCreateParam param;
            param.evType    = EVENTID_BROCAST_SHOW;
            param.cb        = std::bind(&CActor::SendShowToDealyList, this);
            param.tWaitTime = 0;
            param.bPersist  = false;
            EventManager()->ScheduleEvent(param, GetEventMapRef());
        }
    }
    m_setDealySendShow.insert(m_ViewActorsByType[ACT_PLAYER].begin(), m_ViewActorsByType[ACT_PLAYER].end());
    __LEAVE_FUNCTION
}

void CActor::RemoveDelaySendShowTo(OBJID id)
{
    __ENTER_FUNCTION
    if(m_setDealySendShow.empty())
        return;

    m_setDealySendShow.erase(id);
    if(m_setDealySendShow.empty())
    {
        GetEventMapRef().Cancel(EVENTID_BROCAST_SHOW);
    }
    __LEAVE_FUNCTION
}

void CActor::SendShowToDealyList()
{
    __ENTER_FUNCTION
    if(m_setDealySendShow.empty())
        return;
    BroadcastShowTo(SceneService()->IDList2VSMap(m_setDealySendShow, 0));
    m_setDealySendShow.clear();
    __LEAVE_FUNCTION
}

void CActor::SendShowTo(CPlayer* pPlayer)
{
    __ENTER_FUNCTION
    if(pPlayer == nullptr)
        return;

    SC_AOI_NEW msg;
    MakeShowData(msg);
    pPlayer->SendMsg(msg);
    __LEAVE_FUNCTION
}

bool CActor::IsNeedAddToBroadCastSet(CSceneObject* pActor)
{
    return NEED_ADD_TO_BROADCASTSET.test(this, pActor);
}

bool CActor::IsMustAddToBroadCastSet(CSceneObject* pActor)
{
    __ENTER_FUNCTION
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

    if(pActor->IsPlayer())
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
    __LEAVE_FUNCTION
    return false;
}

void CActor::OnAOIProcess(const BROADCAST_SET& setBCActorDel, const BROADCAST_SET& setBCActor, const BROADCAST_SET& setBCActorAdd)
{
    // step4: 需要离开视野的角色Remove
    OnAOIProcess_ActorRemoveFromAOI(setBCActorDel);

    // 设置角色广播集=当前广播集-离开视野的差集
    m_ViewActors = setBCActor;
    OnAOIProcess_PosUpdate();

    //////////////////////////////////////////////////////////////////////////
    // step5: 新进入视野的角色和地图物品Add
    OnAOIProcess_ActorAddToAOI(setBCActorAdd);

    SendAOIChangeToAI(setBCActorDel, setBCActorAdd);
}

void CActor::SendAOIChangeToAI(const BROADCAST_SET& setBCActorDel, const BROADCAST_SET& setBCActorAdd)
{
    ServerMSG::AOIChange msg;
    msg.set_scene_idx(GetCurrentScene()->GetSceneIdx());
    msg.set_actor_id(GetID());
    for(const auto& v: setBCActorDel)
    {
        msg.add_actor_del(v);
    }
    for(const auto& v: setBCActorAdd)
    {
        msg.add_actor_add(v);
    }
    SceneService()->SendProtoMsgToAIService(msg);
}

void CActor::OnAOIProcess_ActorRemoveFromAOI(const BROADCAST_SET& setBCActorDel)
{
    __ENTER_FUNCTION
    SC_AOI_REMOVE hold_info;
    hold_info.set_scene_idx(GetSceneIdx());

    //////////////////////////////////////////////////////////////////////////
    // step4: 需要离开视野的角色Remove
    {
        BROADCAST_SET setBCActorDelPlayer;
        for(const auto& id: setBCActorDel)
        {
            CActor* pActor = static_cast<CActor*>(GetCurrentScene()->QuerySceneObj(id));
            if(pActor == nullptr)
            {
                continue;
            }
            // 通知自己对方消失
            //不需要从自己的m_ViewActors移除,因为等下会一次性移除,
            RemoveFromViewList(pActor, id, false);
            // 通知对方自己消失,
            pActor->RemoveFromViewList(this, this->GetID(), true);

            //为了减少发送次数,发送给自己的移除消息一次性发送
            _AddToAOIRemoveMessage(hold_info, id);

            //如果目标是Player，需要收到删除我的广播
            if(pActor->GetActorType() == ACT_PLAYER)
            {
                setBCActorDelPlayer.insert(id);
            }
        }

        //通知Del列表删除自己
        if(setBCActorDelPlayer.empty() == false)
        {
            SC_AOI_REMOVE ntc_aoiInfo;
            ntc_aoiInfo.set_scene_idx(GetSceneIdx());
            ntc_aoiInfo.add_idlist(GetID());

            auto setSocketMap = SceneService()->IDList2VSMap(setBCActorDelPlayer, 0);
            SceneService()->SendProtoMsgTo(setSocketMap, ntc_aoiInfo);
        }
    }
    //通知自己删除del列表
    _TrySendAOIRemoveMessage(hold_info);
    __LEAVE_FUNCTION
}

void CActor::OnAOIProcess_PosUpdate()
{
    __ENTER_FUNCTION
    //发送移动同步
    SC_AOI_UPDATE ntc;
    ntc.set_scene_idx(GetSceneIdx());
    ntc.set_actor_id(GetID());
    ntc.set_posx(GetPosX());
    ntc.set_posy(GetPosY());

    SendRoomMessage(ntc);
    __LEAVE_FUNCTION
}

void CActor::OnAOIProcess_ActorAddToAOI(const BROADCAST_SET& setBCActorAdd)
{
    __ENTER_FUNCTION
    BROADCAST_SET setBCActorAddPlayer;
    for(const auto& id: setBCActorAdd)
    {
        CActor* pActor = static_cast<CActor*>(GetCurrentScene()->QuerySceneObj(id));
        if(pActor == nullptr)
        {
            continue;
        }

        AddToViewList(pActor);
        pActor->AddToViewList(this);

        // 如果自己是Player， 加入我视野的对象， 需要将他们的信息发送给我
        if(GetActorType() == ACT_PLAYER)
        {
            pActor->AddDelaySendShowTo(GetID());
        }

        //如果目标是Player，需要收到我的show广播
        if(pActor->GetActorType() == ACT_PLAYER)
        {
            setBCActorAddPlayer.insert(id);
        }
    }

    //合并延迟发送Show的队列一起发送
    if(setBCActorAddPlayer.size() > 0 || m_setDealySendShow.size() > 0)
    {
        if(m_setDealySendShow.empty())
        {
            BroadcastShowTo(SceneService()->IDList2VSMap(setBCActorAddPlayer, 0));
        }
        else if(setBCActorAddPlayer.empty())
        {
            BroadcastShowTo(SceneService()->IDList2VSMap(m_setDealySendShow, 0));
            m_setDealySendShow.clear();
            GetEventMapRef().Cancel(EVENTID_BROCAST_SHOW);
        }
        else
        {
            m_setDealySendShow.insert(setBCActorAddPlayer.begin(), setBCActorAddPlayer.end());
            BroadcastShowTo(SceneService()->IDList2VSMap(m_setDealySendShow, 0));
            m_setDealySendShow.clear();
            GetEventMapRef().Cancel(EVENTID_BROCAST_SHOW);
        }
    }
    __LEAVE_FUNCTION
}