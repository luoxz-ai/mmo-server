#include "SocketService.h"

#include <functional>

#include "EventManager.h"
#include "MemoryHelp.h"
#include "MessagePort.h"
#include "MessageRoute.h"
#include "MonitorMgr.h"
#include "MsgProcessRegister.h"
#include "NetSocket.h"
#include "SettingMap.h"
#include "msg/ts_cmd.pb.h"
#include "msg/world_service.pb.h"
#include "server_msg/server_side.pb.h"
#include "protomsg_to_cmd.h"

extern "C" __attribute__((visibility("default"))) IService* ServiceCreate(uint16_t idWorld, uint8_t idServiceType, uint8_t idServiceIdx)
{
    return CSocketService::CreateNew(ServerPort{idWorld, idServiceType, idServiceIdx});
}

CGameClient::CGameClient()
    : m_nDestServerPort(0, AUTH_SERVICE, 0)
    , m_nMessageAllowBegin(CMD_CS_LOGIN) // only accept CS_AUTH
    , m_nMessageAllowEnd(CMD_CS_LOGIN)   // only accept CS_AUTH
    , m_idUser(0)
{
}

CGameClient::~CGameClient() {}

void CGameClient::Interrupt()
{
    CHECK(m_pService);
    LOGNETDEBUG("Client {}:{} Close.", GetSocketAddr().c_str(), GetSocketPort());
    SetMessageAllow(0, 0);
    m_pService->GetNetworkService()->_CloseSocket(m_VirtualSocket.GetSocketIdx());
    m_VirtualSocket.SetSocketIdx(0);
}

bool CGameClient::SendSocketMsg(byte* pBuffer, size_t len)
{
    CHECKF(m_pService);
    return m_pService->GetNetworkService()->SendSocketMsgByIdx(m_VirtualSocket.GetSocketIdx(), pBuffer, len);
}

bool CGameClient::SendSocketMsg(CNetworkMessage* pMsg)
{
    CHECKF(m_pService);
    return m_pService->GetNetworkService()->SendSocketMsgByIdx(m_VirtualSocket.GetSocketIdx(), pMsg);
}

bool CGameClient::IsVaild() const
{
    return m_VirtualSocket.GetSocketIdx() != 0;
}

static thread_local CSocketService* tls_pService = nullptr;

CSocketService* SocketService()
{
    return tls_pService;
}

void SetSocketServicePtr(CSocketService* ptr)
{
    tls_pService = ptr;
}

//////////////////////////////////////////////////////////////////////////
CSocketService::CSocketService()
{
    m_tLastDisplayTime.Startup(20);
}

CSocketService::~CSocketService() {}

void CSocketService::Release()
{

    Destory();
    delete this;
}

void CSocketService::Destory()
{
    tls_pService = this;
    scope_guards scope_exit;
    scope_exit += []() {
        tls_pService = nullptr;
    };
    DestoryServiceCommon();

    for(auto& [k, v]: m_setVirtualSocket)
    {
        SAFE_DELETE(v);
    }
    m_setVirtualSocket.clear();
    m_mapClientByUserID.clear();
}

bool CSocketService::Init(const ServerPort& nServerPort)
{
    __ENTER_FUNCTION

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

    const ServerAddrInfo* pAddrInfo = GetMessageRoute()->QueryServiceInfo(GetServerPort());
    if(pAddrInfo == nullptr)
    {
        LOGFATAL("CSocketService::Create QueryServerInfo {} fail", GetServerPort().GetServiceID());
        return false;
    }

    CHECKF(CreateNetworkService());

    //开启对外监听端口
    if(GetNetworkService()->Listen(pAddrInfo->bind_addr.c_str(), pAddrInfo->publish_port, this) == nullptr)
    {
        return false;
    }
    GetNetworkService()->EnableListener(nullptr, false);
    GetNetworkService()->StartIOThread(GetServiceName() + "_Network");
    // websocket监听
    //		if (!GetNetworkService()->ListenWebSocket(9555, this))
    //		{
    //			return false;
    //		}

    if(CreateService(100) == false)
        return false;

    // SetIPCheck(true);
    {
        auto pNetMsgProcess = GetNetMsgProcess();
        for(const auto& [k, v]: MsgProcRegCenter<CSocketService>::instance().m_MsgProc)
        {
            pNetMsgProcess->Register(k, std::get<0>(v), std::get<1>(v));
        }
    }

    ServerMSG::ServiceReady msg;
    msg.set_serverport(GetServerPort());
    SendProtoMsgToZonePort(ServerPort(GetWorldID(), WORLD_SERVICE, 0),  msg);

    return true;

    __LEAVE_FUNCTION

    return false;
}

void CSocketService::MapClientByUserID(OBJID idUser, CGameClient* pClient)
{
    if(idUser == 0)
        return;

    m_mapClientByUserID[idUser] = pClient;
    pClient->SetUserID(idUser);
}

CGameClient* CSocketService::QueryClientByUserID(OBJID idUser)
{
    if(idUser == 0)
        return nullptr;
    auto it = m_mapClientByUserID.find(idUser);
    if(it != m_mapClientByUserID.end())
        return it->second;
    else
        return nullptr;
}

bool CSocketService::DelClientByUserID(OBJID idUser)
{
    if(idUser == 0)
        return false;

    auto it = m_mapClientByUserID.find(idUser);
    if(it != m_mapClientByUserID.end())
    {
        m_mapClientByUserID.erase(it);
        return true;
    }
    else
        return false;
}

CGameClient* CSocketService::QueryClient(const VirtualSocket& vs)
{
    auto it = m_setVirtualSocket.find(vs);
    if(it != m_setVirtualSocket.end())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

void CSocketService::AddClient(const VirtualSocket& vs, CGameClient* pClient)
{
    m_setVirtualSocket[vs] = pClient;
}

void CSocketService::RemoveClient(const VirtualSocket& vs)
{
    auto it = m_setVirtualSocket.find(vs);
    if(it != m_setVirtualSocket.end())
    {
        CGameClient* pClient = it->second;
        if(pClient->IsAuth())
        {
            ServerMSG::SocketClose msg;
            msg.set_vs(vs);
            SendProtoMsgToZonePort(ServerPort(GetServerPort().GetWorldID(), WORLD_SERVICE, 0), msg);
        }
        else
        {
            ServerMSG::SocketClose msg;
            msg.set_vs(vs);
            SendProtoMsgToZonePort(ServerPort(GetServerPort().GetWorldID(), AUTH_SERVICE, 0), msg);
        }

        SAFE_DELETE(pClient);
        m_setVirtualSocket.erase(it);
    }
}

void CSocketService::OnConnected(CNetSocket* pSocket) {}

void CSocketService::OnConnectFailed(CNetSocket*) {}

void CSocketService::OnDisconnected(CNetSocket* pSocket)
{
    __ENTER_FUNCTION
    RemoveClient(VirtualSocket::CreateVirtualSocket(GetServerPort(), pSocket->GetSocketIdx()));
    __LEAVE_FUNCTION
}

void CSocketService::OnAccepted(CNetSocket* pSocket)
{
    __ENTER_FUNCTION

    CGameClient* pClient = new CGameClient();
    pClient->SetService(this);
    pClient->SetDestServerPort(ServerPort(GetServerPort().GetWorldID(), AUTH_SERVICE, 0));
    pClient->SetVirtualSocket(VirtualSocket::CreateVirtualSocket(GetServerPort(), pSocket->GetSocketIdx()));
    pClient->SetSocketAddr(pSocket->GetAddrString());
    pClient->SetSocketPort(pSocket->GetPort());
    AddClient(pClient->GetVirtualSocket(), pClient);

    // send crypto key to client
    uint32_t seed = TimeGetSecond();
    pSocket->InitDecryptor(seed);
    SC_KEY msg;
    msg.set_key(seed);
    CNetworkMessage _msg(to_cmd(msg), msg);
    pSocket->SendSocketMsg(_msg.GetBuf(), _msg.GetSize());
    __LEAVE_FUNCTION
}

void CSocketService::OnRecvData(CNetSocket* pSocket, byte* pBuffer, size_t len)
{
    __ENTER_FUNCTION
    m_nSocketMessageProcess++;
    CGameClient* pClient = QueryClient(VirtualSocket::CreateVirtualSocket(GetServerPort(), pSocket->GetSocketIdx()));
    if(pClient == nullptr)
        return;
    if(pClient->IsVaild() == false)
    {
        return;
    }
    // recv msg from client
    MSG_HEAD* pHead = (MSG_HEAD*)pBuffer;
    if(pHead->usCmd < pClient->GetMessageAllowBegin() || pHead->usCmd > pClient->GetMessageAllowEnd())
    {
        LOGWARNING("RECV ClientMsg:{} not Allow {}.{}", pHead->usCmd, pSocket->GetAddrString().c_str(), pSocket->GetPort());
        pSocket->Interrupt();
        return;
    }

    switch(pHead->usCmd)
    {
        default:
        {
            if(pClient->GetDestServerPort().IsVaild() == false)
                return;
            // send to other server
            CNetworkMessage msg(pBuffer, len, pClient->GetVirtualSocket(), VirtualSocket(pClient->GetDestServerPort(), 0));
            _SendMsgToZonePort(msg);
        }
        break;
    }
    __LEAVE_FUNCTION
}

ON_SERVERMSG(CSocketService, ServiceReady)
{
}

ON_SERVERMSG(CSocketService, SocketStartAccept)
{
    LOGMESSAGE("START_ACCEPT");
    SocketService()->GetNetworkService()->EnableListener(nullptr, true);
}

ON_SERVERMSG(CSocketService, SocketStopAccept)
{
    LOGMESSAGE("STOP_ACCEPT");
    SocketService()->GetNetworkService()->EnableListener(nullptr, false);
}

ON_SERVERMSG(CSocketService, SocketChangeDest)
{
    CGameClient* pClient = SocketService()->QueryClient(msg.vs());
    if(pClient && pClient->IsVaild())
    {
        ServerPort destport{msg.destport()};

        pClient->SetDestServerPort(destport);
        LOGNETDEBUG("SCK_CHG_DEST {}:{} To Service:{}-{}",
                    pClient->GetSocketAddr().c_str(),
                    pClient->GetSocketPort(),
                    destport.GetWorldID(),
                    destport.GetServiceID());
    }
}

ON_SERVERMSG(CSocketService, SocketClose)
{
    CGameClient* pClient = SocketService()->QueryClient(msg.vs());
    if(pClient && pClient->IsVaild())
    {
        LOGDEBUG("CLOSE CLIENT BYVS:{}:{} FROM OTHER SERVER", pClient->GetSocketAddr().c_str(), pClient->GetSocketPort());
        //主动关闭客户端连接，需要通知客户端不要重连
        pClient->Interrupt();
    }
}

ON_SERVERMSG(CSocketService, SocketAuth)
{
    CGameClient* pClient = SocketService()->QueryClient(msg.vs());

    if(pClient && pClient->IsVaild())
    {
        LOGDEBUG("AuthSucc BYVS:{}:{} ", pClient->GetSocketAddr().c_str(), pClient->GetSocketPort());
        pClient->SetAuth(true);
        pClient->SetMessageAllow(CLIENT_MSG_ID_BEGIN, CLIENT_MSG_ID_END);
        pClient->SetDestServerPort(ServerPort(GetWorldID(), WORLD_SERVICE,0));

        ServerMSG::SocketLogin login_msg;
        login_msg.set_vs(msg.vs());
        login_msg.set_open_id(msg.open_id());
        SocketService()->SendProtoMsgToZonePort(ServerPort(GetWorldID(), WORLD_SERVICE,0), login_msg);
    }
}

void CSocketService::OnProcessMessage(CNetworkMessage* pNetworkMsg)
{
    __ENTER_FUNCTION

    //只需要处理来自其他服务器的消息
    //来自客户端的消息已经直接发往对应服务器了
    if(m_pNetMsgProcess->Process(pNetworkMsg) == true)
    {
        return;
    }
    // send to socket
    switch(pNetworkMsg->GetMultiType())
    {
        case MULTITYPE_NONE:
        {
            CGameClient* pClient = QueryClient(pNetworkMsg->GetTo());
            if(pClient && pClient->IsVaild())
            {
                pClient->SendSocketMsg(pNetworkMsg);
            }
            return;
        }
        break;
        case MULTITYPE_BROADCAST:
        {
            for(const auto& v: m_setVirtualSocket)
            {
                CGameClient* pClient = v.second;
                if(pClient && pClient->IsVaild() && pClient->IsAuth())
                {
                    pClient->SendSocketMsg(pNetworkMsg);
                }
            }
            return;
        }
        break;
        case MULTITYPE_VIRTUALSOCKET:
        {
            const auto& refSet = pNetworkMsg->GetMultiTo();
            for(auto it = refSet.begin(); it != refSet.end(); it++)
            {
                CGameClient* pClient = QueryClient(*it);
                if(pClient && pClient->IsVaild())
                {
                    pClient->SendSocketMsg(pNetworkMsg);
                }
            }
            return;
        }
        break;
        case MULTITYPE_USERID:
        {
        }
        break;
        case MULTITYPE_GROUPID:
        {
        }
        break;
        default:
        {
        }
        break;
    }

    __LEAVE_FUNCTION
}

void CSocketService::OnLogicThreadCreate()
{
    tls_pService = this;
    CServiceCommon::OnLogicThreadCreate();
}

void CSocketService::OnLogicThreadExit()
{
    CServiceCommon::OnLogicThreadExit();
}

void CSocketService::OnLogicThreadProc()
{
    __ENTER_FUNCTION
    CServiceCommon::OnLogicThreadProc();

    if(m_tLastDisplayTime.ToNextTime())
    {
        std::string buf = std::string("\n======================================================================") +
                          fmt::format(FMT_STRING("\nMessageProcess:{}\tSocketMsg:{}\tMem:{}"),
                                      GetMessageProcess(),
                                      m_nSocketMessageProcess,
                                      get_thread_memory_allocted()) +
                          fmt::format(FMT_STRING("\nRecvTotal:{}\tRecvAvg:{}"),
                                      GetNetworkService()->GetRecvBPS().GetTotal(),
                                      GetNetworkService()->GetRecvBPS().GetAvgBPS()) +
                          fmt::format(FMT_STRING("\nSendTotal:{}\tSendAvg:{}"),
                                      GetNetworkService()->GetSendBPS().GetTotal(),
                                      GetNetworkService()->GetSendBPS().GetAvgBPS());

        GetMessageRoute()->ForEach([&buf](auto pMessagePort) {
            if(pMessagePort && pMessagePort->GetWriteBufferSize())
            {
                buf += fmt::format(FMT_STRING("\nMsgPort:{}-{}\tSendBuff:{}"),
                                   pMessagePort->GetServerPort().GetWorldID(),
                                   pMessagePort->GetServerPort().GetServiceID(),
                                   pMessagePort->GetWriteBufferSize());
            }
        });

        LOGMONITOR("{}", buf.c_str());
        m_pMonitorMgr->Print();
        SetMessageProcess(0);
        m_nSocketMessageProcess = 0;
    }

    __LEAVE_FUNCTION
}

void CSocketService::OnRecvTimeout(CNetSocket* pSocket) {}
