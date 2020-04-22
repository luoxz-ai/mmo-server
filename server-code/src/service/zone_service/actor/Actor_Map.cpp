#include "Actor.h"
#include "ActorManager.h"
#include "Monster.h"
#include "Player.h"
#include "Phase.h"
#include "Scene.h"
#include "SceneTree.h"
#include "SceneManager.h"
#include "ZoneService.h"
#include "server_msg/server_side.pb.h"

void CActor::ChangePhase(uint64_t idPhaseID)
{
    if(GetPhaseID() == idPhaseID)
    {
        return;
    }
    //make sure scene have this phase
    SceneManager()->CreatePhase(m_pScene->GetSceneID(), idPhaseID);

    _SetPhaseID(idPhaseID);
    UpdateViewList();
}

void CActor::OnEnterMap(CSceneBase* pScene)
{
    CHECK(pScene);
    CSceneObject::OnEnterMap(pScene);

    static_cast<CPhase*>(pScene)->TryExecScript<void>(SCB_MAP_ONENTERMAP, this);
}

void CActor::OnLeaveMap(uint64_t idTargetScene)
{
    SendDelayAttribChage();

    if(IsMonster() || IsPlayer())
    {
        ServerMSG::ActorDestory ai_msg;
        ai_msg.set_actor_id(GetID());
        ZoneService()->SendPortMsgToAIService(ai_msg);
    }

    if(m_pScene)
        static_cast<CPhase*>(m_pScene)->TryExecScript<void>(SCB_MAP_ONLEAVEMAP, this, idTargetScene);

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

void CActor::SendRoomMessage(const google::protobuf::Message& msg, bool bIncludeSelf /*= true*/)
{
    return SendRoomMessage(to_sc_cmd(msg), msg, bIncludeSelf);
}

void CActor::SendRoomMessage(uint16_t cmd, const google::protobuf::Message& msg, bool bIncludeSelf /*= true*/)
{
    SendShowToDealyList();
    auto setSocketMap = ZoneService()->IDList2VSMap(m_ViewActorsByType[ACT_PLAYER], (bIncludeSelf) ?0:GetID());
    ZoneService()->SendMsgTo(setSocketMap, cmd, msg);
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
    {
        ZoneService()->SendMsgToWorld(GetWorldID(), cmd, msg);
    }   
}
