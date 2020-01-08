#include "ZoneService.h"
#include "Actor.h"
#include "Player.h"
#include "Monster.h"

void CZoneService::OnMsgPlayerEnterZone(CNetworkMessage* pMsg)
{
	ServerMSG::PlayerEnterZone msg;
	if (msg.ParseFromArray(pMsg->GetMsgBody(), pMsg->GetBodySize()) == false)
	{
		return;
	}

	CHECK(msg.idplayer() != 0);
	CHECK(msg.socket() != 0);
	CHECK(msg.idscene() != 0);
	CHECK(msg.posx() != 0);
	CHECK(msg.posy() != 0);
	
	m_pLoadingThread->CancleWaiting(msg.idplayer());

	LOGLOGIN("AddLoginPlayer: {}", msg.idplayer());
	m_pLoadingThread->AddLoginPlayer(msg.idplayer(), msg.socket(), false, msg.idscene(), msg.posx(), msg.posy(), 0.0f, msg.face());

	CreateSocketMessagePool(msg.socket());

	



}

void CZoneService::OnMsgPlayerChangeZone(CNetworkMessage* pMsg)
{
	ServerMSG::PlayerChangeZone msg;
	if (msg.ParseFromArray(pMsg->GetMsgBody(), pMsg->GetBodySize()) == false)
	{
		return;
	}

	CHECK(msg.idplayer() != 0);
	CHECK(msg.socket() != 0);
	CHECK(msg.idzone() != 0);
	CHECK(msg.idscene() != 0);
	CHECK(msg.posx() >= 0);
	CHECK(msg.posy() >= 0);
	CHECK(msg.range() > 0);


	m_pLoadingThread->CancleWaiting(msg.idplayer());

	m_pLoadingThread->AddLoginPlayer(msg.idplayer(), msg.socket(), true, msg.idscene(), msg.posx(), msg.posy(), msg.range(), msg.face());

	CreateSocketMessagePool(msg.socket());

	return;
}

void CZoneService::OnMsgPlayerChangeZone_Data(CNetworkMessage* pMsg)
{
	ServerMSG::PlayerChangeZone_Data msg;
	if (msg.ParseFromArray(pMsg->GetMsgBody(), pMsg->GetBodySize()) == false)
	{
		return;
	}

	CHECK(msg.idplayer() != 0);
	CHECK(msg.socket() != 0);
	

	auto itFind = m_MessagePoolBySocket.find(msg.socket());
	if (itFind == m_MessagePoolBySocket.end())
	{
		return;
	}
	
	PushMsgToMessagePool(msg.socket(), pMsg);

}

void CZoneService::OnMsgPlayerLogout(CNetworkMessage* pMsg)
{
	ServerMSG::PlayerLogout msg;
	if (msg.ParseFromArray(pMsg->GetMsgBody(), pMsg->GetBodySize()) == false)
	{
		return;
	}

	CHECK(msg.idplayer() != 0);
	CHECK(msg.socket() != 0);
	

	m_pLoadingThread->CancleWaiting(msg.idplayer());

	CActor* pActor = ActorManager()->QueryActor(msg.idplayer());
	if (pActor == nullptr)
	{
		//log error
		return;
	}

	CPlayer* pPlayer = pActor->ConvertToDerived<CPlayer>();
	pPlayer->OnLogout();
	

}


