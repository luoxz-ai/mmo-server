#ifndef MSGPROCESSREGISTER_H
#define MSGPROCESSREGISTER_H


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

template<class Service>
struct MsgProcRegCenter
{
    static MsgProcRegCenter<Service>& instance()
    {
        static MsgProcRegCenter<Service> s_instance;
        return s_instance;
    }
    void reg(int cmd, MsgProcessFunc&& func)
    {
        CHECK_FMT(m_MsgProc.find(cmd) == m_MsgProc.end(), "dup register msg:{}", cmd);
        m_MsgProc.emplace(cmd, std::move(func));
    }
    std::unordered_map<uint32_t, MsgProcessFunc> m_MsgProc;
};

template<class Service>
struct MsgProcRegister
{
    MsgProcRegister(int cmd, MsgProcessFunc&& func)
    {
        MsgProcRegCenter<Service>::instance().reg(cmd, std::move(func));
    }
};

#define ON_MSG(Service,MsgType)                                                                                       \
    void OnMsg_##MsgType(const MsgType& msg, CNetworkMessage* pMsg);                                          \
                                                                                                              \
    MsgProcRegister<Service> register_##MsgType(                                                                  \
        CMD_##MsgType,                                                                                        \
        std::bind(&ProcessMsg<MsgType, decltype(OnMsg_##MsgType)>, std::placeholders::_1, &OnMsg_##MsgType)); \
                                                                                                              \
    void OnMsg_##MsgType(const MsgType& msg, CNetworkMessage* pMsg)

#define ON_SERVERMSG(Service, MsgType)                                                                                     \
    void OnMsg_##MsgType(const ServerMSG::MsgType& msg, CNetworkMessage* pMsg);                                   \
                                                                                                                  \
    MsgProcRegister<Service> register_##MsgType(ServerMSG::MsgID_##MsgType,                                           \
                                            std::bind(&ProcessMsg<ServerMSG::MsgType, decltype(OnMsg_##MsgType)>, \
                                                      std::placeholders::_1,                                      \
                                                      &OnMsg_##MsgType));                                         \
                                                                                                                  \
    void OnMsg_##MsgType(const ServerMSG::MsgType& msg, CNetworkMessage* pMsg)

#define ON_RAWMSG(Service, MsgID, MsgType)                                                \
    void OnMsg_##MsgType(CNetworkMessage* pMsg);                                 \
                                                                                 \
    MsgProcRegister<Service> register_##MsgType(MsgID, std::bind(&OnMsg_##MsgType, std::placeholders::_1)); \
                                                                                 \
    void OnMsg_##MsgType(CNetworkMessage* pMsg)


#endif /* MSGPROCESSREGISTER_H */
