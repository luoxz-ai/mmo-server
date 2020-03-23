#ifndef MessagePort_h__
#define MessagePort_h__

#include "EventManager.h"
#include "LockfreeQueue.h"
#include "NetworkDefine.h"
#include "NetworkService.h"

class CNetSocket;
class CMessageRoute;
class CMessagePortEventHandler
{
public:
    CMessagePortEventHandler() {}
    virtual ~CMessagePortEventHandler() {}

public:
    // connect to other server succ
    virtual void OnPortConnected(CNetSocket*) {}
    // connect to other server failed, can set CNetSocket::setReconnectTimes = 0 to stop reconnect
    virtual void OnPortConnectFailed(CNetSocket*) {}
    // lost connect
    virtual void OnPortDisconnected(CNetSocket*) {}
    // accept a new client
    virtual void OnPortAccepted(CNetSocket*) {}
    // receive data
    virtual void OnPortRecvData(CNetSocket*, byte* pBuffer, size_t len) {}
    // recv over time
    virtual void OnPortRecvTimeout(CNetSocket*) {}
};
class CMessagePort : public CNetEventHandler
{
public:
    CMessagePort(const ServerPort& nServerPort, CMessageRoute* pRoute);
    virtual ~CMessagePort();

public:
    virtual void OnConnected(CNetSocket* pSocket);
    virtual void OnConnectFailed(CNetSocket*);
    virtual void OnDisconnected(CNetSocket*);
    virtual void OnAccepted(CNetSocket*);
    virtual void OnRecvData(CNetSocket*, byte* pBuffer, size_t len);
    virtual void OnRecvTimeout(CNetSocket*);

    void              SetPortEventHandler(CMessagePortEventHandler* pHandler) { m_pPortEventHandler = pHandler; }
    void              SetRemoteSocket(CNetSocket* pSocket);
    CNetSocket*       GetRemoteSocket() const { return m_pRemoteServerSocket; }
    const ServerPort& GetServerPort() const { return m_nServerPort; }
    bool              GetLocalPort() const { return m_bLocalPort; }
    void              SetLocalPort(bool val) { m_bLocalPort = val; }

    bool   TakeMsg(CNetworkMessage*& msg);
    bool   SendMsg(const CNetworkMessage& msg);
    bool   SendBroadcastMsg(const CNetworkMessage& msg);
    bool   SendMultiMsg(const std::vector<VirtualSocket>& setVS, const CNetworkMessage& msg);
    bool   SendMultiIDMsg(const std::vector<OBJID>& setID, const CNetworkMessage& msg);
    size_t GetWriteBufferSize();

private:
    void PostSend();
    void _SendAllMsg();

private:
    CMessageRoute*                         m_pRoute = nullptr;
    MPSCQueue<CNetworkMessage*>            m_RecvMsgQueue;
    MPSCQueue<CNetworkMessage*>            m_SendMsgQueue;
    CNetSocket*                            m_pRemoteServerSocket = nullptr;
    bool                                   m_bLocalPort          = false;
    ServerPort                             m_nServerPort;
    std::atomic<CMessagePortEventHandler*> m_pPortEventHandler;
    CEventEntryPtr                         m_Event;
};

#endif // MessagePort_h__
