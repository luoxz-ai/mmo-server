#include "AIActor.h"
#include "AIActorManager.h"
#include "AIMessageHandler.h"
#include "AIMonster.h"
#include "AIPhase.h"
#include "AIPlayer.h"
#include "AIScene.h"
#include "AISceneManagr.h"
#include "AIService.h"
#include "NetMSGProcess.h"
#include "msg/world_service.pb.h"
#include "msg/zone_service.pb.h"
#include "server_msg/server_side.pb.h"


ON_MSG(CAIService, SC_AOI_NEW) {}

ON_MSG(CAIService, SC_AOI_REMOVE) {}

ON_MSG(CAIService, SC_AOI_UPDATE)
{
    __ENTER_FUNCTION
    CAIActor* pActor = AIActorManager()->QueryActor(msg.actor_id());
    CHECK_FMT(pActor, "actorid:{}", msg.actor_id());
    CHECK(pActor->GetCurrentScene());
    CHECK(pActor->GetCurrentScene()->GetMapID() == msg.mapid());

    pActor->SetPos(Vector2(msg.posx(), msg.posy()));
    pActor->UpdateViewList();
    LOGAIDEBUG(true,  pActor->GetID(),
               "Actor:{} MoveTo {} {:.2f}, {:.2f}",
               pActor->GetID(),
               pActor->GetCurrentScene()->GetMapID(),
               pActor->GetPosX(),
               pActor->GetPosY());
    __LEAVE_FUNCTION
}

ON_MSG(CAIService, SC_ATTRIB_CHANGE)
{
    __ENTER_FUNCTION
    CAIActor* pActor = AIActorManager()->QueryActor(msg.actor_id());
    CHECK(pActor);
    for(int32_t i = 0; i < msg.datalist_size(); i++)
    {
        const auto& data = msg.datalist(i);
        pActor->SetProperty(data.actype(), data.val());
    }
    __LEAVE_FUNCTION
}

ON_MSG(CAIService, SC_CASTSKILL)
{
    __ENTER_FUNCTION
    CAIActor* pActor = AIActorManager()->QueryActor(msg.actor_id());
    CHECK(pActor);
    pActor->OnCastSkill(msg.skill_id());
    __LEAVE_FUNCTION
}

ON_MSG(CAIService, SC_SKILL_STUN)
{
    __ENTER_FUNCTION
    CAIActor* pActor = AIActorManager()->QueryActor(msg.actor_id());
    CHECK(pActor);
    pActor->OnCastSkillFinish(msg.stun_ms());
    __LEAVE_FUNCTION
}

ON_MSG(CAIService, SC_SKILL_BREAK)
{
    __ENTER_FUNCTION
    CAIActor* pActor = AIActorManager()->QueryActor(msg.actor_id());
    CHECK(pActor);
    pActor->OnCastSkillFinish(0);
    __LEAVE_FUNCTION
}

ON_MSG(CAIService, SC_DAMAGE)
{
    __ENTER_FUNCTION
    CAIActor* pActor = AIActorManager()->QueryActor(msg.actor_id());
    CHECK(pActor);
    pActor->OnUnderAttack(msg.attacker_id(), msg.damage());
    __LEAVE_FUNCTION
}

ON_SERVERMSG(CAIService, SceneCreate)
{
    __ENTER_FUNCTION
    AISceneManager()->CreateScene(msg.scene_id());
    __LEAVE_FUNCTION
}

ON_SERVERMSG(CAIService, PhaseCreate)
{
    __ENTER_FUNCTION
    CAIScene* pScene = AISceneManager()->QueryScene(msg.scene_id());
    CHECK(pScene);
    pScene->CreatePhase(msg.scene_id(), msg.phase_id());
    __LEAVE_FUNCTION
}

ON_SERVERMSG(CAIService, PhaseDestory)
{
    __ENTER_FUNCTION
    CAIScene* pScene = AISceneManager()->QueryScene(msg.scene_id());
    CHECK(pScene);
    pScene->DestoryPhase(msg.phase_id());
    __LEAVE_FUNCTION
}

ON_SERVERMSG(CAIService, ActiveGen)
{
    __ENTER_FUNCTION
    CAIPhase* pScene = AISceneManager()->QueryPhase(msg.scene_id());
    CHECK(pScene);
    pScene->GetMonsterGen().ActiveGen(msg.gen_id(), msg.active());
    __LEAVE_FUNCTION
}

ON_SERVERMSG(CAIService, MonsterGenOnce)
{
    __ENTER_FUNCTION
    CAIPhase* pScene = AISceneManager()->QueryPhase(msg.scene_id());
    CHECK(pScene);
    pScene->GetMonsterGen().GenOnce(msg.gen_id(), msg.phase_id());
    __LEAVE_FUNCTION
}

ON_SERVERMSG(CAIService, KillGen)
{
    __ENTER_FUNCTION
    CAIPhase* pScene = AISceneManager()->QueryPhase(msg.scene_id());
    CHECK(pScene);
    pScene->GetMonsterGen().KillGen(msg.gen_id());
    __LEAVE_FUNCTION
}

ON_SERVERMSG(CAIService, ActorCreate)
{
    __ENTER_FUNCTION
    CAIPhase* pScene = AISceneManager()->QueryPhase(msg.scene_id());
    CHECK(pScene);
    CAIActor* pActor = AIActorManager()->QueryActor(msg.actor_id());;
    CHECK(pActor == nullptr);
    switch(msg.actortype())
    {
        case ACT_MONSTER:
        {
            pActor = CAIMonster::CreateNew(msg);
        }
        break;
        case ACT_PLAYER:
        {
            pActor = CAIPlayer::CreateNew(msg);
            LOGDEBUG("Create AIPlayer id:{} ptr:{:p}", pActor->GetID(), (void*)pActor);
        }
        break;
        case ACT_PET:
        {
        }
        break;
        case ACT_NPC:
        {
        }
        break;
    }

    if(pActor)
    {
        pScene->EnterMap(pActor, msg.posx(), msg.posy(), msg.face());
        AIActorManager()->AddActor(pActor);
        pActor->OnBorn();
    }
    __LEAVE_FUNCTION
}

ON_SERVERMSG(CAIService, ActorDestory)
{
    __ENTER_FUNCTION
    CAIActor* pActor = AIActorManager()->QueryActor(msg.actor_id());
    CHECK(pActor);
    LOGDEBUG("ActorDestory id:{} ptr:{:p}", pActor->GetID(), (void*)pActor);

    if(msg.dead())
    {
        pActor->OnDead();
    }
    
    if(pActor->GetCurrentScene())
        pActor->GetCurrentScene()->LeaveMap(pActor);

    AIActorManager()->DelActor(pActor);
    __LEAVE_FUNCTION
}

ON_SERVERMSG(CAIService, ActorCastSkill_Fail)
{
    __ENTER_FUNCTION
    CAIActor* pActor = AIActorManager()->QueryActor(msg.actor_id());
    CHECK(pActor);

    pActor->OnCastSkillFinish();
    __LEAVE_FUNCTION
}

ON_SERVERMSG(CAIService, ActorSetHide)
{
    __ENTER_FUNCTION
    CAIActor* pActor = AIActorManager()->QueryActor(msg.actor_id());
    CHECK(pActor);
    pActor->SetHideCoude(msg.hide_count());
    __LEAVE_FUNCTION
}

ON_SERVERMSG(CAIService, SyncTaskPhase)
{
    __ENTER_FUNCTION
    auto pActor = AIActorManager()->QueryActor(msg.player_id());
    CHECK(pActor);
    CAIPlayer* pPlayer = pActor->CastTo<CAIPlayer>();
    CHECK(pPlayer);
    pPlayer->ClearTaskPhase();
    for(int32_t i = 0; i < msg.task_phase_id_size(); i++)
    {
        pPlayer->AddTaskPhase(msg.task_phase_id(i));
    }
    __LEAVE_FUNCTION
}

ON_SERVERMSG(CAIService, AddTaskPhase)
{
    __ENTER_FUNCTION
    auto pActor = AIActorManager()->QueryActor(msg.player_id());
    CHECK(pActor);
    CAIPlayer* pPlayer = pActor->CastTo<CAIPlayer>();
    CHECK(pPlayer);
    pPlayer->AddTaskPhase(msg.task_phase_id());
    __LEAVE_FUNCTION
}

ON_SERVERMSG(CAIService, RemoveTaskPhase)
{
    __ENTER_FUNCTION
    auto pActor = AIActorManager()->QueryActor(msg.player_id());
    CHECK(pActor);
    CAIPlayer* pPlayer = pActor->CastTo<CAIPlayer>();
    CHECK(pPlayer);
    pPlayer->RemoveTaskPhase(msg.task_phase_id());
    __LEAVE_FUNCTION
}

void AIServiceMessageHandlerRegister()
{
    auto pNetMsgProcess = AIService()->GetNetMsgProcess();

    for(const auto& [k, v]: MsgProcRegCenter<CAIService>::instance().m_MsgProc)
    {
        pNetMsgProcess->Register(k, std::get<0>(v), std::get<1>(v));
    }
}
