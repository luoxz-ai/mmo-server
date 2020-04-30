#include "ServiceComm.h"

#include <iostream>

#include "EventManager.h"
#include "MessagePort.h"
#include "MessageRoute.h"
#include "MonitorMgr.h"
#include "msg/ts_cmd.pb.h"
#include "server_msg/server_side.pb.h"

CServiceCommon::CServiceCommon()
    : m_pNetworkService(nullptr)
    , m_pMessagePort(nullptr)
{
}

CServiceCommon::~CServiceCommon()
{

}

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
    m_nServerPort    = nServerPort;
    m_ServiceName    = ::GetServiceName(nServerPort.GetServiceID());
    m_pNetMsgProcess = std::make_unique<CNetMSGProcess>();
    m_pEventManager.reset(CEventManager::CreateNew(nullptr));
    CHECKF(m_pEventManager.get());
    m_pMonitorMgr.reset(CMonitorMgr::CreateNew());
    CHECKF(m_pMonitorMgr.get());
    return true;
}

uint64_t CServiceCommon::CreateUID()
{
    return m_UIDFactory.CreateID();
}

bool CServiceCommon::CreateNetworkService()
{
    if(m_pNetworkService)
        return false;
    m_pNetworkService = std::make_unique<CNetworkService>();
    return true;
}

bool CServiceCommon::CreateService(int32_t                         nWorkInterval /*= 100*/,
                                   class CMessagePortEventHandler* pEventHandler /*= nullptr*/)
{
    __ENTER_FUNCTION
    m_UIDFactory.Init(GetServerPort().GetWorldID(), GetServerPort().GetServiceID());
    if(ListenMessagePort(GetServiceName(), pEventHandler) == false)
        return false;
    //开启逻辑线程处理来自其他服务器的消息
    StartLogicThread(nWorkInterval, GetServiceName() + "_Logic");
    BaseCode::InitMonitorLog(m_ServiceName);
    LOGMESSAGE("{} {} Create", GetServiceName().c_str(), GetServerPort().GetServiceID());
    __LEAVE_FUNCTION
    return true;
}

bool CServiceCommon::ListenMessagePort(const std::string&        service_name,
                                       CMessagePortEventHandler* pEventHandler /*= nullptr*/)
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
    if(m_pLogicThread)
    {
        return;
    }
    m_pLogicThread = std::make_unique<CNormalThread>(nWorkInterval,
                                                     name,
                                                     std::bind(&CServiceCommon::OnLogicThreadProc, this),
                                                     std::bind(&CServiceCommon::OnLogicThreadCreate, this),
                                                     std::bind(&CServiceCommon::OnLogicThreadExit, this));
}

void CServiceCommon::StopLogicThread()
{
    if(m_pLogicThread)
    {
        m_pLogicThread->Stop();
        m_pLogicThread->Join();
    }
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
}

void CServiceCommon::OnLogicThreadExit() {}

bool CServiceCommon::SendPortBroadcastMsg(const ServerPort& nServerPort, const google::protobuf::Message& msg) const
{
    return SendPortBroadcastMsg(nServerPort, to_server_msgid(msg), msg);
}

bool CServiceCommon::SendPortBroadcastMsg(const ServerPort&                nServerPort,
                                          uint16_t                         usCmd,
                                          const google::protobuf::Message& msg) const
{
    __ENTER_FUNCTION
    if(GetMessageRoute() && nServerPort.IsVaild())
    {
        CMessagePort* pMessagePort = GetMessageRoute()->QueryMessagePort(nServerPort);
        if(pMessagePort)
        {
            CNetworkMessage _msg(usCmd, msg, GetServerVirtualSocket(), VirtualSocket(nServerPort));
            _msg.SetBroadcast();
            return pMessagePort->SendBroadcastMsg(_msg);
        }
        else
        {
            LOGWARNING("SendPortMsg To ServerPort:{} {}, not find",
                       nServerPort.GetWorldID(),
                       nServerPort.GetServiceID());
        }
    }
    __LEAVE_FUNCTION
    return false;
}

bool CServiceCommon::SendPortMultiMsg(const ServerPort&                 nServerPort,
                                      const std::vector<VirtualSocket>& setVS,
                                      const CNetworkMessage&            msg) const
{
    __ENTER_FUNCTION
    if(GetMessageRoute() && nServerPort.IsVaild())
    {
        CMessagePort* pMessagePort = GetMessageRoute()->QueryMessagePort(nServerPort);
        if(pMessagePort)
        {
            return pMessagePort->SendMultiMsg(setVS, msg);
        }
        else
        {
            LOGWARNING("SendPortMsg To ServerPort:{} {}, not find",
                       nServerPort.GetWorldID(),
                       nServerPort.GetServiceID());
        }
    }
    __LEAVE_FUNCTION
    return false;
}

bool CServiceCommon::SendPortMultiIDMsg(const ServerPort&         nServerPort,
                                        const std::vector<OBJID>& setVS,
                                        const CNetworkMessage&    msg) const
{
    __ENTER_FUNCTION
    if(GetMessageRoute() && nServerPort.IsVaild())
    {
        CMessagePort* pMessagePort = GetMessageRoute()->QueryMessagePort(nServerPort);
        if(pMessagePort)
        {
            return pMessagePort->SendMultiIDMsg(setVS, msg);
        }
        else
        {
            LOGWARNING("SendPortMsg To ServerPort:{} {}, not find",
                       nServerPort.GetWorldID(),
                       nServerPort.GetServiceID());
        }
    }
    __LEAVE_FUNCTION
    return false;
}

bool CServiceCommon::SendPortMsg(const ServerPort& nServerPort, const google::protobuf::Message& msg) const
{
    return SendPortMsg(nServerPort, to_server_msgid(msg), msg);
}

bool CServiceCommon::SendPortMsg(const ServerPort&                nServerPort,
                                 uint16_t                         usCmd,
                                 const google::protobuf::Message& msg) const
{
    CNetworkMessage _msg(usCmd, msg, GetServerVirtualSocket(), nServerPort);
    return SendPortMsg(_msg);
}

bool CServiceCommon::SendToVirtualSocket(const VirtualSocket& vsTo, const google::protobuf::Message& msg) const
{
    return SendToVirtualSocket(vsTo, to_sc_cmd(msg), msg);
}

bool CServiceCommon::SendToVirtualSocket(const VirtualSocket&             vsTo,
                                         uint16_t                         usCmd,
                                         const google::protobuf::Message& msg) const
{
    CNetworkMessage _msg(usCmd, msg, GetServerVirtualSocket(), vsTo);
    return SendPortMsg(_msg);
}

bool CServiceCommon::TransmitPortMsg(const ServerPort& nServerPort, CNetworkMessage* pMsg) const
{
    CNetworkMessage _msg(*pMsg);
    _msg.SetTo(nServerPort);
    _msg.CopyBuffer();

    return SendPortMsg(_msg);
}

bool CServiceCommon::SendPortMsg(const CNetworkMessage& msg) const
{
    __ENTER_FUNCTION
    VirtualSocket vs(msg.GetTo());
    if(GetMessageRoute() && vs.GetServerPort() != 0)
    {
        m_pMonitorMgr->AddSendInfo(msg.GetCmd(), msg.GetSize());
        CMessagePort* pMessagePort = GetMessageRoute()->QueryMessagePort(vs.GetServerPort());
        if(pMessagePort)
        {
            return pMessagePort->SendPortMsg(msg);
        }
        else
        {
            LOGWARNING("SendPortMsg To ServerPort:{} {} {}, not find",
                       vs.GetServerPort().GetWorldID(),
                       vs.GetServerPort().GetServiceID(),
                       vs.GetSocketIdx());
        }
        return false;
    }
    else if(vs.GetServerPort() == m_nServerPort && vs.GetSocketIdx() != 0)
    {
        m_pMonitorMgr->AddSendInfo(msg.GetCmd(), msg.GetSize());
        // direct send message
        if(m_pNetworkService)
            return m_pNetworkService->SendSocketMsgByIdx(msg.GetTo().GetSocketIdx(), msg.GetBuf(), msg.GetSize());
        return false;
    }
    else
    {
        LOGWARNING("Message Want Send To Worng: {} {} {}",
                   msg.GetTo().GetServerPort().GetWorldID(),
                   msg.GetTo().GetServerPort().GetServiceID(),
                   msg.GetTo().GetSocketIdx());
#ifdef DEBUG
        DumpStack(CallFrameMap(1));
#endif
        return false;
    }
    __LEAVE_FUNCTION
    return false;
}

bool CServiceCommon::SendBroadcastMsg(const CNetworkMessage& msg) const
{
    __ENTER_FUNCTION
    VirtualSocket vs(msg.GetTo());
    if(GetMessageRoute() && vs.GetServerPort() != 0)
    {
        m_pMonitorMgr->AddSendInfo_broad(msg.GetCmd(), msg.GetSize());
        CMessagePort* pMessagePort = GetMessageRoute()->QueryMessagePort(vs.GetServerPort());
        if(pMessagePort)
        {
            return pMessagePort->SendBroadcastMsg(msg);
        }
        else
        {
            LOGWARNING("SendBroadcastMsg:{} {} {}, not find",
                       vs.GetServerPort().GetWorldID(),
                       vs.GetServerPort().GetServiceID(),
                       vs.GetSocketIdx());
        }
        return false;
    }
    else if(vs.GetServerPort() == m_nServerPort)
    {
        m_pMonitorMgr->AddSendInfo_broad(msg.GetCmd(), msg.GetSize());
        // direct send message
        if(m_pNetworkService)
        {
            m_pNetworkService->BrocastMsg(msg.GetBuf(), msg.GetSize());
            return true;
        }
        return false;
    }
    else
    {
        LOGWARNING("Message Want Send To Worng: {} {} {}",
                   msg.GetTo().GetServerPort().GetWorldID(),
                   msg.GetTo().GetServerPort().GetServiceID(),
                   msg.GetTo().GetSocketIdx());
        return false;
    }
    __LEAVE_FUNCTION
    return false;
}

bool CServiceCommon::SendMsgTo(const VirtualSocketMap_t& setSocketMap, const google::protobuf::Message& msg) const
{
    return SendMsgTo(setSocketMap, to_sc_cmd(msg), msg);
}

bool CServiceCommon::SendMsgTo(const VirtualSocketMap_t&        setSocketMap,
                               uint16_t                         nCmd,
                               const google::protobuf::Message& msg) const
{
    __ENTER_FUNCTION

    CNetworkMessage _msg(nCmd, msg, GetServerVirtualSocket());
    for(auto& [nServerPort, socket_list]: setSocketMap)
    {
        if(socket_list.size() == 1)
        {
            _msg.SetTo(socket_list.front());
            m_pMonitorMgr->AddSendInfo(nCmd, _msg.GetSize());
            SendPortMsg(_msg);
        }
        else
        {
            m_pMonitorMgr->AddSendInfo_some(nCmd, _msg.GetSize(), socket_list.size());
            SendPortMultiMsg(nServerPort, socket_list, _msg);
        }
    }
    return true;
    __LEAVE_FUNCTION
    return false;
}