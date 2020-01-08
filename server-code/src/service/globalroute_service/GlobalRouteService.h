#ifndef GlobalRouteService_h__
#define GlobalRouteService_h__


#include "IService.h"
#include "ServiceComm.h"
#include "NetSocket.h"
#include "UIDFactory.h"



struct event;
class CNetMSGProcess;
class CGlobalRouteService : public IService, public CServiceCommon
{
public:
	CGlobalRouteService(const ServerPort& nServerPort);
	virtual ~CGlobalRouteService();
	void Release() { delete this; }
	bool Create();
	
public:
	virtual void OnLogicThreadProc() override;
	virtual void OnLogicThreadCreate() override;
	virtual void OnLogicThreadExit() override;

	void OnReciveHttp(struct evhttp_request *req);
	virtual void OnProcessMessage(CNetworkMessage*) override;

	void AddDelayResponse(uint64_t uid, struct evhttp_request *req);
	struct evhttp_request * FindDelayResponse(uint64_t uid);

private:
	std::map<uint64_t, struct evhttp_request *> m_RequestMap;

};


#define GlobalRouteService() MyTLSTypePtr<CGlobalRouteService>::get()
#define EventManager() GlobalRouteService()->GetEventManager()
#define NetMsgProcess() GlobalRouteService()->GetNetMsgProcess()

#endif // GlobalRouteService_h__
