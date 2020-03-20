#include <regex>

#include "MsgZoneProcess.h"
#include "Player.h"
#include "ZoneService.h"
#include "msg/ts_cmd.pb.h"
#include "msg/zone_service.pb.h"

void OnMsg_CS_CASTSKILL(CPlayer* pPlayer, const CS_CASTSKILL& msg, CNetworkMessage* pMsg)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;

	if(pPlayer->GetCurrentScene()->GetMapID() != msg.mapid())
		return;

	if(pPlayer->GetCurrentScene()->GetMap()->HasMapFlag(MAPFLAG_DISABLE_ATTACK) == false)
		return;

	pPlayer->GetSkillManager()->CastSkill(msg.target_id(), msg.skill_id(), Vector2(msg.x(), msg.y()));

	__LEAVE_FUNCTION
}

void OnMsg_CS_SKILL_BREAK(CPlayer* pPlayer, const CS_SKILL_BREAK& msg, CNetworkMessage* pMsg)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;
	if(pPlayer->GetCurrentScene()->GetMapID() != msg.mapid())
		return;

	if(pPlayer->GetCurrentScene()->GetMap()->HasMapFlag(MAPFLAG_DISABLE_ATTACK) == false)
		return;

	pPlayer->GetSkillFSM().BreakIntone();
	pPlayer->GetSkillFSM().BreakLaunch();

	__LEAVE_FUNCTION
}













/////////////////////////////////////////////////////////////////////////

void ZoneSkillMessageHandlerRegister()
{
	__ENTER_FUNCTION

	auto pNetMsgProcess = ZoneService()->GetNetMsgProcess();
#define REGISTER_MSG(MsgT) pNetMsgProcess->Register(CMD_##MsgT, std::bind(&ProcPlayerMsg<MsgT, decltype(OnMsg_##MsgT)>, std::placeholders::_1, &OnMsg_##MsgT) );

	REGISTER_MSG(CS_SKILL_BREAK);
	REGISTER_MSG(CS_CASTSKILL);


#undef REGISTER_MSG
	__LEAVE_FUNCTION
}
