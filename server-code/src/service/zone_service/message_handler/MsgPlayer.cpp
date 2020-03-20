#include "Item.h"
#include "MsgZoneProcess.h"
#include "Player.h"
#include "ZoneService.h"
#include "msg/ts_cmd.pb.h"
#include "msg/zone_service.pb.h"


void OnMsg_CS_CHANGE_PKMODE(CPlayer* pPlayer, const CS_CHANGE_PKMODE& msg, CNetworkMessage* pMsg)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;

	if(pPlayer->GetCurrentScene()->GetMap()->HasMapFlag(MAPFLAG_DISABLE_CHANGEPK) == false)
		return;

	pPlayer->SetPKMode(msg.pkmode());

	__LEAVE_FUNCTION
}

void OnMsg_CS_ACHI_TAKE(CPlayer* pPlayer, const CS_ACHI_TAKE& msg, CNetworkMessage* pMsg)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;

	if(pPlayer->IsDead() == false)
		return;

	pPlayer->GetAchievement()->TakeReward(msg.achi_id());
	__LEAVE_FUNCTION
}




/////////////////////////////////////////////////////////////////////////

void ZonePlayerMessageHandlerRegister()
{
	__ENTER_FUNCTION

	auto pNetMsgProcess = ZoneService()->GetNetMsgProcess();
#define REGISTER_MSG(MsgT) pNetMsgProcess->Register(CMD_##MsgT, std::bind(&ProcPlayerMsg<MsgT, decltype(OnMsg_##MsgT)>, std::placeholders::_1, &OnMsg_##MsgT) );
#define EXTERN_MSG(MsgT) extern void OnMsg_##MsgT(CPlayer* pPlayer, const MsgT& msg, CNetworkMessage* pMsg);
	

	EXTERN_MSG(CS_TALK);
	REGISTER_MSG(CS_TALK);
	REGISTER_MSG(CS_CHANGE_PKMODE);
	REGISTER_MSG(CS_ACHI_TAKE);


#undef REGISTER_MSG
#undef EXTERN_MSG

	__LEAVE_FUNCTION
}