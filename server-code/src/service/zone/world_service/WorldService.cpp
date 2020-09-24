#include "WorldService.h"

#include <functional>

#include "AccountManager.h"
#include "BornPos.h"
#include "EventManager.h"
#include "GMManager.h"

#include "MapManager.h"
#include "MemoryHelp.h"
#include "MessagePort.h"
#include "MessageRoute.h"
#include "MonitorMgr.h"
#include "MsgWorldProcess.h"
#include "MysqlTableCheck.h"
#include "NetMSGProcess.h"
#include "NetSocket.h"
#include "NetworkMessage.h"
#include "SettingMap.h"
#include "SystemVars.h"
#include "TeamManager.h"
#include "Thread.h"
#include "User.h"
#include "UserAttr.h"
#include "UserManager.h"
#include "gamedb.h"
#include "globaldb.h"
#include "serverinfodb.h"

static thread_local CWorldService* tls_pService = nullptr;
CWorldService*                     WorldService()
{
    return tls_pService;
}

void SetWorldServicePtr(CWorldService* ptr)
{
    tls_pService = ptr;
}

extern "C" __attribute__((visibility("default"))) IService* ServiceCreate(uint16_t idWorld, uint8_t idServiceType, uint8_t idServiceIdx)
{
    return CWorldService::CreateNew(ServerPort{idWorld, idServiceType, idServiceIdx});
}

//////////////////////////////////////////////////////////////////////////
CWorldService::CWorldService()
{

    m_tLastDisplayTime.Startup(30);
}

CWorldService::~CWorldService() {}

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

    m_UIDFactory.Init(GetServerPort().GetWorldID(), WORLD_SERVICE_UID);
    m_pAccountManager.reset(CAccountManager::CreateNew(this));
    CHECKF(m_pAccountManager.get());
    m_pUserManager.reset(CUserManager::CreateNew());
    CHECKF(m_pUserManager.get());
    m_pTeamManager.reset(CTeamManager::CreateNew());
    CHECKF(m_pTeamManager.get());

    RegisterWorldMessageHandler();

    auto pGlobalDB = ConnectGlobalDB();
    CHECKF(pGlobalDB.get());
    CHECKF(MysqlTableCheck::CheckAllTable<GLOBALDB_TABLE_LIST>(pGlobalDB.get()));
    //通过globaldb查询localdb

    auto result = pGlobalDB->QueryKeyLimit<TBLD_DBINFO, TBLD_DBINFO::WORLDID>(GetWorldID(), 1);
    if(result == nullptr || result->get_num_row() == 0)
    {
        LOGFATAL("CWorldService::Create fail:gamedb info error");
        return false;
    }

    auto row = result->fetch_row(false);
    if(row)
    {
        std::string host   = row->Field(TBLD_DBINFO::DB_IP).get<std::string>();
        uint16_t    port   = row->Field(TBLD_DBINFO::DB_PORT);
        std::string user   = row->Field(TBLD_DBINFO::DB_USER).get<std::string>();
        std::string passwd = row->Field(TBLD_DBINFO::DB_PASSWD).get<std::string>();
        std::string dbname = row->Field(TBLD_DBINFO::DB_NAME).get<std::string>();

        auto pDB = std::make_unique<CMysqlConnection>();
        if(pDB->Connect(host, user, passwd, dbname, port) == false)
        {

            return false;
        }
        m_pGameDB.reset(pDB.release());

        CHECKF(MysqlTableCheck::CheckAllTableAndFix<GAMEDB_TABLE_LIST>(m_pGameDB.get()));

        m_nCurPlayerMaxID = GetDefaultPlayerID(GetWorldID());
        auto result_playercount =
            m_pGameDB->UnionQuery(fmt::format(FMT_STRING("SELECT ifnull(max(id),{}) as id FROM tbld_player"), m_nCurPlayerMaxID));
        if(result_playercount && result_playercount->get_num_row() == 1)
        {
            auto row_result = result_playercount->fetch_row(false);
            if(row_result)
            {
                m_nCurPlayerMaxID = row_result->Field(0).get<int64_t>();
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

    if(CreateService(100) == false)
        return false;

    //设置等待哪些服ready
    GetMessageRoute()->ForeachServiceInfoByWorldID(GetWorldID(), false, [this](const ServerAddrInfo* info) {
        if(info->idServiceType == WORLD_SERVICE || info->idServiceType == AI_SERVICE || info->idServiceType == AUTH_SERVICE)
        {
            return true;
        }

        m_setServiceNeedReady.emplace(info->idServiceType, info->idServiceIdx);
        return true;
    });

    return true;
    __LEAVE_FUNCTION
    return false;
}

uint64_t CWorldService::CreateUID()
{
    return m_UIDFactory.CreateID();
}

void CWorldService::OnProcessMessage(CNetworkMessage* pNetworkMsg)
{
    __ENTER_FUNCTION
    //只需要处理来自其他服务器的消息
    //来自客户端的消息已经直接发往对应服务器了
    MSG_HEAD* pHead = pNetworkMsg->GetMsgHead();

    //需要转发的，直接转发
    if(pNetworkMsg->GetForward().empty() == false && pNetworkMsg->GetForward().front().GetServerPort() != GetServerPort())
    {
        if(GetMessageRoute()->IsConnected(pNetworkMsg->GetForward().front().GetServerPort()) == true)
        {
            pNetworkMsg->SetTo(pNetworkMsg->GetForward().front());
            pNetworkMsg->PopForward();
            _SendMsgToZonePort(*pNetworkMsg);
        }
        else
        {
            //尝试使用route来中转
            pNetworkMsg->SetTo(ServerPort(GetWorldID(), ROUTE_SERVICE, 0));
            _SendMsgToZonePort(*pNetworkMsg);
        }
        return;
    }

    //转发消息给对应ID的玩家
    if(pNetworkMsg->GetMultiIDTo().empty() == false)
    {
        CNetworkMessage send_msg;
        send_msg.CopyRawMessage(*pNetworkMsg);
        auto setSocketMap = IDList2VSMap(pNetworkMsg->GetMultiIDTo(), 0);
        for(auto& [nServerPort, socket_list]: setSocketMap)
        {
            if(socket_list.size() == 1)
            {
                send_msg.SetTo(socket_list.front());
                
                _SendMsgToZonePort(send_msg);
            }
            else
            {
                send_msg.SetMultiTo(socket_list);
                _SendMsgToZonePort(send_msg);
            }
        }  
        
        return;
    }

    if(m_pNetMsgProcess->Process(pNetworkMsg) == false)
    {
        LOGERROR("CMD {} from {} to {} forward {} didn't have ProcessHandler",
                 pNetworkMsg->GetCmd(),
                 pNetworkMsg->GetFrom(),
                 pNetworkMsg->GetTo(),
                 pNetworkMsg->GetForward().size());
        return;
    }

    __LEAVE_FUNCTION
}

void CWorldService::_ID2VS(OBJID id, VirtualSocketMap_t& VSMap) const
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


std::unique_ptr<CMysqlConnection> CWorldService::ConnectGlobalDB()
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

            {
                ServerMSG::ServiceRegister send_msg;
                send_msg.set_serverport(GetServerPort());
                send_msg.set_update_time(TimeGetSecond());
                WorldService()->SendProtoMsgToZonePort(ServerPort(0, ROUTE_SERVICE, 0), send_msg);
            }

            {
                ServerMSG::SocketStartAccept send_msg;
                // send notify to socket
                auto serverport_list = GetMessageRoute()->GetServerPortListByWorldIDAndServiceType(GetWorldID(), SOCKET_SERVICE, false);
                for(const auto& serverport: serverport_list)
                {
                    SendProtoMsgToZonePort(serverport, send_msg);
                }
            }
            
        }
    }

    __LEAVE_FUNCTION
}

bool CWorldService::SendProtoMsgToAllPlayer(const proto_msg_t& msg) const
{
    __ENTER_FUNCTION
    auto serverport_list = GetMessageRoute()->GetServerPortListByWorldIDAndServiceType(GetWorldID(), SOCKET_SERVICE, false);
    for(const auto& serverport: serverport_list)
    {
        SendBroadcastMsgToPort(serverport, msg);
    }
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CWorldService::SendProtoMsgToAllScene(const proto_msg_t& msg) const
{
    __ENTER_FUNCTION
    auto serverport_list = GetMessageRoute()->GetServerPortListByWorldIDAndServiceType(GetWorldID(), SCENE_SERVICE, false);
    for(const auto& serverport: serverport_list)
    {
        SendProtoMsgToZonePort(serverport, msg);
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
        std::string buf =
            std::string("\n======================================================================") +
            fmt::format(FMT_STRING("\nMessageProcess:{}"), GetMessageProcess()) +
            fmt::format(FMT_STRING("\nEvent:{}\tActive:{}\tMem:{}"),
                        EventManager()->GetEventCount(),
                        EventManager()->GetRunningEventCount(),
                        get_thread_memory_allocted()) +
            fmt::format(FMT_STRING("\nAccount:{}\tWait:{}"), AccountManager()->GetAccountSize(), AccountManager()->GetWaitAccountSize());

        auto check_func = [&buf](const ServerPort& serverport) {
            auto pMessagePort = GetMessageRoute()->QueryMessagePort(serverport, false);
            if(pMessagePort && pMessagePort->GetWriteBufferSize() > 0)
            {
                buf += fmt::format(FMT_STRING("\nMsgPort:{}-{}\tSendBuff:{}"),
                                   pMessagePort->GetServerPort().GetWorldID(),
                                   pMessagePort->GetServerPort().GetServiceID(),
                                   pMessagePort->GetWriteBufferSize());
            }
        };
        {
            auto serverport_list = GetMessageRoute()->GetServerPortListByWorldIDAndServiceType(GetWorldID(), SOCKET_SERVICE, false);
            for(const auto& serverport: serverport_list)
            {
                check_func(serverport);
            }
        }
        {
            auto serverport_list = GetMessageRoute()->GetServerPortListByWorldIDAndServiceType(GetWorldID(), SCENE_SERVICE, true);
            for(const auto& serverport: serverport_list)
            {
                check_func(serverport);
            }
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
