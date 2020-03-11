#include "MsgPlayerProcess.h"

#include <regex>

#include "Player.h"
#include "ZoneService.h"
#include "msg/ts_cmd.pb.h"
#include "msg/zone_service.pb.h"

DEFINE_MSG_PROCESS(CS_CHANGE_PKMODE)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;

	if(pPlayer->GetCurrentScene()->GetMap()->HasMapFlag(MAPFLAG_DISABLE_CHANGEPK) == false)
		return;

	pPlayer->SetPKMode(msg.pkmode());

	__LEAVE_FUNCTION
}

DEFINE_MSG_PROCESS(CS_ACHI_TAKE)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;

	if(pPlayer->IsDead() == false)
		return;

	pPlayer->GetAchievement()->TakeReward(msg.achi_id());
	__LEAVE_FUNCTION
}

//////////////////////////////////////////////////////////////////////////
PlayerMsgRegisterMgr::ProcessMap_t PlayerMsgRegisterMgr::s_ProcessMap;
void							   PlayerMessageHandlerRegister()
{
	__ENTER_FUNCTION

	auto pNetMsgProcess = ZoneService()->GetNetMsgProcess();
	using namespace std::placeholders;

	for(auto& [key, value]: PlayerMsgRegisterMgr::s_ProcessMap)
	{
		pNetMsgProcess->Register(key, std::move(value));
		value = nullptr;
	}
	PlayerMsgRegisterMgr::s_ProcessMap.clear();

	__LEAVE_FUNCTION
}
