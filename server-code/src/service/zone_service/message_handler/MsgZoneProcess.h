#ifndef MSGZONEPROCESS_H
#define MSGZONEPROCESS_H

#include <functional>
#include <memory>
#include <array>
#include <utility>

#include "BaseCode.h"
#include "NetworkMessage.h"
#include "Player.h"
#include "ZoneService.h"
#include "server_msg/server_side.pb.h"

class CNetworkMessage;

struct ZoneMsgRegisterMgr
{
	using FuncType	   = std::function<void(CNetworkMessage*)>;
	using ProcessMap_t = std::array<FuncType, ServerMSG::MsgID_END>;
	static ProcessMap_t s_ProcessMap;
};

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

template<uint32_t Cmd>
struct ZoneMsgRegister
{
	ZoneMsgRegister(ZoneMsgRegisterMgr::FuncType&& func) 
	{
		ZoneMsgRegisterMgr::s_ProcessMap[Cmd] =  std::move(func); 
		LOGDEBUG("AddMsgProc:{}", Cmd);
	}
};

#define DEFINE_MSG_PROCESS(MsgType)                                                                                       \
	static void								OnMsg_##MsgType(CPlayer* pPlayer, const MsgType& msg, CNetworkMessage* pMsg); \
	static ZoneMsgRegister<CMD_##MsgType> s_ZoneMsgRegister_##MsgType(                                                \
		std::bind(&ProcPlayerMsg<MsgType, decltype(OnMsg_##MsgType)>, std::placeholders::_1, &OnMsg_##MsgType));          \
	void OnMsg_##MsgType(CPlayer* pPlayer, const MsgType& msg, CNetworkMessage* pMsg)

#define DEFINE_SERVERSIDE_MSG_PROCESS(MsgType)                                                                                  \
	static void											 OnMsg_##MsgType(const ServerMSG::MsgType& msg, CNetworkMessage* pMsg); \
	static ZoneMsgRegister<ServerMSG::MsgID_##MsgType> s_ZoneMsgRegister_##MsgType(                                         \
		std::bind(&ProcessMsg<ServerMSG::MsgType, decltype(OnMsg_##MsgType)>, std::placeholders::_1, &OnMsg_##MsgType));     \
	void OnMsg_##MsgType(const ServerMSG::MsgType& msg, CNetworkMessage* pMsg)

#define DEFINE_RAWMSG_PROCESS(MsgType)                                                                                         \
	static void								OnMsg_##MsgType(CNetworkMessage* pMsg);                                            \
	static ZoneMsgRegister<CMD_##MsgType> s_ZoneMsgRegister_##MsgType(std::bind(&OnMsg_##MsgType, std::placeholders::_1)); \
	void									OnMsg_##MsgType(CNetworkMessage* pMsg)


#endif /* MSGZONEPROCESS_H */
