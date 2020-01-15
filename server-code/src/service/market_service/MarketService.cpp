#include "MarketService.h"

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
thread_local CMarketService* MyTLSTypePtr<CMarketService>::m_pPtr = nullptr;

extern "C" IService* ServiceCreate(uint16_t idWorld, uint16_t idService)
{

	CMarketService* pService = new CMarketService(ServerPort{idWorld, idService});
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
CMarketService::CMarketService(const ServerPort& nServerPort)
	: CServiceCommon(nServerPort, "Market")
{
}

CMarketService::~CMarketService()
{
	MyTLSTypePtr<CMarketService>::set(this);
	scope_guards scope_exit;
	scope_exit += []() {
		MyTLSTypePtr<CMarketService>::set(nullptr);
	};
}

bool CMarketService::Create()
{
	//各种初始化
	scope_guards scope_exit;
	MyTLSTypePtr<CMarketService>::set(this);
	scope_exit += []() {
		MyTLSTypePtr<CMarketService>::set(nullptr);
	};

	BaseCode::SetNdc(GetServiceName());
	scope_exit += []() {
		BaseCode::SetNdc(std::string());
	};

	if(CreateService(20) == false)
		return false;

	return true;
}

void CMarketService::OnProcessMessage(CNetworkMessage* pNetworkMsg)
{
	switch(pNetworkMsg->GetMsgHead()->usCmd)
	{
		case NETMSG_SCK_CLOSE:
		{
			MSG_SCK_CLOSE* pMsg = (MSG_SCK_CLOSE*)pNetworkMsg->GetBuf();
		}
		break;
		default:
		{
			m_pNetMsgProcess->Process(pNetworkMsg);
		}
		break;
	}
}

void CMarketService::OnLogicThreadProc()
{
	CServiceCommon::OnLogicThreadProc();
}

void CMarketService::OnLogicThreadCreate()
{
	MyTLSTypePtr<CMarketService>::set(this);
	CServiceCommon::OnLogicThreadCreate();
}

void CMarketService::OnLogicThreadExit() {}
