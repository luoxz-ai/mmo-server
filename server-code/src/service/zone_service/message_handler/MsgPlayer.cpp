#include "ActorManager.h"
#include "Item.h"
#include "MsgZoneProcess.h"
#include "Phase.h"
#include "Player.h"
#include "ZoneService.h"
#include "msg/ts_cmd.pb.h"
#include "msg/zone_service.pb.h"

ON_PLAYERMSG(CS_CHANGE_PKMODE)
{
    __ENTER_FUNCTION
    if(pPlayer->GetCurrentScene() == nullptr)
        return;

    if(pPlayer->GetCurrentScene()->GetMap()->HasMapFlag(MAPFLAG_DISABLE_CHANGEPK) == false)
        return;

    pPlayer->SetPKMode(msg.pkmode());

    __LEAVE_FUNCTION
}

ON_PLAYERMSG(CS_ACHI_TAKE)
{
    __ENTER_FUNCTION
    if(pPlayer->GetCurrentScene() == nullptr)
        return;

    if(pPlayer->IsDead() == false)
        return;

    pPlayer->GetAchievement()->TakeReward(msg.achi_id());
    __LEAVE_FUNCTION
}
