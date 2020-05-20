#include "MsgWorldProcess.h"

#include "User.h"
#include "UserManager.h"
#include "AccountManager.h"
#include "WorldService.h"
#include "msg/world_service.pb.h"
#include "msg/zone_service.pb.h"
#include "server_msg/server_side.pb.h"


ON_MSG(CWorldService, SC_TALK)
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

            WorldService()->SendToVirtualSocket(pUser->GetSocket(), msg);
        }
        break;
        case CHANNEL_TEAM: //组队
        {
            // auto pTeam = TeamManager()->QueryTeam(pSender->GetTeamID());
            // pTeam->BroadcastToAllMember(msg);
        }
        break;
        case CHANNEL_GUILD: //公会
        {
            // auto pGuild = GuildManager()->QueryGuild(pSender->GetGuildID());
            // pGuild->BroadcastToAllMember(msg);
        }
        break;
        case CHANNEL_WORLD:   //世界
        case CHANNEL_TRUMPET: //小喇叭
        {
            WorldService()->BroadcastToAllPlayer(msg);
        }
        break;
        case CHANNEL_GLOBAL: //全游戏
        case CHANNEL_SYSTEM: //系统
        case CHANNEL_RUMOR:  //广播
        {
            WorldService()->BroadcastToAllPlayer(msg);
        }
        break;
        default:
            break;
    }
}

ON_SERVERMSG(CWorldService, PlayerChangeZone)
{
    CUser* pUser = UserManager()->QueryUser(msg.socket());
    if(pUser == nullptr)
    {
        return;
    }

    pUser->OnChangeZone(msg.idzone());

    //将消息直接转发给对应的zone
    pMsg->SetTo(ServerPort(WorldService()->GetWorldID(), msg.idzone()));
    WorldService()->SendPortMsg(*pMsg);
}

ON_SERVERMSG(CWorldService, PlayerChangeZone_Data)
{
    CUser* pUser = UserManager()->QueryUser(msg.socket());
    if(pUser == nullptr)
    {
        return;
    }

    //将消息直接转发给对应的zone
    pMsg->SetTo(ServerPort(WorldService()->GetWorldID(), pUser->GetZoneID()));
    WorldService()->SendPortMsg(*pMsg);
}

ON_SERVERMSG(CWorldService, ServiceReady)
{
    WorldService()->SetServiceReady(ServerPort(msg.serverport()).GetServiceID());
}

ON_SERVERMSG(CWorldService, ServiceCmd)
{
    LOGDEBUG("ServiceCmd recv, cmd:{}", msg.cmds(0).c_str());
}

ON_SERVERMSG(CWorldService, SocketConnect) {}

ON_SERVERMSG(CWorldService, SocketClose) 
{
    auto pUser= UserManager()->QueryUserBySocket(msg.vs());
    if(pUser)
    {
        pUser->Logout();
        UserManager()->RemoveUser(pUser);
    }

    AccountManager()->Logout(msg.vs());
}

//////////////////////////////////////////////////////////////////////////
void RegisterWorldMessageHandler()
{
    __ENTER_FUNCTION

    auto pNetMsgProcess = WorldService()->GetNetMsgProcess();
    for(const auto& [k, v]: MsgProcRegCenter<CWorldService>::instance().m_MsgProc)
    {
        pNetMsgProcess->Register(k, std::get<0>(v), std::get<1>(v));
    }

    __LEAVE_FUNCTION
}