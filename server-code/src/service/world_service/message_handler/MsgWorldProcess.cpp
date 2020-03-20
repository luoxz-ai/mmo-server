#include "User.h"
#include "UserManager.h"
#include "WorldService.h"
#include "server_msg/server_side.pb.h"
#include "msg/zone_service.pb.h"


void OnMsg_SC_TALK(const SC_TALK& msg, CNetworkMessage* pMsg)
{

	switch(msg.channel())
	{
		case CHANNEL_PRIVATE: //私聊
		{
			if(msg.reciver_id())
			{
				return;
			}

			CUser* pUser = UserManager()->QueryUser(msg.reciver_id());
			if(pUser == nullptr)
			{
				// send player not find to sender
				return;
			}

			WorldService()->SendToVirtualSocket(pUser->GetSocket(), CMD_SC_TALK, msg);
		}
		break;
		case CHANNEL_TEAM: //组队
		{
			// auto pTeam = TeamManager()->QueryTeam(pSender->GetTeamID());
			// pTeam->BroadcastToAllMember(CMD_SC_TALK, msg);
		}
		break;
		case CHANNEL_GUILD: //公会
		{
			// auto pGuild = GuildManager()->QueryGuild(pSender->GetGuildID());
			// pGuild->BroadcastToAllMember(CMD_SC_TALK, msg);
		}
		break;
		case CHANNEL_WORLD:	  //世界
		case CHANNEL_TRUMPET: //小喇叭
		{
			WorldService()->BroadcastToAllPlayer(CMD_SC_TALK, msg);
		}
		break;
		case CHANNEL_GLOBAL: //全游戏
		case CHANNEL_SYSTEM: //系统
		case CHANNEL_RUMOR:	 //广播
		{
			WorldService()->BroadcastToAllPlayer(CMD_SC_TALK, msg);
		}
		break;
		default:
			break;
	}
}

void OnMsg_PlayerChangeZone(const ServerMSG::PlayerChangeZone& msg, CNetworkMessage* pMsg)
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

void OnMsg_PlayerChangeZone_Data(const ServerMSG::PlayerChangeZone_Data& msg, CNetworkMessage* pMsg)
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

void OnMsg_ServiceReady(const ServerMSG::ServiceReady& msg, CNetworkMessage* pMsg)
{
	WorldService()->SetServiceReady(ServerPort(msg.serverport()).GetServiceID());
}

void OnMsg_ServiceCmd(const ServerMSG::ServiceCmd& msg, CNetworkMessage* pMsg)
{
	LOGDEBUG("ServiceCmd recv, cmd:{}", msg.cmds(0).c_str());
}

template<class MsgType, class FuncType>
void ProcWorldMsg(CNetworkMessage* pMsg, FuncType func)
{
	__ENTER_FUNCTION

	MsgType msg;
	if(msg.ParseFromArray(pMsg->GetMsgBody(), pMsg->GetBodySize()) == false)
	{
		return;
	}
	std::invoke(func, msg, pMsg);

	__LEAVE_FUNCTION
}

//////////////////////////////////////////////////////////////////////////
void RegisterWorldMessageHandler()
{
	__ENTER_FUNCTION

	auto pNetMsgProcess = WorldService()->GetNetMsgProcess();
#define REGISTER_MSG(MsgT) pNetMsgProcess->Register(CMD_##MsgT, std::bind(&ProcWorldMsg<MsgT, decltype(OnMsg_##MsgT)>, std::placeholders::_1, &OnMsg_##MsgT) );
#define REGISTER_SERVERMSG(MsgT) pNetMsgProcess->Register(ServerMSG::MsgID_##MsgT, std::bind(&ProcWorldMsg<ServerMSG::MsgT, decltype(OnMsg_##MsgT)>, std::placeholders::_1, &OnMsg_##MsgT) );

	REGISTER_MSG(SC_TALK);

	REGISTER_SERVERMSG(PlayerChangeZone);
	REGISTER_SERVERMSG(PlayerChangeZone);
	REGISTER_SERVERMSG(PlayerChangeZone_Data);
	REGISTER_SERVERMSG(ServiceReady);
	REGISTER_SERVERMSG(ServiceCmd);
	

#undef REGISTER_MSG
#undef REGISTER_SERVERMSG
	__LEAVE_FUNCTION
}