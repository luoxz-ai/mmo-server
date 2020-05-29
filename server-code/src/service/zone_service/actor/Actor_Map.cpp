#include "Actor.h"
#include "ActorManager.h"
#include "Monster.h"
#include "Phase.h"
#include "Player.h"
#include "Scene.h"
#include "SceneManager.h"
#include "SceneTree.h"
#include "ZoneService.h"
#include "server_msg/server_side.pb.h"

void CActor::FlyTo(const Vector2& pos)
{
    __ENTER_FUNCTION

    CHECK(GetCurrentScene());
    CPhase* pPhase = static_cast<CPhase*>(GetCurrentScene());
    pPhase->LeaveMap(this, pPhase->GetMapID());
    m_pScene = nullptr;
    auto findPos = pPhase->FindPosNearby(pos, 2.0f);
    pPhase->EnterMap(this, findPos.x, findPos.y, GetFace());


    __LEAVE_FUNCTION
}

void CActor::ChangePhase(uint64_t idPhaseID)
{
    __ENTER_FUNCTION

    if(GetPhaseID() == idPhaseID)
    {
        return;
    }
    // make sure scene have this phase
    SceneManager()->CreatePhase(m_pScene->GetSceneIdx(), idPhaseID);

    _SetPhaseID(idPhaseID);
    UpdateViewList();

    __LEAVE_FUNCTION
}

void CActor::OnEnterMap(CSceneBase* pScene)
{
    CHECK(pScene);
    __ENTER_FUNCTION
    CSceneObject::OnEnterMap(pScene);

    static_cast<CPhase*>(pScene)->TryExecScript<void>(SCB_MAP_ONENTERMAP, this);
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
        ZoneService()->SendServerMsgToAIService(ai_msg);
    }

    if(m_pScene)
        static_cast<CPhase*>(m_pScene)->TryExecScript<void>(SCB_MAP_ONLEAVEMAP, this, idTargetMap);

    CSceneObject::OnLeaveMap(idTargetMap);

    m_EventMap.Clear();
    m_EventQueue.Clear();

    __LEAVE_FUNCTION
}

uint32_t CActor::GetMapID() const
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

void CActor::SendRoomMessage(const google::protobuf::Message& msg, bool bIncludeSelf /*= true*/)
{
    __ENTER_FUNCTION
    SendRoomMessage(to_sc_cmd(msg), msg, bIncludeSelf);
    __LEAVE_FUNCTION
}

void CActor::SendRoomMessage(uint16_t cmd, const google::protobuf::Message& msg, bool bIncludeSelf /*= true*/)
{
    __ENTER_FUNCTION
    SendShowToDealyList();
    auto setSocketMap = ZoneService()->IDList2VSMap(m_ViewActorsByType[ACT_PLAYER], (bIncludeSelf) ?  GetID() : 0 );
    ZoneService()->SendMsgTo(setSocketMap, cmd, msg);
    // send message to ai_service
    if((IsMonster() || IsPlayer()) &&
       (cmd == CMD_SC_AOI_UPDATE || cmd == CMD_SC_CASTSKILL || cmd == CMD_SC_ATTRIB_CHANGE))
    {
        ZoneService()->SendMsgToAIService(cmd, msg);
    }

    __LEAVE_FUNCTION
}

void CActor::SendWorldMessage(uint16_t cmd, const google::protobuf::Message& msg)
{
    __ENTER_FUNCTION
    
    if(GetWorldID() != 0)
    {
        ZoneService()->SendMsgToWorld(GetWorldID(), cmd, msg);
    }

    __LEAVE_FUNCTION
}
