#include "MessageRoute.h"

#include "EventManager.h"
#include "MessagePort.h"
#include "NetClientSocket.h"
#include "NetServerSocket.h"
#include "NetSocket.h"
#include "tinyxml2/tinyxml2.h"
#include "serverinfodb.h"

constexpr uint32_t SERVICE_LOAD_REDIS_TIMEOUT = 60 * 1000; // redis上的serviceload数据60秒丢弃
constexpr uint32_t SERVICE_LOAD_TIMEOUT       = 30 * 1000; //每30秒发送1次service_load数据

static CMessageRoute* g_pMessageRoute = nullptr;
CMessageRoute*        GetMessageRoute()
{
    return g_pMessageRoute;
}

void CreateMessageRoute()
{
    if(g_pMessageRoute == nullptr)
        g_pMessageRoute = new CMessageRoute;
}

void ReleaseMessageRoute()
{
    if(g_pMessageRoute)
    {
        g_pMessageRoute->Destory();
        SAFE_DELETE(g_pMessageRoute);
    }
}

CMessageRoute::CMessageRoute()
    : m_pNetworkService(std::make_unique<CNetworkService>())
{
    m_pEventManager.reset(CEventManager::CreateNew(m_pNetworkService->GetEVBase()));
    m_pNetworkService->StartIOThread("MessageRoute");
    LOGDEBUG("MessageRoute Create.");
}

CMessageRoute::~CMessageRoute()
{
    Destory();
}

void CMessageRoute::Destory()
{
    __ENTER_FUNCTION
    if(m_pNetworkService == nullptr)
    {
        return;
    }
    m_setServerInfoByWorldID.clear();
    m_setServerInfo.clear();
    m_MergeList.clear();
    m_WorldReadyList.clear();

    m_pGlobalDB.reset();
    for(auto& [k, v]: m_setMessagePort)
    {
        v->Destory();
        SAFE_DELETE(v);
    }
    m_setMessagePort.clear();

    m_pEventManager.reset();
    m_pNetworkService.reset();

    LOGDEBUG("MessageRoute Delete.");

    __LEAVE_FUNCTION
}

bool CMessageRoute::LoadServiceSetting(const std::string& filename, uint16_t nWorldID)
{
    __ENTER_FUNCTION
    if(filename.empty() == false)
    {
        tinyxml2::XMLDocument doc;
        doc.LoadFile(filename.c_str());
        tinyxml2::XMLElement* pElement = doc.RootElement();
        if(pElement == nullptr)
        {
            LOGFATAL("CMessageRoute::LoadServiceSetting need RootElement");
            return false;
        }
        m_setDataMap.Prase(pElement);
    }
    else
    {
        CSettingNode settingGlobalDB;
        settingGlobalDB.SetVal("host", std::getenv("GlobalMYSQL_HOST"));
        settingGlobalDB.SetVal("user", std::getenv("GlobalMYSQL_USER"));
        settingGlobalDB.SetVal("passwd", std::getenv("GlobalMYSQL_PASSWD"));
        settingGlobalDB.SetVal("port", std::getenv("GlobalMYSQL_PORT"));
        settingGlobalDB.SetVal("dbname", std::getenv("GlobalMYSQL_DBNAME"));
        m_setDataMap["GlobalMYSQL"].emplace_back(std::move(settingGlobalDB));
    }

    SetWorldID(nWorldID);

    //读取全服IP表
    {
        const auto& settingGlobalDB = m_setDataMap["GlobalMYSQL"][0];
        if(ConnectGlobalDB(settingGlobalDB.Query("host"),
                           settingGlobalDB.Query("user"),
                           settingGlobalDB.Query("passwd"),
                           settingGlobalDB.Query("dbname"),
                           settingGlobalDB.QueryULong("port")) == false)
        {
            LOGFATAL("CMessageRoute::LoadServiceSetting ConnectGlobalDB fail");
            return false;
        }
    }

    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CMessageRoute::ConnectGlobalDB(const std::string& host,
                                    const std::string& user,
                                    const std::string& password,
                                    const std::string& db,
                                    uint32_t           port)
{
    __ENTER_FUNCTION
    // connect db
    std::unique_ptr<CMysqlConnection> pDB = std::make_unique<CMysqlConnection>();
    if(pDB->Connect(host, user, password, db, port) == false)
    {
        return false;
    }
    m_pGlobalDB.reset(pDB.release());
    ReloadServiceInfo(TimeGetSecond(), 0);
    return true;
    __LEAVE_FUNCTION
    return false;
}


void CMessageRoute::ReloadServiceInfo(uint32_t update_time, uint16_t nNewWorldID)
{
    __ENTER_FUNCTION

    if(update_time <= m_lastUpdateTime)
        return;
    LOGMESSAGE("reload service info");
    m_lastUpdateTime = update_time;
    std::lock_guard<std::mutex> locker(m_mutex);

    //读取合服信息
    _ReadMergeList();
    //读取ip列表
    _ReadServerIPList(nNewWorldID);
    

    __LEAVE_FUNCTION
}



void CMessageRoute::_ReadMergeList()
{
    __ENTER_FUNCTION
    std::string SQL = "SELECT * FROM tbld_serverlist WHERE mergeto<>0";

    auto result = m_pGlobalDB->Query(TBLD_SERVERLIST::table_name(), SQL);
    CHECK_M(result, "GlobalDB can't query tbld_serverlist");
    std::unordered_map<uint16_t, uint16_t> MergeToList;
    for(size_t i = 0; i < result->get_num_row(); i++)
    {
        auto row = result->fetch_row(false);
        if(row)
        {
            uint16_t idWorld     = row->Field(TBLD_SERVERLIST::WORLDID);
            uint16_t idMergeTo   = row->Field(TBLD_SERVERLIST::MERGETO);
            MergeToList[idWorld] = idMergeTo;
            _CloseRemoteServerByWorldID(idWorld);
        }
    }
    m_MergeList.clear();
    for(auto it = MergeToList.begin(); it != MergeToList.end(); it++)
    {
        //转换到最终合服id
        uint16_t idMergeTo = it->second;
        auto     itMergeTo = MergeToList.find(idMergeTo);
        while(itMergeTo != MergeToList.end())
        {
            idMergeTo = itMergeTo->second;
            itMergeTo = MergeToList.find(idMergeTo);
        }

        m_MergeList[it->first] = idMergeTo;
    }
    __LEAVE_FUNCTION
}

void CMessageRoute::_ReadServerIPList(uint16_t nNewWorldID)
{
    __ENTER_FUNCTION
    std::string SQL;
    if(GetWorldID() == 0)
    {
        // global服务 默认情况全读取
        if(nNewWorldID == 0)
        {
            SQL = "SELECT * FROM tbld_servicedetail";
        }
        else
        {
            //某个特定World启动了, 只需要读取该World数据
            SQL = fmt::format("SELECT * FROM tbld_servicedetail WHERE worldid={}", nNewWorldID);
        }
    }
    else
    {
        SQL = fmt::format(FMT_STRING("SELECT * FROM tbld_servicedetail WHERE worldid={} OR worldid=0"), GetWorldID());
    }
    auto result = m_pGlobalDB->Query(TBLD_SERVICEDETAIL::table_name(), SQL);
    CHECK_M(result, "GlobalDB can't query tbld_servicedetail");
    for(size_t i = 0; i < result->get_num_row(); i++)
    {
        auto row = result->fetch_row(false);
        if(row)
        {
            uint16_t   idWorld       = row->Field(TBLD_SERVICEDETAIL::WORLDID);
            uint8_t    idServiceType = row->Field(TBLD_SERVICEDETAIL::SERVICE_TYPE);
            uint8_t    idServiceIdx  = row->Field(TBLD_SERVICEDETAIL::SERVICE_IDX);
            ServerPort serverport(idWorld, idServiceType, idServiceIdx);
            //写入ServerInfo
            ServerAddrInfo refInfo;
            refInfo.idWorld       = idWorld;
            refInfo.idServiceType = idServiceType;
            refInfo.idServiceIdx  = idServiceIdx;

            refInfo.lib_name     = row->Field(TBLD_SERVICEDETAIL::LIB_NAME).get<std::string>();
            refInfo.route_addr   = row->Field(TBLD_SERVICEDETAIL::ROUTE_ADDR).get<std::string>();
            refInfo.route_port   = row->Field(TBLD_SERVICEDETAIL::ROUTE_PORT);
            refInfo.bind_addr    = row->Field(TBLD_SERVICEDETAIL::BIND_ADDR).get<std::string>();
            refInfo.publish_addr = row->Field(TBLD_SERVICEDETAIL::PUBLISH_ADDR).get<std::string>();
            refInfo.publish_port = row->Field(TBLD_SERVICEDETAIL::PUBLISH_PORT);
            refInfo.debug_port   = row->Field(TBLD_SERVICEDETAIL::DEBUG_PORT);

            LOGDEBUG("ServerInfo Load WORLD:{} Service:{}-{} route_addr:{} route_port:{}",
                     idWorld,
                     idServiceType,
                     idServiceIdx,
                     refInfo.route_addr,
                     refInfo.route_port);

            OnServerAddrInfoChange(serverport, refInfo);
        }
    }
    __LEAVE_FUNCTION
}

void CMessageRoute::OnServerAddrInfoChange(const ServerPort& serverport, const ServerAddrInfo& new_info)
{
    __ENTER_FUNCTION
    auto& old_info = m_setServerInfo[serverport];
    if(old_info.route_addr != new_info.route_addr || old_info.route_port != new_info.route_port)
    {
        auto itPort = m_setMessagePort.find(serverport);
        if(itPort != m_setMessagePort.end())
        {
            auto pMessagePort = itPort->second;
            auto pRemoteSocket = pMessagePort->GetRemoteSocket();
            if(pRemoteSocket && pRemoteSocket->CreateByListener() == false)
            {
                //内网绑定端口修改了, 又是自己主动连接过去,就要重连
                //自己监听的端口，必须要重启
                pRemoteSocket->Interrupt();
                _ConnectRemoteServer(serverport, new_info);
            }
        }
    }
    m_setServerInfo[serverport] = new_info;
    m_setServerInfoByWorldID[serverport.GetWorldID()][serverport] = new_info;

    __LEAVE_FUNCTION
}

const ServerAddrInfo* CMessageRoute::QueryServiceInfo(const ServerPort& nServerPort)
{
    __ENTER_FUNCTION
    std::lock_guard<std::mutex> locker(m_mutex);
    return _QueryServiceInfo(nServerPort);
    __LEAVE_FUNCTION
    return nullptr;
}

const ServerAddrInfo* CMessageRoute::_QueryServiceInfo(const ServerPort& nServerPort)
{
    __ENTER_FUNCTION
    auto it = m_setServerInfo.find(nServerPort);
    if(it != m_setServerInfo.end())
    {
        return &(it->second);
    }
    else
    {
        //查找共享区有没有对应的ip, worldid=0 代表共享
        it = m_setServerInfo.find(ServerPort(0, nServerPort.GetServiceID()));
        if(it != m_setServerInfo.end())
        {
            return &(it->second);
        }
    }
    __LEAVE_FUNCTION
    return nullptr;
}

bool CMessageRoute::IsWorldReady(uint16_t idWorld)
{
    __ENTER_FUNCTION
    if(idWorld == 0 || idWorld == m_nWorldID)
        return true;
    std::unique_lock<std::mutex> locker(m_mutex);
    auto                         it = m_WorldReadyList.find(idWorld);
    if(it == m_WorldReadyList.end())
        return false;
    return TimeGetSecond() <= it->second;
    __LEAVE_FUNCTION
    return false;
}

void CMessageRoute::SetWorldReady(uint16_t idWorld, bool bReady)
{
    __ENTER_FUNCTION
    std::unique_lock<std::mutex> locker(m_mutex);
    if(bReady)
    {
        m_WorldReadyList[idWorld] = TimeGetSecond() + 60;
    }
    else
    {
        m_WorldReadyList.erase(idWorld);
        _CloseRemoteServerByWorldID(idWorld);
    }
    __LEAVE_FUNCTION
}

void CMessageRoute::ForeachServiceInfoByWorldID(uint16_t idWorld, bool bIncludeShare, ForeachServiceInfoFunc&& func)
{
    __ENTER_FUNCTION
    std::unique_lock<std::mutex> locker(m_mutex);
    auto                         copyList = m_setServerInfoByWorldID[idWorld];
    locker.unlock();
    for(const auto& pair_val: copyList)
    {
        const auto& server_addr_info = pair_val.second;
        if(func(&server_addr_info) == false)
            return;
    }

    if(bIncludeShare)
    {
        locker.lock();
        auto copyList = m_setServerInfoByWorldID[0];
        locker.unlock();
        for(const auto& pair_val: copyList)
        {
            const auto& server_addr_info = pair_val.second;
            if(func(&server_addr_info) == false)
                return;
        }
    }
    __LEAVE_FUNCTION
}

ServerPortList CMessageRoute::GetServerPortListByWorldID(uint16_t idWorld, bool bIncludeShare)
{
    ServerPortList service_list;
    __ENTER_FUNCTION
    std::unique_lock<std::mutex> locker(m_mutex);

    for(const auto& [server_port, server_info]: m_setServerInfoByWorldID[idWorld])
    {
        service_list.push_back(server_port);
    }
    if(bIncludeShare)
    {
        for(const auto& [server_port, server_info]: m_setServerInfoByWorldID[0])
        {
            service_list.push_back(server_port);
        }
    }
    locker.unlock();

    __LEAVE_FUNCTION
    return service_list;
}

ServerPortList CMessageRoute::GetServerPortListByWorldIDAndServiceType(uint16_t idWorld, uint8_t idServiceType, bool bIncludeShare)
{
    ServerPortList service_list;
    __ENTER_FUNCTION

    std::unique_lock<std::mutex> locker(m_mutex);

    for(const auto& [server_port, server_info]: m_setServerInfoByWorldID[idWorld])
    {
        if(server_port.GetServiceType() != idServiceType)
            continue;
        service_list.push_back(server_port);
    }
    if(bIncludeShare)
    {
        for(const auto& [server_port, server_info]: m_setServerInfoByWorldID[0])
        {
            if(server_port.GetServiceType() != idServiceType)
                continue;
            service_list.push_back(server_port);
        }
    }
    locker.unlock();

    __LEAVE_FUNCTION

    return service_list;
}


ServerPortList CMessageRoute::GetServerPortListByWorldIDExcept(uint16_t idWorld, uint8_t idExceptServiceType, bool bIncludeShare)
{
    ServerPortList service_list;
    __ENTER_FUNCTION

    std::unique_lock<std::mutex> locker(m_mutex);

    for(const auto& [server_port, server_info]: m_setServerInfoByWorldID[idWorld])
    {
        if(server_port.GetServiceType() == idExceptServiceType)
            continue;
        service_list.push_back(server_port);
    }
    if(bIncludeShare)
    {
        for(const auto& [server_port, server_info]: m_setServerInfoByWorldID[0])
        {
            if(server_port.GetServiceType() == idExceptServiceType)
                continue;
            service_list.push_back(server_port);
        }
    }
    locker.unlock();

    __LEAVE_FUNCTION

    return service_list;
}

ServerPortList CMessageRoute::GetServerPortListByServiceType(uint8_t idServiceType)
{
    ServerPortList service_list;
    __ENTER_FUNCTION

    std::unique_lock<std::mutex> locker(m_mutex);

    for(const auto& [idWorldID, info_list]: m_setServerInfoByWorldID)
    {
        for(const auto& [server_port, server_info]: info_list)
        {
            if(server_port.GetServiceType() != idServiceType)
                continue;
            service_list.push_back(server_port);
        }
    }
    locker.unlock();

    __LEAVE_FUNCTION
    return service_list;
}

bool CMessageRoute::IsConnected(const ServerPort& nServerPort)
{
    __ENTER_FUNCTION
    std::lock_guard<std::mutex> locker(m_mutex);

    auto it = m_setMessagePort.find(nServerPort);
    if(it != m_setMessagePort.end())
    {
        if(it->second->GetLocalPort() == true)
            return true;
        else
            return it->second->GetRemoteSocket() != nullptr;
    }
    else
    {
        return false;
    }
    __LEAVE_FUNCTION
    return false;
}

uint16_t CMessageRoute::GetMergeTo(uint16_t idWorld)
{
    std::unique_lock<std::mutex> locker(m_mutex);
    auto                         itMerge = m_MergeList.find(idWorld);
    if(itMerge != m_MergeList.end())
    {
        return itMerge->second;
    }
    return idWorld;
}

CMessagePort* CMessageRoute::QueryMessagePort(const ServerPort& nServerPort, bool bAutoConnectRemote /* = true*/)
{
    __ENTER_FUNCTION
    ServerPort DestServerPort(nServerPort);
    DestServerPort.SetWorldID(GetMergeTo(nServerPort.GetWorldID()));

    std::unique_lock<std::mutex> locker(m_mutex);

    auto it = m_setMessagePort.find(DestServerPort);
    if(it != m_setMessagePort.end())
    {
        return it->second;
    }
    else
    {
        locker.unlock();
        if(bAutoConnectRemote && IsWorldReady(DestServerPort.GetWorldID()))
            return ConnectRemoteServer(DestServerPort);
        else
            return nullptr;
    }
    __LEAVE_FUNCTION
    return nullptr;
}

CMessagePort* CMessageRoute::_ConnectRemoteServer(const ServerPort& nServerPort, const ServerAddrInfo& info)
{
    __ENTER_FUNCTION
    CMessagePort* pMessagePort = m_setMessagePort[nServerPort];
    if(pMessagePort == nullptr)
    {
        pMessagePort = new CMessagePort(nServerPort, this);

        m_setMessagePort[nServerPort] = pMessagePort;
    }
    else
    {
        if(pMessagePort->GetRemoteSocket() != nullptr)
            return pMessagePort;
        if(pMessagePort->GetLocalPort() == true)
            return pMessagePort;
    }
    auto pRemoteSocket = m_pNetworkService->AsyncConnectTo(info.route_addr.c_str(), info.route_port, pMessagePort);
    if(pRemoteSocket == nullptr)
    {
        LOGFATAL("CMessageRoute::ConnectRemoteServer AsyncConnectTo {}:{} fail", info.route_addr.c_str(), info.route_port);
        return nullptr;
    }
    pRemoteSocket->SetReconnect(true);
    pRemoteSocket->SetLogWriteHighWateMark(100 * 1024 * 1024);
    pMessagePort->SetRemoteSocket(pRemoteSocket);

    LOGMESSAGE("CMessageRoute::ConnectRemoteServer:{}, {}:{}", nServerPort.GetServiceID(), info.route_addr.c_str(), info.route_port);

    return pMessagePort;
    __LEAVE_FUNCTION
    return nullptr;
}

bool CMessageRoute::CreateAllMessagePort(uint16_t nWorldID, const std::set<ServiceID>& create_service_set)
{
    __ENTER_FUNCTION

    std::unique_lock<std::mutex> lock(m_mutex);
    const auto&                  refList = m_setServerInfoByWorldID[nWorldID];
    for(const auto& pair_val: refList)
    {
        auto nServerPort = pair_val.first;
        //创建所有的MessagePort
        if(create_service_set.empty())
        {
            auto pInfo = _QueryServiceInfo(nServerPort);
            if(pInfo == nullptr)
            {
                LOGFATAL("ListenMessagePort {} not find", nServerPort);
                return false;
            }
            CHECKF(_ListenMessagePort(nServerPort, *pInfo) != nullptr);
        }
        else
        {
            //在本Exe上将要创建的Service,都开始监听
            if(create_service_set.find(nServerPort.GetServiceID()) != create_service_set.end())
            {
                auto pInfo = _QueryServiceInfo(nServerPort);
                if(pInfo == nullptr)
                {
                    LOGFATAL("ListenMessagePort {} not find", nServerPort);
                    return false;
                }
                CHECKF(_ListenMessagePort(nServerPort, *pInfo) != nullptr);
            }
            //其他的都是Remote,会自动连接
        }
    }

    return true;
    __LEAVE_FUNCTION
    return false;
}
void CMessageRoute::_CloseRemoteServerByWorldID(uint16_t idWorld)
{
    __ENTER_FUNCTION
    auto itInfo = m_setServerInfoByWorldID.find(idWorld);
    if(itInfo == m_setServerInfoByWorldID.end())
        return;

    auto& refMap = itInfo->second;
    for(const auto& [k, v]: refMap)
    {
        _CloseRemoteServer(k);
    }
    __LEAVE_FUNCTION
}

void CMessageRoute::_CloseRemoteServer(const ServerPort& nServerPort)
{
    __ENTER_FUNCTION
    auto itPort = m_setMessagePort.find(nServerPort);
    if(itPort == m_setMessagePort.end())
        return;
    CMessagePort* pPort = itPort->second;
    if(pPort)
    {
        auto pRemoteSocket = pPort->GetRemoteSocket();
        if(pRemoteSocket)
        {
            pRemoteSocket->Interrupt();
        }

        SAFE_DELETE(pPort);
    }
    m_setMessagePort.erase(itPort);
    LOGMESSAGE("CMessageRoute::CloseRemoteServer:{}", nServerPort);
    __LEAVE_FUNCTION
}

CMessagePort* CMessageRoute::ConnectRemoteServer(const ServerPort& nServerPort)
{
    __ENTER_FUNCTION
    auto pInfo = QueryServiceInfo(nServerPort);
    if(pInfo == nullptr)
    {
        LOGERROR("ConnectRemoteServer Fail, Can't File Info:{}", nServerPort);
        return nullptr;
    }

    std::lock_guard<std::mutex> locker(m_mutex);
    return _ConnectRemoteServer(nServerPort, *pInfo);
    __LEAVE_FUNCTION
    return nullptr;
}

CMessagePort* CMessageRoute::_ListenMessagePort(const ServerPort& nServerPort, const ServerAddrInfo& info)
{
    __ENTER_FUNCTION
    CMessagePort* pMessagePort = m_setMessagePort[nServerPort];
    if(pMessagePort != nullptr)
    {
        return pMessagePort;
    }

    pMessagePort = new CMessagePort(nServerPort, this);
    pMessagePort->SetLocalPort(true);
    if(m_pNetworkService->Listen(info.bind_addr.c_str(), info.route_port, pMessagePort) == nullptr)
    {
        LOGFATAL("ListenMessagePort {}:{} fail", info.route_addr.c_str(), info.route_port);
        SAFE_DELETE(pMessagePort);
        return nullptr;
    }
    LOGMESSAGE("CMessageRoute::ListenMessagePort:{}-{}-{}, {}:{}",
               nServerPort.GetWorldID(),
               nServerPort.GetServiceID().GetServiceType(),
               nServerPort.GetServiceID().GetServiceIdx(),
               info.route_addr.c_str(),
               info.route_port);
    m_setMessagePort[nServerPort] = pMessagePort;
    return pMessagePort;
    __LEAVE_FUNCTION
    return nullptr;
}

void CMessageRoute::ForEach(const std::function<void(CMessagePort*)>& func)
{
    __ENTER_FUNCTION
    std::lock_guard<std::mutex> lock(m_mutex);
    for(const auto& v: m_setMessagePort)
    {
        func(v.second);
    }
    __LEAVE_FUNCTION
}

void CMessageRoute::ConnectAllRemoteServerWithWorldID(uint16_t nWorldID)
{
    __ENTER_FUNCTION
    std::lock_guard<std::mutex> lock(m_mutex);
    const auto&                 refList = m_setServerInfoByWorldID[nWorldID];
    for(const auto& pair_val: refList)
    {
        _ConnectRemoteServer(pair_val.first, pair_val.second);
    }

    {
        const auto& refList = m_setServerInfoByWorldID[0];
        for(const auto& pair_val: refList)
        {
            _ConnectRemoteServer(pair_val.first, pair_val.second);
        }
    }
    __LEAVE_FUNCTION
}

void CMessageRoute::CloseMessagePort(CMessagePort*& pMessagePort)
{
    __ENTER_FUNCTION
    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = m_setMessagePort.find(pMessagePort->GetServerPort());
    if(it != m_setMessagePort.end())
    {
        SAFE_DELETE(pMessagePort);
        m_setMessagePort.erase(it);
    }
    __LEAVE_FUNCTION
}


