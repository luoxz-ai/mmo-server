#include "MsgZoneProcess.h"
#include "Player.h"
#include "ZoneService.h"
#include "msg/ts_cmd.pb.h"
#include "msg/zone_service.pb.h"

ON_SERVERMSG(MonsterGen)
{
    CScene* pScene = SceneManager()->QueryScene(msg.scene_id());
    CHECK(pScene);
    CMonster* pMonster = pScene->OnMsgCreateMonster(msg.monster_type(),
                                                msg.gen_id(),
                                                msg.camp_id(),
                                                0,
                                                Vector2(msg.posx(), msg.posy()),
                                                random_float(0.0f, 1.0f));
    CHECK(pMonster);
}

ON_SERVERMSG(MonsterDestory)
{
    for(int32_t i = 0; i < msg.monster_id_size(); i++)
    {
        ActorManager()->DelActorByID(msg.monster_id(i));
    }
}

ON_SERVERMSG(ActorMove)
{
    CActor* pActor = ActorManager()->QueryActor(msg.actor_id());
    if(pActor == nullptr)
        return;
    pActor->MoveTo(Vector2(msg.x(), msg.y()));
    pActor->FaceTo(Vector2(msg.x(), msg.y()));
}

ON_SERVERMSG(ActorCastSkill)
{
    CActor* pActor = ActorManager()->QueryActor(msg.actor_id());
    if(pActor == nullptr)
        return;
    if(pActor->_CastSkill(msg.skill_id(), msg.target_id(), pActor->GetPos()) == false)
    {
        ServerMSG::ActorCastSkill_Fail send_msg;
        send_msg.set_actor_id(msg.actor_id());
        ZoneService()->SendMsgToAIService(ServerMSG::MsgID_ActorCastSkill_Fail, send_msg);
    }
}
        