#include <regex>

#include "ActorManager.h"
#include "Equipment.h"
#include "GMManager.h"
#include "Item.h"
#include "MsgSceneProcess.h"
#include "Package.h"
#include "Phase.h"
#include "Player.h"
#include "Scene.h"
#include "SceneService.h"
#include "msg/zone_service.pb.h"

ON_PLAYERMSG(CS_TALK)
{
    __ENTER_FUNCTION
    if(pPlayer->GetCurrentScene() == nullptr)
        return;

    if(msg.words().empty())
        return;

    if(pPlayer->IsTalkEnable(msg.channel()) == false)
    {
        return;
    }

    uint32_t nGMLev = GMManager()->GetGMLevel(pPlayer->GetOpenID());
    if(nGMLev > 0)
    {
        // gm发言全部记录
        LOGGM("[{}]{}", pPlayer->GetName(), msg.words());
        if(msg.words()[0] == GMCMD_PREFIX)
        {
            GMManager()->ProcessGMCmd(pPlayer, msg.words());
            return;
        }
    }
    else
    {
        if(msg.words()[0] == GMCMD_PREFIX)
        {
            //非GM则屏蔽所有以GMCMD_PREFIX打头的文字
            return;
        }
    }

    std::string send_txt = msg.words();

    if(nGMLev == 0)
    {
        // 屏蔽字过滤
        replace_illegawords(send_txt);
    }
    // 物品展示信息替换
    //找到字串中[#i 0-9]，提取出来
    // Extraction of a sub-match
    static std::regex  base_regex("\\[#i ([0-9]+)\\]", std::regex::optimize);
    std::smatch        base_match;
    std::vector<OBJID> vecIDItem;
    try
    {
        std::string txt = send_txt;
        while(std::regex_search(txt, base_match, base_regex) == true)
        {
            vecIDItem.push_back(std::stoul(base_match[1].str()));
            txt = base_match.suffix();
        }
    }
    catch(...)
    {
    }

    SC_TALK send_msg;
    send_msg.set_words(send_txt);
    send_msg.set_channel(msg.channel());
    send_msg.set_sender_id(pPlayer->GetID());
    send_msg.set_sender_name(pPlayer->GetName());
    send_msg.set_sender_viplev(pPlayer->GetVipLev());
    send_msg.set_sender_sign(0); //发送者标志（是否新手辅导员，或者其他的特殊标志)
    send_msg.set_send_time(TimeGetSecond());
    send_msg.set_reciver_id(msg.reciver_id());
    //将提取出来的itemid找到iteminfo，加入msg
    for(auto idItem: vecIDItem)
    {
        CItem* pItem = pPlayer->GetBag()->QueryItem(idItem);
        if(pItem == nullptr)
            pItem = pPlayer->GetEquipmentSet()->QueryEquipmentById(idItem);

        if(pItem)
        {
            SC_ITEM_INFO* pData = send_msg.add_item_list();
            pItem->FillItemInfo(pData);
        }
    }

    switch(msg.channel())
    {
        case CHANNEL_NORMAL: //周围聊天
        {
            pPlayer->SendRoomMessage(send_msg);
        }
        break;
        case CHANNEL_SCENE: //场景内聊天
        {
            CPhase* pPhase = static_cast<CPhase*>(pPlayer->GetCurrentScene());
            CScene* pScene = pPhase->GetScene();
            pScene->ForEach([&send_msg](const CPhase* pPhase) { pPhase->SendSceneMessage(send_msg); });
        }
        break;
        case CHANNEL_PHASE: //场景内聊天
        {
            static_cast<CPhase*>(pPlayer->GetCurrentScene())->SendSceneMessage(send_msg);
        }
        break;
        case CHANNEL_PRIVATE: //私聊
        {
            //发送给reciver所在的world来转发
            auto target_worldid = GetWorldIDFromPlayerID(msg.reciver_id());
            SceneService()->SendProtoMsgToZonePort(ServerPort(target_worldid, WORLD_SERVICE, 0), send_msg);
        }
        break;
        case CHANNEL_TEAM: //组队
        {
            if(pPlayer->GetTeamID() == 0)
            {
                return;
            }
            //发送给自己当前的World来处理
            auto target_worldid = pPlayer->GetWorldID();
            SceneService()->SendProtoMsgToZonePort(ServerPort(target_worldid, WORLD_SERVICE, 0), send_msg);
        }
        break;
        case CHANNEL_GUILD: //公会
        {
            if(pPlayer->GetGuildID() == 0)
            {
                return;
            }
            //发送给GuildServer来转发
            SceneService()->SendProtoMsgToZonePort(ServerPort(0, GUILD_SERVICE, 0), send_msg);
        }
        break;
        case CHANNEL_WORLD: //世界
        {
            if(SceneService()->IsSharedZone() == true)
                return;
            //扣除消费道具

            //直接通过gate广播
            SceneService()->SendProtoMsgToAllPlayer(send_msg);
        }
        break;
        case CHANNEL_TRUMPET: //小喇叭
        {
            if(SceneService()->IsSharedZone() == true)
                return;
            //扣除消费道具

            //直接通过gate广播
            SceneService()->SendProtoMsgToAllPlayer(send_msg);
        }
        break;
        case CHANNEL_GLOBAL: //全游戏
        case CHANNEL_SYSTEM: //系统
        case CHANNEL_RUMOR:  //广播
        {
            return;
        }
        break;
        default:
            break;
    }

    __LEAVE_FUNCTION
}
