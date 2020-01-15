#include "NetworkService.h"

#include <iostream>

#include "EventManager.h"
#include "NetSocket.h"
#include "NetWebSocket.h"
#include "event2/buffer.h"
#include "event2/bufferevent.h"
#include "event2/event.h"
#include "event2/http.h"
#include "event2/listener.h"
#include "event2/thread.h"
#include "event2/util.h"
#include "libwebsockets.h"

CNetworkService::CNetworkService()
	: m_pBase(event_base_new())
	, m_bStop(false)
	, m_nWorkInterval(0)
	, m_pHttpHandle(nullptr)
{
	m_SocketIdxPool.resize(0x8000, 0);
	for(size_t i = 0; i < m_SocketIdxPool.size(); i++)
	{
		m_SocketIdxPool[i] = i + 1;
	}

	m_WebSocketIdxPool.resize(0x7fff, 0);
	for(size_t i = 0; i < m_WebSocketIdxPool.size(); i++)
	{
		m_WebSocketIdxPool[i] = i + 1 + 0x8000;
	}
}

CNetworkService::~CNetworkService()
{
	Destroy();
}

void CNetworkService::Destroy()
{
	__ENTER_FUNCTION
	if(m_bStop == true)
		return;

	Stop();
	m_bStop = true;

	JoinIOThread();
	if(m_pIOTimeOutEvent)
	{
		event_del(m_pIOTimeOutEvent);
		event_free(m_pIOTimeOutEvent);
		m_pIOTimeOutEvent = nullptr;
	}

	for(auto& v: m_setListener)
	{
		evconnlistener_free(v.first);
	}
	m_setListener.clear();
	for(auto& v: m_setEvTimed)
	{
		event_free(v);
	}
	m_setEvTimed.clear();

	for(auto& v: m_setSocket)
	{
		SAFE_DELETE(v.second);
	}
	m_setSocket.clear();

	for(auto it = m_setCloseingSocket.begin(); it != m_setCloseingSocket.end();)
	{
		auto v = *it;
		SAFE_DELETE(v);
		it = m_setCloseingSocket.erase(it);
	}

	if(m_pHttpHandle != nullptr)
	{
		evhttp_free(m_pHttpHandle);
		m_pHttpHandle = nullptr;
	}

	if(m_pBase)
	{
		event_base_free(m_pBase);
		m_pBase = nullptr;
	}

	CNetworkMessage* pMsg;
	while(m_MessageQueue.get(pMsg))
	{
		SAFE_DELETE(pMsg);
	}
	__LEAVE_FUNCTION
}

bool CNetworkService::EnableListener(evconnlistener* listener, bool bEnable)
{
	__ENTER_FUNCTION
	if(listener)
	{
		if(bEnable)
			return evconnlistener_enable(listener) == 0;
		else
			return evconnlistener_disable(listener) == 0;
	}
	else
	{
		for(const auto& pair_val: m_setListener)
		{
			if(bEnable)
				evconnlistener_enable(pair_val.first);
			else
				evconnlistener_disable(pair_val.first);
		}
		return true;
	}
	__LEAVE_FUNCTION
	return false;
}

bool CNetworkService::ListenHttpPort(const char* addr, int port, std::function<void(struct evhttp_request* req)> func)
{
	__ENTER_FUNCTION
	if(m_pHttpHandle != nullptr)
		evhttp_free(m_pHttpHandle);

	m_pHttpHandle = evhttp_new(m_pBase);
	if(m_pHttpHandle == nullptr)
		return false;

	int ret = evhttp_bind_socket(m_pHttpHandle, addr, port);
	if(ret != 0)
	{
		return false;
	}

	m_funcOnReciveHttp = func;
	evhttp_set_gencb(m_pHttpHandle, http_process_cb, this);
	return true;
	__LEAVE_FUNCTION
	return false;
}

void CNetworkService::http_process_cb(struct evhttp_request* req, void* arg)
{
	__ENTER_FUNCTION
	CNetworkService* pThis = (CNetworkService*)arg;
	pThis->m_funcOnReciveHttp(req);
	__LEAVE_FUNCTION
}

evconnlistener* CNetworkService::Listen(const char* addr, int port, CNetEventHandler* pEventHandler)
{
	__ENTER_FUNCTION
	struct evutil_addrinfo	hits;
	struct evutil_addrinfo* answer = nullptr;
	memset(&hits, 0, sizeof(hits));
	hits.ai_family	 = AF_UNSPEC;
	hits.ai_socktype = SOCK_STREAM;
	hits.ai_protocol = IPPROTO_TCP;
	hits.ai_flags	 = EVUTIL_AI_ADDRCONFIG;

	if(evutil_getaddrinfo(addr, std::to_string(port).c_str(), &hits, &answer) != 0)
	{
		LOGERROR("CNetworkService::Listen:{}:{} evutil_getaddrinfo fail", addr, port);
		return nullptr;
	}
	std::unique_ptr<addrinfo, decltype(evutil_freeaddrinfo)*> answer_ptr(answer, evutil_freeaddrinfo);

	evconnlistener* pListener = evconnlistener_new_bind(m_pBase, accept_conn_cb, this, LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE | LEV_OPT_THREADSAFE, -1,
														answer_ptr->ai_addr, answer_ptr->ai_addrlen);
	if(pListener == nullptr)
	{
		LOGERROR("CNetworkService::Listen:{}:{} evconnlistener_new_bind fail:{}", addr, port, strerror(errno));
		return nullptr;
	}
	evconnlistener_set_error_cb(pListener, accept_error_cb);

	m_setListener[pListener] = pEventHandler;
	LOGNETDEBUG("CNetworkService::Listen:{}:{}", addr, port);

	return pListener;
	__LEAVE_FUNCTION
	return nullptr;
}

CNetSocket* CNetworkService::ConnectTo(const char* addr, int port, CNetEventHandler* pEventHandler)
{
	__ENTER_FUNCTION
	struct evutil_addrinfo	hits;
	struct evutil_addrinfo* answer = nullptr;
	memset(&hits, 0, sizeof(hits));
	hits.ai_family	 = AF_UNSPEC;
	hits.ai_socktype = SOCK_STREAM;
	hits.ai_protocol = IPPROTO_TCP;
	hits.ai_flags	 = EVUTIL_AI_ADDRCONFIG;

	if(evutil_getaddrinfo(addr, std::to_string(port).c_str(), &hits, &answer) != 0)
	{
		LOGERROR("CNetworkService::ConnectTo:{}:{} evutil_getaddrinfo fail", addr, port);
		return nullptr;
	}
	std::unique_ptr<addrinfo, decltype(evutil_freeaddrinfo)*> answer_ptr(answer, evutil_freeaddrinfo);

	int fd = socket(answer_ptr->ai_family, answer_ptr->ai_socktype, answer_ptr->ai_protocol);
	if(fd < 0)
	{
		LOGERROR("CNetworkService::ConnectTo:{}:{} socket fail", addr, port);
		return nullptr;
	}
	if(connect(fd, answer_ptr->ai_addr, answer_ptr->ai_addrlen))
	{
		LOGERROR("CNetworkService::ConnectTo:{}:{} connect fail", addr, port);
		evutil_closesocket(fd);
		return nullptr;
	}

	bufferevent* pBufferEvent = bufferevent_socket_new(m_pBase, fd, BEV_OPT_CLOSE_ON_FREE | BEV_OPT_THREADSAFE);
	if(pBufferEvent == nullptr)
	{
		LOGERROR("CNetworkService::ConnectTo:{}:{} bufferevent_socket_new fail", addr, port);
		return nullptr;
	}

	CNetSocket* pSocket = CreateSocket(pEventHandler, false);
	pSocket->SetAddrAndPort(addr, port);
	pSocket->SetSocket(fd);
	pSocket->Init(pBufferEvent);
	_AddSocket(pSocket);
	pSocket->OnConnected();
	LOGNETDEBUG("CNetworkService::ConnectTo:{}:{}", addr, port);

	return pSocket;
	__LEAVE_FUNCTION
	return nullptr;
}

bool CNetworkService::_Reconnect(CNetSocket* pSocket)
{
	__ENTER_FUNCTION
	struct evutil_addrinfo	hits;
	struct evutil_addrinfo* answer = nullptr;
	memset(&hits, 0, sizeof(hits));
	hits.ai_family	 = AF_UNSPEC;
	hits.ai_socktype = SOCK_STREAM;
	hits.ai_protocol = IPPROTO_TCP;
	hits.ai_flags	 = EVUTIL_AI_ADDRCONFIG;

	if(evutil_getaddrinfo(pSocket->GetAddrString().c_str(), std::to_string(pSocket->GetPort()).c_str(), &hits, &answer) != 0)
	{
		LOGERROR("CNetworkService::_Reconnect:{}:{} evutil_getaddrinfo fail", pSocket->GetAddrString().c_str(), pSocket->GetPort());
		return false;
	}
	std::unique_ptr<addrinfo, decltype(evutil_freeaddrinfo)*> answer_ptr(answer, evutil_freeaddrinfo);

	int fd = socket(answer_ptr->ai_family, answer_ptr->ai_socktype, answer_ptr->ai_protocol);
	if(fd < 0)
	{
		LOGERROR("CNetworkService::_Reconnect:{}:{} socket fail", pSocket->GetAddrString().c_str(), pSocket->GetPort());

		return false;
	}
	if(connect(fd, answer_ptr->ai_addr, answer_ptr->ai_addrlen))
	{
		LOGERROR("CNetworkService::_Reconnect:{}:{} connect fail", pSocket->GetAddrString().c_str(), pSocket->GetPort());

		evutil_closesocket(fd);
		return false;
	}

	pSocket->SetSocket(fd);
	bufferevent_setfd(pSocket->GetBufferevent(), fd);
	pSocket->Init(pSocket->GetBufferevent());
	pSocket->OnConnected();
	LOGNETDEBUG("CNetworkService::_Reconnect:{}:{}", pSocket->GetAddrString().c_str(), pSocket->GetPort());

	return true;
	__LEAVE_FUNCTION
	return false;
}

CNetSocket* CNetworkService::AsyncConnectTo(const char* addr, int port, CNetEventHandler* pEventHandler)
{
	__ENTER_FUNCTION
	struct evutil_addrinfo	hits;
	struct evutil_addrinfo* answer = nullptr;
	memset(&hits, 0, sizeof(hits));
	hits.ai_family	 = AF_UNSPEC;
	hits.ai_socktype = SOCK_STREAM;
	hits.ai_protocol = IPPROTO_TCP;
	hits.ai_flags	 = EVUTIL_AI_ADDRCONFIG;

	if(evutil_getaddrinfo(addr, std::to_string(port).c_str(), &hits, &answer) != 0)
	{
		LOGERROR("CNetworkService::AsyncConnectTo:{}:{} evutil_getaddrinfo fail", addr, port);

		return nullptr;
	}
	std::unique_ptr<addrinfo, decltype(evutil_freeaddrinfo)*> answer_ptr(answer, evutil_freeaddrinfo);

	bufferevent* pBufferEvent = bufferevent_socket_new(m_pBase, -1, BEV_OPT_CLOSE_ON_FREE | BEV_OPT_THREADSAFE);
	if(pBufferEvent == nullptr)
	{
		LOGERROR("CNetworkService::AsyncConnectTo:{}:{} bufferevent_socket_new fail", addr, port);

		return nullptr;
	}

	CNetSocket* pSocket = CreateSocket(pEventHandler, false);
	pSocket->SetAddrAndPort(addr, port);
	pSocket->InitWaitConnecting(pBufferEvent);

	if(bufferevent_socket_connect(pBufferEvent, answer_ptr->ai_addr, answer_ptr->ai_addrlen) != 0)
	{
		LOGERROR("CNetworkService::AsyncConnectTo:{}:{} bufferevent_socket_connect fail", addr, port);
		delete pSocket;
		bufferevent_free(pBufferEvent);
		return nullptr;
	}

	_AddConnectingSocket(pSocket);
	LOGNETDEBUG("CNetworkService::AsyncConnectTo:{}:{}", addr, port);

	return pSocket;
	__LEAVE_FUNCTION
	return nullptr;
}

bool CNetworkService::_AsyncReconnect(CNetSocket* pSocket)
{
	__ENTER_FUNCTION
	struct evutil_addrinfo	hits;
	struct evutil_addrinfo* answer = nullptr;
	memset(&hits, 0, sizeof(hits));
	hits.ai_family	 = AF_UNSPEC;
	hits.ai_socktype = SOCK_STREAM;
	hits.ai_protocol = IPPROTO_TCP;
	hits.ai_flags	 = EVUTIL_AI_ADDRCONFIG;

	if(evutil_getaddrinfo(pSocket->GetAddrString().c_str(), std::to_string(pSocket->GetPort()).c_str(), &hits, &answer) != 0)
	{
		LOGERROR("CNetworkService::_AsyncReconnect:{}:{} evutil_getaddrinfo fail", pSocket->GetAddrString().c_str(), pSocket->GetPort());

		return false;
	}
	std::unique_ptr<addrinfo, decltype(evutil_freeaddrinfo)*> answer_ptr(answer, evutil_freeaddrinfo);
	pSocket->InitWaitConnecting(pSocket->GetBufferevent());
	if(bufferevent_socket_connect(pSocket->GetBufferevent(), answer_ptr->ai_addr, answer_ptr->ai_addrlen) != 0)
	{
		int			err	   = EVUTIL_SOCKET_ERROR();
		const char* errstr = evutil_socket_error_to_string(err);

		LOGERROR("CNetworkService::_AsyncReconnect:{}:{} bufferevent_socket_connect fail:{}", pSocket->GetAddrString().c_str(), pSocket->GetPort(), errstr);
		return false;
	}
	LOGNETDEBUG("CNetworkService::_AsyncReconnect:{}:{}", pSocket->GetAddrString().c_str(), pSocket->GetPort());

	return true;
	__LEAVE_FUNCTION
	return false;
}

void CNetworkService::Stop()
{
	__ENTER_FUNCTION
	event_base_loopbreak(m_pBase);
	__LEAVE_FUNCTION
}

static void _IOThreadTimeOut(int, short, void* ctx)
{
	__ENTER_FUNCTION
	CNetworkService* pThis = (CNetworkService*)ctx;
	pThis->OnIOThreadTimeOut();
	__LEAVE_FUNCTION
}

void CNetworkService::OnIOThreadTimeOut()
{
	__ENTER_FUNCTION
	if(m_IOThreadTimeOutFunc)
		m_IOThreadTimeOutFunc();
	__LEAVE_FUNCTION
}

void CNetworkService::StartIOThread(const std::string& thread_name, std::function<void()> time_out_func, uint32_t time_out_ms, uint16_t idService)
{
	__ENTER_FUNCTION
	if(m_pIOThread)
	{
		return;
	}
	m_IOThreadTimeOutFunc = std::move(time_out_func);
	m_pIOTimeOutEvent	  = event_new(m_pBase, -1, EV_PERSIST, _IOThreadTimeOut, this);
	struct timeval tv
	{
		time_out_ms / 1000, time_out_ms % 1000
	};
	event_add(m_pIOTimeOutEvent, &tv);
	m_pIOThread = std::unique_ptr<std::thread>(new std::thread([pBase = m_pBase, _thread_name = thread_name, idService]() {
		__ENTER_FUNCTION
		pthread_setname_np(pthread_self(), _thread_name.c_str());

		LOGMESSAGE("ThreadID:{}", get_cur_thread_id());
		int result = 0;
		do
		{
			result = event_base_dispatch(pBase);
			if(event_base_got_break(pBase) || event_base_got_exit(pBase))
				break;
		} while(result != -1);
		if(result == -1)
		{
			// showerror

			LOGERROR("CNetworkService {} IOThread Close with ERROR:", idService);
		}
		LOGNETDEBUG("CNetworkService IOThread Close:{}", idService);
		__LEAVE_FUNCTION
	}));

	__LEAVE_FUNCTION
}

void CNetworkService::RunOnce()
{
	__ENTER_FUNCTION
	if(m_pIOThread)
	{
		return;
	}

	event_base_loop(m_pBase, EVLOOP_ONCE | EVLOOP_NONBLOCK);

	if(m_pLwsContext)
	{
		/*0 means return immediately if nothing needed
		service otherwise block and service immediately, returning
		after the timeout if nothing needed service.*/
		lws_service(m_pLwsContext, 0);
	}

	_ProceseCloseingSocket();
	__LEAVE_FUNCTION
}

CNetSocket* CNetworkService::CreateSocket(CNetEventHandler* pHandle, bool bPassive)
{
	__ENTER_FUNCTION
	return new CNetSocket(this, pHandle, bPassive);
	__LEAVE_FUNCTION
	return nullptr;
}

void CNetworkService::accept_conn_cb(evconnlistener* listener, int fd, sockaddr* addr, int socklen, void* arg)
{
	__ENTER_FUNCTION((CNetworkService*)arg)->OnAccept(fd, addr, socklen, listener);
	__LEAVE_FUNCTION
}

void CNetworkService::accept_error_cb(struct evconnlistener* listener, void* arg)
{
	__ENTER_FUNCTION
	struct event_base* base	  = evconnlistener_get_base(listener);
	int				   err	  = EVUTIL_SOCKET_ERROR();
	const char*		   errstr = evutil_socket_error_to_string(err);

	// log error
	LOGERROR("CNetworkService::accept_error_cb {}", errstr);
	__LEAVE_FUNCTION
}

void CNetworkService::OnAccept(int fd, sockaddr* addr, int, evconnlistener* listener)
{
	__ENTER_FUNCTION
	char		   szHost[512];
	unsigned short uPort = 0;

	if(addr)
	{
		sockaddr_in* addr_in = (sockaddr_in*)addr;
		evutil_inet_ntop(addr->sa_family, &addr_in->sin_addr, szHost, 512);
		uPort = ntohs(addr_in->sin_port);
	}

	if(m_bIPCheck)
	{
		auto&  refData = m_setIP[szHost];
		time_t now	   = TimeGetSecond();
		if(now > refData.nextResetTime)
		{
			refData.nextResetTime = now + 60;
			refData.nCount		  = 1;
		}
		else
		{
			refData.nCount++;
		}

		if(refData.nCount > m_nIPCheckNum)
		{
			evutil_closesocket(fd);
			return;
		}
	}
	bufferevent* pBufferEvent = bufferevent_socket_new(GetEVBase(), fd, BEV_OPT_CLOSE_ON_FREE | BEV_OPT_THREADSAFE);
	if(pBufferEvent == nullptr)
	{
		LOGERROR("CNetworkService::OnAccept bufferevent_socket_new fail:{}:{} ", szHost, uPort);
		evutil_closesocket(fd);
		return;
	}
	CNetEventHandler* pEventHandler = m_setListener[listener];
	CNetSocket*		  pSocket		= CreateSocket(pEventHandler, true);
	pSocket->SetAddrAndPort(szHost, uPort);
	pSocket->SetSocket(fd);

	pSocket->Init(pBufferEvent);
	pSocket->SetReconnectTimes(0);
	_AddSocket(pSocket);
	pSocket->OnAccepted();

	LOGNETDEBUG("CNetworkService::OnAccept:{}:{}", szHost, uPort);
	__LEAVE_FUNCTION
}

void CNetworkService::_AddSocket(CNetSocket* pSocket)
{
	__ENTER_FUNCTION
	CHECK(pSocket);
	CHECK(pSocket->GetSocket() != INVALID_SOCKET);
	std::lock_guard<std::mutex> lock(m_mutex);
	m_setConnectingSocket.erase(pSocket);
	m_setSocket[pSocket->GetSocket()] = pSocket;
	pSocket->SetSocketIdx(m_SocketIdxPool.front());
	m_setSocketByIdx[pSocket->GetSocketIdx()] = pSocket;
	m_SocketIdxPool.pop_front();

	__LEAVE_FUNCTION
}

void CNetworkService::_AddConnectingSocket(CNetSocket* pSocket)
{
	__ENTER_FUNCTION
	CHECK(pSocket);
	CHECK(pSocket->GetSocket() == INVALID_SOCKET);
	std::lock_guard<std::mutex> lock(m_mutex);
	m_setSocket.erase(pSocket->GetSocket());
	m_setConnectingSocket.insert(pSocket);
	__LEAVE_FUNCTION
}

void CNetworkService::_RemoveSocket(CNetSocket* pSocket)
{
	__ENTER_FUNCTION
	CHECK(pSocket);
	std::lock_guard<std::mutex> lock(m_mutex);
	m_setConnectingSocket.erase(pSocket);
	if(pSocket->GetSocket() != INVALID_SOCKET)
	{
		m_setSocket.erase(pSocket->GetSocket());
		m_setSocketByIdx[pSocket->GetSocketIdx()] = nullptr;
	}
	m_SocketIdxPool.push_back(pSocket->GetSocketIdx());
	__LEAVE_FUNCTION
}

void CNetworkService::_AddCloseingSocket(CNetSocket* pSocket)
{
	__ENTER_FUNCTION
	CHECK(pSocket);
	std::lock_guard<std::mutex> lock(m_mutex);
	m_setCloseingSocket.insert(pSocket);
	__LEAVE_FUNCTION
}

void CNetworkService::_ProceseCloseingSocket()
{
	__ENTER_FUNCTION
	std::lock_guard<std::mutex> lock(m_mutex);

	for(auto it = m_setCloseingSocket.begin(); it != m_setCloseingSocket.end();)
	{
		auto v = *it;
		SAFE_DELETE(v);
		it = m_setCloseingSocket.erase(it);
	}
	__LEAVE_FUNCTION
}

void CNetworkService::JoinIOThread()
{
	__ENTER_FUNCTION
	if(m_pIOThread)
	{
		m_pIOThread->join();
		m_pIOThread.reset();
	}
	__LEAVE_FUNCTION
}

void CNetworkService::_CloseSocket(uint32_t nSocketIdx)
{
	__ENTER_FUNCTION
	std::unique_lock<std::mutex> lock(m_mutex);
	auto						 pSocket = m_setSocketByIdx[nSocketIdx];
	if(pSocket)
	{
		lock.unlock();
		pSocket->Close();
	}

	__LEAVE_FUNCTION
}

size_t CNetworkService::GetSocketAmount()
{
	__ENTER_FUNCTION
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_setSocket.size();
	__LEAVE_FUNCTION
	return 0;
}

void CNetworkService::BrocastMsg(byte* buf, size_t len, SOCKET execpt_this)
{
	__ENTER_FUNCTION
	CHECK(buf != nullptr)
	CHECK(len != 0);
	std::lock_guard<std::mutex> lock(m_mutex);

	for(auto it = m_setSocket.begin(); it != m_setSocket.end(); it++)
	{
		if(execpt_this != 0 && it->first == execpt_this)
			continue;

		it->second->SendMsg(buf, len);
	}
	__LEAVE_FUNCTION
}

bool CNetworkService::KickSocket(SOCKET _socket)
{
	__ENTER_FUNCTION
	CNetSocket* pSocket = nullptr;
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		auto						it = m_setSocket.find(_socket);
		if(it != m_setSocket.end())
		{
			pSocket = it->second;
		}
	}
	// must do it out of lock,because it will remove self from m_setSocket;
	if(pSocket)
	{
		pSocket->Close();
		return true;
	}
	else
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_setSocket.erase(_socket);
		return false;
	}
	__LEAVE_FUNCTION
	return false;
}

bool CNetworkService::SendSocketMsg(SOCKET _socket, byte* buf, size_t len)
{
	__ENTER_FUNCTION
	std::lock_guard<std::mutex> lock(m_mutex);
	auto						it = m_setSocket.find(_socket);
	if(it != m_setSocket.end())
	{
		return it->second->SendMsg(buf, len);
	}
	__LEAVE_FUNCTION
	return false;
}

bool CNetworkService::SendSocketMsgByIdx(uint16_t nSocketIdx, byte* buf, size_t len)
{
	__ENTER_FUNCTION
	std::lock_guard<std::mutex> lock(m_mutex);
	auto						pSocket = m_setSocketByIdx[nSocketIdx];
	if(pSocket)
	{
		return pSocket->SendMsg(buf, len);
	}
	__LEAVE_FUNCTION
	return false;
}

void CNetworkService::AddRecvByteCount(size_t len)
{
	__ENTER_FUNCTION
	m_RecvBPS.AddCount(len);
	__LEAVE_FUNCTION
}

void CNetworkService::AddSendByteCount(size_t len)
{
	__ENTER_FUNCTION
	m_SendBPS.AddCount(len);
	__LEAVE_FUNCTION
}

// lws为每条连接内部分配的user_space, 用于用户自定义数据
struct lws_session_data
{
	CNetWebSocket* pWebSocket; //连接绑定的CNetWebSocket对象
};

static struct lws_protocols server_protocols[] = {
	{
		"websocketserver",					  // name
		CNetworkService::OnWebSocketCallback, // callback
		sizeof(struct lws_session_data),	  // per_session_data_size
		65535,								  // rx_buffer_size
		0,									  // id
		NULL,								  // user
		0									  // tx_packet_size
	},
	{NULL, NULL, 0, 0} /* terminator */
};

bool CNetworkService::ListenWebSocket(int port, CWebSocketEventHandler* pEventHandler)
{
	__ENTER_FUNCTION
	if(m_pWebSocketEventHandler != nullptr)
	{
		return false;
	}

	struct lws_context_creation_info info;
	memset(&info, 0, sizeof(info));
	info.port	   = port;
	info.protocols = server_protocols;
	info.user	   = this;
	m_pLwsContext  = lws_create_context(&info);
	if(m_pLwsContext == nullptr)
	{
		// error output
		return false;
	}

	m_pWebSocketEventHandler = pEventHandler;
	return true;
	__LEAVE_FUNCTION
	return false;
}

void CNetworkService::StopWebSocket()
{
	__ENTER_FUNCTION
	if(m_pLwsContext)
	{
		lws_context_destroy(m_pLwsContext);
		m_pLwsContext = nullptr;
	}
	__LEAVE_FUNCTION
}

CNetWebSocket* CNetworkService::CreateWebSocket()
{
	__ENTER_FUNCTION
	uint16_t index = _GetWebSocketIndex();
	if(index == 0)
	{
		// index 已分配完
		return nullptr;
	}

	CNetWebSocket* pWebSocket = new CNetWebSocket(this, m_pWebSocketEventHandler);
	pWebSocket->SetSocketIdx(index);
	m_setWebSocketByIdx[index - 0x8000] = pWebSocket;

	return pWebSocket;
	__LEAVE_FUNCTION
	return nullptr;
}

void CNetworkService::DestroyWebSocket(CNetWebSocket* pWebSocket)
{
	__ENTER_FUNCTION
	if(pWebSocket)
	{
		_PushWebSocketIndexBack(pWebSocket->GetSocketIdx());
		m_setWebSocketByIdx[pWebSocket->GetSocketIdx() - 0x8000] = nullptr;
		SAFE_DELETE(pWebSocket);
	}
	__LEAVE_FUNCTION
}

void CNetworkService::AddWebSocket(SOCKET sockfd, CNetWebSocket* pWebSocket)
{
	__ENTER_FUNCTION
	if(sockfd != INVALID_SOCKET && pWebSocket != nullptr)
	{
		m_mapWebSockets.insert(std::make_pair(sockfd, pWebSocket));
	}
	__LEAVE_FUNCTION
}

void CNetworkService::RemoveWebSocket(SOCKET sockfd)
{
	__ENTER_FUNCTION
	auto iter = m_mapWebSockets.find(sockfd);
	if(iter != m_mapWebSockets.end())
	{
		m_mapWebSockets.erase(iter);
	}
	__LEAVE_FUNCTION
}

void CNetworkService::KickWebSocket(SOCKET sockfd)
{
	__ENTER_FUNCTION
	CNetWebSocket* pWebSocket = nullptr;
	auto		   iter		  = m_mapWebSockets.find(sockfd);
	if(iter != m_mapWebSockets.end())
	{
		pWebSocket = iter->second;
	}

	if(pWebSocket)
	{
		pWebSocket->Close();
		RemoveWebSocket(sockfd);
		DestroyWebSocket(pWebSocket);
	}
	__LEAVE_FUNCTION
}

uint16_t CNetworkService::_GetWebSocketIndex()
{
	if(m_WebSocketIdxPool.size() > 0)
	{
		uint16_t index = m_WebSocketIdxPool.front();
		m_WebSocketIdxPool.pop_front();
		return index;
	}
	return 0;
}

void CNetworkService::_PushWebSocketIndexBack(uint16_t index)
{
	m_WebSocketIdxPool.push_back(index);
}

CNetWebSocket* CNetworkService::_GetWebSocketFromLWS(struct lws* wsi)
{
	void* pUserSpace = lws_wsi_user(wsi);
	if(pUserSpace)
	{
		return ((lws_session_data*)pUserSpace)->pWebSocket;
	}
	return nullptr;
}

void CNetworkService::_SetWebSocketToLWS(struct lws* wsi, CNetWebSocket* pWebSocket)
{
	void* pUserSpace = lws_wsi_user(wsi);
	if(pUserSpace)
	{
		lws_session_data* pSessionData = (lws_session_data*)pUserSpace;
		pSessionData->pWebSocket	   = pWebSocket;
	}
}

void CNetworkService::StartWebSocketIOThread() {}

int CNetworkService::OnWebSocketCallback(struct lws* wsi, enum lws_callback_reasons reason, void* user, void* in, size_t len)
{
	__ENTER_FUNCTION

	struct lws_vhost* vhost = lws_get_vhost(wsi);
	if(vhost == nullptr)
	{
		return 0;
	}

	CNetworkService* pNetworkService = (CNetworkService*)lws_get_vhost_user(vhost);
	;
	if(pNetworkService == nullptr)
	{
		return 0;
	}

	switch(reason)
	{
		case LWS_CALLBACK_PROTOCOL_INIT:
		{
			break;
		}
		case LWS_CALLBACK_ESTABLISHED:
		{
			CNetWebSocket* pWebSocket = pNetworkService->CreateWebSocket();
			if(pWebSocket == nullptr)
			{
				return -1;
			}

			pWebSocket->Init(wsi);
			pWebSocket->OnWsAccepted();
			pNetworkService->AddWebSocket(pWebSocket->GetSocket(), pWebSocket);

			_SetWebSocketToLWS(wsi, pWebSocket);
			lws_callback_on_writable(wsi);
			break;
		}
		case LWS_CALLBACK_SERVER_WRITEABLE:
		{
			CNetWebSocket* pWebSocket = _GetWebSocketFromLWS(wsi);
			if(pWebSocket)
			{
				// pWebSocket->RealSend();
				pWebSocket->SendTestData();
			}
			lws_callback_on_writable(wsi);
			break;
		}
		case LWS_CALLBACK_RECEIVE:
		{
			CNetWebSocket* pWebSocket = _GetWebSocketFromLWS(wsi);
			if(pWebSocket)
			{
				pWebSocket->OnWsRecvData((byte*)in, len);
			}
			// lws_callback_on_writable(wsi);
			break;
		}
		case LWS_CALLBACK_CLOSED:
		{
			CNetWebSocket* pWebSocket = _GetWebSocketFromLWS(wsi);
			if(pWebSocket)
			{
				pWebSocket->OnWsDisconnected();
				pNetworkService->RemoveWebSocket(pWebSocket->GetSocket());
				pNetworkService->DestroyWebSocket(pWebSocket);
			}
			LOGFATAL("websocket CALLBACK_CLOSED");
			break;
		}
		default:
			break;
	}

	return 0;
	__LEAVE_FUNCTION
	return -1;
}
