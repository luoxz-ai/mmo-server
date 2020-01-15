#include "ServiceCtrlService.h"

#include <functional>

#include "EventManager.h"
#include "MessagePort.h"
#include "MessageRoute.h"
#include "NetMSGProcess.h"
#include "NetSocket.h"
#include "NetworkMessage.h"
#include "SettingMap.h"
#include "tinyxml2/tinyxml2.h"

template<>
thread_local CServiceCtrlService* MyTLSTypePtr<CServiceCtrlService>::m_pPtr = nullptr;

extern "C" IService* ServiceCreate(uint16_t idWorld, uint16_t idService)
{

	CServiceCtrlService* pService = new CServiceCtrlService(ServerPort{idWorld, idService});
	if(pService == nullptr)
		return nullptr;

	if(pService->Create() == false)
	{
		pService->Release();
		return nullptr;
	}

	return pService;
}
__attribute__((visibility("default"))) IService* ServiceCreate(uint16_t idWorld, uint16_t idService);

//////////////////////////////////////////////////////////////////////////
CServiceCtrlService::CServiceCtrlService(const ServerPort& nServerPort)
	: CServiceCommon(nServerPort, "ServiceCtrl")
{
}

CServiceCtrlService::~CServiceCtrlService()
{
	MyTLSTypePtr<CServiceCtrlService>::set(this);
	scope_guards scope_exit;
	scope_exit += []() {
		MyTLSTypePtr<CServiceCtrlService>::set(nullptr);
	};
}

bool CServiceCtrlService::Create()
{
	//各种初始化
	scope_guards scope_exit;
	MyTLSTypePtr<CServiceCtrlService>::set(this);
	scope_exit += []() {
		MyTLSTypePtr<CServiceCtrlService>::set(nullptr);
	};

	BaseCode::SetNdc(GetServiceName());
	scope_exit += []() {
		BaseCode::SetNdc(std::string());
	};

	if(CreateService(20, this) == false)
		return false;

	return true;
}

void CServiceCtrlService::OnPortConnected(CNetSocket* pSocket) {}

void CServiceCtrlService::OnPortConnectFailed(CNetSocket*) {}

void CServiceCtrlService::OnPortDisconnected(CNetSocket* pSocket) {}

void CServiceCtrlService::OnPortAccepted(CNetSocket* pSocket) {}

void CServiceCtrlService::OnPortRecvData(CNetSocket* pSocket, byte* pBuffer, size_t len) {}

void CServiceCtrlService::OnPortRecvTimeout(CNetSocket* pSocket) {}

void CServiceCtrlService::OnProcessMessage(CNetworkMessage* pNetworkMsg)
{
	switch(pNetworkMsg->GetMsgHead()->usCmd)
	{
		case NETMSG_SCK_CLOSE:
		{
			MSG_SCK_CLOSE* pMsg = (MSG_SCK_CLOSE*)pNetworkMsg->GetBuf();
		}
		break;
		case NETMSG_INTERNAL_SERVICE_REGISTER:
		{

			//通知所有的global_route更新
			MSG_INTERNAL_SERVICE_REGISTER* pMsg = (MSG_INTERNAL_SERVICE_REGISTER*)pNetworkMsg->GetBuf();
			LOGMESSAGE("World:{} start", pMsg->idWorld);
			GetMessageRoute()->SetWorldReady(pMsg->idWorld, true);
			for(int i = MIN_GLOBAL_ROUTE_SERVICE_ID; i <= MAX_GLOBAL_ROUTE_SERVICE_ID; i++)
			{
				SendPortMsg(ServerPort(0, i), (byte*)pMsg, sizeof(*pMsg));
			}

			for(int i = MIN_SHAREZONE_SERVICE_ID; i <= MAX_SHAREZONE_SERVICE_ID; i++)
			{
				SendPortMsg(ServerPort(0, i), (byte*)pMsg, sizeof(*pMsg));
				;
			}
		}
		break;
		case NETMSG_INTERNAL_SERVICE_READY:
		{
			MSG_INTERNAL_SERVICE_READY* pMsg = (MSG_INTERNAL_SERVICE_READY*)pNetworkMsg->GetBuf();
			if(pMsg->bReady == false)
			{
				LOGMESSAGE("World:{} shutdown", pMsg->idWorld);
			}
			else
			{
				LOGDEBUG("World:{} Ready", pMsg->idWorld);
			}

			//通知所有的global_route更新
			GetMessageRoute()->SetWorldReady(pMsg->idWorld, pMsg->bReady);

			for(int i = MIN_GLOBAL_ROUTE_SERVICE_ID; i <= MAX_GLOBAL_ROUTE_SERVICE_ID; i++)
			{
				SendPortMsg(ServerPort(0, i), (byte*)pMsg, sizeof(*pMsg));
			}

			for(int i = MIN_SHAREZONE_SERVICE_ID; i <= MAX_SHAREZONE_SERVICE_ID; i++)
			{
				SendPortMsg(ServerPort(0, i), (byte*)pMsg, sizeof(*pMsg));
			}
		}
		break;
		default:
		{
			//需要转发的，直接转发
			if(pNetworkMsg->GetForward().IsVaild())
			{
				pNetworkMsg->SetTo(pNetworkMsg->GetForward());
				SendMsg(*pNetworkMsg);
				return;
			}

			switch(pNetworkMsg->GetMultiType())
			{
				case MULTITYPE_BROADCAST:
				{
					//转发给所有的World
					time_t now = TimeGetSecond();
					for(const auto& [k, v]: GetMessageRoute()->GetWorldReadyList())
					{
						if(now < v)
						{
							SendPortMsg(ServerPort(k, WORLD_SERVICE_ID), pNetworkMsg->GetBuf(), pNetworkMsg->GetSize());
						}
					}
				}
				break;
				case MULTITYPE_VIRTUALSOCKET:
				{
					//转发给对应的Service
					CNetworkMessage send_msg(*pNetworkMsg);
					const auto&		refSet = pNetworkMsg->GetMultiTo();
					for(auto it = refSet.begin(); it != refSet.end(); it++)
					{
						const auto& vs = *it;
						send_msg.SetTo(vs);
						SendMsg(send_msg);
					}
				}
				break;
				default:
					break;
			}

			m_pNetMsgProcess->Process(pNetworkMsg);
		}
		break;
	}
}

void CServiceCtrlService::OnLogicThreadProc()
{

	static const int MAX_PROCESS_PER_LOOP = 1000;
	int				 nCount				  = 0;
	CNetworkMessage* pMsg				  = nullptr;
	if(m_pMessagePort)
	{
		// process message_port msg
		while(nCount < MAX_PROCESS_PER_LOOP && m_pMessagePort->TakeMsg(pMsg))
		{
			OnProcessMessage(pMsg);
			SAFE_DELETE(pMsg);
		}
	}
}

void CServiceCtrlService::OnLogicThreadCreate()
{
	MyTLSTypePtr<CServiceCtrlService>::set(this);
	BaseCode::SetNdc("ServiceCtrl");
	LOGMESSAGE("ThreadID:{}", get_cur_thread_id());
}

void CServiceCtrlService::OnLogicThreadExit() {}
