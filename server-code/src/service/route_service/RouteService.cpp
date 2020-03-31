#include "RouteService.h"

#include <functional>

#include "EventManager.h"
#include "MessagePort.h"
#include "MessageRoute.h"
#include "NetMSGProcess.h"
#include "NetSocket.h"
#include "NetworkMessage.h"
#include "SettingMap.h"

static thread_local CRouteService* tls_pService = nullptr;
CRouteService*                     RouteService()
{
    return tls_pService;
}

extern "C" __attribute__((visibility("default"))) IService* ServiceCreate(uint16_t idWorld, uint16_t idService)
{
    return CRouteService::CreateNew(ServerPort{idWorld, idService});
}

//////////////////////////////////////////////////////////////////////////
CRouteService::CRouteService()
{
}

CRouteService::~CRouteService()
{

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
        ;
    };

    if(CreateService(20, this) == false)
        return false;

    return true;
}

void CRouteService::OnPortConnected(CNetSocket* pSocket) {}

void CRouteService::OnPortConnectFailed(CNetSocket*) {}

void CRouteService::OnPortDisconnected(CNetSocket* pSocket) {}

void CRouteService::OnPortAccepted(CNetSocket* pSocket) {}

void CRouteService::OnPortRecvData(CNetSocket* pSocket, byte* pBuffer, size_t len) {}

void CRouteService::OnPortRecvTimeout(CNetSocket* pSocket) {}

void CRouteService::OnProcessMessage(CNetworkMessage* pNetworkMsg)
{
    switch(pNetworkMsg->GetMsgHead()->usCmd)
    {
        case NETMSG_SCK_CLOSE:
        {
            MSG_SCK_CLOSE* pMsg = (MSG_SCK_CLOSE*)pNetworkMsg->GetBuf();
        }
        break;
        case NETMSG_INTERNAL_SERVICE_REGISTER:
        {

            //通知所有的global_route更新
            MSG_INTERNAL_SERVICE_REGISTER* pMsg = (MSG_INTERNAL_SERVICE_REGISTER*)pNetworkMsg->GetBuf();
            LOGMESSAGE("World:{} start", pMsg->idWorld);
            GetMessageRoute()->SetWorldReady(pMsg->idWorld, true);
            for(int32_t i = MIN_GM_PROYX_SERVICE_ID; i <= MAX_GM_PROYX_SERVICE_ID; i++)
            {
                SendPortMsg(ServerPort(0, i), (byte*)pMsg, sizeof(*pMsg));
            }

            for(int32_t i = MIN_SHAREZONE_SERVICE_ID; i <= MAX_SHAREZONE_SERVICE_ID; i++)
            {
                SendPortMsg(ServerPort(0, i), (byte*)pMsg, sizeof(*pMsg));
            }
        }
        break;
        case NETMSG_INTERNAL_SERVICE_READY:
        {
            MSG_INTERNAL_SERVICE_READY* pMsg = (MSG_INTERNAL_SERVICE_READY*)pNetworkMsg->GetBuf();
            if(pMsg->bReady == false)
            {
                LOGMESSAGE("World:{} shutdown", pMsg->idWorld);
            }
            else
            {
                LOGDEBUG("World:{} Ready", pMsg->idWorld);
            }

            //通知所有的global_route更新
            GetMessageRoute()->SetWorldReady(pMsg->idWorld, pMsg->bReady);

            for(int32_t i = MIN_GM_PROYX_SERVICE_ID; i <= MAX_GM_PROYX_SERVICE_ID; i++)
            {
                SendPortMsg(ServerPort(0, i), (byte*)pMsg, sizeof(*pMsg));
            }

            for(int32_t i = MIN_SHAREZONE_SERVICE_ID; i <= MAX_SHAREZONE_SERVICE_ID; i++)
            {
                SendPortMsg(ServerPort(0, i), (byte*)pMsg, sizeof(*pMsg));
            }
        }
        break;
        default:
        {
            //需要转发的，直接转发
            if(pNetworkMsg->GetForward().IsVaild())
            {
                pNetworkMsg->SetTo(pNetworkMsg->GetForward());
                SendMsg(*pNetworkMsg);
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
                            SendPortMsg(ServerPort(k, WORLD_SERVICE_ID), pNetworkMsg->GetBuf(), pNetworkMsg->GetSize());
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
                        SendMsg(send_msg);
                    }
                }
                break;
                default:
                    break;
            }

            m_pNetMsgProcess->Process(pNetworkMsg);
        }
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
        while(nCount < MAX_PROCESS_PER_LOOP && m_pMessagePort->TakeMsg(pMsg))
        {
            OnProcessMessage(pMsg);
            SAFE_DELETE(pMsg);
        }
    }
}

void CRouteService::OnLogicThreadCreate()
{
    tls_pService = this;
    BaseCode::SetNdc("Route");
    LOGMESSAGE("ThreadID:{}", get_cur_thread_id());
}

void CRouteService::OnLogicThreadExit()
{
    LOGMESSAGE("ExitThreadID:{}", get_cur_thread_id());
    BaseCode::ClearNdc();
    ;
}
