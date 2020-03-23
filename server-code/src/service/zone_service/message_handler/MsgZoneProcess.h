#ifndef MSGZONEPROCESS_H
#define MSGZONEPROCESS_H

#include <array>
#include <functional>
#include <memory>
#include <utility>

#include "BaseCode.h"
#include "NetworkMessage.h"
#include "Player.h"
#include "ZoneService.h"
#include "server_msg/server_side.pb.h"

class CNetworkMessage;

template<class MsgType, class FuncType>
void ProcPlayerMsg(CNetworkMessage* pMsg, FuncType func)
{
    __ENTER_FUNCTION

    CPlayer* pPlayer = ZoneService()->GetActorManager()->QueryPlayer(pMsg->GetFrom());
    CHECK(pPlayer);
    MsgType msg;
    if(msg.ParseFromArray(pMsg->GetMsgBody(), pMsg->GetBodySize()) == false)
    {
        return;
    }
    std::invoke(func, pPlayer, msg, pMsg);

    __LEAVE_FUNCTION
}

template<class T, class FuncType>
void ProcessMsg(CNetworkMessage* pMsg, FuncType func)
{
    __ENTER_FUNCTION

    T msg;
    if(msg.ParseFromArray(pMsg->GetMsgBody(), pMsg->GetBodySize()) == false)
        return;
    std::invoke(func, msg, pMsg);

    __LEAVE_FUNCTION
}

void ZoneItemMessageHandlerRegister();
void ZoneMapMessageHandlerRegister();
void ZoneSkillMessageHandlerRegister();
void ZoneTaskMessageHandlerRegister();
void ZoneTeamMessageHandlerRegister();
void ZonePlayerMessageHandlerRegister();

#endif /* MSGZONEPROCESS_H */
