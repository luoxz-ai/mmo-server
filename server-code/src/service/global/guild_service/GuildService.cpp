#include "GuildService.h"

#include <functional>

#include "EventManager.h"
#include "GuildManager.h"
#include "MessagePort.h"
#include "MessageRoute.h"
#include "MsgProcessRegister.h"
#include "NetMSGProcess.h"
#include "NetSocket.h"
#include "NetworkMessage.h"
#include "SettingMap.h"
#include "server_msg/server_side.pb.h"

static thread_local CGuildService* tls_pService = nullptr;
CGuildService*                     GuildService()
{
    return tls_pService;
}
void SetGuildServicePtr(CGuildService* ptr)
{
    tls_pService = ptr;
}

extern "C" __attribute__((visibility("default"))) IService* ServiceCreate(WorldID_t idWorld, ServiceType_t idServiceType, ServiceIdx_t idServiceIdx)
{
    return CGuildService::CreateNew(ServerPort{idWorld, idServiceType, idServiceIdx});
}

//////////////////////////////////////////////////////////////////////////
CGuildService::CGuildService() {}

CGuildService::~CGuildService() {}

void CGuildService::Release()
{

    Destory();
    delete this;
}

void CGuildService::Destory()
{
    tls_pService = this;
    scope_guards scope_exit;
    scope_exit += []() {
        tls_pService = nullptr;
    };
    DestoryServiceCommon();
}

bool CGuildService::Init(const ServerPort& nServerPort)
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

    m_UIDFactory.Init(0, GUILD_SERIVE_UID);

    auto pGlobalDB = ConnectGlobalDB();

    CHECKF(pGlobalDB.get());
    m_pGlobalDB.reset(pGlobalDB.release());

    m_pGuildManager.reset(CGuildManager::CreateNew());
    CHECKF(m_pGuildManager.get());

    if(CreateService(100) == false)
        return false;

    //注册消息
    {
        auto pNetMsgProcess = GetNetMsgProcess();
        for(const auto& [k, v]: MsgProcRegCenter<CGuildService>::instance().m_MsgProc)
        {
            pNetMsgProcess->Register(k, std::get<0>(v), std::get<1>(v));
        }
    }

    return true;
}

void CGuildService::OnProcessMessage(CNetworkMessage* pNetworkMsg)
{
    if(m_pNetMsgProcess->Process(pNetworkMsg) == false)
    {
        LOGERROR("CMD {} from {} to {} forward_size {} didn't have ProcessHandler",
                 pNetworkMsg->GetCmd(),
                 pNetworkMsg->GetFrom(),
                 pNetworkMsg->GetTo(),
                 pNetworkMsg->GetForward().size());
    }
}

void CGuildService::OnLogicThreadProc()
{
    CServiceCommon::OnLogicThreadProc();
}

void CGuildService::OnLogicThreadCreate()
{
    tls_pService = this;
    CServiceCommon::OnLogicThreadCreate();
}

void CGuildService::OnLogicThreadExit()
{
    CServiceCommon::OnLogicThreadExit();
}
