#ifndef NETSERVERSOCKET_H
#define NETSERVERSOCKET_H

#include "NetSocket.h"

class CServerSocket : public CNetSocket
{
public:
    CServerSocket(CNetworkService* pService, CNetEventHandler* pEventHandler, bool bReconnect = false);
    ~CServerSocket();

    // init by connect to
    virtual bool Init(bufferevent* pBufferEvent) override;
    virtual void Interrupt(bool bClearEventHandler) override;
    virtual bool CreateByListener() const override { return false; }

public:
    bool   GetReconnect() const { return m_bReconnect; }
    void   SetReconnect(bool val) { m_bReconnect = val; }
    size_t GetReconnectTimes() const { return m_nReconnectTimes; }
    void   SetReconnectTimes(size_t val) { m_nReconnectTimes = val; }

    virtual void _OnClose(short what) override;
    static void  _OnReconnect(int32_t fd, short what, void* ctx);
    static void  _OnSocketConnectorEvent(bufferevent*, short what, void* ctx);

    virtual void OnRecvTimeout(bool& bReconnect) override;
    virtual void OnRecvData(byte* pBuffer, size_t len) override;

    void OnConnected();
    void OnConnectFailed();

private:
    bool   m_bReconnect;
    event* m_pReconnectEvent;
    size_t m_nReconnectTimes;
};

#endif /* NETSERVERSOCKET_H */
