#ifndef MSGWORLD_PROCESS_H
#define MSGWORLD_PROCESS_H

#include <memory>
#include <unordered_map>
#include <functional>
#include "BaseCode.h"
#include "NetworkMessage.h"

struct WorldMsgRegisterMgr
{
	using FuncType	   = std::function<void(CNetworkMessage*)>;
	using ProcessMap_t = std::unordered_map<uint32_t, FuncType>;
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
	func(msg,pMsg);

	__LEAVE_FUNCTION
}


template<uint32_t Cmd>
struct WorldMsgRegister
{
	WorldMsgRegister(WorldMsgRegisterMgr::FuncType&& func) 
	{ 
		WorldMsgRegisterMgr::s_ProcessMap.emplace(Cmd, std::move(func)); 
	}
};

#define DEFINE_MSG_PROCESS(MsgType)                                                                             \
	static void OnMsg_##MsgType(const MsgType& msg, CNetworkMessage* pMsg);                                     \
	static WorldMsgRegister<CMD_##MsgType> s_WorldMsgRegister_##MsgType(                                        \
		std::bind(&ProcWorldMsg<MsgType, decltype(OnMsg_##MsgType)>, std::placeholders::_1, &OnMsg_##MsgType)); \
	void OnMsg_##MsgType(const MsgType& msg, CNetworkMessage* pMsg)

#define DEFINE_SERVERSIDE_MSG_PROCESS(MsgType)                                                                             \
	static void OnMsg_##MsgType(const ServerMSG::MsgType& msg, CNetworkMessage* pMsg);                                     \
	static WorldMsgRegister<ServerMSG::MsgID_##MsgType> s_WorldMsgRegister_##MsgType(                                      \
		std::bind(&ProcWorldMsg<ServerMSG::MsgType, decltype(OnMsg_##MsgType)>, std::placeholders::_1, &OnMsg_##MsgType)); \
	void OnMsg_##MsgType(const ServerMSG::MsgType& msg, CNetworkMessage* pMsg)

#define DEFINE_RAWMSG_PROCESS(MsgType)                                                                                       \
	static void OnMsg_##MsgType(CNetworkMessage* pMsg);                                                                      \
	static WorldMsgRegister<CMD_##MsgType> s_WorldMsgRegister_##MsgType(std::bind(&OnMsg_##MsgType, std::placeholders::_1)); \
                                                                                                                             \
	void OnMsg_##MsgType(CNetworkMessage* pMsg)

#endif /* MSGWORLD_PROCESS_H */
