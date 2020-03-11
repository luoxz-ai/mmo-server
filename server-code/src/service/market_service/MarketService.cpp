#include "MarketService.h"

#include <functional>

#include "EventManager.h"
#include "MessagePort.h"
#include "MessageRoute.h"
#include "NetMSGProcess.h"
#include "NetSocket.h"
#include "NetworkMessage.h"
#include "SettingMap.h"

static thread_local CMarketService* tls_pService = nullptr;
CMarketService*						MarketService()
{
	return tls_pService;
}

extern "C" __attribute__((visibility("default"))) IService* ServiceCreate(uint16_t idWorld, uint16_t idService)
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

//////////////////////////////////////////////////////////////////////////
CMarketService::CMarketService(const ServerPort& nServerPort)
	: CServiceCommon(nServerPort, "Market")
{
}

CMarketService::~CMarketService()
{
	tls_pService = this;
	scope_guards scope_exit;
	scope_exit += []() {
		tls_pService = nullptr;
	};
}

bool CMarketService::Create()
{
	//各种初始化
	tls_pService = this;
	scope_guards scope_exit;
	scope_exit += []() {
		tls_pService = nullptr;
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
	tls_pService = this;
	CServiceCommon::OnLogicThreadCreate();
}

void CMarketService::OnLogicThreadExit() {}
