#ifndef SERVICECOMM_H
#define SERVICECOMM_H
#include "BaseCode.h"
#include "EventManager.h"
#include "NetMSGProcess.h"
#include "NetworkService.h"
#include "UIDFactory.h"

class CMessageRoute;
class CMessagePort;
class CMonitorMgr;

using VirtualSocketMap_t = std::unordered_map<ServerPort, std::vector<VirtualSocket>>;

export_lua class CServiceCommon : public NoncopyableT<CServiceCommon>
{
protected:
    CServiceCommon();
    bool Init(const ServerPort& nServerPort);
public:
    
    virtual ~CServiceCommon();
    void DestoryServiceCommon();
public:
    export_lua VirtualSocket GetServerVirtualSocket() const { return VirtualSocket(m_nServerPort, 0); }
    export_lua VirtualSocket GetAIServerVirtualSocket() const
    {
        return VirtualSocket(ServerPort(GetWorldID(), GetServiceID() + 10), 0);
    }
    export_lua const ServerPort& GetServerPort() const { return m_nServerPort; }
    export_lua void              SetServerPort(const ServerPort& val) { m_nServerPort = val; }
    export_lua uint16_t          GetWorldID() const { return m_nServerPort.GetWorldID(); }
    export_lua uint16_t          GetServiceID() const { return m_nServerPort.GetServiceID(); }

    export_lua CNetworkService*  GetNetworkService() const { return m_pNetworkService.get(); }
    export_lua const std::string& GetServiceName() const { return m_ServiceName; }
    void                          SetServiceName(const std::string& val) { m_ServiceName = val; }
    uint32_t                      GetMessageProcess() const { return m_nMessageProcess; }
    void                          SetMessageProcess(uint32_t val) { m_nMessageProcess = val; }

    export_lua uint64_t CreateUID();

public:
    bool CreateNetworkService();
    bool CreateService(int32_t nWorkInterval /*= 100*/, class CMessagePortEventHandler* pEventHandler = nullptr);



public:
    //开启逻辑线程，如果没有开启IO线程，也会每一个循环触发一次RunOnce
    void         StartLogicThread(int32_t nWorkInterval = 100, const std::string& name = std::string());
    void         StopLogicThread();
    virtual void OnProcessMessage(CNetworkMessage*);
    virtual void OnLogicThreadProc();
    virtual void OnLogicThreadCreate();
    virtual void OnLogicThreadExit();

public:
    bool ListenMessagePort(const std::string& service_name, class CMessagePortEventHandler* pEventHandler = nullptr);

public:
    bool SendMsg(const CNetworkMessage& msg);
    bool SendToVirtualSocket(const VirtualSocket& vsTo, uint16_t usCmd, const google::protobuf::Message& msg);
    bool SendBroadcastMsg(const CNetworkMessage& msg);
    //发送消息给MessagePort
    bool SendPortMsg(const ServerPort& nServerPort, byte* buf, size_t len);
    bool SendPortMsg(const ServerPort& nServerPort, uint16_t usCmd, const google::protobuf::Message& msg);
    bool SendPortBroadcastMsg(const ServerPort& nServerPort, byte* buf, size_t len);
    bool SendPortBroadcastMsg(const ServerPort& nServerPort, uint16_t usCmd, const google::protobuf::Message& msg);
    //发送多播的消息给MessagePort
    bool SendPortMultiMsg(const ServerPort&                 nServerPort,
                          const std::vector<VirtualSocket>& setVS,
                          const CNetworkMessage&            msg);
    bool SendPortMultiIDMsg(const ServerPort& nServerPort, const std::vector<OBJID>& setVS, const CNetworkMessage& msg);

    template<class T>
    VirtualSocketMap_t IDList2VSMap(const T& idList, OBJID idExtInclude = 0)
    {
        VirtualSocketMap_t setSocketMap;
        if constexpr(std::is_invocable<T>::value)
        {
            idList([this, &setSocketMap](OBJID id) { _ID2VS(id, setSocketMap); });
        }
        else if constexpr(std::is_same<typename T::value_type, OBJID>::value)
        {
            for(OBJID id: idList)
            {
                _ID2VS(id, setSocketMap);
            }
        }
        else
        {
            for(const auto& [id, v]: idList)
            {
                _ID2VS(id, setSocketMap);
            }
        }

        if(idExtInclude != 0)
            _ID2VS(idExtInclude, setSocketMap);

        return setSocketMap;
    }
    virtual void _ID2VS(OBJID id, VirtualSocketMap_t& VSMap){}
    bool SendMsgTo(uint16_t nCmd, const google::protobuf::Message& msg,const VirtualSocketMap_t& setSocketMap);
public:
    export_lua CEventManager* GetEventManager() const { return m_pEventManager.get(); }
    export_lua CNetMSGProcess* GetNetMsgProcess() const { return m_pNetMsgProcess.get(); }
    export_lua CMonitorMgr* GetMonitorMgr() { return m_pMonitorMgr.get(); }
protected:
    std::unique_ptr<CNetworkService> m_pNetworkService;
    CMessagePort*    m_pMessagePort;
    ServerPort       m_nServerPort;

    std::unique_ptr<CNormalThread>  m_pLogicThread;
    std::unique_ptr<CEventManager>  m_pEventManager;
    std::unique_ptr<CNetMSGProcess> m_pNetMsgProcess;
    CUIDFactory                     m_UIDFactory;
    std::string                     m_ServiceName;
    uint32_t                        m_nMessageProcess = 0;
    std::unique_ptr<CMonitorMgr>    m_pMonitorMgr;

};
#endif /* SERVICECOMM_H */
