#include "User.h"
#include "UserManager.h"
#include "WorldService.h"
#include "MsgWorldProcess.h"
#include "msg/zone_service.pb.h"

DEFINE_MSG_PROCESS(SC_TALK)
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