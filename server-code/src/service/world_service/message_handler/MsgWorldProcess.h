#ifndef MSGWORLD_PROCESS_H
#define MSGWORLD_PROCESS_H

#include <functional>
#include <memory>
#include <array>

#include "BaseCode.h"
#include "NetworkMessage.h"
#include "server_msg/server_side.pb.h"

struct WorldMsgRegisterMgr
{
	using FuncType	   = std::function<void(CNetworkMessage*)>;
	using ProcessMap_t = std::array<FuncType, ServerMSG::MsgID_END>;
	static ProcessMap_t s_ProcessMap;
};

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

template<uint32_t Cmd>
struct WorldMsgRegister
{
	WorldMsgRegister(WorldMsgRegisterMgr::FuncType&& func) { WorldMsgRegisterMgr::s_ProcessMap[Cmd] = std::move(func); }
};

#define DEFINE_MSG_PROCESS(MsgType)                                                                             \
	static void							   OnMsg_##MsgType(const MsgType& msg, CNetworkMessage* pMsg);          \
	static WorldMsgRegister<CMD_##MsgType> s_WorldMsgRegister_##MsgType{                                        \
		std::bind(&ProcWorldMsg<MsgType, decltype(OnMsg_##MsgType)>, std::placeholders::_1, &OnMsg_##MsgType)}; \
	void OnMsg_##MsgType(const MsgType& msg, CNetworkMessage* pMsg)

#define DEFINE_SERVERSIDE_MSG_PROCESS(MsgType)                                                                                 \
	static void											OnMsg_##MsgType(const ServerMSG::MsgType& msg, CNetworkMessage* pMsg); \
	static WorldMsgRegister<ServerMSG::MsgID_##MsgType> s_WorldMsgRegister_##MsgType(                                          \
		std::bind(&ProcWorldMsg<ServerMSG::MsgType, decltype(OnMsg_##MsgType)>, std::placeholders::_1, &OnMsg_##MsgType));     \
	void OnMsg_##MsgType(const ServerMSG::MsgType& msg, CNetworkMessage* pMsg)

#define DEFINE_RAWMSG_PROCESS(MsgType)                                                                                       \
	static void							   OnMsg_##MsgType(CNetworkMessage* pMsg);                                           \
	static WorldMsgRegister<CMD_##MsgType> s_WorldMsgRegister_##MsgType(std::bind(&OnMsg_##MsgType, std::placeholders::_1)); \
                                                                                                                             \
	void OnMsg_##MsgType(CNetworkMessage* pMsg)

#endif /* MSGWORLD_PROCESS_H */
