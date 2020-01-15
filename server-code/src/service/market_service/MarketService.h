#ifndef MarketService_h__
#define MarketService_h__

#include "IService.h"
#include "NetSocket.h"
#include "ServiceComm.h"
#include "UIDFactory.h"

struct event;
class CNetMSGProcess;
class CMarketService : public IService, public CServiceCommon
{
public:
	CMarketService(const ServerPort& nServerPort);
	virtual ~CMarketService();
	void Release() { delete this; }
	bool Create();

public:
	virtual void OnLogicThreadProc() override;
	virtual void OnLogicThreadCreate() override;
	virtual void OnLogicThreadExit() override;

	virtual void OnProcessMessage(CNetworkMessage*) override;
};

#define MarketService() MyTLSTypePtr<CMarketService>::get()
#define EventManager()	MarketService()->GetEventManager()
#define NetMsgProcess() MarketService()->GetNetMsgProcess()

#endif // MarketService_h__
