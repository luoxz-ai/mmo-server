#pragma once
#include "BaseCode.h"
#include "NetworkService.h"
#include "NetMSGProcess.h"
#include "EventManager.h"
#include "UIDFactory.h"

class CMessageRoute;
class CMessagePort;
namespace brpc
{
	class Server;
}
namespace google
{
	namespace protobuf
	{
		class Service;
	}
}

export_lua class CServiceCommon 
{
public:
	CServiceCommon(const ServerPort& nServerPort, const std::string& service_name);
	virtual ~CServiceCommon();

public:
	export_lua VirtualSocket GetServerVirtualSocket() const { return VirtualSocket(m_nServerPort, 0); }
	export_lua VirtualSocket GetAIServerVirtualSocket() const { return VirtualSocket(ServerPort(GetWorldID(),GetServiceID()+10), 0); }
	export_lua const ServerPort& GetServerPort() const { return m_nServerPort; }
	export_lua void SetServerPort(const ServerPort& val) { m_nServerPort = val; }
	export_lua uint16_t GetWorldID() const { return m_nServerPort.GetWorldID(); }
	export_lua uint16_t GetServiceID() const { return m_nServerPort.GetServiceID(); }

	export_lua CNetworkService* GetNetworkService() const { return m_pNetworkService; }
	export_lua const std::string& GetServiceName() const { return m_ServiceName; }
	void SetServiceName(const std::string& val) { m_ServiceName = val; }
	uint32_t GetMessageProcess() const { return m_nMessageProcess; }
	void SetMessageProcess(uint32_t val) { m_nMessageProcess = val; }

	export_lua uint64_t CreateUID();
public:
	bool CreateNetworkService();
	bool CreateService(int32_t nWorkInterval /*= 100*/, class CMessagePortEventHandler* pEventHandler = nullptr);
public:
	bool CreateRPCServer();
	bool AddRPCService(google::protobuf::Service* pService, const std::string& restful_map = std::string());
	google::protobuf::Service* RemoveRPCService(const std::string& name);
	void StopRPCServer();
	bool StartRPCServer(uint16_t publish_port, uint16_t internal_port = 0,  bool bEnableSSL = true, google::protobuf::Service* pHttpMasterService = nullptr);

public:
	//开启逻辑线程，如果没有开启IO线程，也会每一个循环触发一次RunOnce
	void StartLogicThread(int32_t nWorkInterval = 100, const std::string& name = std::string());
	void StopLogicThread();
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
	bool SendPortMultiMsg(const ServerPort& nServerPort, const std::vector<VirtualSocket>& setVS, const CNetworkMessage& msg);
	bool SendPortMultiIDMsg(const ServerPort& nServerPort, const std::vector<OBJID>& setVS, const CNetworkMessage& msg);

public:
	export_lua CEventManager* GetEventManager() const { return m_pEventManager.get(); }
	export_lua CNetMSGProcess* GetNetMsgProcess() const { return m_pNetMsgProcess.get(); }

protected:
	CNetworkService* m_pNetworkService;
	CMessagePort* m_pMessagePort;
	ServerPort m_nServerPort;

	std::unique_ptr<CNormalThread> m_pLogicThread;
	std::unique_ptr<CEventManager>	 m_pEventManager;
	std::unique_ptr<CNetMSGProcess>	 m_pNetMsgProcess;
	CUIDFactory		m_UIDFactory;
	std::string		m_ServiceName;
	uint32_t		m_nMessageProcess;

	brpc::Server* m_pBRPCServer;

};