#include <regex>

#include "MsgZoneProcess.h"
#include "Player.h"
#include "ZoneService.h"
#include "msg/ts_cmd.pb.h"
#include "msg/zone_service.pb.h"

void OnMsg_CS_TEAMCREATE(CPlayer* pPlayer, const CS_TEAMCREATE& msg, CNetworkMessage* pMsg)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;

	pPlayer->TeamCreate();

	__LEAVE_FUNCTION
}

void OnMsg_CS_TEAMQUIT(CPlayer* pPlayer, const CS_TEAMQUIT& msg, CNetworkMessage* pMsg)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;
	pPlayer->TeamQuit();

	__LEAVE_FUNCTION
}

void OnMsg_CS_TEAMKICKMEMBER(CPlayer* pPlayer, const CS_TEAMKICKMEMBER& msg, CNetworkMessage* pMsg)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;
	pPlayer->TeamKickMember(msg.member_id());

	__LEAVE_FUNCTION
}

void OnMsg_CS_TEAMINVITEMEMBER(CPlayer* pPlayer, const CS_TEAMINVITEMEMBER& msg, CNetworkMessage* pMsg)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;

	pPlayer->TeamInviteMember(msg.invitee_id());

	__LEAVE_FUNCTION
}

void OnMsg_CS_TEAMACCEPTINVITE(CPlayer* pPlayer, const CS_TEAMACCEPTINVITE& msg, CNetworkMessage* pMsg)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;

	pPlayer->TeamAcceptInvite(msg.team_id(), msg.inviter_id(), msg.result());

	__LEAVE_FUNCTION
}

void OnMsg_CS_TEAMAPPLYMEMBER(CPlayer* pPlayer, const CS_TEAMAPPLYMEMBER& msg, CNetworkMessage* pMsg)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;

	pPlayer->TeamApplyMember(msg.respondent_id());

	__LEAVE_FUNCTION
}

void OnMsg_CS_TEAMACCEPTAPPLY(CPlayer* pPlayer, const CS_TEAMACCEPTAPPLY& msg, CNetworkMessage* pMsg)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;

	pPlayer->TeamAcceptApply(msg.applicant_id(), msg.result());

	__LEAVE_FUNCTION
}

void OnMsg_CS_TEAMNEWLEADER(CPlayer* pPlayer, const CS_TEAMNEWLEADER& msg, CNetworkMessage* pMsg)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;

	pPlayer->TeamChangeLeader(msg.new_leader_id());

	__LEAVE_FUNCTION
}





/////////////////////////////////////////////////////////////////////////

void ZoneTeamMessageHandlerRegister()
{
	__ENTER_FUNCTION

	auto pNetMsgProcess = ZoneService()->GetNetMsgProcess();
#define REGISTER_MSG(MsgT) pNetMsgProcess->Register(CMD_##MsgT, std::bind(&ProcPlayerMsg<MsgT, decltype(OnMsg_##MsgT)>, std::placeholders::_1, &OnMsg_##MsgT) );

	REGISTER_MSG(CS_TEAMCREATE);
	REGISTER_MSG(CS_TEAMQUIT);
	REGISTER_MSG(CS_TEAMKICKMEMBER);
	REGISTER_MSG(CS_TEAMINVITEMEMBER);
	REGISTER_MSG(CS_TEAMACCEPTINVITE);
	
	REGISTER_MSG(CS_TEAMAPPLYMEMBER);
	REGISTER_MSG(CS_TEAMACCEPTAPPLY);
	REGISTER_MSG(CS_TEAMNEWLEADER);


#undef REGISTER_MSG
	__LEAVE_FUNCTION
}
