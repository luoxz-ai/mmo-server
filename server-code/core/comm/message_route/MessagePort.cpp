#include "MessagePort.h"

#include "CheckUtil.h"
#include "MessageRoute.h"
#include "NetSocket.h"
#include "NetworkMessage.h"
#include "msg_internal.pb.h"

CMessagePort::CMessagePort(const ServerPort& nServerPort, CMessageRoute* pRoute)
    : m_nServerPort(nServerPort)
    , m_pRoute(pRoute)
{
}

CMessagePort::~CMessagePort()
{
    __ENTER_FUNCTION
    Destory();
    __LEAVE_FUNCTION
}

void CMessagePort::Destory()
{
    __ENTER_FUNCTION
    CNetworkMessage* pMsg;
    while(TakePortMsg(pMsg))
    {
        SAFE_DELETE(pMsg);
    }
    while(m_SendMsgQueue.get(pMsg))
    {
        SAFE_DELETE(pMsg);
    }
    if(m_pRemoteServerSocket)
    {
        m_pRemoteServerSocket->Interrupt();
        m_pRemoteServerSocket = nullptr;
    }
    __LEAVE_FUNCTION
}

bool CMessagePort::TakePortMsg(CNetworkMessage*& msg)
{
    __ENTER_FUNCTION
    return m_RecvMsgQueue.get(msg);
    __LEAVE_FUNCTION
    return false;
}

void CMessagePort::OnConnected(CNetSocket* pSocket)
{
    __ENTER_FUNCTION
    MSG_HEAD msg;
    msg.usCmd  = COMMON_CMD_PING;
    msg.usSize = sizeof(MSG_HEAD);
    pSocket->_SendMsg((byte*)&msg, sizeof(msg));
    //服务器间通信扩充recv缓冲区大小
    pSocket->SetPacketSizeMax(_MAX_MSGSIZE * 10);
    LOGNETDEBUG("MessagePort:{}-{} OnConnected {}:{}",
                GetServerPort().GetServiceID().GetServiceType(),
                GetServerPort().GetServiceID().GetServiceIdx(),
                pSocket->GetAddrString().c_str(),
                pSocket->GetPort());
    if(auto pHandler = m_pPortEventHandler.load())
    {
        pHandler->OnPortConnected(pSocket);
    }
    __LEAVE_FUNCTION
}

void CMessagePort::OnConnectFailed(CNetSocket* pSocket)
{
    __ENTER_FUNCTION
    LOGNETINFO("MessagePort:{}-{} OnConnectFailed {}:{}",
               GetServerPort().GetServiceID().GetServiceType(),
               GetServerPort().GetServiceID().GetServiceIdx(),
               pSocket->GetAddrString().c_str(),
               pSocket->GetPort());
    if(auto pHandler = m_pPortEventHandler.load())
    {
        pHandler->OnPortConnectFailed(pSocket);
    }
    __LEAVE_FUNCTION
}

void CMessagePort::OnDisconnected(CNetSocket* pSocket)
{
    __ENTER_FUNCTION
    LOGNETINFO("MessagePort:{}-{} OnDisconnected {}:{}",
               GetServerPort().GetServiceID().GetServiceType(),
               GetServerPort().GetServiceID().GetServiceIdx(),
               pSocket->GetAddrString().c_str(),
               pSocket->GetPort());
    if(auto pHandler = m_pPortEventHandler.load())
    {
        pHandler->OnPortDisconnected(pSocket);
    }
    __LEAVE_FUNCTION
}

void CMessagePort::OnAccepted(CNetSocket* pSocket)
{
    __ENTER_FUNCTION
    LOGNETDEBUG("MessagePort:{}-{} OnAccpet {}:{}",
                GetServerPort().GetServiceID().GetServiceType(),
                GetServerPort().GetServiceID().GetServiceIdx(),
                pSocket->GetAddrString().c_str(),
                pSocket->GetPort());
    //服务器间通信扩充recv缓冲区大小
    pSocket->SetPacketSizeMax(_MAX_MSGSIZE * 10);
    if(auto pHandler = m_pPortEventHandler.load())
    {
        pHandler->OnPortAccepted(pSocket);
    }
    __LEAVE_FUNCTION
}

void CMessagePort::SetRemoteSocket(CNetSocket* pSocket)
{
    __ENTER_FUNCTION
    m_pRemoteServerSocket = pSocket;
    //服务器间通信扩充recv缓冲区大小
    m_pRemoteServerSocket->SetPacketSizeMax(_MAX_MSGSIZE * 10);
    __LEAVE_FUNCTION
}

void CMessagePort::OnRecvData(CNetSocket* pSocket, byte* pBuffer, size_t len)
{
    __ENTER_FUNCTION
    MSG_HEAD*   pHead = (MSG_HEAD*)pBuffer;
    InternalMsg internal_msg;
    if(internal_msg.ParseFromArray(pBuffer + sizeof(MSG_HEAD), len - sizeof(MSG_HEAD)) == false)
    {
        LOGNETERROR("MessagePort:{}-{} Recv a unknow cmd:{} size:{}",
                    GetServerPort().GetServiceID().GetServiceType(),
                    GetServerPort().GetServiceID().GetServiceIdx(),
                    pHead->usCmd,
                    pHead->usSize);
        return;
    }

    CNetworkMessage* pMsg =
        new CNetworkMessage((byte*)internal_msg.proto_msg().c_str(), internal_msg.proto_msg().size(), internal_msg.from(), internal_msg.to());
    pMsg->SetForward(internal_msg.forward().data(), internal_msg.forward_size());
    pMsg->SetMultiTo(internal_msg.brocast_vs().data(), internal_msg.brocast_vs_size());
    pMsg->SetMultiTo(internal_msg.brocast_id().data(), internal_msg.brocast_id_size());
    if(internal_msg.brocast_all())
        pMsg->SetBroadcast();
    pMsg->CopyBuffer();
    m_RecvMsgQueue.push(pMsg);

    auto pHandler = m_pPortEventHandler.load();
    if(pHandler)
    {
        pHandler->OnPortRecvData(pSocket, pBuffer, len);
    }

    __LEAVE_FUNCTION
}

void CMessagePort::OnRecvTimeout(CNetSocket* pSocket)
{
    auto pHandler = m_pPortEventHandler.load();
    if(pHandler)
    {
        pHandler->OnPortRecvTimeout(pSocket);
    }
}

size_t CMessagePort::GetWriteBufferSize()
{
    __ENTER_FUNCTION
    if(m_pRemoteServerSocket)
    {
        size_t nNeedWrite = m_pRemoteServerSocket->GetWaitWriteSize();
        return nNeedWrite;
    }
    __LEAVE_FUNCTION
    return 0;
}

void CMessagePort::PostSend()
{
    __ENTER_FUNCTION
    if(m_Event.IsRunning() == false)
    {
        CEventEntryCreateParam param;
        param.cb = std::bind(&CMessagePort::_SendAllMsg, this);
        m_pRoute->GetEventManager()->ScheduleEvent(param, m_Event);
    }

    __LEAVE_FUNCTION
}

void CMessagePort::_SendAllMsg()
{
    __ENTER_FUNCTION
    CHECK(m_pRemoteServerSocket);
    CNetworkMessage* pMsg;
    while(m_SendMsgQueue.get(pMsg))
    {
        __ENTER_FUNCTION
        InternalMsg internal_msg;
        internal_msg.set_from(pMsg->GetFrom());
        internal_msg.set_to(pMsg->GetTo());
        internal_msg.set_proto_msg(pMsg->GetMsgBody(), pMsg->GetBodySize());

        for(const auto& v: pMsg->GetForward())
        {
            internal_msg.add_forward(v);
        }
        for(const auto& v: pMsg->GetMultiTo())
        {
            internal_msg.add_brocast_vs(v);
        }
        for(const auto& v: pMsg->GetMultiIDTo())
        {
            internal_msg.add_brocast_id(v);
        }
        internal_msg.set_brocast_all(pMsg->IsBroadcast());

        CNetworkMessage send_msg(0xFFFF, internal_msg);

        m_pRemoteServerSocket->SendNetworkMessage(std::move(send_msg));

        __LEAVE_FUNCTION
        SAFE_DELETE(pMsg);
    }
    __LEAVE_FUNCTION
}

bool CMessagePort::SendMsgToPort(const CNetworkMessage& msg)
{
    __ENTER_FUNCTION
    CNetworkMessage* pMsg = new CNetworkMessage(msg);
    pMsg->CopyBuffer();

    if(m_bLocalPort == false)
    {
        m_SendMsgQueue.push(pMsg);
        PostSend();
        return true;
    }
    else
    {
        m_RecvMsgQueue.push(pMsg);
        return true;
    }
    __LEAVE_FUNCTION
    return false;
}
