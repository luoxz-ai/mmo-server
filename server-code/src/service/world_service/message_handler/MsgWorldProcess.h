#ifndef MSGWORLDPROCESS_H
#define MSGWORLDPROCESS_H



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

struct WorldMsgProcRegCenter
{
    static WorldMsgProcRegCenter& instance()
    {
        static WorldMsgProcRegCenter s_instance;
        return s_instance;
    }
    void reg(int cmd, MsgProcessFunc&& func)
    {
        m_MsgProc.emplace(cmd, std::move(func));
    }
    std::unordered_map<uint32_t, MsgProcessFunc> m_MsgProc;
};

struct WorldMsgProcRegister
{
    WorldMsgProcRegister(int cmd, MsgProcessFunc&& func)
    {
        WorldMsgProcRegCenter::instance().reg(cmd, std::move(func));
    }
};

#define ON_MSG(MsgType)                                                                                       \
    void OnMsg_##MsgType(const MsgType& msg, CNetworkMessage* pMsg);                                          \
                                                                                                              \
    WorldMsgProcRegister register_##MsgType(                                                                  \
        CMD_##MsgType,                                                                                        \
        std::bind(&ProcessMsg<MsgType, decltype(OnMsg_##MsgType)>, std::placeholders::_1, &OnMsg_##MsgType)); \
                                                                                                              \
    void OnMsg_##MsgType(const MsgType& msg, CNetworkMessage* pMsg)

#define ON_SERVERMSG(MsgType)                                                                                     \
    void OnMsg_##MsgType(const ServerMSG::MsgType& msg, CNetworkMessage* pMsg);                                   \
                                                                                                                  \
    WorldMsgProcRegister register_##MsgType(ServerMSG::MsgID_##MsgType,                                           \
                                            std::bind(&ProcessMsg<ServerMSG::MsgType, decltype(OnMsg_##MsgType)>, \
                                                      std::placeholders::_1,                                      \
                                                      &OnMsg_##MsgType));                                         \
                                                                                                                  \
    void OnMsg_##MsgType(const ServerMSG::MsgType& msg, CNetworkMessage* pMsg)

#define ON_RAWMSG(MsgType)                                                               \
    void OnMsg_##MsgType(CNetworkMessage* pMsg);                                         \
                                                                                         \
    WorldMsgProcRegister register_##MsgType(CMD_##MsgType, std::bind(&OnMsg_##MsgType)); \
                                                                                         \
    void OnMsg_##MsgType(CNetworkMessage* pMsg)

#endif /* MSGWORLDPROCESS_H */
