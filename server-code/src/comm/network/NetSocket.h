#ifndef NetSocket_h__
#define NetSocket_h__

#include <string>
#include <deque>
#include <numeric>
#include <time.h>

#include "NetworkDefine.h"
#include "BytePerSecondCount.h"
#include "MemoryHeap.h"
#include "Decryptor.h"
#include "Encryptor.h"

class CNetworkService;
class CNetEventHandler;

enum NET_SOCKET_STATUS
{
	NSS_NOT_INIT,
	NSS_CONNECTING,
	NSS_WAIT_RECONNECT,
	NSS_READY,
	NSS_CLOSEING,
	NSS_CLOSED,
};

struct bufferevent;
struct event;
class CNetSocket
{
public:
	CNetSocket(CNetworkService* pService, CNetEventHandler* pEventHandler, bool bPassive, bool bReconnect = false);
	virtual ~CNetSocket();
	//init by accept
	bool Init(bufferevent* pBufferEvent);
	//init by connect to
	bool InitWaitConnecting(bufferevent* pBufferEvent);

	virtual void Close();
	template<class T>
	bool SendMsg(T* pBuffer, bool bFlush = true)
	{
		static_assert(std::is_base_of<MSG_HEAD, T>::value, "Only Can Send Data inherit of MSG_HEAD");
		return SendMsg((byte*)pBuffer, sizeof(*pBuffer), bFlush);
	}

	bool SendMsg(byte* pBuffer, size_t len, bool bFlush = true);
	bool _SendMsg(byte* pBuffer, size_t len, bool bFlush = true);
	bool GetReconnect() const { return m_bReconnect; }
	void SetReconnect(bool val) { m_bReconnect = val; }
	void InitDecryptor(uint32_t seed)
	{
		SAFE_DELETE(m_pDecryptor);
		m_pDecryptor = new CDecryptor;
		m_pDecryptor->Init(seed);
	}
	void InitEncryptor(uint32_t seed)
	{
		SAFE_DELETE(m_pEncryptor);
		m_pEncryptor = new CEncryptor;
		m_pEncryptor->Init(seed);
	}

public:
	void OnConnected();
	void OnConnectFailed();
	void OnDisconnected();
	void OnAccepted();
	void OnRecvData(byte* pBuffer, size_t len);
	void OnRecvTimeout(bool& bReconnect);

public:
	void _SetTimeout();
	void _OnReceive(bufferevent* b);
	void _OnClose(short what);

	static void _OnReconnect(int fd, short what, void* ctx);
	static void _OnSocketRead(bufferevent *, void *ctx);
	static void _OnSendOK(bufferevent* b, void *ctx);
	static void _OnSocketEvent(bufferevent *, short, void *ctx);
	static void _OnSocketConnectorEvent(bufferevent *, short what, void *ctx);
public:
	CNetworkService* GetService() const { return m_pService; }
	bufferevent* GetBufferevent() const { return m_pBufferevent; }
	std::mutex& GetSendMutex() { return m_mutexSend; }

	NET_SOCKET_STATUS GetStatus() const { return m_Status; }
	void SetStatus(NET_SOCKET_STATUS val) { m_Status = val; }

	void SetAddrAndPort(const char* addr, int port);

	SOCKET GetSocket() const { return m_socket; }
	void SetSocket(SOCKET val) { m_socket = val; }
	uint16_t GetSocketIdx() const { return m_nSocketIdx; }
	void SetSocketIdx(uint16_t val) { m_nSocketIdx = val; }
	std::string GetAddrString() const { return m_strAddr; }
	uint64_t GetAddr() const { return m_addr; }
	void SetAddr(const std::string& val);
	int GetPort() const { return m_nPort; }
	void SetPort(int val) { m_nPort = val; }
	size_t GetReconnectTimes() const { return m_nReconnectTimes; }
	void SetReconnectTimes(size_t val) { m_nReconnectTimes = val; }
	int GetRecvTimeOutSec() const { return m_nRecvTimeOutSec; }
	void SetRecvTimeOutSec(int val) { m_nRecvTimeOutSec = val; }
	size_t GetLogWriteHighWateMark() const { return m_nLogWriteHighWateMark; }
	void SetLogWriteHighWateMark(size_t val) { m_nLogWriteHighWateMark = val; }
	size_t GetWaitWriteSize();
	size_t GetPacketSizeMax() const { return m_nPacketSizeMax; }
	void SetPacketSizeMax(size_t val);

public:
	MEMORYHEAP_DECLARATION(s_Heap);
protected:
	CNetworkService*	m_pService;
	CNetEventHandler*   m_pEventHandler;
	bufferevent*		m_pBufferevent;
	event*				m_pReconnectEvent;
	struct evbuffer*	m_Sendbuf;
	std::mutex			m_mutexSend;

	bool				m_bPassive;
	std::string			m_strAddr;
	int					m_nPort;
	uint64_t			m_addr;

	SOCKET				m_socket;
	uint16_t			m_nSocketIdx;
	
	bool				m_bReconnect;

	NET_SOCKET_STATUS	m_Status;
	size_t				m_nReconnectTimes;
	int					m_nRecvTimeOutSec;

	bool				m_bCreateByListener;
	CDecryptor*			m_pDecryptor;
	CEncryptor*			m_pEncryptor;
	size_t				m_nPacketSizeMax;
	size_t m_nLogWriteHighWateMark = 4*1024*1024;	//default log when output>=4M
	byte*				m_ReadBuff;
	unsigned short m_nLastProcessMsgCMD = 0;
	unsigned short m_nLastCMDSize = 0;
};
#endif // NetSocket_h__
