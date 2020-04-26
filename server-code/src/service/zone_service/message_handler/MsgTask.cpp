#include <regex>

#include "ActorManager.h"
#include "MsgZoneProcess.h"
#include "Npc.h"
#include "Phase.h"
#include "Player.h"
#include "ZoneService.h"
#include "msg/ts_cmd.pb.h"
#include "msg/zone_service.pb.h"

ON_PLAYERMSG(CS_TASK_SUBMIT)
{
    __ENTER_FUNCTION
    if(pPlayer->GetCurrentScene() == nullptr)
        return;

    if(pPlayer->IsDead() == false)
        return;

    pPlayer->GetTaskSet()->SubmitTaskByMessage(msg.task_id(), msg.submit_multiple());
    __LEAVE_FUNCTION
}

ON_PLAYERMSG(CS_ACTIVE_NPC)
{
    __ENTER_FUNCTION
    if(pPlayer->GetCurrentScene() == nullptr)
        return;

    if(pPlayer->IsDead() == false)
        return;

    if(pPlayer->IsInViewActorByID(msg.npc_id()) == false)
        return;

    CActor* pActor = ActorManager()->QueryActor(msg.npc_id());
    if(pActor->IsNpc() == false)
        return;

    if(GameMath::distance(pActor->GetPos(), pPlayer->GetPos()) < MIN_INTERACT_DIS)
        return;

    CNpc* pNpc = pActor->CastTo<CNpc>();
    pNpc->ActiveNpc(pPlayer);
    __LEAVE_FUNCTION
}

ON_PLAYERMSG(CS_DIALOG_CLICK)
{
    __ENTER_FUNCTION
    if(pPlayer->GetCurrentScene() == nullptr)
        return;

    if(pPlayer->IsDead() == false)
        return;

    pPlayer->OnDialogClick(msg.dialog_id(), msg.dialog_button_idx());
    __LEAVE_FUNCTION
}
