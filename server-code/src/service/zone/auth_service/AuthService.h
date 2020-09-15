#ifndef MarketService_h__
#define MarketService_h__

#include "IService.h"
#include "NetSocket.h"
#include "ServiceComm.h"
#include "UIDFactory.h"

struct event;
class CNetMSGProcess;
class CAuthService : public IService, public CServiceCommon
{
    CAuthService();
    bool Init(const ServerPort& nServerPort);
    virtual ~CAuthService();
    void Destory();

public:
    void             Release() override;
    export_lua const std::string& GetServiceName() const override { return CServiceCommon::GetServiceName(); }
    CreateNewRealeaseImpl(CAuthService);

public:
    virtual void OnLogicThreadProc() override;
    virtual void OnLogicThreadCreate() override;
    virtual void OnLogicThreadExit() override;

    virtual void OnProcessMessage(CNetworkMessage*) override;


public:
    std::unique_ptr<CMysqlConnection> ConnectGlobalDB();

    CGMManager*                       GetGMManager() const { return m_pGMManager.get(); }
private:
    std::unique_ptr<CGMManager>      m_pGMManager;
};

CAuthService* AuthService();
inline auto     EventManager()
{
    return MarketService()->GetEventManager();
}
inline auto NetMsgProcess()
{
    return MarketService()->GetNetMsgProcess();
}
inline auto GMManager()
{
    return WorldService()->GetGMManager();
}

#endif // MarketService_h__
