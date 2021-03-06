#include "NetSocket.h"

#include <event2/bufferevent.h>
#include <event2/event.h>

#include "EventManager.h"
#include "NetworkMessage.h"
#include "NetworkService.h"
#include "event2/buffer.h"
#include "event2/util.h"

#ifdef __linux
#include <netinet/tcp.h>
#else
#include <ws2def.h>
#endif

OBJECTHEAP_IMPLEMENTATION(CNetSocket, s_Heap);

CNetSocket::CNetSocket(CNetworkService* pService, CNetEventHandler* pEventHandler)
    : m_pService(pService)
    , m_pEventHandler(pEventHandler)
    , m_pBufferevent(nullptr)
    , m_Status(NSS_NOT_INIT)
    , m_nRecvTimeOutSec(30)
    , m_Sendbuf(evbuffer_new())
    , m_nSocketIdx(0xFFFF)
    , m_pDecryptor(nullptr)
    , m_pEncryptor(nullptr)
    , m_nPacketSizeMax(_MAX_MSGSIZE)
    , m_socket(INVALID_SOCKET)
    , m_ReadBuff{std::make_unique<byte[]>(m_nPacketSizeMax)}
{
}

CNetSocket::~CNetSocket()
{
    __ENTER_FUNCTION

    if(m_pBufferevent)
    {
        bufferevent_disable(m_pBufferevent, EV_READ | EV_WRITE);
        bufferevent_setcb(m_pBufferevent, nullptr, nullptr, nullptr, nullptr);
        bufferevent_free(m_pBufferevent);
        m_pBufferevent = nullptr;
    }

    if(m_Sendbuf)
    {
        evbuffer_free(m_Sendbuf);
        m_Sendbuf = nullptr;
    }

    SendMsgData* pData = nullptr;
    while(m_SendMsgQueue.get(pData))
    {
        SAFE_DELETE(pData);
    }

    __LEAVE_FUNCTION
}

CNetSocket::SendMsgData::SendMsgData(CNetworkMessage&& msg)
    : send_msg(std::move(msg))
{
    send_msg.CopyBuffer();
}

CNetSocket::SendMsgData::SendMsgData(const CNetworkMessage& msg)
    : send_msg(msg)
{
    send_msg.CopyBuffer();
}

bool CNetSocket::SendNetworkMessage(CNetworkMessage&& msg)
{
    __ENTER_FUNCTION
    CHECKF(msg.GetSize() < GetPacketSizeMax());
    SendMsgData* pData = new SendMsgData{std::move(msg)};
    m_SendMsgQueue.push(pData);
    PostSend();
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CNetSocket::SendNetworkMessage(const CNetworkMessage& msg, bool bNeedDuplicate)
{
    __ENTER_FUNCTION
    CHECKF(msg.GetSize() < GetPacketSizeMax());
    CNetworkMessage send_msg;
    send_msg.CopyRawMessage(msg);
    if(m_pEncryptor && bNeedDuplicate)
    {
        send_msg.DuplicateBuffer();
    }
    SendMsgData* pData = new SendMsgData{std::move(send_msg)};
    m_SendMsgQueue.push(pData);
    PostSend();
    return true;
    __LEAVE_FUNCTION
    return false;
}

void CNetSocket::PostSend()
{
    __ENTER_FUNCTION
    if(m_Event.IsRunning() == false)
    {
        CEventEntryCreateParam param;
        param.cb = std::bind(&CNetSocket::_SendAllMsg, this);
        m_pService->GetEventManager()->ScheduleEvent(param, m_Event);
    }

    __LEAVE_FUNCTION
}

void CNetSocket::_SendAllMsg()
{
    SendMsgData* pData = nullptr;
    while(m_SendMsgQueue.get(pData))
    {
        __ENTER_FUNCTION

        if(m_pEncryptor)
        {
            pData->send_msg.Encryptor(m_pEncryptor.get());
        }
        _SendMsg(pData->send_msg.GetBuf(), pData->send_msg.GetSize());
        SAFE_DELETE(pData);
        __LEAVE_FUNCTION
    }
}

bool CNetSocket::_SendMsg(byte* pBuffer, size_t len)
{
    __ENTER_FUNCTION

    if(m_pBufferevent == nullptr)
        return false;

    if(GetStatus() == NSS_CONNECTING || GetStatus() == NSS_WAIT_RECONNECT)
    {
        // hold msg
        int32_t result   = evbuffer_add(m_Sendbuf, pBuffer, len);
        m_nWaitWriteSize = evbuffer_get_length(m_Sendbuf);
        return result == 0;
    }
    else if(GetStatus() == NSS_READY)
    {
        m_pService->AddSendByteCount(len);
        bufferevent_write(m_pBufferevent, pBuffer, len);
        static constexpr bool bFlush = false;
        if(bFlush)
            bufferevent_flush(m_pBufferevent, EV_WRITE, BEV_FLUSH);
        size_t nNeedWrite = evbuffer_get_length(bufferevent_get_output(m_pBufferevent));
        m_nWaitWriteSize  = nNeedWrite;
        if(nNeedWrite > m_nLogWriteHighWateMark)
        {
            LOGNETERROR("Write Buffer {}:{} oversize:{}", GetAddrString().c_str(), GetPort(), nNeedWrite);
        }

        return true;
    }
    else
    {
        return false;
    }
    __LEAVE_FUNCTION
    return false;
}

void CNetSocket::_SetTimeout()
{
    __ENTER_FUNCTION
    if(GetStatus() != NSS_CLOSEING)
    {
        struct timeval timeout_read;
        timeout_read.tv_sec  = GetRecvTimeOutSec();
        timeout_read.tv_usec = 0;
        // struct timeval timeout_write;
        // timeout_write.tv_sec = 5;
        // timeout_write.tv_usec = 0;

        // bufferevent_set_timeouts(m_pBufferevent, nullptr, nullptr);
        if(m_pBufferevent)
        {
            bufferevent_set_timeouts(m_pBufferevent, &timeout_read, nullptr);
        }
    }
    __LEAVE_FUNCTION
}

void CNetSocket::_OnReceive(bufferevent* b)
{
    __ENTER_FUNCTION

    evbuffer* input = bufferevent_get_input(b);
    while(GetStatus() == NSS_READY)
    {
        size_t nSize = evbuffer_get_length(input);
        if(nSize < sizeof(MSG_HEAD))
            return;

        MSG_HEAD* pHeader = (MSG_HEAD*)evbuffer_pullup(input, sizeof(MSG_HEAD));
        if(pHeader->msg_size < sizeof(MSG_HEAD) || pHeader->msg_size > GetPacketSizeMax())
        {
            LOGNETERROR("CNetSocket _OnReceive Msg:{} size:{} > MaxSize:{}, LastProcessCMD:{}",
                        pHeader->msg_cmd,
                        pHeader->msg_size,
                        GetPacketSizeMax(),
                        m_nLastProcessMsgCMD);
            std::string data;
            for(size_t i = 0; i < m_nLastCMDSize; i++)
            {
                unsigned char v = m_ReadBuff[i];
                data += fmt::format(FMT_STRING("{0:x} "), v);
            }
            LOGNETDEBUG("{}", data.c_str());
            _OnClose(BEV_EVENT_READING);
            return;
        }
        if(nSize < pHeader->msg_size)
            return;
        byte* pReadBuf = m_ReadBuff.get();
        evbuffer_remove(input, pReadBuf, pHeader->msg_size);
        pHeader = (MSG_HEAD*)pReadBuf;

        if(m_pDecryptor)
        {
            if(pHeader->is_ciper == false)
            {
                LOGNETERROR("CNetSocket _OnReceive Msg:{} size:{} need decryptor, but msg is not chiper",
                        pHeader->msg_cmd,
                        pHeader->msg_size);
                _OnClose(BEV_EVENT_READING);
                return;
            }
            byte*  pBody    = pReadBuf + sizeof(MSG_HEAD);
            size_t nBodyLen = pHeader->msg_size - sizeof(MSG_HEAD);
            m_pDecryptor->Decryptor(pBody, nBodyLen, pBody, nBodyLen);
        }

        m_nLastProcessMsgCMD = pHeader->msg_cmd;
        m_nLastCMDSize       = pHeader->msg_size;
        OnRecvData(pReadBuf, pHeader->msg_size);
    }
    __LEAVE_FUNCTION
}

void CNetSocket::_OnSocketRead(bufferevent* b, void* ctx)
{
    __ENTER_FUNCTION

    CNetSocket* pSocket = (CNetSocket*)ctx;
    pSocket->_OnReceive(b);
    pSocket->set_sock_quickack();
    __LEAVE_FUNCTION
}

void CNetSocket::_OnSendOK(bufferevent* b, void* ctx)
{
    __ENTER_FUNCTION
    CNetSocket* pSocket       = (CNetSocket*)ctx;
    pSocket->m_nWaitWriteSize = evbuffer_get_length(bufferevent_get_output(b));
    __LEAVE_FUNCTION
}

void CNetSocket::_OnCheckAllSendOK(bufferevent* b, void* ctx)
{
    __ENTER_FUNCTION
    CNetSocket* pSocket       = (CNetSocket*)ctx;
    pSocket->m_nWaitWriteSize = evbuffer_get_length(bufferevent_get_output(b));
    if(pSocket->GetStatus() == NSS_CLOSEING && pSocket->m_nWaitWriteSize == 0)
    {
        pSocket->OnDisconnected();
    }
    __LEAVE_FUNCTION
}

void CNetSocket::_OnSocketEvent(bufferevent* b, short what, void* ctx)
{
    __ENTER_FUNCTION
    if(b == nullptr || ctx == nullptr)
        return;
    CNetSocket* pSocket = (CNetSocket*)ctx;
    bool        bClose  = false;
    if(what & BEV_EVENT_TIMEOUT)
    {
        if(what & BEV_EVENT_READING)
        {
            MSG_HEAD msg;
            msg.msg_cmd  = COMMON_CMD_PING;
            msg.msg_size = sizeof(MSG_HEAD);
            pSocket->_SendMsg((byte*)&msg, sizeof(msg));
            bufferevent_enable(b, EV_READ);
        }
        else if(what & BEV_EVENT_WRITING)
        {
            bClose = true;
            LOGNETDEBUG("CNetSocket write timeout {}:{}", pSocket->GetAddrString().c_str(), pSocket->GetPort());
        }
    }
    if(what & BEV_EVENT_ERROR)
    {
        int32_t     err    = evutil_socket_geterror(bufferevent_getfd(b));
        const char* errstr = evutil_socket_error_to_string(err);
        if(err == 10054 || err == 104 || err == 32)
        {
            LOGNETDEBUG("CNetSocket error{}: {}, {}:{}", err, errstr, pSocket->GetAddrString().c_str(), pSocket->GetPort());
        }
        else
        {
            LOGERROR("CNetSocket error{}: {}, {}:{}", err, errstr, pSocket->GetAddrString().c_str(), pSocket->GetPort());
        }
        bClose = true;
    }
    if(what & BEV_EVENT_EOF)
    {
        LOGNETDEBUG("CNetSocket eof {}:{}", pSocket->GetAddrString().c_str(), pSocket->GetPort());
        //尝试将剩余的接收缓冲区内的消息处理完(判断是否是主动关闭)
        pSocket->_OnReceive(b);
        bufferevent_setcb(b, nullptr, nullptr, nullptr, nullptr);
        bClose = true;
    }
    if(bClose)
        pSocket->_OnClose(what);

    __LEAVE_FUNCTION
}

void CNetSocket::SetAddrAndPort(const char* addr, int32_t port)
{
    __ENTER_FUNCTION
    if(addr == nullptr)
        m_strAddr.clear();
    else
        m_strAddr = addr;
    m_nPort = port;
    evutil_inet_pton(AF_INET, addr, &m_addr);
    __LEAVE_FUNCTION
}

void CNetSocket::SetAddr(const std::string& val)
{
    __ENTER_FUNCTION
    m_strAddr = val;
    evutil_inet_pton(AF_INET, val.c_str(), &m_addr);
    __LEAVE_FUNCTION
}

size_t CNetSocket::GetWaitWriteSize()
{
    return m_nWaitWriteSize;
}

void CNetSocket::SetPacketSizeMax(size_t val)
{
    if(m_nPacketSizeMax != val)
    {
        m_nPacketSizeMax = val;
        m_ReadBuff       = std::make_unique<byte[]>(val);
    }
}

void CNetSocket::OnDisconnected()
{
    __ENTER_FUNCTION
    LOGNETDEBUG("CNetSocket OnDisconnected: {}:{}", GetAddrString().c_str(), GetPort());

    SetStatus(NSS_CLOSED);
    if(m_pEventHandler)
        m_pEventHandler->OnDisconnected(this);
    m_pService->_RemoveSocket(this);
    m_pService->_AddCloseingSocket(this);
    __LEAVE_FUNCTION
}

void CNetSocket::OnRecvData(byte* pBuffer, size_t len)
{
    __ENTER_FUNCTION
    m_pService->AddRecvByteCount(len);

    MSG_HEAD* pHeader = (MSG_HEAD*)pBuffer;
    switch(pHeader->msg_cmd)
    {
        case COMMON_CMD_INTERRUPT:
        {
            LOGNETDEBUG("COMMON_CMD_INTERRUPT:{}:{}", GetAddrString().c_str(), GetPort());

            _OnClose(COMMON_CMD_INTERRUPT);
            return;
        }
        break;
        case COMMON_CMD_PING:
        {
            MSG_HEAD msg;
            msg.msg_cmd  = COMMON_CMD_PONG;
            msg.msg_size = sizeof(MSG_HEAD);
            _SendMsg((byte*)&msg, sizeof(msg));
            // LOGNETDEBUG("MSG_PING_RECV:{}:{}", GetAddrString().c_str(), GetPort());
            return;
        }
        break;
        case COMMON_CMD_PONG:
        {
            // LOGNETDEBUG("MSG_PONG_RECV:{}:{}", GetAddrString().c_str(), GetPort());
            return;
        }
        break;
        default:
            break;
    }

    if(m_pEventHandler)
        m_pEventHandler->OnRecvData(this, pBuffer, len);
    __LEAVE_FUNCTION
}

void CNetSocket::OnRecvTimeout(bool& bReconnect)
{
    __ENTER_FUNCTION
    if(m_pEventHandler)
        m_pEventHandler->OnRecvTimeout(this);

    __LEAVE_FUNCTION
}

void CNetSocket::set_sock_nodely()
{
    bool option_true  = true;
    bool option_false = true;
    setsockopt(m_socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&option_true, sizeof(option_true));
    setsockopt(m_socket, IPPROTO_TCP, TCP_CORK, (const char*)&option_false, sizeof(option_false));

    // turn off SIGPIPE signal
#ifdef __linux__
    setsockopt(m_socket, SOL_SOCKET, MSG_NOSIGNAL, (const char*)&option_true, sizeof(option_true));
#else
    setsockopt(m_socket, SOL_SOCKET, SO_NOSIGPIPE, (const char*)&option_true, sizeof(option_true));
#endif

    // set SO_LINGER so socket closes gracefully
    struct linger ling;
    ling.l_onoff  = 1;
    ling.l_linger = 0;
    setsockopt(m_socket, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling));
}

void CNetSocket::set_sock_quickack()
{
    bool option_true = true;
    setsockopt(m_socket, IPPROTO_TCP, TCP_QUICKACK, (const char*)&option_true, sizeof(option_true));
}