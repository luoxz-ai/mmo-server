#include "Actor.h"
#include "ActorManager.h"
#include "Monster.h"
#include "Player.h"
#include "Scene.h"
#include "SceneTree.h"
#include "ZoneService.h"

void CActor::OnEnterMap(CSceneBase* pScene)
{
    CHECK(pScene);
    CSceneObject::OnEnterMap(pScene);

    static_cast<CScene*>(pScene)->TryExecScript<void>(SCB_MAP_ONENTERMAP, this);
}

void CActor::OnLeaveMap(uint64_t idTargetScene)
{
    SendDelayAttribChage();

    if(IsMonster() || IsPlayer())
    {
        ServerMSG::ActorDestory ai_msg;
        ai_msg.set_actor_id(GetID());
        ZoneService()->SendMsgToAIService(ServerMSG::MsgID_ActorDestory, ai_msg);
    }

    if(m_pScene)
        static_cast<CScene*>(m_pScene)->TryExecScript<void>(SCB_MAP_ONLEAVEMAP, this, idTargetScene);

    CSceneObject::OnLeaveMap(idTargetScene);

    m_EventMap.Clear();
    m_EventQueue.Clear();
}

uint32_t CActor::GetMapID() const
{
    if(m_pScene)
        return m_pScene->GetMapID();
    return 0;
}

uint64_t CActor::GetSceneID() const
{
    if(m_pScene)
        return m_pScene->GetSceneID();
    return 0;
}


void CActor::SendRoomMessage(uint16_t cmd, const google::protobuf::Message& msg, bool bIncludeSelf /*= true*/)
{
    SendShowToDealyList();

    if(bIncludeSelf)
        ZoneService()->BroadcastMessageToPlayer(m_ViewActorsByType[ACT_PLAYER], cmd, msg, GetID());
    else
        ZoneService()->BroadcastMessageToPlayer(m_ViewActorsByType[ACT_PLAYER], cmd, msg, 0);

    // send message to ai_service
    if( (IsMonster() || IsPlayer()) &&
        (cmd == CMD_SC_AOI_UPDATE || cmd == CMD_SC_CASTSKILL || cmd == CMD_SC_ATTRIB_CHANGE) )
    {
        ZoneService()->SendMsgToAIService(cmd, msg);
    }
}

void CActor::SendWorldMessage(uint16_t cmd, const google::protobuf::Message& msg)
{
    if(GetWorldID() != 0)
        ZoneService()->SendPortMsg(ServerPort(GetWorldID(), WORLD_SERVICE_ID), cmd, msg);
}
