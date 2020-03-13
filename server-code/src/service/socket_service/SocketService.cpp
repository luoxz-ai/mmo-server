#include "SocketService.h"

#include <functional>

#include "EventManager.h"
#include "MessagePort.h"
#include "MessageRoute.h"
#include "NetSocket.h"
#include "SettingMap.h"
#include "msg/ts_cmd.pb.h"
#include "msg/world_service.pb.h"

extern "C" __attribute__((visibility("default"))) IService* ServiceCreate(uint16_t idWorld, uint16_t idService)
{

	CSocketService* pService = new CSocketService(ServerPort{idWorld, idService});
	if(pService == nullptr)
		return nullptr;

	if(pService->Create() == false)
	{
		pService->Release();
		return nullptr;
	}

	return pService;
}

CGameClient::CGameClient()
	: m_nDestServerPort(0)
	, m_nMessageAllowBegin(CMD_CS_LOGIN) // only accept CS_AUTH
	, m_nMessageAllowEnd(CMD_CS_LOGIN)	 // only accept CS_AUTH
	, m_idUser(0)
{
}

CGameClient::~CGameClient() {}

void CGameClient::Close()
{
	CHECK(m_pService);
	LOGNETDEBUG("Client {}:{} Close.", GetSocketAddr().c_str(), GetSocketPort());
	SetMessageAllow(0, 0);
	m_pService->GetNetworkService()->_CloseSocket(m_VirtualSocket.GetSocketIdx());
	m_VirtualSocket.SetSocketIdx(0);
}

bool CGameClient::SendMsg(byte* pBuffer, size_t len)
{
	CHECKF(m_pService);
	return m_pService->GetNetworkService()->SendSocketMsgByIdx(m_VirtualSocket.GetSocketIdx(), pBuffer, len);
}

bool CGameClient::IsVaild() const
{
	return m_VirtualSocket.GetSocketIdx() != 0;
}

static thread_local CSocketService* tls_pService = nullptr;
CSocketService*						SocketService()
{
	return tls_pService;
}
//////////////////////////////////////////////////////////////////////////
CSocketService::CSocketService(const ServerPort& nServerPort)
	: CServiceCommon(nServerPort, std::string("Socket") + std::to_string(nServerPort.GetServiceID()))
{
	m_tLastDisplayTime.Startup(10);
}

CSocketService::~CSocketService()
{
	tls_pService = this;
	scope_guards scope_exit;
	scope_exit += []() {
		tls_pService = nullptr;
	};
}

bool CSocketService::Create()
{
	tls_pService = this;
	scope_guards scope_exit;
	scope_exit += []() {
		tls_pService = nullptr;
	};

	auto oldNdc = BaseCode::SetNdc(GetServiceName());
	scope_exit += [oldNdc]() {
		BaseCode::SetNdc(oldNdc);;
	};

	{
		const ServerAddrInfo* pAddrInfo = GetMessageRoute()->QueryServiceInfo(GetServerPort());
		if(pAddrInfo == nullptr)
		{
			LOGFATAL("CSocketService::Create QueryServerInfo {} fail", GetServerPort().GetServiceID());
			return false;
		}

		CHECKF(CreateNetworkService());

		//开启对外监听端口
		if(GetNetworkService()->Listen(pAddrInfo->bind_addr.c_str(), pAddrInfo->publish_port, this) == nullptr)
		{
			return false;
		}
		GetNetworkService()->EnableListener(nullptr, false);

		// websocket监听
		//		if (!GetNetworkService()->ListenWebSocket(9555, this))
		//		{
		//			return false;
		//		}

		if(CreateService(20) == false)
			return false;

		// SetIPCheck(true);

		return true;
	}

	return false;
}

void CSocketService::MapClientByUserID(OBJID idUser, CGameClient* pClient)
{
	if(idUser == 0)
		return;

	m_mapClientByUserID[idUser] = pClient;
	pClient->SetUserID(idUser);
}

CGameClient* CSocketService::QueryClientByUserID(OBJID idUser)
{
	if(idUser == 0)
		return nullptr;
	auto it = m_mapClientByUserID.find(idUser);
	if(it != m_mapClientByUserID.end())
		return it->second;
	else
		return nullptr;
}

bool CSocketService::DelClientByUserID(OBJID idUser)
{
	if(idUser == 0)
		return false;

	auto it = m_mapClientByUserID.find(idUser);
	if(it != m_mapClientByUserID.end())
	{
		m_mapClientByUserID.erase(it);
		return true;
	}
	else
		return false;
}

CGameClient* CSocketService::QueryClient(const VirtualSocket& vs)
{
	auto it = m_setVirtualSocket.find(vs);
	if(it != m_setVirtualSocket.end())
	{
		return it->second;
	}
	else
	{
		return nullptr;
	}
}

void CSocketService::AddClient(const VirtualSocket& vs, CGameClient* pClient)
{
	m_setVirtualSocket[vs] = pClient;
}

void CSocketService::RemoveClient(const VirtualSocket& vs)
{
	auto it = m_setVirtualSocket.find(vs);
	if(it != m_setVirtualSocket.end())
	{
		CGameClient* pClient = it->second;
		{
			MSG_SCK_CLOSE msg;
			msg.vs = vs;
			SendPortMsg(ServerPort(GetServerPort().GetWorldID(), WORLD_SERVICE_ID), (byte*)&msg, sizeof(msg));
		}

		SAFE_DELETE(pClient);
		m_setVirtualSocket.erase(it);
	}
}

void CSocketService::OnConnected(CNetSocket* pSocket) {}

void CSocketService::OnConnectFailed(CNetSocket*) {}

void CSocketService::OnDisconnected(CNetSocket* pSocket)
{
	__ENTER_FUNCTION
	RemoveClient(VirtualSocket::CreateVirtualSocket(GetServerPort(), pSocket->GetSocketIdx()));
	__LEAVE_FUNCTION
}

void CSocketService::OnAccepted(CNetSocket* pSocket)
{
	__ENTER_FUNCTION

	CGameClient* pClient = new CGameClient();
	pClient->SetService(this);
	pClient->SetDestServerPort(ServerPort(GetServerPort().GetWorldID(), WORLD_SERVICE_ID));
	pClient->SetVirtualSocket(VirtualSocket::CreateVirtualSocket(GetServerPort(), pSocket->GetSocketIdx()));
	pClient->SetSocketAddr(pSocket->GetAddrString());
	pClient->SetSocketPort(pSocket->GetPort());
	AddClient(pClient->GetVirtualSocket(), pClient);

	// send crypto key to client
	uint32_t seed = TimeGetSecond();
	pSocket->InitDecryptor(seed);
	SC_KEY msg;
	msg.set_key(seed);
	CNetworkMessage _msg(CMD_SC_KEY, msg);
	pSocket->SendMsg(_msg.GetBuf(), _msg.GetSize());
	__LEAVE_FUNCTION
}

void CSocketService::OnRecvData(CNetSocket* pSocket, byte* pBuffer, size_t len)
{
	__ENTER_FUNCTION
	m_nSocketMessageProcess++;
	CGameClient* pClient = QueryClient(VirtualSocket::CreateVirtualSocket(GetServerPort(), pSocket->GetSocketIdx()));
	if(pClient == nullptr)
		return;
	if(pClient->IsVaild() == false)
	{
		return;
	}
	// recv msg from client
	MSG_HEAD* pHead = (MSG_HEAD*)pBuffer;
	if(pHead->usCmd < pClient->GetMessageAllowBegin() || pHead->usCmd > pClient->GetMessageAllowEnd())
	{
		LOGWARNING("RECV ClientMsg:{} not Allow {}.{}", pHead->usCmd, pSocket->GetAddrString().c_str(), pSocket->GetPort());
		pSocket->Close();
		return;
	}

	switch(pHead->usCmd)
	{
		default:
		{
			if(pClient->GetDestServerPort().IsVaild() == false)
				return;
			// send to other server
			CNetworkMessage msg(pBuffer, len, pClient->GetVirtualSocket(), VirtualSocket(pClient->GetDestServerPort(), 0));
			SendMsg(msg);
		}
		break;
	}
	__LEAVE_FUNCTION
}

void CSocketService::OnWsAccepted(CNetWebSocket* pWebSocket)
{
	__ENTER_FUNCTION

	CGameClient* pClient = new CGameClient();
	pClient->SetService(this);
	pClient->SetDestServerPort(ServerPort(GetServerPort().GetWorldID(), WORLD_SERVICE_ID));
	pClient->SetVirtualSocket(VirtualSocket::CreateVirtualSocket(GetServerPort(), pWebSocket->GetSocketIdx()));
	pClient->SetSocketAddr(pWebSocket->GetAddrString());
	pClient->SetSocketPort(pWebSocket->GetPort());
	AddClient(pClient->GetVirtualSocket(), pClient);

	// send crypto key to client
	uint32_t seed = TimeGetSecond();
	pWebSocket->InitDecryptor(seed);
	SC_KEY msg;
	msg.set_key(seed);

	CNetworkMessage _msg(CMD_SC_KEY, msg);
	pWebSocket->SendMsg(_msg.GetBuf(), _msg.GetSize());
	__LEAVE_FUNCTION
}

void CSocketService::OnWsConnected(CNetWebSocket* pWebSocket)
{
	// not implemented
}

void CSocketService::OnWsConnectFailed(CNetWebSocket* pWebSocket)
{
	// not implemented
}

void CSocketService::OnWsDisconnected(CNetWebSocket* pWebSocket) {}

void CSocketService::OnWsRecvData(CNetWebSocket* pWebSocket, byte* pBuffer, size_t len)
{
	// as same as OnRecvData()
	__ENTER_FUNCTION
	m_nWebSocketMessageProcess++;
	CGameClient* pClient = QueryClient(VirtualSocket::CreateVirtualSocket(GetServerPort(), pWebSocket->GetSocketIdx()));
	if(pClient == nullptr)
		return;
	if(pClient->IsVaild() == false)
	{
		return;
	}
	// recv msg from client
	MSG_HEAD* pHead = (MSG_HEAD*)pBuffer;
	if(pHead->usCmd < pClient->GetMessageAllowBegin() || pHead->usCmd > pClient->GetMessageAllowEnd())
	{
		LOGWARNING("RECV ClientMsg:{} not Allow {}.{}", pHead->usCmd, pWebSocket->GetAddrString().c_str(), pWebSocket->GetPort());
		pWebSocket->Close();
		return;
	}

	switch(pHead->usCmd)
	{
		default:
		{
			if(pClient->GetDestServerPort().IsVaild() == false)
				return;
			// send to other server
			CNetworkMessage msg(pBuffer, len, pClient->GetVirtualSocket(), VirtualSocket(pClient->GetDestServerPort(), 0));
			SendMsg(msg);
		}
		break;
	}
	__LEAVE_FUNCTION
}

void CSocketService::OnProcessMessage(CNetworkMessage* pNetworkMsg)
{
	__ENTER_FUNCTION

	//只需要处理来自其他服务器的消息
	//来自客户端的消息已经直接发往对应服务器了
	MSG_HEAD* pHead = pNetworkMsg->GetMsgHead();
	switch(pHead->usCmd)
	{
		case NETMSG_SERVICE_READY:
		{
			LOGMESSAGE("START_ACCEPT");
			GetNetworkService()->EnableListener(nullptr, true);
		}
		break;
		case NETMSG_SCK_STOP_ACCEPT:
		{
			LOGMESSAGE("STOP_ACCEPT");
			GetNetworkService()->EnableListener(nullptr, false);
		}
		break;
		case NETMSG_SCK_CHG_DEST:
		{
			MSG_SCK_CHG_DEST* pMsg	  = (MSG_SCK_CHG_DEST*)pNetworkMsg->GetBuf();
			CGameClient*	  pClient = QueryClient(pMsg->vs);
			if(pClient && pClient->IsVaild())
			{
				pClient->SetDestServerPort(ServerPort(GetWorldID(), pMsg->idService));
				LOGNETDEBUG("SCK_CHG_DEST {}:{} To Service:{}", pClient->GetSocketAddr().c_str(), pClient->GetSocketPort(), pMsg->idService);
			}
		}
		break;
		case NETMSG_SCK_CLOSE:
		{
			MSG_SCK_CLOSE* pMsg	   = (MSG_SCK_CLOSE*)pNetworkMsg->GetBuf();
			CGameClient*   pClient = QueryClient(pMsg->vs);
			if(pClient && pClient->IsVaild())
			{
				LOGDEBUG("CLOSE CLIENT BYVS:{}:{} FROM OTHER SERVER", pClient->GetSocketAddr().c_str(), pClient->GetSocketPort());
				//主动关闭客户端连接，需要通知客户端不要重连
				pClient->Close();
			}
		}
		break;
		case NETMSG_SCK_AUTH:
		{
			MSG_SCK_AUTH* pMsg	  = (MSG_SCK_AUTH*)pNetworkMsg->GetBuf();
			CGameClient*  pClient = QueryClient(pMsg->vs);
			if(pClient && pClient->IsVaild())
			{
				pClient->SetAuth(true);
				pClient->SetMessageAllow(CLIENT_MSG_ID_BEGIN, CLIENT_MSG_ID_END);
			}
		}
		break;
		default:
		{
			// send to socket
			switch(pNetworkMsg->GetMultiType())
			{
				case MULTITYPE_NONE:
				{
					CGameClient* pClient = QueryClient(pNetworkMsg->GetTo());
					if(pClient && pClient->IsVaild())
					{
						pClient->SendMsg(pNetworkMsg->GetBuf(), pNetworkMsg->GetSize());
					}
				}
				break;
				case MULTITYPE_BROADCAST:
				{
					for(const auto& v: m_setVirtualSocket)
					{
						CGameClient* pClient = v.second;
						if(pClient && pClient->IsVaild() && pClient->IsAuth())
						{
							pClient->SendMsg(pNetworkMsg->GetBuf(), pNetworkMsg->GetSize());
						}
					}
				}
				break;
				case MULTITYPE_VIRTUALSOCKET:
				{
					const auto& refSet = pNetworkMsg->GetMultiTo();
					for(auto it = refSet.begin(); it != refSet.end(); it++)
					{
						CGameClient* pClient = QueryClient(*it);
						if(pClient && pClient->IsVaild())
						{
							pClient->SendMsg(pNetworkMsg->GetBuf(), pNetworkMsg->GetSize());
						}
					}
				}
				break;
				case MULTITYPE_USERID:
				{
				}
				break;
				case MULTITYPE_GROUPID:
				{
				}
				break;
			}
		}
		break;
	}
	__LEAVE_FUNCTION
}

void CSocketService::OnLogicThreadCreate()
{
	tls_pService = this;
	CServiceCommon::OnLogicThreadCreate();
}

void CSocketService::OnLogicThreadExit()
{
	CServiceCommon::OnLogicThreadExit();
}

void CSocketService::OnLogicThreadProc()
{
	__ENTER_FUNCTION
	CServiceCommon::OnLogicThreadProc();

	if(m_tLastDisplayTime.ToNextTime())
	{
		std::string buf =
			std::string("\n======================================================================") +
			fmt::format(FMT_STRING("\nMessageProcess:{}\tSocketMsg:{}"), GetMessageProcess(), m_nSocketMessageProcess) +
			fmt::format(FMT_STRING("\nRecvTotal:{}\tRecvAvg:{}"), GetNetworkService()->GetRecvBPS().GetTotal(), GetNetworkService()->GetRecvBPS().GetAvgBPS()) +
			fmt::format(FMT_STRING("\nSendTotal:{}\tSendAvg:{}"), GetNetworkService()->GetSendBPS().GetTotal(), GetNetworkService()->GetSendBPS().GetAvgBPS());
		static const uint16_t ServiceID[] = {1, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};

		for(size_t i = 0; i < sizeOfArray(ServiceID); i++)
		{
			auto pMessagePort = GetMessageRoute()->QueryMessagePort(ServerPort(GetWorldID(), ServiceID[i]), false);
			if(pMessagePort)
			{
				buf += fmt::format(FMT_STRING("\nMsgPort:{}\tSendBuff:{}"), ServiceID[i], pMessagePort->GetWriteBufferSize());
			}
		}

		LOGMONITOR("{}", buf.c_str());
		SetMessageProcess(0);
		m_nSocketMessageProcess = 0;
	}

	__LEAVE_FUNCTION
}

void CSocketService::OnRecvTimeout(CNetSocket* pSocket) {}
