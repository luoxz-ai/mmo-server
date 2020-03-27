#include "MessagePort.h"

#include "MessageRoute.h"
#include "NetSocket.h"
#include "NetworkMessage.h"

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
    while(TakeMsg(pMsg))
    {
        SAFE_DELETE(pMsg);
    }
    while(m_SendMsgQueue.get(pMsg))
    {
        SAFE_DELETE(pMsg);
    }
    if(m_pRemoteServerSocket)
    {
        m_pRemoteServerSocket->Close();
        m_pRemoteServerSocket = nullptr;
    }   
    __LEAVE_FUNCTION 
}

bool CMessagePort::TakeMsg(CNetworkMessage*& msg)
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
    pSocket->SendMsg((byte*)&msg, sizeof(msg));
    pSocket->SetPacketSizeMax(_MAX_MSGSIZE * 2);
    LOGNETDEBUG("MessagePort:{} OnConnected {}:{}",
                GetServiceName(GetServerPort().GetServiceID()),
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
    LOGNETDEBUG("MessagePort:{} OnConnectFailed {}:{}",
                GetServiceName(GetServerPort().GetServiceID()),
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
    LOGNETDEBUG("MessagePort:{} OnDisconnected {}:{}",
                GetServiceName(GetServerPort().GetServiceID()),
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
    LOGNETDEBUG("MessagePort:{} OnAccpet {}:{}",
                GetServiceName(GetServerPort().GetServiceID()),
                pSocket->GetAddrString().c_str(),
                pSocket->GetPort());
    pSocket->SetPacketSizeMax(_MAX_MSGSIZE * 2);
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
    m_pRemoteServerSocket->SetPacketSizeMax(_MAX_MSGSIZE * 2);
    __LEAVE_FUNCTION
}

void CMessagePort::OnRecvData(CNetSocket* pSocket, byte* pBuffer, size_t len)
{
    __ENTER_FUNCTION
    MSG_HEAD* pHead = (MSG_HEAD*)pBuffer;
    switch(pHead->usCmd)
    {
        case NETMSG_INTERNAL:
        {
            MSG_INTERNAL_MSG_HEAD* pInternal_msg_head = (MSG_INTERNAL_MSG_HEAD*)pBuffer;

            CNetworkMessage* pMsg = new CNetworkMessage(pBuffer + sizeof(MSG_INTERNAL_MSG_HEAD),
                                                        pHead->usSize - sizeof(MSG_INTERNAL_MSG_HEAD),
                                                        pInternal_msg_head->nFrom,
                                                        pInternal_msg_head->nTo);
            pMsg->CopyBuffer();
            m_RecvMsgQueue.push(pMsg);
        }
        break;
        case NETMSG_INTERNAL_FORWARD:
        {
            MSG_INTERNAL_FORWARD_MSG_HEAD* pInternal_msg_head = (MSG_INTERNAL_FORWARD_MSG_HEAD*)pBuffer;

            CNetworkMessage* pMsg = new CNetworkMessage{pBuffer + sizeof(MSG_INTERNAL_FORWARD_MSG_HEAD),
                                                        pHead->usSize - sizeof(MSG_INTERNAL_FORWARD_MSG_HEAD),
                                                        pInternal_msg_head->nFrom,
                                                        pInternal_msg_head->nTo,
                                                        pInternal_msg_head->nForward};
            pMsg->CopyBuffer();
            m_RecvMsgQueue.push(pMsg);
        }
        break;
        case NETMSG_INTERNAL_BROCAST_ALL:
        {
            MSG_INTERNAL_MSG_HEAD* pInternal_msg_head = (MSG_INTERNAL_MSG_HEAD*)pBuffer;

            CNetworkMessage* pMsg = new CNetworkMessage{pBuffer + sizeof(MSG_INTERNAL_MSG_HEAD),
                                                        pHead->usSize - sizeof(MSG_INTERNAL_MSG_HEAD),
                                                        pInternal_msg_head->nFrom,
                                                        pInternal_msg_head->nTo};
            pMsg->SetBroadcast();
            pMsg->CopyBuffer();
            m_RecvMsgQueue.push(pMsg);
        }
        break;
        case NETMSG_INTERNAL_BROCAST_BYVS:
        {
            MSG_INTERNAL_BROCAST_MSG_HEAD* pBrocast_msg_head = (MSG_INTERNAL_BROCAST_MSG_HEAD*)pBuffer;

            CNetworkMessage* pMsg = new CNetworkMessage(pBuffer + pBrocast_msg_head->GetSize(),
                                                        pHead->usSize - pBrocast_msg_head->GetSize(),
                                                        pBrocast_msg_head->nFrom);
            pMsg->SetMultiTo(&pBrocast_msg_head->setTo[0], pBrocast_msg_head->nAmount);
            pMsg->SetMultiType(MULTITYPE_VIRTUALSOCKET);
            pMsg->CopyBuffer();
            m_RecvMsgQueue.push(pMsg);
        }
        break;
        case NETMSG_INTERNAL_BROCAST_BYID:
        {
            MSG_INTERNAL_BROCAST_BYID_MSG_HEAD* pBrocast_msg_head = (MSG_INTERNAL_BROCAST_BYID_MSG_HEAD*)pBuffer;

            CNetworkMessage* pMsg = new CNetworkMessage(pBuffer + pBrocast_msg_head->GetSize(),
                                                        pHead->usSize - pBrocast_msg_head->GetSize(),
                                                        pBrocast_msg_head->nFrom);
            pMsg->SetMultiIDTo(&pBrocast_msg_head->setTo[0], pBrocast_msg_head->nAmount);
            pMsg->SetMultiType(MULTITYPE_USERID);
            pMsg->CopyBuffer();
            m_RecvMsgQueue.push(pMsg);
        }
        break;
        case NETMSG_INTERNAL_BROCAST_BYGROUPID:
        {
            MSG_INTERNAL_BROCAST_BYGROUPID_MSG_HEAD* pBrocast_msg_head =
                (MSG_INTERNAL_BROCAST_BYGROUPID_MSG_HEAD*)pBuffer;

            CNetworkMessage* pMsg = new CNetworkMessage(pBuffer + pBrocast_msg_head->GetSize(),
                                                        pHead->usSize - pBrocast_msg_head->GetSize(),
                                                        pBrocast_msg_head->nFrom);
            pMsg->SetMultiIDTo(&pBrocast_msg_head->setTo[0], pBrocast_msg_head->nAmount);
            pMsg->SetMultiType(MULTITYPE_GROUPID);
            pMsg->CopyBuffer();
            m_RecvMsgQueue.push(pMsg);
        }
        break;
        default:
        {
            LOGNETERROR("MessagePort:{} Recv a unknow cmd:{} size:{}",
                        GetServiceName(GetServerPort().GetServiceID()),
                        pHead->usCmd,
                        pHead->usSize);
        }
        break;
    }

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

bool CMessagePort::SendMultiMsg(const std::vector<VirtualSocket>& setVS, const CNetworkMessage& msg)
{
    __ENTER_FUNCTION
    CNetworkMessage* pMsg = new CNetworkMessage(msg);
    pMsg->SetMultiTo(setVS);
    pMsg->SetMultiType(MULTITYPE_VIRTUALSOCKET);
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

bool CMessagePort::SendMultiIDMsg(const std::vector<OBJID>& setID, const CNetworkMessage& msg)
{
    __ENTER_FUNCTION
    CNetworkMessage* pMsg = new CNetworkMessage(msg);
    pMsg->SetMultiIDTo(setID);
    pMsg->SetMultiType(MULTITYPE_USERID);
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
        m_pRoute->GetEventManager()->ScheduleEvent(0, std::bind(&CMessagePort::_SendAllMsg, this), 0, false, m_Event);
    }

    __LEAVE_FUNCTION
}

void CMessagePort::_SendAllMsg()
{
    __ENTER_FUNCTION
    CNetworkMessage* pMsg;
    while(m_SendMsgQueue.get(pMsg))
    {
        __ENTER_FUNCTION
        switch(pMsg->GetMultiType())
        {
            case MULTITYPE_NONE:
            {
                if(pMsg->GetForward().IsVaild())
                {
                    MSG_INTERNAL_MSG_HEAD warp_msg_header;
                    warp_msg_header.nFrom  = pMsg->GetFrom();
                    warp_msg_header.nTo    = pMsg->GetTo();
                    warp_msg_header.usSize = sizeof(MSG_INTERNAL_MSG_HEAD) + pMsg->GetSize();
                    warp_msg_header.usCmd  = NETMSG_INTERNAL;
                    m_pRemoteServerSocket->SendMsg(&warp_msg_header, sizeof(warp_msg_header));
                    m_pRemoteServerSocket->SendMsg(pMsg->GetBuf(), pMsg->GetSize());
                }
                else
                {
                    MSG_INTERNAL_MSG_HEAD warp_msg_header;
                    warp_msg_header.nFrom  = pMsg->GetFrom();
                    warp_msg_header.nTo    = pMsg->GetTo();
                    warp_msg_header.usSize = sizeof(MSG_INTERNAL_MSG_HEAD) + pMsg->GetSize();
                    warp_msg_header.usCmd  = NETMSG_INTERNAL;
                    m_pRemoteServerSocket->SendMsg(&warp_msg_header, sizeof(warp_msg_header));
                    m_pRemoteServerSocket->SendMsg(pMsg->GetBuf(), pMsg->GetSize());
                }
            }
            break;
            case MULTITYPE_BROADCAST:
            {
                MSG_INTERNAL_MSG_HEAD warp_msg_header;
                warp_msg_header.nFrom  = pMsg->GetFrom();
                warp_msg_header.nTo    = pMsg->GetTo();
                warp_msg_header.usSize = sizeof(MSG_INTERNAL_MSG_HEAD) + pMsg->GetSize();
                warp_msg_header.usCmd  = NETMSG_INTERNAL_BROCAST_ALL;
                m_pRemoteServerSocket->SendMsg(&warp_msg_header, sizeof(warp_msg_header));
                m_pRemoteServerSocket->SendMsg(pMsg->GetBuf(), pMsg->GetSize());
            }
            break;
            case MULTITYPE_VIRTUALSOCKET:
            {
                MSG_INTERNAL_BROCAST_MSG_HEAD warp_msg_header;
                warp_msg_header.usCmd   = NETMSG_INTERNAL_BROCAST_BYVS;
                warp_msg_header.nAmount = 0;
                warp_msg_header.nFrom   = pMsg->GetFrom();
                for(auto it = pMsg->GetMultiTo().begin(); it != pMsg->GetMultiTo().end(); it++)
                {
                    warp_msg_header.setTo[warp_msg_header.nAmount++] = *it;
                    if(warp_msg_header.nAmount >= warp_msg_header.GetMaxAmount())
                    {
                        warp_msg_header.usSize = warp_msg_header.GetSize() + pMsg->GetSize();
                        {
                            m_pRemoteServerSocket->SendMsg((byte*)&warp_msg_header, warp_msg_header.GetSize(), false);
                            m_pRemoteServerSocket->SendMsg(pMsg->GetBuf(), pMsg->GetSize());
                        }

                        warp_msg_header.nAmount = 0;
                    }
                }
                if(warp_msg_header.nAmount > 0)
                {
                    warp_msg_header.usSize = warp_msg_header.GetSize() + pMsg->GetSize();
                    {
                        m_pRemoteServerSocket->SendMsg((byte*)&warp_msg_header, warp_msg_header.GetSize(), false);
                        m_pRemoteServerSocket->SendMsg(pMsg->GetBuf(), pMsg->GetSize());
                    }
                }
            }
            break;
            case MULTITYPE_USERID:
            {
                MSG_INTERNAL_BROCAST_BYID_MSG_HEAD warp_msg_header;
                warp_msg_header.usCmd   = NETMSG_INTERNAL_BROCAST_BYID;
                warp_msg_header.nAmount = 0;
                warp_msg_header.nFrom   = pMsg->GetFrom();
                for(auto it = pMsg->GetMultiIDTo().begin(); it != pMsg->GetMultiIDTo().end(); it++)
                {
                    warp_msg_header.setTo[warp_msg_header.nAmount++] = *it;
                    if(warp_msg_header.nAmount >= warp_msg_header.GetMaxAmount())
                    {
                        warp_msg_header.usSize = warp_msg_header.GetSize() + pMsg->GetSize();
                        {
                            m_pRemoteServerSocket->SendMsg((byte*)&warp_msg_header, warp_msg_header.GetSize(), false);
                            m_pRemoteServerSocket->SendMsg(pMsg->GetBuf(), pMsg->GetSize());
                        }

                        warp_msg_header.nAmount = 0;
                    }
                }
                if(warp_msg_header.nAmount > 0)
                {
                    warp_msg_header.usSize = warp_msg_header.GetSize() + pMsg->GetSize();
                    {
                        m_pRemoteServerSocket->SendMsg((byte*)&warp_msg_header, warp_msg_header.GetSize(), false);
                        m_pRemoteServerSocket->SendMsg(pMsg->GetBuf(), pMsg->GetSize());
                    }
                }
            }
            break;
            case MULTITYPE_GROUPID:
            {
            }
            break;
        }

        __LEAVE_FUNCTION
        SAFE_DELETE(pMsg);
    }
    __LEAVE_FUNCTION
}

bool CMessagePort::SendMsg(const CNetworkMessage& msg)
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

bool CMessagePort::SendBroadcastMsg(const CNetworkMessage& msg)
{
    __ENTER_FUNCTION
    CNetworkMessage* pMsg = new CNetworkMessage(msg);
    pMsg->SetMultiType(MULTITYPE_BROADCAST);
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
