#ifndef ServiceCtrlService_h__
#define ServiceCtrlService_h__

#include "IService.h"
#include "MessagePort.h"
#include "NetSocket.h"
#include "ServiceComm.h"
#include "UIDFactory.h"

struct event;
class CNetMSGProcess;
class CServiceCtrlService : public IService, public CServiceCommon, public CMessagePortEventHandler
{
public:
    CServiceCtrlService(const ServerPort& nServerPort);
    virtual ~CServiceCtrlService();
    void Release() override { delete this; }
    bool Create();

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

CServiceCtrlService* ServiceCtrlService();
inline auto          EventManager()
{
    return ServiceCtrlService()->GetEventManager();
}
inline auto NetMsgProcess()
{
    return ServiceCtrlService()->GetNetMsgProcess();
}

#endif // ServiceCtrlService_h__
