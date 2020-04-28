#include "NetClientSocket.h"

#include <event2/bufferevent.h>
#include <event2/event.h>

#include "EventManager.h"
#include "NetworkMessage.h"
#include "NetworkService.h"
#include "event2/buffer.h"
#include "event2/util.h"

CClientSocket::CClientSocket(CNetworkService* pService, CNetEventHandler* pEventHandler)
    : CNetSocket(pService, pEventHandler)
{
}

CClientSocket::~CClientSocket()
{

}

bool CClientSocket::Init(bufferevent* pBufferEvent)
{
    __ENTER_FUNCTION
    m_pBufferevent = pBufferEvent;
    if(m_pBufferevent)
    {
        bufferevent_setcb(m_pBufferevent, _OnSocketRead, NULL, _OnSocketEvent, (void*)this);
        bufferevent_enable(m_pBufferevent, EV_WRITE | EV_READ | EV_PERSIST);
        _SetTimeout();
    }

    SetStatus(NSS_READY);
    return true;
    __LEAVE_FUNCTION
    return false;
}

void CClientSocket::Interrupt()
{
    __ENTER_FUNCTION

    if(GetStatus() == NSS_READY || GetStatus() == NSS_CONNECTING)
    {
        bufferevent_disable(m_pBufferevent, EV_READ);
        bufferevent_setcb(m_pBufferevent, nullptr, _OnSendOK, _OnSocketEvent, this);

        MSG_HEAD msg;
        msg.usCmd  = COMMON_CMD_INTERRUPT;
        msg.usSize = sizeof(MSG_HEAD);
        SendSocketMsg((byte*)&msg, sizeof(msg), true);
        SetStatus(NSS_CLOSEING);
    }
    __LEAVE_FUNCTION
}

void CClientSocket::_OnClose(short what)
{
    __ENTER_FUNCTION
    if(GetStatus() == NSS_CLOSEING)
    {
        OnDisconnected();
        return;
    }
    SetStatus(NSS_CLOSEING);

    LOGNETDEBUG("CClientSocket _OnClose {}:{} err:{}", GetAddrString().c_str(), GetPort(), what);
    if(m_pBufferevent)
    {
        bufferevent_disable(m_pBufferevent, EV_READ | EV_PERSIST);
    }

    OnDisconnected();

    __LEAVE_FUNCTION
}

void CClientSocket::OnAccepted()
{
    __ENTER_FUNCTION
    if(m_pEventHandler)
        m_pEventHandler->OnAccepted(this);
    __LEAVE_FUNCTION
}