#include "WorldService.h"

#include <functional>

#include "AccountManager.h"
#include "EventManager.h"
#include "MessagePort.h"
#include "MessageRoute.h"
#include "NetMSGProcess.h"
#include "NetSocket.h"
#include "NetworkMessage.h"
#include "SettingMap.h"
#include "Thread.h"
#include "User.h"
#include "UserManager.h"
#include "MemoryHelp.h"

#include "AccountManager.h"
#include "GMManager.h"
#include "MapManager.h"
#include "SystemVars.h"
#include "TeamManager.h"

#include "UserAttr.h"
#include "BornPos.h"

#include "gamedb.h"
#include "globaldb.h"
#include "MonitorMgr.h"
#include "MemoryHelp.h"


static thread_local CWorldService* tls_pService = nullptr;
CWorldService*                     WorldService()
{
    return tls_pService;
}

void SetWorldServicePtr(CWorldService* ptr)
{
    tls_pService = ptr;
}

extern "C" __attribute__((visibility("default"))) IService* ServiceCreate(uint16_t idWorld, uint16_t idService)
{
    return CWorldService::CreateNew(ServerPort{idWorld, idService});
}

//////////////////////////////////////////////////////////////////////////
CWorldService::CWorldService()
{

    m_tLastDisplayTime.Startup(30);
}

CWorldService::~CWorldService()
{
    
}

void CWorldService::Release()  
{   

    Destory();
    delete this; 
}

void CWorldService::Destory()
{
    __ENTER_FUNCTION
    tls_pService = this;
    scope_guards scope_exit;
    scope_exit += []() {
        tls_pService = nullptr;
    };
    StopLogicThread();
    if(m_pAccountManager)
        m_pAccountManager->Destory();
    m_pAccountManager.reset();
    if(m_pUserManager)
        m_pUserManager->Destory();
    m_pUserManager.reset();
    if(m_pTeamManager)
        m_pTeamManager->Destory();
    m_pTeamManager.reset();
    DestoryServiceCommon();

    __LEAVE_FUNCTION
}

bool CWorldService::Init(const ServerPort& nServerPort)
{
    __ENTER_FUNCTION
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
    
    m_pAccountManager.reset(CAccountManager::CreateNew(this));
    CHECKF(m_pAccountManager.get());
    m_pUserManager.reset(CUserManager::CreateNew());
    CHECKF(m_pUserManager.get());
    m_pTeamManager.reset(CTeamManager::CreateNew());
    CHECKF(m_pTeamManager.get());

    extern void RegisterWorldMessageHandler();
    RegisterWorldMessageHandler();

    auto pGlobalDB = ConnectGlobalDB();
    //通过globaldb查询localdb

    auto result = pGlobalDB->Query(TBLD_DBINFO::table_name,
                                        fmt::format(FMT_STRING("SELECT * FROM {} WHERE worldid={} LIMIT 1"),
                                                    TBLD_DBINFO::table_name,
                                                    GetWorldID()));
    if(result == nullptr || result->get_num_row() == 0)
    {
        LOGFATAL("CWorldService::Create fail:gamedb info error");
        return false;
    }

    auto row = result->fetch_row(false);
    if(row)
    {
        std::string host   = (const char*)row->Field(TBLD_DBINFO::DB_IP);
        uint16_t    port   = row->Field(TBLD_DBINFO::DB_PORT);
        std::string user   = (const char*)row->Field(TBLD_DBINFO::DB_USER);
        std::string passwd = (const char*)row->Field(TBLD_DBINFO::DB_PASSWD);
        std::string dbname = (const char*)row->Field(TBLD_DBINFO::DB_NAME);

        auto pDB = std::make_unique<CMysqlConnection>();
        if(pDB->Connect(host, user, passwd, dbname, port) == false)
        {
            
            return false;
        }
        m_pGameDB.reset(pDB.release());

        {
            CHECKF(m_pGameDB->CheckTable<TBLD_PLAYER>());
            CHECKF(m_pGameDB->CheckTable<TBLD_ITEM>());
            CHECKF(m_pGameDB->CheckTable<TBLD_SKILL>());
            CHECKF(m_pGameDB->CheckTable<TBLD_STATUS>());
            CHECKF(m_pGameDB->CheckTable<TBLD_GUILD>());
            CHECKF(m_pGameDB->CheckTable<TBLD_MAIL>());
            CHECKF(m_pGameDB->CheckTable<TBLD_PET>());
        }

        m_nCurPlayerMaxID       = GetDefaultPlayerID(GetWorldID());
        auto result_playercount = m_pGameDB->UnionQuery(
            fmt::format(FMT_STRING("SELECT ifnull(max(id),{}) as id FROM tbld_player"), m_nCurPlayerMaxID));
        if(result_playercount && result_playercount->get_num_row() == 1)
        {
            auto row_result = result_playercount->fetch_row(false);
            if(row_result)
            {
                m_nCurPlayerMaxID = (int64_t)row_result->Field(TBLD_PLAYER::ID);
                m_nCurPlayerMaxID++;
            }
        }
    }
    else
    {
        LOGFATAL("CWorldService::Create fail:gamedb info error");
        return false;
    }
    
    DEFINE_CONFIG_LOAD(CUserAttrSet);
    DEFINE_CONFIG_LOAD(CBornPosSet);

    m_pMapManager.reset(CMapManager::CreateNew(0));
    CHECKF(m_pMapManager.get());

    m_pGMManager.reset(CGMManager::CreateNew(pGlobalDB.get()));
    CHECKF(m_pGMManager.get());

    m_pSystemVarSet.reset(CSystemVarSet::CreateNew());
    CHECKF(m_pSystemVarSet.get());

    if(CreateService(20) == false)
        return false;


    GetMessageRoute()->ForeachServiceInfoByWorldID(GetWorldID(), false, [this](const ServerAddrInfo* info)
    {
        if(info->idService == WORLD_SERVICE_ID || 
           (info->idService >= MIN_AI_SERVICE_ID && info->idService <= MAX_AI_SERVICE_ID))
        {
            return true;
        }

        m_setServiceNeedReady.emplace(info->idService);
        return true;
    });


    return true;
    __LEAVE_FUNCTION
    return false;
}

void CWorldService::OnProcessMessage(CNetworkMessage* pNetworkMsg)
{
    __ENTER_FUNCTION
    //只需要处理来自其他服务器的消息
    //来自客户端的消息已经直接发往对应服务器了
    MSG_HEAD* pHead = pNetworkMsg->GetMsgHead();

    //需要转发的，直接转发
    if(pNetworkMsg->GetForward().IsVaild() &&
        pNetworkMsg->GetForward().GetServerPort() != GetServerPort() )
    {
        if(GetMessageRoute()->IsConnected(pNetworkMsg->GetForward().GetServerPort()) == true)
        {
            pNetworkMsg->SetTo(pNetworkMsg->GetForward());
            SendPortMsg(*pNetworkMsg);
        }
        else
        {
            //尝试使用route来中转
            pNetworkMsg->SetTo(ServerPort(0, ROUTE_SERVICE_ID));
            SendPortMsg(*pNetworkMsg);
        }
        return;
    }

    if(m_pNetMsgProcess->Process(pNetworkMsg) == false)
    {
        LOGERROR("CMD {} didn't have ProcessHandler", pNetworkMsg->GetCmd());
        return;
    }
        
    __LEAVE_FUNCTION
}

void CWorldService::_ID2VS(OBJID id, VirtualSocketMap_t& VSMap)const
{
    __ENTER_FUNCTION
    CUser* pUser = GetUserManager()->QueryUser(id);
    if(pUser)
    {
        VSMap[pUser->GetSocket().GetServerPort()].push_back(pUser->GetSocket());
    }
    __LEAVE_FUNCTION
}

uint64_t CWorldService::CreatePlayerID()
{
    if(m_setPlayerIDPool.empty() == true)
    {
        return m_nCurPlayerMaxID++;
    }
    else
    {
        OBJID idPlayer = m_setPlayerIDPool.front();
        m_setPlayerIDPool.pop_front();
        return idPlayer;
    }
}

void CWorldService::RecyclePlayerID(OBJID idPlayer)
{
    m_setPlayerIDPool.push_back(idPlayer);
}

bool CWorldService::CheckProgVer(const std::string& prog_ver)const
{
    return true;
}

std::unique_ptr<CMysqlConnection> CWorldService::ConnectGlobalDB()
{
    const auto& settings = GetMessageRoute()->GetSettingMap();
    const auto& settingGlobalDB = settings["GlobalMYSQL"][0];
    auto pGlobalDB = std::make_unique<CMysqlConnection>();
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

void CWorldService::SetServiceReady(uint16_t idService)
{
    __ENTER_FUNCTION

    if(m_setServiceNeedReady.empty())
        return;

    LOGMESSAGE("ServiceReady:{}", ::GetServiceName(idService));

    m_setServiceNeedReady.erase(idService);
    if(m_setServiceNeedReady.empty() == true)
    {
        LOGMESSAGE("AllServiceReady");

        {
            ServerMSG::ServiceReady msg;
            msg.set_serverport(GetServerPort());
            msg.set_ready(true);
            // send notify to socket
            for(uint32_t i = MIN_SOCKET_SERVICE_ID; i <= MAX_SOCKET_SERVICE_ID; i++)
            {
                SendPortMsg(ServerPort(GetWorldID(), i), msg);
            }
            SendPortMsg(ServerPort(GetWorldID(), GM_SERVICE_ID), msg);
        }
    }

    __LEAVE_FUNCTION
}

bool CWorldService::BroadcastToAllPlayer(const google::protobuf::Message& msg)const
{
    return BroadcastToAllPlayer(to_sc_cmd(msg), msg);
}

bool CWorldService::BroadcastToAllPlayer(uint16_t nCmd, const google::protobuf::Message& msg)const
{
    __ENTER_FUNCTION
    CNetworkMessage _msg(nCmd, msg, GetServerVirtualSocket(), 0);
    for(uint32_t i = MIN_SOCKET_SERVICE_ID; i <= MAX_SOCKET_SERVICE_ID; i++)
    {
        _msg.SetTo(VirtualSocket(ServerPort(GetWorldID(), i), 0));
        SendBroadcastMsg(_msg);
    }
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CWorldService::BroadcastToZone(const google::protobuf::Message& msg)const
{
    return BroadcastToZone(to_server_msgid(msg), msg);
}

bool CWorldService::BroadcastToZone(uint16_t nCmd, const google::protobuf::Message& msg)const
{
    __ENTER_FUNCTION
    for(uint32_t i = MIN_ZONE_SERVICE_ID; i <= MAX_ZONE_SERVICE_ID; i++)
    {
        if(i == GetServiceID())
            continue;
        SendPortMsg(ServerPort(GetWorldID(), i), nCmd, msg);
    }
    return true;
    __LEAVE_FUNCTION
    return false;
}

void CWorldService::OnLogicThreadProc()
{
    __ENTER_FUNCTION

    CServiceCommon::OnLogicThreadProc();

    //其他逻辑执行
    m_pAccountManager->OnTimer();

    if(m_tLastDisplayTime.ToNextTime())
    {
        std::string buf = std::string("\n======================================================================") +
                          fmt::format(FMT_STRING("\nMessageProcess:{}"), GetMessageProcess()) +
                          fmt::format(FMT_STRING("\nEvent:{}\tMem:{}"), EventManager()->GetEventCount(), get_thread_memory_allocted()) +
                          fmt::format(FMT_STRING("\nAccount:{}\tWait:{}"),
                                      AccountManager()->GetAccountSize(),
                                      AccountManager()->GetWaitAccountSize());

        auto check_func = [this, &buf](uint16_t idService)
        {
            auto pMessagePort = GetMessageRoute()->QueryMessagePort(ServerPort(GetWorldID(), idService), false);
            if(pMessagePort && pMessagePort->GetWriteBufferSize() > 0)
            {
                buf += fmt::format(FMT_STRING("\nMsgPort:{}-{}\tSendBuff:{}"),
                                   pMessagePort->GetServerPort().GetWorldID(),
                                   pMessagePort->GetServerPort().GetServiceID(),
                                   pMessagePort->GetWriteBufferSize());
            }
        };
        

        for(int i = MIN_SOCKET_SERVICE_ID; i <= MAX_SOCKET_SERVICE_ID; i++)
        {
            check_func(i);
        }
        for(int i = MIN_ZONE_SERVICE_ID; i <= MAX_SHAREZONE_SERVICE_ID; i++)
        {
            check_func(i);
        }
        

        LOGMONITOR("{}", buf.c_str());
        m_pMonitorMgr->Print();
        SetMessageProcess(0);
    }

    __LEAVE_FUNCTION
}

void CWorldService::OnLogicThreadCreate()
{
    tls_pService = this;
    CServiceCommon::OnLogicThreadCreate();
}

void CWorldService::OnLogicThreadExit()
{
    CServiceCommon::OnLogicThreadExit();
}
