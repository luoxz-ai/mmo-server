#include "RouteService.h"

#include <functional>

#include "EventManager.h"
#include "MessagePort.h"
#include "MessageRoute.h"
#include "MonitorMgr.h"
#include "MsgProcessRegister.h"
#include "NetMSGProcess.h"
#include "NetSocket.h"
#include "NetworkMessage.h"
#include "SettingMap.h"
#include "server_msg/server_side.pb.h"


static thread_local CRouteService* tls_pService = nullptr;
CRouteService*                     RouteService()
{
    return tls_pService;
}

extern "C" __attribute__((visibility("default"))) IService* ServiceCreate(uint16_t idWorld, uint8_t idServiceType, uint8_t idServiceIdx)
{
    return CRouteService::CreateNew(ServerPort{idWorld, idServiceType, idServiceIdx});
}

//////////////////////////////////////////////////////////////////////////
CRouteService::CRouteService() {}

CRouteService::~CRouteService() {}

void CRouteService::Release()
{

    Destory();
    delete this;
}

void CRouteService::Destory()
{
    tls_pService = this;
    scope_guards scope_exit;
    scope_exit += []() {
        tls_pService = nullptr;
    };
    DestoryServiceCommon();
}

bool CRouteService::Init(const ServerPort& nServerPort)
{
    //各种初始化
    tls_pService = this;
    scope_guards scope_exit;
    scope_exit += []() {
        tls_pService = nullptr;
    };
    CServiceCommon::Init(nServerPort);
    auto oldNdc = BaseCode::SetNdc(GetServiceName());
    scope_exit += [oldNdc]() {
        BaseCode::SetNdc(oldNdc);
    };

    if(CreateService(100, this) == false)
        return false;

    //注册消息
    {
        auto pNetMsgProcess = GetNetMsgProcess();
        for(const auto& [k, v]: MsgProcRegCenter<CRouteService>::instance().m_MsgProc)
        {
            pNetMsgProcess->Register(k, std::get<0>(v), std::get<1>(v));
        }
    }

    return true;
}

void CRouteService::OnPortConnected(CNetSocket* pSocket) {}

void CRouteService::OnPortConnectFailed(CNetSocket*) {}

void CRouteService::OnPortDisconnected(CNetSocket* pSocket) {}

void CRouteService::OnPortAccepted(CNetSocket* pSocket) {}

void CRouteService::OnPortRecvData(CNetSocket* pSocket, byte* pBuffer, size_t len) {}

void CRouteService::OnPortRecvTimeout(CNetSocket* pSocket) {}

ON_SERVERMSG(CRouteService, ServiceRegister)
{
    ServerPort server_port{msg.serverport()};
    LOGMESSAGE("World:{} start", server_port.GetWorldID());
    GetMessageRoute()->SetWorldReady(server_port.GetWorldID(), true);
    for(int32_t i = MIN_GM_PROYX_SERVICE_ID; i <= MAX_GM_PROYX_SERVICE_ID; i++)
    {
        RouteService()->TransmitMsgToPort(ServerPort(0, i), pMsg);
    }

    for(int32_t i = MIN_SHAREZONE_SERVICE_ID; i <= MAX_SHAREZONE_SERVICE_ID; i++)
    {
        RouteService()->TransmitMsgToPort(ServerPort(0, i), pMsg);
    }
}

ON_SERVERMSG(CRouteService, ServiceReady)
{
    ServerPort server_port{msg.serverport()};
    if(msg.ready() == false)
    {
        LOGMESSAGE("World:{} shutdown", server_port.GetWorldID());
    }
    else
    {
        LOGDEBUG("World:{} Ready", server_port.GetWorldID());
    }

    //通知所有的global_route更新
    GetMessageRoute()->SetWorldReady(server_port.GetWorldID(), msg.ready());

    for(int32_t i = MIN_GM_PROYX_SERVICE_ID; i <= MAX_GM_PROYX_SERVICE_ID; i++)
    {
        RouteService()->TransmitMsgToPort(ServerPort(0, i), pMsg);
    }

    for(int32_t i = MIN_SHAREZONE_SERVICE_ID; i <= MAX_SHAREZONE_SERVICE_ID; i++)
    {
        RouteService()->TransmitMsgToPort(ServerPort(0, i), pMsg);
    }
}

void CRouteService::OnProcessMessage(CNetworkMessage* pNetworkMsg)
{

    if(m_pNetMsgProcess->Process(pNetworkMsg) == true)
    {
        return;
    }

    //需要转发的，直接转发
    if(pNetworkMsg->GetForward().IsVaild())
    {
        pNetworkMsg->SetTo(pNetworkMsg->GetForward());
        SendMsgToPort(*pNetworkMsg);
        return;
    }

    switch(pNetworkMsg->GetMultiType())
    {
        case MULTITYPE_BROADCAST:
        {
            //转发给所有的World
            time_t now = TimeGetSecond();
            for(const auto& [k, v]: GetMessageRoute()->GetWorldReadyList())
            {
                if(now < v)
                {
                    TransmitMsgToPort(ServerPort(k, WORLD_SERVICE_ID), pNetworkMsg);
                }
            }
        }
        break;
        case MULTITYPE_VIRTUALSOCKET:
        {
            //转发给对应的Service
            CNetworkMessage send_msg(*pNetworkMsg);
            const auto&     refSet = pNetworkMsg->GetMultiTo();
            for(auto it = refSet.begin(); it != refSet.end(); it++)
            {
                const auto& vs = *it;
                send_msg.SetTo(vs);
                SendMsgToPort(send_msg);
            }
        }
        break;
        default:
            break;
    }
}

void CRouteService::OnLogicThreadProc()
{

    constexpr int32_t MAX_PROCESS_PER_LOOP = 1000;
    int32_t           nCount               = 0;
    CNetworkMessage*  pMsg                 = nullptr;
    if(m_pMessagePort)
    {
        // process message_port msg
        while(nCount < MAX_PROCESS_PER_LOOP && m_pMessagePort->TakePortMsg(pMsg))
        {
            OnProcessMessage(pMsg);
            SAFE_DELETE(pMsg);
        }
    }
}

void CRouteService::OnLogicThreadCreate()
{
    tls_pService = this;
}

void CRouteService::OnLogicThreadExit() {}
