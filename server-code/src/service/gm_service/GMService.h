#ifndef GMService_h__
#define GMService_h__

#include <functional>
#include <optional>
#include <unordered_map>

#include "IService.h"
#include "NetSocket.h"
#include "ServiceComm.h"
#include "UIDFactory.h"
#include "server_msg/server_side.pb.h"

struct event;
class CNetMSGProcess;
class CRPCService;
class CGMService : public IService, public CServiceCommon
{
    CGMService();
    bool Init(const ServerPort& nServerPort);
    virtual ~CGMService();
    void Destory();

public:
    void             Release() override;
    export_lua const std::string& GetServiceName() const override { return CServiceCommon::GetServiceName(); }
    CreateNewRealeaseImpl(CGMService);

public:
    void SendServiceReady();
    void SendServiceUnReady();
    using HttpRequestHandleFunc = std::function<void(const ServerPort&, const ServerMSG::ServiceHttpRequest&)>;
    const CGMService::HttpRequestHandleFunc* QueryHttpRequestHandler(const std::string& mothed) const;

public:
    virtual void OnLogicThreadProc() override;
    virtual void OnLogicThreadCreate() override;
    virtual void OnLogicThreadExit() override;
    virtual void OnProcessMessage(CNetworkMessage*) override;

public:
    std::unique_ptr<CRPCService> m_pRPCService;

    std::unordered_map<std::string, HttpRequestHandleFunc> m_HttpRequestHandle;
};

CGMService* GMService();
inline auto EventManager()
{
    return GMService()->GetEventManager();
}
inline auto NetMsgProcess()
{
    return GMService()->GetNetMsgProcess();
}

#endif // GMService_h__
