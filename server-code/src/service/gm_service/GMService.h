#ifndef GMService_h__
#define GMService_h__


#include "IService.h"
#include "ServiceComm.h"
#include "NetSocket.h"
#include "UIDFactory.h"
#include <functional>
#include <unordered_map>
#include "server_side.pb.h"

struct event;
class CNetMSGProcess;
class CGMService : public IService, public CServiceCommon
{
public:
	CGMService(const ServerPort& nServerPort);
	virtual ~CGMService();
	void Release() { delete this; }
	bool Create();
	
	void SendServiceReady();
	void SendServiceUnReady();
public:
	virtual void OnLogicThreadProc() override;
	virtual void OnLogicThreadCreate() override;
	virtual void OnLogicThreadExit() override;
	virtual void OnProcessMessage(CNetworkMessage*) override;



public:
	using HttpRequestHandleFunc  = std::function< void(const ServerPort&, const ServerMSG::ServiceHttpRequest&) >;
	std::unordered_map<std::string,  HttpRequestHandleFunc > m_HttpRequestHandle;
};


#define GMService() MyTLSTypePtr<CGMService>::get()
#define EventManager() GMService()->GetEventManager()
#define NetMsgProcess() GMService()->GetNetMsgProcess()

#endif // GMService_h__
