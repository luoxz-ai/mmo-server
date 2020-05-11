#include "MarketService.h"

#include <functional>

#include "EventManager.h"
#include "MessagePort.h"
#include "MessageRoute.h"
#include "MsgProcessRegister.h"
#include "NetMSGProcess.h"
#include "NetSocket.h"
#include "NetworkMessage.h"
#include "SettingMap.h"
#include "server_msg/server_side.pb.h"
#include "proto_help.h"

static thread_local CMarketService* tls_pService = nullptr;
CMarketService*                     MarketService()
{
    return tls_pService;
}

extern "C" __attribute__((visibility("default"))) IService* ServiceCreate(uint16_t idWorld, uint16_t idService)
{
    return CMarketService::CreateNew(ServerPort{idWorld, idService});
}

//////////////////////////////////////////////////////////////////////////
CMarketService::CMarketService() {}

CMarketService::~CMarketService() {}

void CMarketService::Release()
{

    Destory();
    delete this;
}

void CMarketService::Destory()
{
    tls_pService = this;
    scope_guards scope_exit;
    scope_exit += []() {
        tls_pService = nullptr;
    };
    DestoryServiceCommon();
}

bool CMarketService::Init(const ServerPort& nServerPort)
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

    if(CreateService(100) == false)
        return false;

    //注册消息
    {
        auto pNetMsgProcess = GetNetMsgProcess();
        for(const auto& [k, v]: MsgProcRegCenter<CMarketService>::instance().m_MsgProc)
        {
            pNetMsgProcess->Register(k, v, cmd_to_enum_name(k));
        }
    }

    ServerMSG::ServiceReady msg;
    msg.set_serverport(GetServerPort());

    SendPortMsg(ServerPort(GetWorldID(), WORLD_SERVICE_ID), ServerMSG::MsgID_ServiceReady, msg);
    return true;
}

void CMarketService::OnProcessMessage(CNetworkMessage* pNetworkMsg)
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

void CMarketService::OnLogicThreadProc()
{
    CServiceCommon::OnLogicThreadProc();
}

void CMarketService::OnLogicThreadCreate()
{
    tls_pService = this;
    CServiceCommon::OnLogicThreadCreate();
}

void CMarketService::OnLogicThreadExit()
{
    CServiceCommon::OnLogicThreadExit();
}
