#ifndef GlobalRouteService_h__
#define GlobalRouteService_h__

#include "IService.h"
#include "NetSocket.h"
#include "ServiceComm.h"
#include "UIDFactory.h"

struct event;
class CNetMSGProcess;
class CGlobalRouteService : public IService, public CServiceCommon
{
    CGlobalRouteService();
    bool Init(const ServerPort& nServerPort);
    virtual ~CGlobalRouteService();
    void Destory();
public:
    
    void Release() override { Destory();delete this; }
    CreateNewRealeaseImpl(CGlobalRouteService);

public:
    virtual void OnLogicThreadProc() override;
    virtual void OnLogicThreadCreate() override;
    virtual void OnLogicThreadExit() override;

    void         OnReciveHttp(struct evhttp_request* req);
    virtual void OnProcessMessage(CNetworkMessage*) override;

    void                   AddDelayResponse(uint64_t uid, struct evhttp_request* req);
    struct evhttp_request* FindDelayResponse(uint64_t uid);

private:
    std::map<uint64_t, struct evhttp_request*> m_RequestMap;
};

CGlobalRouteService* GlobalRouteService();
inline auto          EventManager()
{
    return GlobalRouteService()->GetEventManager();
}
inline auto NetMsgProcess()
{
    return GlobalRouteService()->GetNetMsgProcess();
}

#endif // GlobalRouteService_h__
