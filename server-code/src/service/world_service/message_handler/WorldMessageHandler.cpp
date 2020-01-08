#include "WorldService.h"
#include "User.h"
#include "UserManager.h"
#include "zone_service.pb.h"

void OnMsgPlayerChangeZone(CNetworkMessage* pMsg)
{
	ServerMSG::PlayerChangeZone msg;
	if (msg.ParseFromArray(pMsg->GetMsgBody(), pMsg->GetBodySize()) == false)
	{
		return;
	}


	CUser* pUser = UserManager()->QueryUser(msg.socket());
	if (pUser == nullptr)
	{
		return;
	}

	pUser->OnChangeZone(msg.idzone());

	//将消息直接转发给对应的zone
	pMsg->SetTo(ServerPort(WorldService()->GetWorldID(), msg.idzone()));
	WorldService()->SendMsg(*pMsg);

}


void OnMsgPlayerChangeZone_Data(CNetworkMessage* pMsg)
{
	ServerMSG::PlayerChangeZone_Data msg;
	if (msg.ParseFromArray(pMsg->GetMsgBody(), pMsg->GetBodySize()) == false)
	{
		return;
	}


	CUser* pUser = UserManager()->QueryUser(msg.socket());
	if (pUser == nullptr)
	{
		return;
	}

	//将消息直接转发给对应的zone
	pMsg->SetTo(ServerPort(WorldService()->GetWorldID(), pUser->GetZoneID()));
	WorldService()->SendMsg(*pMsg);

}

void OnMsgServiceReady(CNetworkMessage* pMsg)
{
	ServerMSG::ServiceReady msg;
	if (msg.ParseFromArray(pMsg->GetMsgBody(), pMsg->GetBodySize()) == false)
	{
		return;
	}

	WorldService()->SetServiceReady(ServerPort(msg.serverport()).GetServiceID());
	

}

void OnMsgServiceCmd(CNetworkMessage* pMsg)
{
	ServerMSG::ServiceCmd msg;
	if (msg.ParseFromArray(pMsg->GetMsgBody(), pMsg->GetBodySize()) == false)
	{
		return;
	}

	LOGDEBUG("ServiceCmd recv, cmd:{}", msg.cmds(0).c_str());
	

}

void OnMsg_CMD_SC_TALK(CNetworkMessage* pMsg)
{
	SC_TALK msg;
	if (msg.ParseFromArray(pMsg->GetMsgBody(), pMsg->GetBodySize()) == false)
	{
		return;
	}

	

	switch(msg.channel())
	{
	case CHANNEL_PRIVATE:	//私聊
		{
			if(msg.reciver_id() )
			{
				return;
			}

			CUser* pUser = UserManager()->QueryUser(msg.reciver_id());
			if(pUser == nullptr)
			{
				//send player not find to sender
				return;
			}

			WorldService()->SendToVirtualSocket(pUser->GetSocket(), CMD_SC_TALK, msg);
		}
		break;
	case CHANNEL_TEAM:		//组队
		{
			//auto pTeam = TeamManager()->QueryTeam(pSender->GetTeamID());
			//pTeam->BroadcastToAllMember(CMD_SC_TALK, msg);
		}
		break;
	case CHANNEL_GUILD:		//公会
		{
			//auto pGuild = GuildManager()->QueryGuild(pSender->GetGuildID());
			//pGuild->BroadcastToAllMember(CMD_SC_TALK, msg);
		}
		break;
	case CHANNEL_WORLD:		//世界
	case CHANNEL_TRUMPET:	//小喇叭
		{
			WorldService()->BroadcastToAllPlayer(CMD_SC_TALK, msg);
		}
		break;
	case CHANNEL_GLOBAL:	//全游戏
	case CHANNEL_SYSTEM:	//系统
	case CHANNEL_RUMOR:		//广播
		{
			WorldService()->BroadcastToAllPlayer(CMD_SC_TALK, msg);
		}
		break;
	default:
		break;
	}
	

}

void RegisterWorldMessageHandler()
{
	using namespace std::placeholders;
	WorldService()->GetNetMsgProcess()->Register(ServerMSG::MsgID_ServiceCmd, std::bind(&OnMsgServiceCmd, _1));
	WorldService()->GetNetMsgProcess()->Register(ServerMSG::MsgID_ServiceReady, std::bind(&OnMsgServiceReady, _1));
	WorldService()->GetNetMsgProcess()->Register(ServerMSG::MsgID_PlayerChangeZone, std::bind(&OnMsgPlayerChangeZone, _1));
	WorldService()->GetNetMsgProcess()->Register(ServerMSG::MsgID_PlayerChangeZone_Data, std::bind(&OnMsgPlayerChangeZone_Data, _1));
	WorldService()->GetNetMsgProcess()->Register(CMD_SC_TALK, std::bind(&OnMsg_CMD_SC_TALK, _1));
}
