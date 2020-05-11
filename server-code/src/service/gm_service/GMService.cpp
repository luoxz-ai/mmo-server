#include "GMService.h"

#include <functional>
#include <unordered_map>

#include <sys/queue.h>

#include "EventManager.h"
#include "MessagePort.h"
#include "MessageRoute.h"
#include "MonitorMgr.h"
#include "MsgProcessRegister.h"
#include "NetMSGProcess.h"
#include "NetSocket.h"
#include "NetworkMessage.h"
#include "RPCService.h"
#include "SettingMap.h"
#include "brpc/server.h"
#include "event2/http.h"
#include "event2/keyvalq_struct.h"
#include "gm_service.pb.h"
#include "server_msg/server_side.pb.h"
#include "proto_help.h"

namespace Game
{
    // Service with static path.
    class GM_ServiceImpl : public GM_Service
    {
        CGMService* m_pService;

    public:
        GM_ServiceImpl(CGMService* pService)
            : m_pService(pService){};
        virtual ~GM_ServiceImpl(){};
        virtual void SetGM(google::protobuf::RpcController* cntl_base,
                           const SetGMRequest*              request,
                           SetGMResponse*                   response,
                           google::protobuf::Closure*       done) override
        {
            // This object helps you to call done->Run() in RAII style. If you need
            // to process the request asynchronously, pass done_guard.release().
            brpc::ClosureGuard done_guard(done);

            brpc::Controller* cntl = static_cast<brpc::Controller*>(cntl_base);

            LOGMESSAGE_NOFMT(fmt::format(FMT_STRING("Received SetGM[log_id={}]: open_id={} gm_level={} sign={}"),
                                         cntl->log_id(),
                                         request->open_id(),
                                         request->gm_level(),
                                         request->sign()));
            UNUSED(m_pService);
            // m_pService->ProcessGM(request, response, done_guard.release());
        }
        virtual void BlockLogin(google::protobuf::RpcController* cntl_base,
                                const BlockLoginRequest*         request,
                                BlockLoginResponse*              response,
                                google::protobuf::Closure*       done) override
        {
            // This object helps you to call done->Run() in RAII style. If you need
            // to process the request asynchronously, pass done_guard.release().
            brpc::ClosureGuard done_guard(done);

            brpc::Controller* cntl = static_cast<brpc::Controller*>(cntl_base);

            LOGMESSAGE_NOFMT(fmt::format(
                FMT_STRING(
                    "Received BlockLogin[log_id={}]: server_id={} open_id={} actor_id={} block_timestamp={} sign={}"),
                cntl->log_id(),
                request->server_id(),
                request->open_id(),
                request->actor_id(),
                request->block_timestamp(),
                request->sign()));

            // send to server_x:gm_service

            // wait for response
        }
        virtual void MuteChat(google::protobuf::RpcController* cntl_base,
                              const MuteChatRequest*           request,
                              MuteChatResponse*                response,
                              google::protobuf::Closure*       done) override
        {
            // This object helps you to call done->Run() in RAII style. If you need
            // to process the request asynchronously, pass done_guard.release().
            brpc::ClosureGuard done_guard(done);

            brpc::Controller* cntl = static_cast<brpc::Controller*>(cntl_base);

            LOGMESSAGE_NOFMT(fmt::format(
                FMT_STRING(
                    "Received MuteChat[log_id={}]: server_id={} open_id={} actor_id={} mute_timestamp={} sign={}"),
                cntl->log_id(),
                request->server_id(),
                request->open_id(),
                request->actor_id(),
                request->mute_timestamp(),
                request->sign()));
        }
    };
} // namespace Game

static thread_local CGMService* tls_pService = nullptr;
CGMService*                     GMService()
{
    return tls_pService;
}

extern "C" __attribute__((visibility("default"))) IService* ServiceCreate(uint16_t idWorld, uint16_t idService)
{
    return CGMService::CreateNew(ServerPort{idWorld, idService});
}

//////////////////////////////////////////////////////////////////////////
CGMService::CGMService() {}

CGMService::~CGMService() {}

void CGMService::Release()
{

    Destory();
    delete this;
}

void CGMService::Destory()
{
    __ENTER_FUNCTION
    tls_pService = this;
    scope_guards scope_exit;
    scope_exit += []() {
        tls_pService = nullptr;
    };

    if(m_pRPCService)
    {
        m_pRPCService->StopRPCServer();
        m_pRPCService->ClearRPCServices();
        m_pRPCService.reset();
    }

    DestoryServiceCommon();

    __LEAVE_FUNCTION
}

bool CGMService::Init(const ServerPort& nServerPort)
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

    if(CreateService(200) == false)
        return false;

    //注册消息
    {
        auto pNetMsgProcess = GetNetMsgProcess();
        for(const auto& [k, v]: MsgProcRegCenter<CGMService>::instance().m_MsgProc)
        {
            pNetMsgProcess->Register(k, v, cmd_to_enum_name(k));
        }
    }

    {
        const ServerAddrInfo* pAddrInfo = GetMessageRoute()->QueryServiceInfo(GetServerPort());
        if(pAddrInfo == nullptr)
        {
            LOGFATAL("CGMService QueryServerInfo {} fail", GetServerPort().GetServiceID());
            return false;
        }
        m_pRPCService.reset(CRPCService::CreateNew(GetServerPort().GetServiceID()));
        CHECKF(m_pRPCService.get());
        CHECKF(m_pRPCService->AddRPCService(new Game::GM_ServiceImpl(this)));
        CHECKF(m_pRPCService->StartRPCServer(pAddrInfo->publish_port, pAddrInfo->debug_port));
    }
    LOGMESSAGE("GMService {} Create", GetServerPort().GetServiceID());

    ServerMSG::ServiceReady msg;
    msg.set_serverport(GetServerPort());

    SendPortMsg(ServerPort(GetWorldID(), WORLD_SERVICE_ID), ServerMSG::MsgID_ServiceReady, msg);

    return true;
}

void CGMService::SendServiceReady()
{
    ServerMSG::ServiceReady send_msg;
    send_msg.set_serverport(GMService()->GetServerPort());
    send_msg.set_ready(true);
    SendPortMsg(ServerPort(0, ROUTE_SERVICE_ID), send_msg);
}

void CGMService::SendServiceUnReady()
{
    ServerMSG::ServiceReady send_msg;
    send_msg.set_serverport(GMService()->GetServerPort());
    send_msg.set_ready(false);
    SendPortMsg(ServerPort(0, ROUTE_SERVICE_ID), send_msg);
}

ON_SERVERMSG(CGMService, ServiceReady)
{
    //发送一条消息给ServiceCtrl,通知有一个新的服被开启了, 要求ServiceCtrl通知所有GlobalRoute服,更新路由信息
    ServerMSG::ServiceRegister send_msg;
    send_msg.set_serverport(GMService()->GetServerPort());
    send_msg.set_update_time(TimeGetSecond());
    GMService()->SendPortMsg(ServerPort(0, ROUTE_SERVICE_ID), send_msg);

    LOGMESSAGE("WorldReady: {}", GMService()->GetServerPort().GetWorldID());

    CEventEntryCreateParam param;
    param.cb = []() { GMService()->SendServiceReady(); };
    param.tWaitTime = 30 * 1000;
    param.bPersist = true;
    EventManager()->ScheduleEvent(param);
}

ON_SERVERMSG(CGMService, ServiceHttpRequest)
{
    LOGMESSAGE("recv_httprequest:{}", msg.uid());
    const std::string& mothed  = msg.kvmap().at("mothed");
    auto               handler = GMService()->QueryHttpRequestHandler(mothed);
    if(handler)
    {
        std::invoke(*handler, pMsg->GetFrom().GetServerPort(), msg);
        LOGMESSAGE("finish_httprequest:{}", msg.uid());
    }
    else
    {
        ServerMSG::ServiceHttpResponse send_msg;
        send_msg.set_uid(msg.uid());
        send_msg.set_response_code(HTTP_BADMETHOD);
        GMService()->SendPortMsg(pMsg->GetFrom().GetServerPort(), send_msg);
    }
}

const CGMService::HttpRequestHandleFunc* CGMService::QueryHttpRequestHandler(const std::string& mothed) const
{
    auto it = m_HttpRequestHandle.find(mothed);
    if(it != m_HttpRequestHandle.end())
    {
        return &it->second;
    }

    return nullptr;
}

void CGMService::OnProcessMessage(CNetworkMessage* pNetworkMsg)
{
    if(m_pNetMsgProcess->Process(pNetworkMsg) == false)
    {
        LOGERROR("CMD {} from {} to {} forward {} didn't have ProcessHandler", 
                cmd_to_enum_name(pNetworkMsg->GetCmd()),
                pNetworkMsg->GetFrom(),
                pNetworkMsg->GetTo(),
                pNetworkMsg->GetForward());
    }
}

void CGMService::OnLogicThreadProc()
{

    CServiceCommon::OnLogicThreadProc();
}

void CGMService::OnLogicThreadCreate()
{
    tls_pService = this;
    CServiceCommon::OnLogicThreadCreate();
}

void CGMService::OnLogicThreadExit()
{
    CServiceCommon::OnLogicThreadExit();
}
