#ifndef GMService_h__
#define GMService_h__

#include <functional>
#include <unordered_map>

#include "IService.h"
#include "NetSocket.h"
#include "ServiceComm.h"
#include "UIDFactory.h"
#include "msg/server_side.pb.h"

struct event;
class CNetMSGProcess;
class CGMService : public IService, public CServiceCommon
{
public:
	CGMService(const ServerPort& nServerPort);
	virtual ~CGMService();
	void Release() override{ delete this; }
	bool Create();

	void SendServiceReady();
	void SendServiceUnReady();

public:
	virtual void OnLogicThreadProc() override;
	virtual void OnLogicThreadCreate() override;
	virtual void OnLogicThreadExit() override;
	virtual void OnProcessMessage(CNetworkMessage*) override;

public:
	using HttpRequestHandleFunc = std::function<void(const ServerPort&, const ServerMSG::ServiceHttpRequest&)>;
	std::unordered_map<std::string, HttpRequestHandleFunc> m_HttpRequestHandle;
};

CGMService* GMService();
inline auto EventManager() { return GMService()->GetEventManager();}  
inline auto NetMsgProcess() { return GMService()->GetNetMsgProcess();}  

#endif // GMService_h__
