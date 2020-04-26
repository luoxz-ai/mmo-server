#include "Monster.h"
#include "MsgZoneProcess.h"
#include "Phase.h"
#include "Player.h"
#include "SceneManager.h"
#include "ZoneService.h"
#include "msg/ts_cmd.pb.h"
#include "msg/zone_service.pb.h"
#include "server_msg/server_side.pb.h"

ON_SERVERMSG(CZoneService, MonsterGen)
{
    CPhase* pPhase = SceneManager()->QueryPhase(msg.scene_id());
    CHECK_FMT(pPhase, "msg.scene_id:{}", msg.scene_id());

    CreateMonsterParam param;
    param.idMonsterType = msg.monster_type();
    param.idGen         = msg.gen_id();
    param.idPhase       = msg.phase_id();
    param.idCamp        = msg.camp_id();
    param.idOwner       = 0;
    param.pos           = Vector2(msg.posx(), msg.posy());
    param.face          = random_float(0.0f, 1.0f);

    CMonster* pMonster = pPhase->CreateMonster(param);
    CHECK(pMonster);
}

ON_SERVERMSG(CZoneService, MonsterGenMulti)
{
    CPhase* pPhase = SceneManager()->QueryPhase(msg.scene_id());
    CHECK(pPhase);

    CreateMonsterParam param;
    param.idMonsterType = msg.monster_type();
    param.idGen         = msg.gen_id();
    param.idPhase       = msg.phase_id();
    param.idCamp        = msg.camp_id();
    param.idOwner       = 0;
    param.pos           = Vector2(msg.posx(), msg.posy());
    param.face          = random_float(0.0f, 1.0f);

    bool bSucc = pPhase->CreateMultiMonster(param, msg.num(), msg.range());
    CHECK(bSucc);
}

ON_SERVERMSG(CZoneService, MonsterDestory)
{
    for(int32_t i = 0; i < msg.monster_id_size(); i++)
    {
        ActorManager()->DelActorByID(msg.monster_id(i));
    }
}

ON_SERVERMSG(CZoneService, ActorMove)
{
    CActor* pActor = ActorManager()->QueryActor(msg.actor_id());
    if(pActor == nullptr)
        return;
    pActor->MoveTo(Vector2(msg.x(), msg.y()));
    pActor->FaceTo(Vector2(msg.x(), msg.y()));
}

ON_SERVERMSG(CZoneService, ActorCastSkill)
{
    CActor* pActor = ActorManager()->QueryActor(msg.actor_id());
    if(pActor == nullptr)
        return;
    if(pActor->_CastSkill(msg.skill_id(), msg.target_id(), pActor->GetPos()) == false)
    {
        ServerMSG::ActorCastSkill_Fail send_msg;
        send_msg.set_actor_id(msg.actor_id());
        ZoneService()->SendServerMsgToAIService(send_msg);
    }
}

ON_SERVERMSG(CZoneService, ServiceReady)
{
    // send message to world, notify zone ready
    if(ZoneService()->IsSharedZone() == false)
    {
        ServerMSG::ServiceReady msg;
        msg.set_serverport(ZoneService()->GetServerPort());

        ZoneService()->SendMsgToWorld(ZoneService()->GetWorldID(), ServerMSG::MsgID_ServiceReady, msg);
    }
}