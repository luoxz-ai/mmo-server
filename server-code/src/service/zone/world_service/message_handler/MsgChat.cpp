#include "GMManager.h"
#include "UserManager.h"
#include "User.h"
#include "TeamManager.h"
#include "Team.h"
#include "MD5.h"
#include "MsgWorldProcess.h"
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
            //发送给reciver所在的world，
            auto pReciver = UserManager()->QueryUser(msg.reciver_id());
            if(pReciver == nullptr)
            {
                //发送错误消息给原来的sender
                return;
            }
            pReciver->SendMsg(msg);

        }
        break;
        case CHANNEL_TEAM: //组队
        {
            //发送给自己当前的World来处理
            auto pSender = UserManager()->QueryUser(msg.sender_id());
            CHECK(pSender)
            auto pTeam = TeamManager()->QueryTeam(pSender->GetTeamID());
            CHECK(pTeam);
            pTeam->SendMsgToAllMember(msg);
            
        }
        break;
        default:
        {

        }
        break;
    }
}