#include "AIMessageHandler.h"

#include "AIActor.h"
#include "AIActorManager.h"
#include "AIMonster.h"
#include "AIPlayer.h"
#include "AIService.h"
#include "NetMSGProcess.h"

#include "msg/zone_service.pb.h"
#include "msg/world_service.pb.h"
#include "server_msg/server_side.pb.h"

ON_MSG(SC_AOI_NEW)
{
 
    
}

ON_MSG(SC_AOI_REMOVE)
{

}

ON_MSG(SC_AOI_UPDATE)
{
    CAIActor* pActor = AIActorManager()->QueryActor(msg.actor_id());
    CHECK_FMT(pActor, "actorid:{}", msg.actor_id());
    CHECK(pActor->GetCurrentScene());
    CHECK(pActor->GetCurrentScene()->GetMapID() == msg.mapid());

    pActor->SetPos(Vector2(msg.posx(), msg.posy()));
    pActor->UpdateViewList();
    LOGAIDEBUG(true, "Actor:{} MoveTo {} {:.2f}, {:.2f}",
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

ON_SERVERMSG(SceneDestory)
{
    AISceneManager()->DestoryDynaScene(msg.scene_id());
}

ON_SERVERMSG(ActiveGen)
{
    CAIScene* pScene = AISceneManager()->QueryScene(msg.scene_id());
    if(pScene)
    {
        pScene->GetMonsterGen().ActiveGen(msg.gen_id(), msg.active());
    }
}

ON_SERVERMSG(KillGen)
{
    CAIScene* pScene = AISceneManager()->QueryScene(msg.scene_id());
    if(pScene)
    {
        pScene->GetMonsterGen().KillGen(msg.gen_id());
    }
}

ON_SERVERMSG(ActorCreate)
{
    CAIScene* pScene = AISceneManager()->QueryScene(msg.scene_id());
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
    if(pActor)
    {
        
        pActor->OnDead();
        if(pActor->GetCurrentScene())
            pActor->GetCurrentScene()->LeaveMap(pActor);

        AIActorManager()->DelActor(pActor);
    }
}      

ON_SERVERMSG(ActorCastSkill_Fail)
{
    CAIActor* pActor = AIActorManager()->QueryActor(msg.actor_id());
    if(pActor)
    {
        pActor->OnCastSkillFinish();
    }
}

ON_SERVERMSG(ActorSetHide)
{
    CAIActor* pActor = AIActorManager()->QueryActor(msg.actor_id());
    if(pActor)
    {
        pActor->SetHideCoude(msg.hide_count());
    }
}  

void AIServiceMessageHandlerRegister()
{
    auto pNetMsgProcess = AIService()->GetNetMsgProcess();
  
    for(const auto& [k,v] : AIMsgProcRegCenter::instance().m_MsgProc)
    {
        pNetMsgProcess->Register(k,v);
    }
}
