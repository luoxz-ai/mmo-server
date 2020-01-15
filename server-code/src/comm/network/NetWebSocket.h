#ifndef NetWebsocket_H__
#define NetWebsocket_H__

#include <libwebsockets.h>

#include "BaseType.h"
#include "Decryptor.h"
#include "Encryptor.h"
#include "NetworkDefine.h"

static const uint32_t g_FrameSize	   = 4096;
static const uint16_t g_RecvBufferSize = g_FrameSize * 4;

class CWebSocketEventHandler;
class CNetworkService;
struct lws;

enum NET_WEBSOCKET_STATUS
{
	NWSS_NOT_INIT,
	NWSS_CONNECTING,
	NWSS_WAIT_RECONNECT,
	NWSS_READY,
	NWSS_CLOSEING,
	NWSS_CLOSED,
};

struct WebSocketFrame
{
	byte*	 data;
	uint16_t length;
	bool	 isStart;
	bool	 isEnd;

	byte* GetPayload() { return data + LWS_PRE; }

	uint16_t GetPayloadLength() { return length - LWS_PRE; }

	static WebSocketFrame* CreateFrame(void* data, uint16_t length, bool isStart, bool isEnd)
	{
		WebSocketFrame* frame = new WebSocketFrame();
		// LWS_PRE: libwebsocket要求发送数据前面保留LWS_PRE可用，用来做协议头填充。
		uint16_t frame_len = length + LWS_PRE;

		frame->data = new byte[frame_len];
		memset(frame->data, 0, frame_len);
		memcpy(frame->data + LWS_PRE, data, length);
		frame->length  = frame_len;
		frame->isStart = isStart;
		frame->isEnd   = isEnd;
		return frame;
	}

	static void DestroyFrame(WebSocketFrame* frame)
	{
		if(frame != nullptr)
		{
			delete[] frame->data;
			delete frame;
		}
	}
};

class CNetWebSocket
{
  public:
	CNetWebSocket(CNetworkService* pService, CWebSocketEventHandler* pEventHandler);
	~CNetWebSocket();

  public:
	bool Init(struct lws* wsi);

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

	uint16_t GetSocketIdx() const { return m_nSocketIdx; }
	void	 SetSocketIdx(uint16_t val) { m_nSocketIdx = val; }

	SOCKET GetSocket() const { return m_socket; }
	void   SetSocket(SOCKET val) { m_socket = val; }

	NET_WEBSOCKET_STATUS GetWebSocketStatus();

	std::string GetAddrString();

	int GetPort();

	bool SendMsg(byte* pBuffer, size_t len, bool bFlush = true);

	void Close();

	size_t GetPacketSizeMax() const { return m_nPacketSizeMax; }
	void   SetPacketSizeMax(size_t val);

  public:
	void RealSend();

	void SendTestData();

	void OnWsAccepted();

	void OnWsConnected();

	void OnWsConnectFailed();

	void OnWsDisconnected();

	void OnWsRecvData(byte* pBuffer, size_t len);

  private:
	bool _AddRecvBuffer(byte* pBuffer, size_t len);

	bool _GetEntirePackage(byte* pBuffer, size_t& len);

	void _ConsumeBuffer(size_t len);

  private:
	NET_WEBSOCKET_STATUS m_WebSocketStatus;

	CNetworkService*		m_pService;
	CWebSocketEventHandler* m_pWebSocketHandler;

	CDecryptor* m_pDecryptor;
	CEncryptor* m_pEncryptor;

	struct lws* m_pLwsInstance;

	std::list<WebSocketFrame*> m_SendList;

	byte	 m_RecvBuffer[g_RecvBufferSize];
	uint16_t m_RecvLength;

	uint16_t m_nSocketIdx;
	SOCKET	 m_socket;
	size_t	 m_nPacketSizeMax;
	uint16_t m_nLastProcessMsgCMD;
	uint16_t m_nLastCMDSize;
};

#endif // NetWebsocket_H__
