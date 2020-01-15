#include "User.h"

#include "Account.h"
#include "MapData.h"
#include "MapManager.h"
#include "WorldService.h"

MEMORYHEAP_IMPLEMENTATION(CUser, s_heap);

CUser::CUser() {}
CUser::~CUser() {}

bool CUser::Init(CAccount* pAccount, ST_ROLE_INFO* pInfo)
{
	//查询角色信息
	CHECKF(pAccount);
	CHECKF(pInfo);
	m_pAccount = pAccount;
	m_pInfo	   = pInfo;
	m_nLev	   = m_pInfo->GetLev();
	return true;
}

const VirtualSocket& CUser::GetSocket() const
{
	return m_pAccount->GetSocket();
}

void CUser::EnterZone()
{
	__ENTER_FUNCTION
	LOGLOGIN("UserEnterZone:{}", GetID());

	uint64_t idRecordScene = m_pInfo->GetRecordSceneID();
	//通过地图id查找玩家所在的Zone
	uint16_t idZone = SceneID(idRecordScene).GetZoneID();
	m_pInfo->SetLastLoginTime(TimeGetSecond());

	//通知对应的Zone开始Loading玩家数据
	{
		ServerMSG::PlayerEnterZone msg;
		msg.set_idplayer(GetID());
		msg.set_socket(m_pAccount->GetSocket());
		msg.set_idscene(idRecordScene);
		msg.set_posx(m_pInfo->GetRecordPosX());
		msg.set_posy(m_pInfo->GetRecordPosY());
		msg.set_face(m_pInfo->GetRecordFace());

		CNetworkMessage _msg(ServerMSG::MsgID_PlayerEnterZone, msg, WorldService()->GetServerPort(), ServerPort(WorldService()->GetWorldID(), idZone));
		WorldService()->SendMsg(_msg);
	}

	//通知Socket，玩家消息指向新的Zone
	{
		MSG_SCK_CHG_DEST sock_msg;
		sock_msg.idService = idZone;
		sock_msg.vs		   = GetSocket();

		WorldService()->SendPortMsg(GetSocket().GetServerPort(), (byte*)&sock_msg, sizeof(sock_msg));
	}

	m_idZone = idZone;
	__LEAVE_FUNCTION
}

void CUser::OnChangeZone(uint16_t idZone)
{
	__ENTER_FUNCTION
	LOGLOGIN("UserChangeZone:{}", GetID());

	//通知Socket，玩家消息指向新的Zone
	{
		MSG_SCK_CHG_DEST sock_msg;
		sock_msg.idService = idZone;
		sock_msg.vs		   = GetSocket();

		WorldService()->SendPortMsg(GetSocket().GetServerPort(), (byte*)&sock_msg, sizeof(sock_msg));
	}

	m_idZone = idZone;
	__LEAVE_FUNCTION
}

void CUser::Logout()
{
	__ENTER_FUNCTION
	LOGLOGIN("UserLogout:{}", GetID());
	m_pInfo->SetLastLogoutTime(TimeGetSecond());

	//通知对应的Zone 卸载对应的Player
	{
		ServerMSG::PlayerLogout msg;
		msg.set_idplayer(GetID());
		msg.set_socket(GetSocket());
		CNetworkMessage _msg(ServerMSG::MsgID_PlayerLogout, msg, WorldService()->GetServerPort(), ServerPort(WorldService()->GetWorldID(), m_idZone));
		WorldService()->SendMsg(_msg);
	}
	//通知Socket， 玩家消息指向回World
	{
		MSG_SCK_CHG_DEST socket_msg;
		socket_msg.idService = WorldService()->GetServiceID();
		socket_msg.vs		 = GetSocket();

		WorldService()->SendPortMsg(GetSocket().GetServerPort(), (byte*)&socket_msg, sizeof(socket_msg));
	}
	__LEAVE_FUNCTION
}

void CUser::SendMsg(uint16_t usCmd, const google::protobuf::Message& msg)
{
	WorldService()->SendToVirtualSocket(GetSocket(), CMD_SC_ACTORINFOLIST, msg);
}

uint32_t CUser::GetLev() const
{
	return m_nLev;
}

const std::string& CUser::GetName() const
{
	return m_pInfo->GetName();
}

uint64_t CUser::GetTeamID() const
{
	return m_pInfo->GetTeamID();
}

uint64_t CUser::GetGuildID() const
{
	return m_pInfo->GetGuildID();
}

bool CUser::ChangeName(const std::string& new_name)
{
	return true;
}

void CUser::OnLevChg(uint32_t v)
{
	m_nLev = v;
}

void CUser::SetTeamID(uint64_t v)
{
	m_pInfo->SetTeamID(v);
}

void CUser::SetGuildID(uint64_t v)
{
	m_pInfo->SetGuildID(v);
}

void CUser::ClearMate()
{
	m_pInfo->ClearMate();
	// send msg to zone
}

OBJID CUser::GetID() const
{
	return m_pInfo->GetID();
}

uint16_t CUser::GetZoneID() const
{
	return m_idZone;
}
