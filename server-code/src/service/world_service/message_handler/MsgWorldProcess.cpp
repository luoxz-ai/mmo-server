#include "MsgWorldProcess.h"

#include "User.h"
#include "UserManager.h"
#include "WorldService.h"
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

void RegisterWorldMessageHandler()
{
	__ENTER_FUNCTION

	auto pNetMsgProcess = WorldService()->GetNetMsgProcess();
	using namespace std::placeholders;

	for(int32_t i = 0; i < WorldMsgRegisterMgr::s_ProcessMap.size(); i++)
	{
		auto& func = WorldMsgRegisterMgr::s_ProcessMap[i];
		if(func)
		{
			pNetMsgProcess->Register(i, std::move(func));
		}
		func = nullptr;
	}


	__LEAVE_FUNCTION
}