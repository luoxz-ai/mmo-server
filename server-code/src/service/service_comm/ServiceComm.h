#ifndef SERVICECOMM_H
#define SERVICECOMM_H
#include "BaseCode.h"
#include "EventManager.h"
#include "NetMSGProcess.h"
#include "NetworkService.h"
#include "UIDFactory.h"
#include "ServiceDefine.h"

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
    export_lua const ServerPort& GetServerPort() const { return m_nServerPort; }
    export_lua void              SetServerPort(const ServerPort& val) { m_nServerPort = val; }
    export_lua uint16_t          GetWorldID() const { return m_nServerPort.GetWorldID(); }
    export_lua uint16_t          GetServiceID() const { return m_nServerPort.GetServiceID(); }

    export_lua CNetworkService* GetNetworkService() const { return m_pNetworkService.get(); }
    export_lua const std::string& GetServiceName() const { return m_ServiceName; }
    void                          SetServiceName(const std::string& val) { m_ServiceName = val; }
    uint32_t                      GetMessageProcess() const { return m_nMessageProcess; }
    void                          SetMessageProcess(uint32_t val) { m_nMessageProcess = val; }



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
    bool SendMsgToPort(const CNetworkMessage& msg) const;
    bool SendMsgToVirtualSocket(const VirtualSocket& vsTo, const google::protobuf::Message& msg) const;
    bool SendMsgToVirtualSocket(const VirtualSocket& vsTo, uint16_t usCmd, const google::protobuf::Message& msg) const;
    bool SendBroadcastMsg(const CNetworkMessage& msg) const;
    //发送消息给MessagePort
    bool SendMsgToPort(const ServerPort& nServerPort, const google::protobuf::Message& msg) const;
    bool SendMsgToPort(const ServerPort& nServerPort, uint16_t usCmd, const google::protobuf::Message& msg) const;
    bool TransmitMsgToPort(const ServerPort& nServerPort, CNetworkMessage* pMsg) const;
    bool SendBroadcastMsgToPort(const ServerPort& nServerPort, const google::protobuf::Message& msg) const;
    bool SendBroadcastMsgToPort(const ServerPort&                nServerPort,
                              uint16_t                         usCmd,
                              const google::protobuf::Message& msg) const;
    //发送多播的消息给MessagePort
    bool SendMultiMsgToPort(const ServerPort&                 nServerPort,
                          const std::vector<VirtualSocket>& setVS,
                          const CNetworkMessage&            msg) const;
    bool SendMultiIDMsgToPort(const ServerPort&         nServerPort,
                            const std::vector<OBJID>& setVS,
                            const CNetworkMessage&    msg) const;

    template<class T>
    VirtualSocketMap_t IDList2VSMap(const T& idList, OBJID idExtInclude = 0) const
    {
        VirtualSocketMap_t setSocketMap;
        __ENTER_FUNCTION

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

        __LEAVE_FUNCTION
        return setSocketMap;
    }
    virtual void _ID2VS(OBJID id, VirtualSocketMap_t& VSMap) const {}

    bool SendMsgTo(const VirtualSocketMap_t& setSocketMap, const google::protobuf::Message& msg) const;
    bool SendMsgTo(const VirtualSocketMap_t& setSocketMap, uint16_t nCmd, const google::protobuf::Message& msg) const;

public:
    export_lua CEventManager* GetEventManager() const { return m_pEventManager.get(); }
    export_lua CNetMSGProcess* GetNetMsgProcess() const { return m_pNetMsgProcess.get(); }
    export_lua CMonitorMgr* GetMonitorMgr() const { return m_pMonitorMgr.get(); }

protected:
    std::unique_ptr<CNetworkService> m_pNetworkService;
    CMessagePort*                    m_pMessagePort;
    ServerPort                       m_nServerPort;

    std::unique_ptr<CNormalThread>  m_pLogicThread;
    std::unique_ptr<CEventManager>  m_pEventManager;
    std::unique_ptr<CNetMSGProcess> m_pNetMsgProcess;
    std::string                     m_ServiceName;
    uint32_t                        m_nMessageProcess = 0;
    std::unique_ptr<CMonitorMgr>    m_pMonitorMgr;
};

#define to_sc_cmd(msg)       to_cmd<SC_CMD>(msg, &SC_CMD_Parse)
#define to_server_msgid(msg) to_msgid<ServerMSG::OMsgID>(msg, &ServerMSG::OMsgID_Parse)

template<class CMD_T, class ParseFunc>
inline uint32_t to_cmd(const google::protobuf::Message& msg, ParseFunc func)
{
    __ENTER_FUNCTION
    std::string name = "CMD_" + msg.GetDescriptor()->name();
    CMD_T       cmd;
    bool        result = func(name, &cmd);
    CHECKF_FMT(result, "{} can't convert to cmd", name);
    return cmd;
    __LEAVE_FUNCTION
    return 0;
}

template<class CMD_T, class ParseFunc>
inline uint32_t to_msgid(const google::protobuf::Message& msg, ParseFunc func)
{
    __ENTER_FUNCTION
    std::string name = "MsgID_" + msg.GetDescriptor()->name();
    CMD_T       cmd;
    bool        result = func(name, &cmd);
    CHECKF_FMT(result, "{} can't convert to cmd", name);
    return cmd;
    __LEAVE_FUNCTION
    return 0;
}

#endif /* SERVICECOMM_H */
