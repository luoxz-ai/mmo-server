#ifndef MSGPLAYER_H
#define MSGPLAYER_H

#include <functional>
#include <memory>
#include <unordered_map>

#include "BaseCode.h"
#include "Player.h"
#include "ZoneService.h"
#include "NetworkMessage.h"

class CNetworkMessage;

struct PlayerMsgRegisterMgr
{
	using FuncType	   = std::function<void(CNetworkMessage*)>;
	using ProcessMap_t = std::unordered_map<uint32_t, FuncType>;
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
	func(pPlayer, msg, pMsg);

	__LEAVE_FUNCTION
}

template<uint32_t Cmd>
struct PlayerMsgRegister
{
	PlayerMsgRegister(PlayerMsgRegisterMgr::FuncType&& func) { PlayerMsgRegisterMgr::s_ProcessMap.emplace(Cmd, std::move(func)); }
};

#define DEFINE_MSG_PROCESS(MsgType)                                                                                                             \
	static void OnMsg_##MsgType(CPlayer* pPlayer, const MsgType& msg, CNetworkMessage* pMsg);                                                   \
	static PlayerMsgRegister<CMD_##MsgType>                                                                                                     \
		 s_PlayerMsgRegister_##MsgType(std::bind(&ProcPlayerMsg<MsgType, decltype(OnMsg_##MsgType)>, std::placeholders::_1, &OnMsg_##MsgType)); \
	void OnMsg_##MsgType(CPlayer* pPlayer, const MsgType& msg, CNetworkMessage* pMsg)

#define DEFINE_SERVERSIDE_MSG_PROCESS(MsgType)                                                                              \
	static void OnMsg_##MsgType(const ServerMSG::MsgType& msg, CNetworkMessage* pMsg);                                      \
	static PlayerMsgRegister<ServerMSG::MsgID_##MsgType> s_PlayerMsgRegister_##MsgType(                                     \
		std::bind(&ProcPlayerMsg<ServerMSG::MsgType, decltype(OnMsg_##MsgType)>, std::placeholders::_1, &OnMsg_##MsgType)); \
	void OnMsg_##MsgType(const ServerMSG::MsgType& msg, CNetworkMessage* pMsg)

#define DEFINE_RAWMSG_PROCESS(MsgType)                                                                                         \
	static void OnMsg_##MsgType(CNetworkMessage* pMsg);                                                                        \
	static PlayerMsgRegister<CMD_##MsgType> s_PlayerMsgRegister_##MsgType(std::bind(&OnMsg_##MsgType, std::placeholders::_1)); \
	void OnMsg_##MsgType(CNetworkMessage* pMsg)

#endif /* MSGPLAYER_H */
