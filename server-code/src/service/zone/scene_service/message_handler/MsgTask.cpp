#include <regex>

#include "ActorManager.h"
#include "MsgSceneProcess.h"
#include "Npc.h"
#include "Phase.h"
#include "Player.h"
#include "PlayerDialog.h"
#include "PlayerTask.h"
#include "SceneService.h"
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
    pPlayer->GetDialog()->ActiveNpc(msg.npc_id());

    __LEAVE_FUNCTION
}

ON_PLAYERMSG(CS_DIALOG_CLICK)
{
    __ENTER_FUNCTION
    if(pPlayer->GetCurrentScene() == nullptr)
        return;

    if(pPlayer->IsDead() == false)
        return;

    pPlayer->GetDialog()->OnDialogClick(msg.dialog_id(), msg.dialog_button_idx());
    __LEAVE_FUNCTION
}
