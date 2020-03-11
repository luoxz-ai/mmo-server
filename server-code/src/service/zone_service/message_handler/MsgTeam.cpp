#include <regex>

#include "MsgPlayerProcess.h"
#include "msg/ts_cmd.pb.h"
#include "msg/zone_service.pb.h"
#include "Player.h"
#include "ZoneService.h"

DEFINE_MSG_PROCESS(CS_TEAMCREATE)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;

	pPlayer->TeamCreate();

	__LEAVE_FUNCTION
}

DEFINE_MSG_PROCESS(CS_TEAMQUIT)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;
	pPlayer->TeamQuit();

	__LEAVE_FUNCTION
}

DEFINE_MSG_PROCESS(CS_TEAMKICKMEMBER)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;
	pPlayer->TeamKickMember(msg.member_id());

	__LEAVE_FUNCTION
}

DEFINE_MSG_PROCESS(CS_TEAMINVITEMEMBER)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;

	pPlayer->TeamInviteMember(msg.invitee_id());

	__LEAVE_FUNCTION
}

DEFINE_MSG_PROCESS(CS_TEAMACCEPTINVITE)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;

	pPlayer->TeamAcceptInvite(msg.team_id(), msg.inviter_id(), msg.result());

	__LEAVE_FUNCTION
}

DEFINE_MSG_PROCESS(CS_TEAMAPPLYMEMBER)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;

	pPlayer->TeamApplyMember(msg.respondent_id());

	__LEAVE_FUNCTION
}

DEFINE_MSG_PROCESS(CS_TEAMACCEPTAPPLY)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;

	pPlayer->TeamAcceptApply(msg.applicant_id(), msg.result());

	__LEAVE_FUNCTION
}

DEFINE_MSG_PROCESS(CS_TEAMNEWLEADER)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;

	pPlayer->TeamChangeLeader(msg.new_leader_id());

	__LEAVE_FUNCTION
}