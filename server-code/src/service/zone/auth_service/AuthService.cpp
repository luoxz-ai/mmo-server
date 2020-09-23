#include "AuthService.h"

#include <functional>

#include "EventManager.h"
#include "GMManager.h"
#include "MessagePort.h"
#include "MessageRoute.h"
#include "MsgProcessRegister.h"
#include "NetMSGProcess.h"
#include "NetSocket.h"
#include "NetworkMessage.h"
#include "SettingMap.h"
#include "AuthManager.h"
#include "MysqlConnection.h"
#include "server_msg/server_side.pb.h"

static thread_local CAuthService* tls_pService = nullptr;
CAuthService*                     AuthService()
{
    return tls_pService;
}
void SetAuthServicePtr(CAuthService* ptr)
{
    tls_pService = ptr;
}

extern "C" __attribute__((visibility("default"))) IService* ServiceCreate(uint16_t idWorld, uint8_t idServiceType, uint8_t idServiceIdx)
{
    return CAuthService::CreateNew(ServerPort{idWorld, idServiceType, idServiceIdx});
}

//////////////////////////////////////////////////////////////////////////
CAuthService::CAuthService() {}

CAuthService::~CAuthService() {}

void CAuthService::Release()
{

    Destory();
    delete this;
}

void CAuthService::Destory()
{
    tls_pService = this;
    scope_guards scope_exit;
    scope_exit += []() {
        tls_pService = nullptr;
    };
    DestoryServiceCommon();
}

bool CAuthService::Init(const ServerPort& nServerPort)
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

    auto pGlobalDB = ConnectGlobalDB();
    CHECKF(pGlobalDB.get());
    m_pGMManager.reset(CGMManager::CreateNew(pGlobalDB.get()));
    CHECKF(m_pGMManager.get());

    if(CreateService(100) == false)
        return false;
    //注册消息
    {
        auto pNetMsgProcess = GetNetMsgProcess();
        for(const auto& [k, v]: MsgProcRegCenter<CAuthService>::instance().m_MsgProc)
        {
            pNetMsgProcess->Register(k, std::get<0>(v), std::get<1>(v));
        }
    }

    ServerMSG::ServiceReady msg;
    msg.set_serverport(GetServerPort());

    SendProtoMsgToZonePort(ServerPort(GetWorldID(), SOCKET_SERVICE, 0),  msg);
    return true;
}

void CAuthService::OnProcessMessage(CNetworkMessage* pNetworkMsg)
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

void CAuthService::OnLogicThreadProc()
{
    CServiceCommon::OnLogicThreadProc();
}

void CAuthService::OnLogicThreadCreate()
{
    tls_pService = this;
    CServiceCommon::OnLogicThreadCreate();
}

void CAuthService::OnLogicThreadExit()
{
    CServiceCommon::OnLogicThreadExit();
}

std::unique_ptr<CMysqlConnection> CAuthService::ConnectGlobalDB()
{
    const auto& settings        = GetMessageRoute()->GetSettingMap();
    const auto& settingGlobalDB = settings["GlobalMYSQL"][0];
    auto        pGlobalDB       = std::make_unique<CMysqlConnection>();
    if(pGlobalDB->Connect(settingGlobalDB.Query("host"),
                          settingGlobalDB.Query("user"),
                          settingGlobalDB.Query("passwd"),
                          settingGlobalDB.Query("dbname"),
                          settingGlobalDB.QueryULong("port")) == false)
    {
        return nullptr;
    }
    return pGlobalDB;
}
