#include "Actor.h"
#include "ActorManager.h"
#include "Monster.h"
#include "Phase.h"
#include "Player.h"
#include "Scene.h"
#include "SceneManager.h"
#include "SceneService.h"
#include "SceneTree.h"
#include "ScriptCallBackType.h"
#include "msg/zone_service.pb.h"
#include "protomsg_to_cmd.h"
#include "server_msg/server_side.pb.h"

void CActor::FlyTo(const Vector2& pos)
{
    __ENTER_FUNCTION

    CHECK(GetCurrentScene());
    CPhase* pPhase = static_cast<CPhase*>(GetCurrentScene());
    pPhase->LeaveMap(this, pPhase->GetMapID());
    m_pScene     = nullptr;
    auto findPos = pPhase->FindPosNearby(pos, 2.0f);
    pPhase->EnterMap(this, findPos.x, findPos.y, GetFace());

    __LEAVE_FUNCTION
}

void CActor::OnEnterMap(CSceneBase* pScene)
{
    CHECK(pScene);
    __ENTER_FUNCTION
    CSceneObject::OnEnterMap(pScene);
    if(pScene && pScene->GetScriptID())
        ScriptManager()->TryExecScript<void>(pScene->GetScriptID(), SCB_MAP_ONENTERMAP, this);
    __LEAVE_FUNCTION
}

void CActor::OnLeaveMap(uint16_t idTargetMap)
{
    __ENTER_FUNCTION
    SendDelayAttribChage();

    if(IsMonster() || IsPlayer())
    {
        ServerMSG::ActorDestory ai_msg;
        ai_msg.set_actor_id(GetID());
        ai_msg.set_dead(IsDead());
        SceneService()->SendProtoMsgToAIService(ai_msg);
    }

    if(m_pScene && m_pScene->GetScriptID())
        ScriptManager()->TryExecScript<void>(m_pScene->GetScriptID(), SCB_MAP_ONLEAVEMAP, this, idTargetMap);

    CSceneObject::OnLeaveMap(idTargetMap);

    m_EventMap->Clear();
    m_EventQueue->Clear();

    __LEAVE_FUNCTION
}

uint16_t CActor::GetMapID() const
{
    __ENTER_FUNCTION
    if(m_pScene)
        return m_pScene->GetMapID();

    __LEAVE_FUNCTION
    return 0;
}

uint64_t CActor::GetSceneIdx() const
{
    __ENTER_FUNCTION
    if(m_pScene)
        return m_pScene->GetSceneIdx();

    __LEAVE_FUNCTION
    return 0;
}

void CActor::SendRoomMessage(const proto_msg_t& msg, bool bIncludeSelf /*= true*/)
{
    __ENTER_FUNCTION
    SendShowToDealyList();
    auto setSocketMap = SceneService()->IDList2VSMap(m_ViewActorsByType[ACT_PLAYER], (bIncludeSelf) ? GetID() : 0);
    SceneService()->SendProtoMsgTo(setSocketMap, msg);
    auto cmd = msg_to_cmd(msg);
    // send message to ai_service
    if((IsMonster() || IsPlayer()) && (cmd == CMD_SC_AOI_UPDATE || cmd == CMD_SC_CASTSKILL || cmd == CMD_SC_ATTRIB_CHANGE))
    {
        SceneService()->SendProtoMsgToAIService(msg);
    }

    __LEAVE_FUNCTION
}

void CActor::SendWorldMessage(const proto_msg_t& msg)
{
    __ENTER_FUNCTION

    if(GetWorldID() != 0)
    {
        SceneService()->SendProtoMsgToWorld(GetWorldID(), msg);
    }

    __LEAVE_FUNCTION
}
