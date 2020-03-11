#include "MsgPlayerProcess.h"
#include "Player.h"
#include "ZoneService.h"
#include "msg/ts_cmd.pb.h"
#include "msg/zone_service.pb.h"

DEFINE_MSG_PROCESS(CS_LOADMAP_SUCC)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() != nullptr)
		return;

	pPlayer->OnLoadMapSucc();
	__LEAVE_FUNCTION
}

DEFINE_MSG_PROCESS(CS_CHANGEMAP)
{
	__ENTER_FUNCTION

	if(pPlayer->GetCurrentScene() == nullptr)
		return;

	if(pPlayer->GetCurrentScene()->GetMapID() != msg.mapid())
		return;
	pPlayer->TryChangeMap(msg.leavepoint_idx());
	__LEAVE_FUNCTION
}

DEFINE_MSG_PROCESS(CS_MOVE)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;

	if(pPlayer->GetCurrentScene()->GetMapID() != msg.mapid())
		return;

	if(pPlayer->MoveTo(Vector2(msg.x(), msg.y())) == false)
	{
		// send kick back to client
		return;
	}
	pPlayer->SetFace(msg.face());

	__LEAVE_FUNCTION
}

DEFINE_MSG_PROCESS(CS_REBORN)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;

	if(pPlayer->IsDead() == false)
		return;

	pPlayer->Reborn(msg.reborn_type());
	__LEAVE_FUNCTION
}