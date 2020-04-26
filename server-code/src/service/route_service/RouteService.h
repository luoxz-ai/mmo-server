#ifndef RouteService_h__
#define RouteService_h__

#include "IService.h"
#include "MessagePort.h"
#include "NetSocket.h"
#include "ServiceComm.h"
#include "UIDFactory.h"

struct event;
class CNetMSGProcess;
class CRouteService : public IService, public CServiceCommon, public CMessagePortEventHandler
{
    CRouteService();
    bool Init(const ServerPort& nServerPort);
    virtual ~CRouteService();
    void Destory();
public:
    

    void Release() override;
    export_lua const std::string& GetServiceName() const override{ return CServiceCommon::GetServiceName(); }
    CreateNewRealeaseImpl(CRouteService);
public:
    // connect to other server succ
    virtual void OnPortConnected(CNetSocket*) override;
    // connect to other server failed, can set CNetSocket::setReconnectTimes = 0 to stop reconnect
    virtual void OnPortConnectFailed(CNetSocket*) override;
    // lost connect
    virtual void OnPortDisconnected(CNetSocket*) override;
    // accept a new client
    virtual void OnPortAccepted(CNetSocket*) override;
    // receive data
    virtual void OnPortRecvData(CNetSocket*, byte* pBuffer, size_t len) override;
    // recv over time
    virtual void OnPortRecvTimeout(CNetSocket*) override;
    virtual void OnLogicThreadProc() override;
    virtual void OnLogicThreadCreate() override;
    virtual void OnLogicThreadExit() override;

    virtual void OnProcessMessage(CNetworkMessage*) override;
};

CRouteService* RouteService();
inline auto          EventManager()
{
    return RouteService()->GetEventManager();
}
inline auto NetMsgProcess()
{
    return RouteService()->GetNetMsgProcess();
}

#endif // RouteService_h__
