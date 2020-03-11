#include "User.h"
#include "UserManager.h"
#include "WorldService.h"
#include "MsgWorldProcess.h"
#include "server_msg/server_side.pb.h"

DEFINE_SERVERSIDE_MSG_PROCESS(PlayerChangeZone)
{
	CUser* pUser = UserManager()->QueryUser(msg.socket());
	if(pUser == nullptr)
	{
		return;
	}

	pUser->OnChangeZone(msg.idzone());

	//将消息直接转发给对应的zone
	pMsg->SetTo(ServerPort(WorldService()->GetWorldID(), msg.idzone()));
	WorldService()->SendMsg(*pMsg);
}

DEFINE_SERVERSIDE_MSG_PROCESS(PlayerChangeZone_Data)
{
	CUser* pUser = UserManager()->QueryUser(msg.socket());
	if(pUser == nullptr)
	{
		return;
	}

	//将消息直接转发给对应的zone
	pMsg->SetTo(ServerPort(WorldService()->GetWorldID(), pUser->GetZoneID()));
	WorldService()->SendMsg(*pMsg);
}

DEFINE_SERVERSIDE_MSG_PROCESS(ServiceReady)
{
	WorldService()->SetServiceReady(ServerPort(msg.serverport()).GetServiceID());
}

DEFINE_SERVERSIDE_MSG_PROCESS(ServiceCmd)
{
	LOGDEBUG("ServiceCmd recv, cmd:{}", msg.cmds(0).c_str());
}



////////////////////////////////////////////////////////////////////////// 
WorldMsgRegisterMgr::ProcessMap_t WorldMsgRegisterMgr::s_ProcessMap;
void WorldMessageHandlerRegister()
{
	__ENTER_FUNCTION

	auto pNetMsgProcess = WorldService()->GetNetMsgProcess();
	using namespace std::placeholders;
	
	for(auto& [key, value] : WorldMsgRegisterMgr::s_ProcessMap)
	{
		pNetMsgProcess->Register(key, std::move(value));
		value = nullptr;
	}
	WorldMsgRegisterMgr::s_ProcessMap.clear();

	__LEAVE_FUNCTION      
}