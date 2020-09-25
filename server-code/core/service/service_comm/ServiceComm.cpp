#include "ServiceComm.h"

#include <iostream>

#include "CheckUtil.h"
#include "EventManager.h"
#include "MessagePort.h"
#include "MessageRoute.h"
#include "MonitorMgr.h"
#include "msg/ts_cmd.pb.h"
#include "protomsg_to_cmd.h"
#include "server_msg/server_side.pb.h"
#include "serverinfodb.h"
#include "DB2PB.h"
#include "MysqlConnection.h"
#include "serverinfodb.pb.h"

CServiceCommon::CServiceCommon()
    : m_pNetworkService(nullptr)
    , m_pMessagePort(nullptr)
{
}

CServiceCommon::~CServiceCommon() {}

void CServiceCommon::DestoryServiceCommon()
{
    __ENTER_FUNCTION
    // GetMessageRoute()->CloseMessagePort(m_pMessagePort);
    StopLogicThread();
    if(m_pMessagePort)
    {
        m_pMessagePort->SetPortEventHandler(nullptr);
        m_pMessagePort = nullptr;
    }

    if(m_pNetworkService)
    {
        m_pNetworkService->Destroy();
        m_pNetworkService.reset();
    }

    if(m_pEventManager)
    {
        m_pEventManager->Destory();
        m_pEventManager.reset();
    }

    if(m_pMonitorMgr)
    {
        m_pMonitorMgr.reset();
    }

    LOGMESSAGE("{} {} Close", GetServiceName().c_str(), GetServerPort().GetServiceID());
    __LEAVE_FUNCTION
}

bool CServiceCommon::Init(const ServerPort& nServerPort)
{
    __ENTER_FUNCTION
    m_nServerPort    = nServerPort;
    m_ServiceName    = ::GetServiceName(nServerPort.GetServiceID());
    m_pNetMsgProcess = std::make_unique<CNetMSGProcess>();
    m_pEventManager.reset(CEventManager::CreateNew(nullptr));
    CHECKF(m_pEventManager.get());
    m_pMonitorMgr.reset(CMonitorMgr::CreateNew());
    CHECKF(m_pMonitorMgr.get());
    return true;
    __LEAVE_FUNCTION
    return true;
}

bool CServiceCommon::CreateNetworkService()
{
    __ENTER_FUNCTION
    if(m_pNetworkService)
        return false;
    m_pNetworkService = std::make_unique<CNetworkService>();
    return true;
    __LEAVE_FUNCTION
    return true;
}

bool CServiceCommon::CreateService(int32_t nWorkInterval /*= 100*/, class CMessagePortEventHandler* pEventHandler /*= nullptr*/)
{
    __ENTER_FUNCTION
    if(ListenMessagePort(GetServiceName(), pEventHandler) == false)
        return false;
    //开启逻辑线程处理来自其他服务器的消息
    StartLogicThread(nWorkInterval, GetServiceName() + "_Logic");

    LOGMESSAGE("{} {} Create", GetServiceName().c_str(), GetServerPort().GetServiceID());
    __LEAVE_FUNCTION
    return true;
}

bool CServiceCommon::ListenMessagePort(const std::string& service_name, CMessagePortEventHandler* pEventHandler /*= nullptr*/)
{
    __ENTER_FUNCTION
    m_pMessagePort = GetMessageRoute()->QueryMessagePort(GetServerPort(), false);
    if(m_pMessagePort != nullptr && pEventHandler != nullptr)
        m_pMessagePort->SetPortEventHandler(pEventHandler);
    return m_pMessagePort != nullptr;
    __LEAVE_FUNCTION
    return false;
}

void CServiceCommon::StartLogicThread(int32_t nWorkInterval, const std::string& name)
{
    __ENTER_FUNCTION
    if(m_pLogicThread)
    {
        return;
    }
    m_pLogicThread = std::make_unique<CNormalThread>(nWorkInterval,
                                                     name,
                                                     std::bind(&CServiceCommon::OnLogicThreadProc, this),
                                                     std::bind(&CServiceCommon::OnLogicThreadCreate, this),
                                                     std::bind(&CServiceCommon::OnLogicThreadExit, this));
    __LEAVE_FUNCTION
}

void CServiceCommon::StopLogicThread()
{
    __ENTER_FUNCTION
    if(m_pLogicThread)
    {
        m_pLogicThread->Stop();
        m_pLogicThread->Join();
    }
    __LEAVE_FUNCTION
}

void CServiceCommon::OnProcessMessage(CNetworkMessage*) {}

void CServiceCommon::OnLogicThreadProc()
{
    __ENTER_FUNCTION
    if(m_pNetworkService)
    {
        m_pNetworkService->RunOnce();
    }

    constexpr int32_t MAX_PROCESS_PER_LOOP = 1000;
    int32_t           nCount               = 0;

    CNetworkMessage* pMsg = nullptr;
    if(m_pMessagePort)
    {
        // process message_port msg
        while(nCount < MAX_PROCESS_PER_LOOP && m_pMessagePort->TakePortMsg(pMsg))
        {
            nCount++;
            OnProcessMessage(pMsg);
            SAFE_DELETE(pMsg);
        }
    }

    m_nMessageProcess += nCount;

    // process message from client
    if(m_pNetworkService)
    {
        nCount = 0;
        while(nCount < MAX_PROCESS_PER_LOOP && m_pNetworkService->_GetMessageQueue().get(pMsg))
        {
            nCount++;
            OnProcessMessage(pMsg);
            SAFE_DELETE(pMsg);
        }
    }

    m_nMessageProcess += nCount;
    //定时器回掉
    m_pEventManager->OnTimer();

    __LEAVE_FUNCTION
}

void CServiceCommon::OnLogicThreadCreate()
{
    __ENTER_FUNCTION
    BaseCode::InitMonitorLog(m_ServiceName);
    __LEAVE_FUNCTION
}

void CServiceCommon::OnLogicThreadExit() {}

bool CServiceCommon::ForwardProtoMsgToPlayer(const ServerPort& nServerPort, uint64_t id_player, const proto_msg_t& msg) const
{
    __ENTER_FUNCTION
    CNetworkMessage _msg(to_cmd(msg), msg, GetServerVirtualSocket(), nServerPort);
    _msg.AddMultiIDTo(id_player);
    return _SendMsgToZonePort(_msg);
    __LEAVE_FUNCTION
    return false;
}

bool CServiceCommon::SendBroadcastMsgToPort(const ServerPort& nServerPort, const proto_msg_t& msg) const
{
    __ENTER_FUNCTION
    CNetworkMessage _msg(to_cmd(msg), msg, GetServerVirtualSocket(), nServerPort);
    _msg.SetBroadcast();
    return _SendMsgToZonePort(_msg);
    __LEAVE_FUNCTION
    return false;
}

bool CServiceCommon::SendProtoMsgToZonePort(const ServerPort& nServerPort, const proto_msg_t& msg) const
{
    __ENTER_FUNCTION
    CNetworkMessage _msg(to_cmd(msg), msg, GetServerVirtualSocket(), nServerPort);
    return _SendMsgToZonePort(_msg);
    __LEAVE_FUNCTION
    return false;
}

bool CServiceCommon::SendMsgToVirtualSocket(const VirtualSocket& vsTo, const proto_msg_t& msg) const
{
    __ENTER_FUNCTION
    CNetworkMessage _msg(to_cmd(msg), msg, GetServerVirtualSocket(), vsTo);
    return _SendMsgToZonePort(_msg);
    __LEAVE_FUNCTION
    return false;
}

bool CServiceCommon::TransmitMsgToPort(const ServerPort& nServerPort, const CNetworkMessage* pMsg) const
{
    __ENTER_FUNCTION
    CNetworkMessage _msg(*pMsg);
    _msg.SetTo(nServerPort);
    _msg.CopyBuffer();

    return _SendMsgToZonePort(_msg);
    __LEAVE_FUNCTION
    return false;
}

bool CServiceCommon::TransmitMsgToThisZoneAllPort(const CNetworkMessage* pMsg) const
{
    __ENTER_FUNCTION
    auto serverport_list = GetMessageRoute()->GetServerPortListByWorldID(GetWorldID(), false);
    for(const auto& server_port: serverport_list)
    {
        CNetworkMessage _msg(*pMsg);
        _msg.SetTo(server_port);
        _msg.CopyBuffer();
        _SendMsgToZonePort(_msg);
    }
    return serverport_list.empty() == false;
    __LEAVE_FUNCTION
    return false;
}

bool CServiceCommon::TransmitMsgToThisZoneAllPortExcept(const CNetworkMessage* pMsg, uint8_t idServiceType) const
{
    __ENTER_FUNCTION
    auto serverport_list = GetMessageRoute()->GetServerPortListByWorldIDExcept(GetWorldID(), idServiceType, false);
    for(const auto& server_port: serverport_list)
    {
        CNetworkMessage _msg(*pMsg);
        _msg.SetTo(server_port);
        _msg.CopyBuffer();
        _SendMsgToZonePort(_msg);
    }
    return serverport_list.empty() == false;
    __LEAVE_FUNCTION
    return false;
}

bool CServiceCommon::TransmitMsgToAllRoute(const CNetworkMessage* pMsg) const
{
    __ENTER_FUNCTION
    auto serverport_list = GetMessageRoute()->GetServerPortListByServiceType(ROUTE_SERVICE);
    for(const auto& server_port: serverport_list)
    {
        CNetworkMessage _msg(*pMsg);
        _msg.SetTo(server_port);
        _msg.CopyBuffer();
        _SendMsgToZonePort(_msg);
    }
    return serverport_list.empty() == false;
    __LEAVE_FUNCTION
    return false;
}

bool CServiceCommon::TransmitMsgToAllRouteExcept(const CNetworkMessage* pMsg, uint16_t idWorld) const
{
    __ENTER_FUNCTION
    auto serverport_list = GetMessageRoute()->GetServerPortListByServiceType(ROUTE_SERVICE);
    for(const auto& server_port: serverport_list)
    {
        if(server_port.GetWorldID() == idWorld)
            continue;
        CNetworkMessage _msg(*pMsg);
        _msg.SetTo(server_port);
        _msg.CopyBuffer();
        _SendMsgToZonePort(_msg);
    }
    return serverport_list.empty() == false;
    __LEAVE_FUNCTION
    return false;
}

bool CServiceCommon::_SendMsgToZonePort(const CNetworkMessage& msg) const
{
    __ENTER_FUNCTION
    VirtualSocket vs(msg.GetTo());
    if(GetMessageRoute() && vs.GetServerPort() != m_nServerPort)
    {
        m_pMonitorMgr->AddSendInfo(msg.GetCmd(), msg.GetSize());
        if(vs.GetServerPort().GetWorldID() == GetWorldID() ||
           ((GetServiceID().GetServiceType() == ROUTE_SERVICE) && (vs.GetServerPort().GetServiceType() == ROUTE_SERVICE)))
        {
            CMessagePort* pMessagePort = GetMessageRoute()->QueryMessagePort(vs.GetServerPort());
            if(pMessagePort)
            {
                return pMessagePort->SendMsgToPort(msg);
            }
            else
            {
                LOGWARNING("SendMsgToZonePort To ServerPort:{}, not find", vs);
                LOGWARNING("CallStack：{}", GetStackTraceString(CallFrameMap(2, 7)));
            }
        }
        else
        {
            //通过route转发
            CNetworkMessage newmsg(msg);
            newmsg.AddForward(msg.GetTo());
            return TransmitMsgToPort(ServerPort(GetWorldID(), ROUTE_SERVICE, 0), &newmsg);
        }
        return false;
    }
    else if((vs.GetServerPort() == m_nServerPort || vs.GetServerPort().IsVaild() == false) && vs.GetSocketIdx() != 0)
    {
        // direct send message
        if(m_pNetworkService)
        {
            if(msg.IsBroadcast())
            {
                m_pMonitorMgr->AddSendInfo_broad(msg.GetCmd(), msg.GetSize());
                m_pNetworkService->BrocastMsg(msg, 0);
            }
            else
            {
                m_pMonitorMgr->AddSendInfo(msg.GetCmd(), msg.GetSize());
                return m_pNetworkService->SendSocketMsgByIdx(msg.GetTo().GetSocketIdx(), msg);
            }
        }

        return false;
    }
    else
    {
        LOGWARNING("Message Want Send To Worng: {}", msg.GetTo());
        LOGWARNING("CallStack：{}", GetStackTraceString(CallFrameMap(2, 7)));
        return false;
    }
    __LEAVE_FUNCTION
    return false;
}

bool CServiceCommon::SendProtoMsgTo(const VirtualSocketMap_t& setSocketMap, const proto_msg_t& msg) const
{
    __ENTER_FUNCTION
    auto            nCmd = to_cmd(msg);
    CNetworkMessage _msg(nCmd, msg, GetServerVirtualSocket());
    for(auto& [nServerPort, socket_list]: setSocketMap)
    {
        if(socket_list.size() == 1)
        {
            _msg.SetTo(socket_list.front());
            m_pMonitorMgr->AddSendInfo(nCmd, _msg.GetSize());
            _SendMsgToZonePort(_msg);
        }
        else
        {
            m_pMonitorMgr->AddSendInfo_some(nCmd, _msg.GetSize(), socket_list.size());
            _msg.SetMultiTo(socket_list);
            _SendMsgToZonePort(_msg);
        }
    }
    return true;
    __LEAVE_FUNCTION
    return false;
}

std::unique_ptr<db::tbld_dbinfo> CServiceCommon::QueryDBInfo(uint16_t nWorldID, CMysqlConnection* pServerInfoDB)
{
    return DB2PB::QueryOneConst<TBLD_DBINFO, db::tbld_dbinfo, TBLD_DBINFO::WORLDID>(pServerInfoDB, 1);
}

std::unique_ptr<CMysqlConnection> CServiceCommon::ConnectDB(const db::tbld_dbinfo* pInfo)
{
    CHECKF(pInfo);
    auto pDB    = std::make_unique<CMysqlConnection>();
    auto result = pDB->Connect(pInfo->db_ip(),
                                pInfo->db_user(),
                                pInfo->db_passwd(),
                                pInfo->db_name(),
                                pInfo->db_port());

    if(result == false)
    {
        return nullptr;
    }
    return pDB;
}

std::unique_ptr<CMysqlConnection> CServiceCommon::ConnectGlobalDB(CMysqlConnection* pServerInfoDB)
{
    __ENTER_FUNCTION
    CHECKF(pServerInfoDB);
    //通过ServerInfodb查询localdb
    auto db_info = QueryDBInfo(0, pServerInfoDB);
    CHECKF(db_info);
    m_globaldb_info.reset(db_info.release());
    return ConnectDB(m_globaldb_info.get());
       
    __LEAVE_FUNCTION
    return nullptr;
}

std::unique_ptr<CMysqlConnection> CServiceCommon::ConnectGlobalDB()
{
    __ENTER_FUNCTION

   return ConnectDB(m_globaldb_info.get());

    __LEAVE_FUNCTION
    return nullptr;
}

std::unique_ptr<CMysqlConnection> CServiceCommon::ConnectServerInfoDB()
{
    const auto& settings            = GetMessageRoute()->GetSettingMap();
    const auto& settingServerInfoDB = settings["ServerInfoMYSQL"][0];
    auto        pServerInfoDB       = std::make_unique<CMysqlConnection>();
    if(pServerInfoDB->Connect(settingServerInfoDB.Query("host"),
                              settingServerInfoDB.Query("user"),
                              settingServerInfoDB.Query("passwd"),
                              settingServerInfoDB.Query("dbname"),
                              settingServerInfoDB.QueryULong("port")) == false)
    {
        return nullptr;
    }
    return pServerInfoDB;
}