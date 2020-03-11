#include <regex>

#include "MsgPlayerProcess.h"
#include "msg/ts_cmd.pb.h"
#include "msg/zone_service.pb.h"
#include "Player.h"
#include "Npc.h"
#include "ZoneService.h"

DEFINE_MSG_PROCESS(CS_TASK_SUBMIT)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;

	if(pPlayer->IsDead() == false)
		return;

	pPlayer->GetTaskSet()->SubmitTaskByMessage(msg.task_id(), msg.submit_multiple());
	__LEAVE_FUNCTION
}

DEFINE_MSG_PROCESS(CS_ACTIVE_NPC)
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

	CNpc* pNpc = pActor->ConvertToDerived<CNpc>();
	pNpc->ActiveNpc(pPlayer);
	__LEAVE_FUNCTION
}

DEFINE_MSG_PROCESS(CS_DIALOG_CLICK)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;

	if(pPlayer->IsDead() == false)
		return;

	pPlayer->OnDialogClick(msg.dialog_id(), msg.dialog_button_idx());
	__LEAVE_FUNCTION
}