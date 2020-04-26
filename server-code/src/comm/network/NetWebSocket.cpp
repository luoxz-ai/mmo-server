#include "NetWebSocket.h"

#include "BaseCode.h"
#include "NetworkService.h"

CNetWebSocket::CNetWebSocket(CNetworkService* pService, CWebSocketEventHandler* pEventHandler)
    : m_WebSocketStatus(NWSS_NOT_INIT)
    , m_pService(pService)
    , m_pWebSocketHandler(pEventHandler)
    , m_pDecryptor(nullptr)
    , m_pEncryptor(nullptr)
    , m_pLwsInstance(nullptr)
    , m_nSocketIdx(0)
    , m_socket(0)
    , m_nPacketSizeMax(_MAX_MSGSIZE)
    , m_nLastProcessMsgCMD(0)
    , m_nLastCMDSize(0)
{
    memset(m_RecvBuffer, 0, g_RecvBufferSize);
}

CNetWebSocket::~CNetWebSocket() {}

bool CNetWebSocket::Init(struct lws* wsi)
{
    __ENTER_FUNCTION
    if(m_WebSocketStatus == NWSS_READY)
    {
        // init twice
        return false;
    }

    m_WebSocketStatus = NWSS_READY;
    m_pLwsInstance    = wsi;
    return true;
    __LEAVE_FUNCTION
    return false;
}

NET_WEBSOCKET_STATUS CNetWebSocket::GetWebSocketStatus()
{
    return m_WebSocketStatus;
}

std::string CNetWebSocket::GetAddrString()
{
    if(m_pLwsInstance)
    {
        char    addr[128] = {};
        int32_t addr_len  = 128;
        lws_get_peer_simple(m_pLwsInstance, addr, addr_len);
        return addr;
    }
    return "unknow-addr";
}

int32_t CNetWebSocket::GetPort()
{
    // libwebsockets没有提供获得client端口，一定要获得可考虑通过fd来获得
    return 0;
}

bool CNetWebSocket::SendSocketMsg(byte* pBuffer, size_t len, bool bFlush)
{
    __ENTER_FUNCTION
    if(m_WebSocketStatus != NWSS_READY)
    {
        return false;
    }

    if(m_pEncryptor)
    {
        // 加密接口需要调整，暂时不使用 18.05.08
        // m_pEncryptor->Encryptor(pBuffer+sizeof(MSG_HEAD), len-sizeof(MSG_HEAD), pBuffer, len-sizeof(MSG_HEAD));
    }

    // 入消息队列时，将大于单帧的数据包分, 当wsi可写时发送
    if(len > g_FrameSize)
    {
        uint32_t split   = 0;
        uint32_t remain  = 0;
        bool     isStart = true;
        do
        {
            WebSocketFrame* frame = WebSocketFrame::CreateFrame(pBuffer + split, g_FrameSize, isStart, false);
            m_SendList.push_back(frame);

            isStart = false;
            split += g_FrameSize;
            remain = len - split;
        } while(remain > g_FrameSize);

        if(remain > 0)
        {
            WebSocketFrame* frame = WebSocketFrame::CreateFrame(pBuffer + split, remain, false, true);
            m_SendList.push_back(frame);
        }
    }
    else
    {
        WebSocketFrame* frame = WebSocketFrame::CreateFrame(pBuffer, len, true, true);
        m_SendList.push_back(frame);
    }

    return true;
    __LEAVE_FUNCTION
    return false;
}

void CNetWebSocket::RealSend()
{
    __ENTER_FUNCTION
    // 无论发送列表剩多少，每次只发送一帧
    if(m_SendList.size() > 0)
    {
        WebSocketFrame* frame = m_SendList.front();
        int32_t         flags = lws_write_ws_flags(LWS_WRITE_BINARY, frame->isStart ? 1 : 0, frame->isEnd ? 1 : 0);

        /*
         * The OS may not accept everything you asked to write on the connection.
         *
         * Posix defines POLLOUT indication from poll() to show that the connection
         * will accept more write data, but it doesn't specifiy how much.  It may just
         * accept one byte of whatever you wanted to send.
         *
         * LWS will buffer the remainder automatically, and send it out autonomously.
         */
        lws_write(m_pLwsInstance, frame->GetPayload(), frame->GetPayloadLength(), (enum lws_write_protocol)flags);
        m_SendList.pop_front();
        WebSocketFrame::DestroyFrame(frame);
    }
    __LEAVE_FUNCTION
}

void CNetWebSocket::SendTestData()
{
    __ENTER_FUNCTION
    char        buf[1024] = {};
    const char* handshake = "hello websocket! i'm come from server!";
    memcpy(buf + LWS_PRE, handshake, strlen(handshake));

    lws_write(m_pLwsInstance, (unsigned char*)(buf + LWS_PRE), strlen(handshake), LWS_WRITE_TEXT);

    __LEAVE_FUNCTION
}

void CNetWebSocket::Close()
{
    __ENTER_FUNCTION
    if(m_WebSocketStatus != NWSS_READY)
    {
        return;
    }

    m_WebSocketStatus = NWSS_CLOSEING;
    lws_close_reason(m_pLwsInstance, LWS_CLOSE_STATUS_NORMAL, nullptr, 0);
    __LEAVE_FUNCTION
}

void CNetWebSocket::OnWsAccepted()
{
    __ENTER_FUNCTION
    m_pWebSocketHandler->OnWsAccepted(this);
    __LEAVE_FUNCTION
}

void CNetWebSocket::OnWsConnected()
{
    __ENTER_FUNCTION
    m_pWebSocketHandler->OnWsConnected(this);
    __LEAVE_FUNCTION
}

void CNetWebSocket::OnWsConnectFailed()
{
    __ENTER_FUNCTION
    m_pWebSocketHandler->OnWsConnectFailed(this);
    __LEAVE_FUNCTION
}

void CNetWebSocket::OnWsDisconnected()
{
    __ENTER_FUNCTION
    m_WebSocketStatus = NWSS_CLOSED;
    m_pWebSocketHandler->OnWsDisconnected(this);
    __LEAVE_FUNCTION
}

void CNetWebSocket::OnWsRecvData(byte* pBuffer, size_t len)
{
    __ENTER_FUNCTION
    _AddRecvBuffer(pBuffer, len);

    byte*  pPack    = nullptr;
    size_t nPackLen = 0;
    // notice: pPack指向缓存区中的地址，不要操作内存长度超过nPackLen，不然影响下一个包的内容
    // 		   这里更安全的做法是copy，但是copy影响效率。
    while(_GetEntirePackage(pPack, nPackLen))
    {
        MSG_HEAD* pHeader = (MSG_HEAD*)pPack;
        switch(pHeader->usCmd)
        {
            case COMMON_CMD_CLOSE:
            {
                LOGNETDEBUG("COMMON_CMD_CLOSE:{}:{}", GetAddrString().c_str(), GetPort());
                _ConsumeBuffer(nPackLen);
                return;
            }
            break;
            case COMMON_CMD_PING:
            {
                MSG_HEAD msg;
                msg.usCmd  = COMMON_CMD_PONG;
                msg.usSize = sizeof(MSG_HEAD);
                SendSocketMsg((byte*)&msg, sizeof(msg));
                // LOGNETDEBUG("MSG_PING_RECV:{}:{}", GetAddrString().c_str(), GetPort());
                _ConsumeBuffer(nPackLen);
                return;
            }
            break;
            case COMMON_CMD_PONG:
            {
                // LOGNETDEBUG("MSG_PONG_RECV:{}:{}", GetAddrString().c_str(), GetPort());
                _ConsumeBuffer(nPackLen);
                return;
            }
            break;
            default:
                break;
        }

        if(m_pDecryptor)
        {
            byte*  pBody    = pPack + sizeof(MSG_HEAD);
            size_t nBodyLen = pHeader->usSize - sizeof(MSG_HEAD);
            // notice: 加解密要注意采用最终pBody的长度不大于原有长度的算法，不然问题很大，会影响到缓存区中的数据；
            // 		  否则需要把缓存区中的数据先copy到足够长的Buffer中再解密。
            m_pDecryptor->Decryptor(pBody, nBodyLen, pBody, nBodyLen);
        }

        m_nLastProcessMsgCMD = pHeader->usCmd;
        m_nLastCMDSize       = pHeader->usSize;
        m_pWebSocketHandler->OnWsRecvData(this, pPack, nPackLen);
        _ConsumeBuffer(nPackLen);
    }

    __LEAVE_FUNCTION
}

bool CNetWebSocket::_AddRecvBuffer(byte* pBuffer, size_t len)
{
    __ENTER_FUNCTION
    if(len + m_RecvLength > g_RecvBufferSize)
    {
        // 缓冲区不够
        return false;
    }

    memcpy(m_RecvBuffer + m_RecvLength, pBuffer, len);
    m_RecvLength += len;
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CNetWebSocket::_GetEntirePackage(byte* pBuffer, size_t& len)
{
    __ENTER_FUNCTION
    if(m_RecvLength < sizeof(MSG_HEAD))
    {
        // 包不足
        return false;
    }

    MSG_HEAD* pHeader = (MSG_HEAD*)m_RecvBuffer;
    if(pHeader->usSize < sizeof(MSG_HEAD) || pHeader->usSize > GetPacketSizeMax())
    {
        LOGNETDEBUG("CNetWebSocket OnWsRecvData Msg:{} size:{} > MaxSize:{}, LastProcessCMD:{}",
                    pHeader->usCmd,
                    pHeader->usSize,
                    GetPacketSizeMax(),
                    m_nLastProcessMsgCMD);
        Close();
        return false;
    }

    if(pHeader->usSize > m_RecvLength)
    {
        // 包不足
        return false;
    }

    pBuffer = m_RecvBuffer;
    len     = pHeader->usSize;
    return true;
    __LEAVE_FUNCTION
    return false;
}

void CNetWebSocket::_ConsumeBuffer(size_t len)
{
    __ENTER_FUNCTION
    memcpy(m_RecvBuffer, m_RecvBuffer + len, len);
    m_RecvLength -= len;
    memset(m_RecvBuffer + m_RecvLength, 0, len);
    __LEAVE_FUNCTION
}
