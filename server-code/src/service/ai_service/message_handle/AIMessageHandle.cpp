#include "AIActor.h"
#include "AIActorManager.h"
#include "AIMessageHandler.h"
#include "AIMonster.h"
#include "AIPhase.h"
#include "AIScene.h"
#include "AIPlayer.h"
#include "AISceneManagr.h"
#include "AIService.h"
#include "NetMSGProcess.h"
#include "msg/world_service.pb.h"
#include "msg/zone_service.pb.h"
#include "server_msg/server_side.pb.h"

ON_MSG(SC_AOI_NEW) {}

ON_MSG(SC_AOI_REMOVE) {}

ON_MSG(SC_AOI_UPDATE)
{
    CAIActor* pActor = AIActorManager()->QueryActor(msg.actor_id());
    CHECK_FMT(pActor, "actorid:{}", msg.actor_id());
    CHECK(pActor->GetCurrentScene());
    CHECK(pActor->GetCurrentScene()->GetMapID() == msg.mapid());

    pActor->SetPos(Vector2(msg.posx(), msg.posy()));
    pActor->UpdateViewList();
    LOGAIDEBUG(true,
               "Actor:{} MoveTo {} {:.2f}, {:.2f}",
               pActor->GetID(),
               pActor->GetCurrentScene()->GetMapID(),
               pActor->GetPosX(),
               pActor->GetPosY());
}

ON_MSG(SC_ATTRIB_CHANGE)
{

    CAIActor* pActor = AIActorManager()->QueryActor(msg.actor_id());
    CHECK(pActor);
    for(int32_t i = 0; i < msg.datalist_size(); i++)
    {
        const auto& data = msg.datalist(i);
        pActor->SetProperty(data.actype(), data.val());
    }
}

ON_MSG(SC_CASTSKILL)
{
    CAIActor* pActor = AIActorManager()->QueryActor(msg.actor_id());
    CHECK(pActor);
    pActor->OnCastSkill(msg.skill_id());
}

ON_MSG(SC_SKILL_STUN)
{
    CAIActor* pActor = AIActorManager()->QueryActor(msg.actor_id());
    CHECK(pActor);
    pActor->OnCastSkillFinish(msg.stun_ms());
}

ON_MSG(SC_SKILL_BREAK)
{
    CAIActor* pActor = AIActorManager()->QueryActor(msg.actor_id());
    CHECK(pActor);
    pActor->OnCastSkillFinish(0);
}

ON_MSG(SC_DAMAGE)
{
    CAIActor* pActor = AIActorManager()->QueryActor(msg.actor_id());
    CHECK(pActor);
    pActor->OnUnderAttack(msg.attacker_id(), msg.damage());
}

ON_SERVERMSG(SceneCreate)
{
    AISceneManager()->CreateScene(msg.scene_id());
}

ON_SERVERMSG(PhaseCreate)
{
    CAIScene* pScene = AISceneManager()->QueryScene(msg.scene_id());
    CHECK(pScene);
    pScene->CreatePhase(msg.scene_id(), msg.phase_id());
}

ON_SERVERMSG(PhaseDestory)
{
    CAIScene* pScene = AISceneManager()->QueryScene(msg.scene_id());
    CHECK(pScene);
    pScene->DestoryPhase(msg.phase_id());
}

ON_SERVERMSG(ActiveGen)
{
    CAIPhase* pScene = AISceneManager()->QueryPhase(msg.scene_id());
    CHECK(pScene);
    pScene->GetMonsterGen().ActiveGen(msg.gen_id(), msg.active());
}

ON_SERVERMSG(MonsterGenOnce)
{
    CAIPhase* pScene = AISceneManager()->QueryPhase(msg.scene_id());
    CHECK(pScene);
    pScene->GetMonsterGen().GenOnce(msg.gen_id(), msg.phase_id());
}

ON_SERVERMSG(KillGen)
{
    CAIPhase* pScene = AISceneManager()->QueryPhase(msg.scene_id());
    CHECK(pScene);
    pScene->GetMonsterGen().KillGen(msg.gen_id());
}

ON_SERVERMSG(ActorCreate)
{
    CAIPhase* pScene = AISceneManager()->QueryPhase(msg.scene_id());
    CHECK(pScene);
    CAIActor* pActor = nullptr;
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
}

ON_SERVERMSG(ActorDestory)
{
    CAIActor* pActor = AIActorManager()->QueryActor(msg.actor_id());
    CHECK(pActor);

    pActor->OnDead();
    if(pActor->GetCurrentScene())
        pActor->GetCurrentScene()->LeaveMap(pActor);

    AIActorManager()->DelActor(pActor);
}

ON_SERVERMSG(ActorCastSkill_Fail)
{
    CAIActor* pActor = AIActorManager()->QueryActor(msg.actor_id());
    CHECK(pActor);

    pActor->OnCastSkillFinish();
}

ON_SERVERMSG(ActorSetHide)
{
    CAIActor* pActor = AIActorManager()->QueryActor(msg.actor_id());
    CHECK(pActor);
    pActor->SetHideCoude(msg.hide_count());
}

ON_SERVERMSG(SyncTaskPhase)
{
    auto pActor = AIActorManager()->QueryActor(msg.player_id());
    CHECK(pActor);
    CAIPlayer* pPlayer = pActor->CastTo<CAIPlayer>();
    CHECK(pPlayer);
    pPlayer->ClearTaskPhase();
    for(int i = 0; i < msg.task_phase_id_size(); i++)
    {
        pPlayer->AddTaskPhase(msg.task_phase_id(i));
    }
}


ON_SERVERMSG(AddTaskPhase)
{
    auto pActor = AIActorManager()->QueryActor(msg.player_id());
    CHECK(pActor);
    CAIPlayer* pPlayer = pActor->CastTo<CAIPlayer>();
    CHECK(pPlayer);
    pPlayer->AddTaskPhase(msg.task_phase_id());
}


ON_SERVERMSG(RemoveTaskPhase)
{
    auto pActor = AIActorManager()->QueryActor(msg.player_id());
    CHECK(pActor);
    CAIPlayer* pPlayer = pActor->CastTo<CAIPlayer>();
    CHECK(pPlayer);
    pPlayer->RemoveTaskPhase(msg.task_phase_id());
}


void AIServiceMessageHandlerRegister()
{
    auto pNetMsgProcess = AIService()->GetNetMsgProcess();

    for(const auto& [k, v]: AIMsgProcRegCenter::instance().m_MsgProc)
    {
        pNetMsgProcess->Register(k, v);
    }
}
