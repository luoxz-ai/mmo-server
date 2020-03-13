#include "MsgZoneProcess.h"

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
ZoneMsgRegisterMgr::ProcessMap_t ZoneMsgRegisterMgr::s_ProcessMap;

void ZoneMessageHandlerRegister()
{
	__ENTER_FUNCTION

	auto pNetMsgProcess = ZoneService()->GetNetMsgProcess();
	using namespace std::placeholders;

	for(int32_t i = 0; i < ZoneMsgRegisterMgr::s_ProcessMap.size(); i++)
	{
		auto& func = ZoneMsgRegisterMgr::s_ProcessMap[i];
		if(func)
		{
			pNetMsgProcess->Register(i, std::move(func));
			LOGDEBUG("RegisterMsgProc:{}", i);
		}
		func = nullptr;
	}
	

	__LEAVE_FUNCTION
}
