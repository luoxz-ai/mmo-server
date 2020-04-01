#ifndef AIMESSAGEHANDLER_H
#define AIMESSAGEHANDLER_H

#include <functional>
#include <memory>
#include <utility>
#include <unordered_map>

#include "BaseCode.h"
#include "NetworkMessage.h"


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

using MsgProcessFunc = std::function<void(CNetworkMessage*)>;

struct AIMsgProcRegCenter
{
    static AIMsgProcRegCenter& instance()
    {
        static AIMsgProcRegCenter s_instance;
        return s_instance;
    }
    void reg(int cmd, MsgProcessFunc&& func)
    {
        CHECK_FMT(m_MsgProc.find(cmd) == m_MsgProc.end(), "dup register msg:{}", cmd);
        m_MsgProc.emplace(cmd, std::move(func));
    }
    std::unordered_map<uint32_t, MsgProcessFunc> m_MsgProc;
};

struct AIMsgProcRegister
{
    AIMsgProcRegister(int cmd, MsgProcessFunc&& func)
    {
        AIMsgProcRegCenter::instance().reg(cmd, std::move(func));
    }
};

#define ON_MSG(MsgType)                                                                                       \
    void OnMsg_##MsgType(const MsgType& msg, CNetworkMessage* pMsg);                                          \
                                                                                                              \
    AIMsgProcRegister register_##MsgType(                                                                  \
        CMD_##MsgType,                                                                                        \
        std::bind(&ProcessMsg<MsgType, decltype(OnMsg_##MsgType)>, std::placeholders::_1, &OnMsg_##MsgType)); \
                                                                                                              \
    void OnMsg_##MsgType(const MsgType& msg, CNetworkMessage* pMsg)

#define ON_SERVERMSG(MsgType)                                                                                     \
    void OnMsg_##MsgType(const ServerMSG::MsgType& msg, CNetworkMessage* pMsg);                                   \
                                                                                                                  \
    AIMsgProcRegister register_##MsgType(ServerMSG::MsgID_##MsgType,                                           \
                                            std::bind(&ProcessMsg<ServerMSG::MsgType, decltype(OnMsg_##MsgType)>, \
                                                      std::placeholders::_1,                                      \
                                                      &OnMsg_##MsgType));                                         \
                                                                                                                  \
    void OnMsg_##MsgType(const ServerMSG::MsgType& msg, CNetworkMessage* pMsg)

#define ON_RAWMSG(MsgID, MsgType)                                                                    \
    void OnMsg_##MsgType(CNetworkMessage* pMsg);                                                     \
                                                                                                     \
    AIMsgProcRegister register_##MsgType(MsgID, std::bind(&OnMsg_##MsgType, std::placeholders::_1)); \
                                                                                                     \
    void OnMsg_##MsgType(CNetworkMessage* pMsg)

#endif /* AIMESSAGEHANDLER_H */
