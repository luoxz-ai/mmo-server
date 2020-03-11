#include "NetSocket.h"

#include <event2/bufferevent.h>
#include <event2/event.h>

#include "NetworkService.h"
#include "event2/buffer.h"
#include "event2/util.h"

MEMORYHEAP_IMPLEMENTATION(CNetSocket, s_Heap);

CNetSocket::CNetSocket(CNetworkService* pService, CNetEventHandler* pEventHandler, bool bPassive, bool bReconnect)
	: m_pService(pService)
	, m_pEventHandler(pEventHandler)
	, m_pBufferevent(nullptr)
	, m_pReconnectEvent(nullptr)
	, m_bPassive(bPassive)
	, m_Status(NSS_NOT_INIT)
	, m_nReconnectTimes(-1)
	, m_nRecvTimeOutSec(30)
	, m_bCreateByListener(true)
	, m_Sendbuf(evbuffer_new())
	, m_bReconnect(bReconnect)
	, m_nSocketIdx(0xFFFF)
	, m_pDecryptor(nullptr)
	, m_pEncryptor(nullptr)
	, m_nPacketSizeMax(_MAX_MSGSIZE)
	, m_socket(INVALID_SOCKET)
{
	m_ReadBuff = new byte[m_nPacketSizeMax];
}

CNetSocket::~CNetSocket()
{
	__ENTER_FUNCTION
	SAFE_DELETE(m_pDecryptor);
	SAFE_DELETE(m_pEncryptor);

	if(m_pBufferevent)
	{
		bufferevent_disable(m_pBufferevent, EV_READ | EV_WRITE);
		bufferevent_setcb(m_pBufferevent, nullptr, nullptr, nullptr, nullptr);
		bufferevent_free(m_pBufferevent);
		m_pBufferevent = nullptr;
	}

	if(m_pReconnectEvent)
	{
		event_free(m_pReconnectEvent);
		m_pReconnectEvent = nullptr;
	}

	if(m_Sendbuf)
	{
		evbuffer_free(m_Sendbuf);
		m_Sendbuf = nullptr;
	}
	SAFE_DELETE_ARRAY(m_ReadBuff);
	__LEAVE_FUNCTION
}

bool CNetSocket::Init(bufferevent* pBufferEvent)
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
	m_bCreateByListener = true;
	return true;
	__LEAVE_FUNCTION
	return false;
}

bool CNetSocket::InitWaitConnecting(bufferevent* pBufferEvent)
{
	__ENTER_FUNCTION
	m_pBufferevent = pBufferEvent;
	if(m_pBufferevent)
	{
		bufferevent_setcb(m_pBufferevent, NULL, NULL, _OnSocketConnectorEvent, (void*)this);
		bufferevent_enable(m_pBufferevent, EV_WRITE | EV_READ | EV_PERSIST);
		_SetTimeout();
	}
	SetStatus(NSS_CONNECTING);
	m_bCreateByListener = false;
	return true;
	__LEAVE_FUNCTION
	return false;
}

void CNetSocket::Close()
{
	__ENTER_FUNCTION

	if(GetStatus() == NSS_READY || GetStatus() == NSS_CONNECTING)
	{
		m_nReconnectTimes = 0;
		if(m_bCreateByListener)
		{
			bufferevent_disable(m_pBufferevent, EV_READ);
			bufferevent_setcb(m_pBufferevent, nullptr, _OnSendOK, _OnSocketEvent, this);

			MSG_HEAD msg;
			msg.usCmd  = COMMON_CMD_CLOSE;
			msg.usSize = sizeof(MSG_HEAD);
			SendMsg((byte*)&msg, sizeof(msg), true);
			SetStatus(NSS_CLOSEING);
		}
		else
		{
			SetStatus(NSS_CLOSEING);
			OnDisconnected();
		}
	}
	__LEAVE_FUNCTION
}

bool CNetSocket::SendMsg(byte* pBuffer, size_t len, bool bFlush)
{
	__ENTER_FUNCTION
	std::unique_lock<std::mutex> lock(m_mutexSend);
	return _SendMsg(pBuffer, len, bFlush);
	__LEAVE_FUNCTION
	return false;
}

bool CNetSocket::_SendMsg(byte* pBuffer, size_t len, bool bFlush)
{
	__ENTER_FUNCTION

	if(m_pBufferevent == nullptr)
		return false;

	if(m_pEncryptor)
	{
		m_pEncryptor->Encryptor(pBuffer + sizeof(MSG_HEAD), len - sizeof(MSG_HEAD), pBuffer + sizeof(MSG_HEAD), len - sizeof(MSG_HEAD));
	}

	if(GetStatus() == NSS_CONNECTING || GetStatus() == NSS_WAIT_RECONNECT)
	{
		// hold msg
		return evbuffer_add(m_Sendbuf, pBuffer, len) == 0;
	}
	else if(GetStatus() == NSS_READY)
	{
		m_pService->AddSendByteCount(len);

		int32_t nSucc = bufferevent_write(m_pBufferevent, pBuffer, len);
		if(bFlush)
			bufferevent_flush(m_pBufferevent, EV_WRITE, BEV_FLUSH);
		size_t nNeedWrite = evbuffer_get_length(bufferevent_get_output(m_pBufferevent));
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
		timeout_read.tv_sec	 = GetRecvTimeOutSec();
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

void CNetSocket::_OnClose(short what)
{
	__ENTER_FUNCTION
	if(GetStatus() == NSS_CLOSEING)
	{
		OnDisconnected();
		return;
	}
	SetStatus(NSS_CLOSEING);

	LOGNETDEBUG("CNetSocket _OnClose {}:{} err:{}", GetAddrString().c_str(), GetPort(), what);

	if(m_nReconnectTimes > 0)
	{
		m_pService->_RemoveSocket(this);
		if(m_pBufferevent)
		{
			bufferevent_setfd(m_pBufferevent, INVALID_SOCKET);
			auto pOutBuf = bufferevent_get_output(m_pBufferevent);
			evbuffer_drain(pOutBuf, evbuffer_get_length(pOutBuf));
		}
		if(GetSocket() != INVALID_SOCKET)
			evutil_closesocket(m_socket);
		SetSocket(INVALID_SOCKET);

		m_nReconnectTimes--;
		if(m_pReconnectEvent == nullptr)
			m_pReconnectEvent = event_new(m_pService->GetEVBase(), -1, 0, _OnReconnect, this);
		struct timeval reconnet_dealy = {5, 0};
		event_add(m_pReconnectEvent, &reconnet_dealy);
		SetStatus(NSS_WAIT_RECONNECT);
		m_pService->_AddConnectingSocket(this);

		LOGNETDEBUG("CNetSocket _OnClose Reconnect Wait,5s, {}:{}", GetAddrString().c_str(), GetPort());
	}
	else
	{
		if(m_pBufferevent)
		{
			bufferevent_disable(m_pBufferevent, EV_READ | EV_PERSIST);
		}

		OnDisconnected();
	}
	__LEAVE_FUNCTION
}

void CNetSocket::_OnReconnect(int32_t fd, short what, void* ctx)
{
	__ENTER_FUNCTION

	CNetSocket* pSocket = (CNetSocket*)ctx;
	LOGNETDEBUG("CNetSocket Reconnect:{}::{}", pSocket->GetAddrString().c_str(), pSocket->GetPort());

	if(pSocket->GetService()->_AsyncReconnect(pSocket) == false)
	{
		pSocket->OnConnectFailed();
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
		if(pHeader->usSize < sizeof(MSG_HEAD) || pHeader->usSize > GetPacketSizeMax())
		{
			LOGNETDEBUG("CNetSocket _OnReceive Msg:{} size:{} > MaxSize:{}, LastProcessCMD:{}", pHeader->usCmd, pHeader->usSize, GetPacketSizeMax(),
						m_nLastProcessMsgCMD);
			std::string data;
			for(size_t i = 0; i < m_nLastCMDSize; i++)
			{
				unsigned char v = m_ReadBuff[i];
				data += fmt::format(FMT_STRING("{0:x} "), v);
			}
			LOGNETDEBUG("{}", data.c_str());
			_OnClose(0);
			return;
		}
		if(nSize < pHeader->usSize)
			return;

		evbuffer_remove(input, m_ReadBuff, pHeader->usSize);
		pHeader = (MSG_HEAD*)m_ReadBuff;

		if(m_pDecryptor)
		{
			byte*  pBody	= m_ReadBuff + sizeof(MSG_HEAD);
			size_t nBodyLen = pHeader->usSize - sizeof(MSG_HEAD);
			m_pDecryptor->Decryptor(pBody, nBodyLen, pBody, nBodyLen);
		}

		m_nLastProcessMsgCMD = pHeader->usCmd;
		m_nLastCMDSize		 = pHeader->usSize;
		OnRecvData(m_ReadBuff, pHeader->usSize);
	}
	__LEAVE_FUNCTION
}

void CNetSocket::_OnSocketRead(bufferevent* b, void* ctx)
{
	__ENTER_FUNCTION

	CNetSocket* pSocket = (CNetSocket*)ctx;
	pSocket->_OnReceive(b);
	__LEAVE_FUNCTION
}

void CNetSocket::_OnSendOK(bufferevent* b, void* ctx)
{
	__ENTER_FUNCTION
	CNetSocket* pSocket = (CNetSocket*)ctx;
	if(pSocket->GetStatus() == NSS_CLOSEING && evbuffer_get_length(bufferevent_get_output(b)) == 0)
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
	bool		bClose	= false;
	if(what & BEV_EVENT_TIMEOUT)
	{
		if(what & BEV_EVENT_READING)
		{
			MSG_HEAD msg;
			msg.usCmd  = COMMON_CMD_PING;
			msg.usSize = sizeof(MSG_HEAD);
			pSocket->SendMsg(&msg);
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
		int32_t			err	   = evutil_socket_geterror(bufferevent_getfd(b));
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

void CNetSocket::_OnSocketConnectorEvent(bufferevent* b, short what, void* ctx)
{
	__ENTER_FUNCTION
	CNetSocket* pSocket = (CNetSocket*)ctx;

	if(what == BEV_EVENT_CONNECTED)
	{
		int32_t fd = bufferevent_getfd(b);
		evutil_make_socket_nonblocking(fd);
		pSocket->SetSocket(fd);
		pSocket->GetService()->_AddSocket(pSocket);
		bufferevent_setcb(b, _OnSocketRead, nullptr, _OnSocketEvent, ctx);
		pSocket->SetStatus(NSS_READY);
		pSocket->OnConnected();
		bufferevent_enable(b, EV_READ | EV_WRITE | EV_PERSIST);
		pSocket->_SetTimeout();
		bufferevent_write_buffer(b, pSocket->m_Sendbuf);

		LOGNETDEBUG("CNetSocket::SocketConnectSucc:{}:{}", pSocket->GetAddrString().c_str(), pSocket->GetPort());
	}
	else
	{
		int32_t			err	   = evutil_socket_geterror(bufferevent_getfd(b));
		const char* errstr = evutil_socket_error_to_string(err);
		LOGNETDEBUG("CNetSocket::SocketConnectFail:{}:{} {}", pSocket->GetAddrString().c_str(), pSocket->GetPort(), errstr);

		pSocket->OnConnectFailed();
	}
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
	return evbuffer_get_length(bufferevent_get_output(GetBufferevent()));
}

void CNetSocket::SetPacketSizeMax(size_t val)
{
	if(m_nPacketSizeMax != val)
	{
		m_nPacketSizeMax = val;
		SAFE_DELETE_ARRAY(m_ReadBuff);
		m_ReadBuff = new byte[val];
	}
}

void CNetSocket::OnConnected()
{
	__ENTER_FUNCTION
	m_bCreateByListener = false;
	if(m_pEventHandler)
		m_pEventHandler->OnConnected(this);
	__LEAVE_FUNCTION
}

void CNetSocket::OnConnectFailed()
{
	__ENTER_FUNCTION
	if(m_pEventHandler)
		m_pEventHandler->OnConnectFailed(this);

	_OnClose(BEV_EVENT_ERROR);
	__LEAVE_FUNCTION
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

void CNetSocket::OnAccepted()
{
	__ENTER_FUNCTION
	if(m_pEventHandler)
		m_pEventHandler->OnAccepted(this);
	__LEAVE_FUNCTION
}

void CNetSocket::OnRecvData(byte* pBuffer, size_t len)
{
	__ENTER_FUNCTION
	m_pService->AddRecvByteCount(len);

	MSG_HEAD* pHeader = (MSG_HEAD*)pBuffer;
	switch(pHeader->usCmd)
	{
		case COMMON_CMD_CLOSE:
		{
			LOGNETDEBUG("COMMON_CMD_CLOSE:{}:{}", GetAddrString().c_str(), GetPort());
			SetReconnectTimes(0);
			_OnClose(COMMON_CMD_CLOSE);
			return;
		}
		break;
		case COMMON_CMD_PING:
		{
			MSG_HEAD msg;
			msg.usCmd  = COMMON_CMD_PONG;
			msg.usSize = sizeof(MSG_HEAD);
			SendMsg((byte*)&msg, sizeof(msg));
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

	bReconnect = (!m_bPassive && m_bReconnect);
	__LEAVE_FUNCTION
}
