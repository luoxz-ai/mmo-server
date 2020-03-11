#include "ServiceComm.h"

#include <iostream>

#include "EventManager.h"
#include "MessagePort.h"
#include "MessageRoute.h"
#include "brpc/server.h"
#include "butil/logging.h"
class BRPCLogSink : public logging::LogSink
{
public:
	BRPCLogSink() {}
	virtual ~BRPCLogSink() {}
	// Called when a log is ready to be written out.
	// Returns true to stop further processing.
	virtual bool OnLogMessage(int32_t severity, const char* file, int32_t line, const butil::StringPiece& log_content)
	{
		int32_t logid  = 0;
		int32_t loglev = 0;
		switch(severity)
		{
			case logging::BLOG_INFO:
			case logging::BLOG_NOTICE:
				logid  = BaseCode::s_message_logger;
				loglev = LOG_LEVEL_INFO;
				break;
			case logging::BLOG_WARNING:
				logid  = BaseCode::s_warning_logger;
				loglev = LOG_LEVEL_WARN;
				break;
			case logging::BLOG_ERROR:
				logid  = BaseCode::s_error_logger;
				loglev = LOG_LEVEL_ERROR;
				break;
			case logging::BLOG_FATAL:
				logid  = BaseCode::s_fatal_logger;
				loglev = LOG_LEVEL_FATAL;
				break;
		}
		ZLOG_STREAM(logid, loglev, file, line, log_content.as_string());

		return true;
	}
};

struct INIT_BRPC_LOG
{
	INIT_BRPC_LOG() { logging::SetLogSink(new BRPCLogSink); }
};
static INIT_BRPC_LOG g_INIT_BRPC_LOG;

CServiceCommon::CServiceCommon(const ServerPort& nServerPort, const std::string& service_name)
	: m_pNetworkService(nullptr)
	, m_pMessagePort(nullptr)
	, m_pEventManager(new CEventManager())
	, m_pNetMsgProcess(new CNetMSGProcess())
	, m_nServerPort(nServerPort)
	, m_ServiceName(service_name)
	, m_pBRPCServer(nullptr)
{
	m_pEventManager->Init(nullptr);
}

CServiceCommon::~CServiceCommon()
{
	__ENTER_FUNCTION
	// GetMessageRoute()->CloseMessagePort(m_pMessagePort);
	if(m_pMessagePort)
		m_pMessagePort->SetPortEventHandler(nullptr);
	m_pMessagePort = nullptr;
	StopLogicThread();

	if(m_pNetworkService)
	{
		m_pNetworkService->Destroy();
		SAFE_DELETE(m_pNetworkService);
	}

	if(m_pBRPCServer)
	{
		m_pBRPCServer->Stop(0);
		m_pBRPCServer->Join();
	}
	SAFE_DELETE(m_pBRPCServer);

	LOGMESSAGE("{} {} Close", GetServiceName().c_str(), GetServerPort().GetServiceID());
	__LEAVE_FUNCTION
}

bool CServiceCommon::CreateRPCServer()
{
	if(m_pBRPCServer == nullptr)
	{
		m_pBRPCServer = new brpc::Server;
		return true;
	}

	return false;
}

bool CServiceCommon::AddRPCService(google::protobuf::Service* pService, const std::string& restful_map)
{
	if(m_pBRPCServer == nullptr)
	{
		m_pBRPCServer = new brpc::Server;
	}
	if(pService == nullptr)
		return false;
	if(restful_map.empty())
	{
		if(m_pBRPCServer->AddService(pService, brpc::SERVER_DOESNT_OWN_SERVICE) != 0)
		{
			LOGFATAL("Fail to AddRPCService");
			return false;
		}
	}
	else
	{
		if(m_pBRPCServer->AddService(pService, brpc::SERVER_DOESNT_OWN_SERVICE, restful_map) != 0)
		{
			LOGFATAL("Fail to AddRPCService");
			return false;
		}
	}

	return true;
}

google::protobuf::Service* CServiceCommon::RemoveRPCService(const std::string& name)
{
	if(m_pBRPCServer == nullptr)
	{
		return nullptr;
	}

	auto pService = m_pBRPCServer->FindServiceByName(name);
	if(pService)
		return nullptr;
	if(m_pBRPCServer->RemoveService(pService) != 0)
	{
		LOGFATAL("Fail to RemoveService");
		return nullptr;
	}

	return pService;
}

void CServiceCommon::StopRPCServer()
{
	if(m_pBRPCServer)
	{
		m_pBRPCServer->Stop(0);
		m_pBRPCServer->Join();
	}
}

bool CServiceCommon::StartRPCServer(uint16_t publish_port, uint16_t internal_port, bool bEnableSSL, google::protobuf::Service* pHttpMasterService)
{
	if(m_pBRPCServer == nullptr)
		return false;
	brpc::ServerOptions options;
	options.idle_timeout_sec = -1;
	if(internal_port == 0)
		options.internal_port = 32000 + GetServerPort().GetServiceID();
	else
		options.internal_port = internal_port;
	if(bEnableSSL)
	{
		options.mutable_ssl_options()->default_cert.certificate = "cert.pem";
		options.mutable_ssl_options()->default_cert.private_key = "key.pem";
		options.mutable_ssl_options()->ciphers					= "";
	}
	if(pHttpMasterService)
		options.http_master_service = pHttpMasterService;

	if(m_pBRPCServer->Start(publish_port, &options) != 0)
	{
		LOGFATAL("Fail to StartRPCServer");
		return false;
	}

	return true;
}

uint64_t CServiceCommon::CreateUID()
{
	return m_UIDFactory.CreateID();
}

bool CServiceCommon::CreateNetworkService()
{
	if(m_pNetworkService)
		return false;
	m_pNetworkService = new CNetworkService();
	return true;
}

bool CServiceCommon::CreateService(int32_t nWorkInterval /*= 100*/, class CMessagePortEventHandler* pEventHandler /*= nullptr*/)
{
	__ENTER_FUNCTION
	m_UIDFactory.Init(GetServerPort().GetWorldID(), GetServerPort().GetServiceID());
	if(ListenMessagePort(GetServiceName(), pEventHandler) == false)
		return false;
	//开启逻辑线程处理来自其他服务器的消息
	StartLogicThread(nWorkInterval, GetServiceName());
	LOGMESSAGE("{} {} Create", GetServiceName().c_str(), GetServerPort().GetServiceID());
	__LEAVE_FUNCTION
	return true;
}

bool CServiceCommon::ListenMessagePort(const std::string& service_name, CMessagePortEventHandler* pEventHandler /*= nullptr*/)
{
	__ENTER_FUNCTION
	m_pMessagePort = GetMessageRoute()->QueryMessagePort(GetServerPort(), false);
	if(m_pMessagePort != nullptr && pEventHandler != nullptr)
		m_pMessagePort->SetPortEventHandler(pEventHandler);
	return m_pMessagePort != nullptr;
	__LEAVE_FUNCTION
	return false;
}

void CServiceCommon::StartLogicThread(int32_t nWorkInterval, const std::string& name)
{
	if(m_pLogicThread)
	{
		return;
	}
	m_pLogicThread =
		std::unique_ptr<CNormalThread>(new CNormalThread(nWorkInterval, name, std::bind(&CServiceCommon::OnLogicThreadProc, this),
														 std::bind(&CServiceCommon::OnLogicThreadCreate, this), std::bind(&CServiceCommon::OnLogicThreadExit, this)));
}

void CServiceCommon::StopLogicThread()
{
	if(m_pLogicThread)
	{
		m_pLogicThread->Stop();
		m_pLogicThread->Join();
	}
}

void CServiceCommon::OnProcessMessage(CNetworkMessage*) {}

void CServiceCommon::OnLogicThreadProc()
{
	__ENTER_FUNCTION
	if(m_pNetworkService)
	{
		m_pNetworkService->RunOnce();
	}

	static const int32_t MAX_PROCESS_PER_LOOP = 1000;
	int32_t				 nCount				  = 0;

	CNetworkMessage* pMsg = nullptr;
	if(m_pMessagePort)
	{
		// process message_port msg
		while(nCount < MAX_PROCESS_PER_LOOP && m_pMessagePort->TakeMsg(pMsg))
		{
			nCount++;
			OnProcessMessage(pMsg);
			SAFE_DELETE(pMsg);
		}
	}

	m_nMessageProcess += nCount;

	// process message from client
	if(m_pNetworkService)
	{
		nCount = 0;
		while(nCount < MAX_PROCESS_PER_LOOP && m_pNetworkService->_GetMessageQueue().get(pMsg))
		{
			nCount++;
			OnProcessMessage(pMsg);
			SAFE_DELETE(pMsg);
		}
	}

	m_nMessageProcess += nCount;
	//定时器回掉
	m_pEventManager->OnTimer();

	__LEAVE_FUNCTION
}

void CServiceCommon::OnLogicThreadCreate()
{
	BaseCode::SetNdc(m_ServiceName);
	BaseCode::InitMonitorLog(m_ServiceName);
	LOGMESSAGE("ThreadID:{}", get_cur_thread_id());
}

void CServiceCommon::OnLogicThreadExit() {}

bool CServiceCommon::SendPortBroadcastMsg(const ServerPort& nServerPort, byte* buf, size_t len)
{
	__ENTER_FUNCTION
	if(GetMessageRoute() && nServerPort.IsVaild())
	{
		CMessagePort* pMessagePort = GetMessageRoute()->QueryMessagePort(nServerPort);
		if(pMessagePort)
		{
			CNetworkMessage msg(buf, len, GetServerPort(), nServerPort);
			return pMessagePort->SendBroadcastMsg(msg);
		}
		else
		{
			LOGWARNING("SendMsg To ServerPort:{} {} not find", nServerPort.GetWorldID(), nServerPort.GetServiceID());
		}
	}
	__LEAVE_FUNCTION
	return false;
}

bool CServiceCommon::SendPortBroadcastMsg(const ServerPort& nServerPort, uint16_t usCmd, const google::protobuf::Message& msg)
{
	__ENTER_FUNCTION
	if(GetMessageRoute() && nServerPort.IsVaild())
	{
		CMessagePort* pMessagePort = GetMessageRoute()->QueryMessagePort(nServerPort);
		if(pMessagePort)
		{
			CNetworkMessage _msg(usCmd, msg, GetServerVirtualSocket(), VirtualSocket(nServerPort));
			_msg.SetBroadcast();
			return pMessagePort->SendBroadcastMsg(_msg);
		}
		else
		{
			LOGWARNING("SendMsg To ServerPort:{} {}, not find", nServerPort.GetWorldID(), nServerPort.GetServiceID());
		}
	}
	__LEAVE_FUNCTION
	return false;
}

bool CServiceCommon::SendPortMultiMsg(const ServerPort& nServerPort, const std::vector<VirtualSocket>& setVS, const CNetworkMessage& msg)
{
	__ENTER_FUNCTION
	if(GetMessageRoute() && nServerPort.IsVaild())
	{
		CMessagePort* pMessagePort = GetMessageRoute()->QueryMessagePort(nServerPort);
		if(pMessagePort)
		{
			return pMessagePort->SendMultiMsg(setVS, msg);
		}
		else
		{
			LOGWARNING("SendMsg To ServerPort:{} {}, not find", nServerPort.GetWorldID(), nServerPort.GetServiceID());
		}
	}
	__LEAVE_FUNCTION
	return false;
}

bool CServiceCommon::SendPortMultiIDMsg(const ServerPort& nServerPort, const std::vector<OBJID>& setVS, const CNetworkMessage& msg)
{
	__ENTER_FUNCTION
	if(GetMessageRoute() && nServerPort.IsVaild())
	{
		CMessagePort* pMessagePort = GetMessageRoute()->QueryMessagePort(nServerPort);
		if(pMessagePort)
		{
			return pMessagePort->SendMultiIDMsg(setVS, msg);
		}
		else
		{
			LOGWARNING("SendMsg To ServerPort:{} {}, not find", nServerPort.GetWorldID(), nServerPort.GetServiceID());
		}
	}
	__LEAVE_FUNCTION
	return false;
}

bool CServiceCommon::SendPortMsg(const ServerPort& nServerPort, byte* buf, size_t len)
{
	__ENTER_FUNCTION
	if(GetMessageRoute() && nServerPort.IsVaild())
	{
		CMessagePort* pMessagePort = GetMessageRoute()->QueryMessagePort(nServerPort);
		if(pMessagePort)
		{
			CNetworkMessage msg(buf, len, GetServerPort(), nServerPort);
			return pMessagePort->SendMsg(msg);
		}
		else
		{
			LOGWARNING("SendMsg To ServerPort:{} {}, not find", nServerPort.GetWorldID(), nServerPort.GetServiceID());
		}
	}
	__LEAVE_FUNCTION
	return false;
}

bool CServiceCommon::SendPortMsg(const ServerPort& nServerPort, uint16_t usCmd, const google::protobuf::Message& msg)
{
	CNetworkMessage _msg(usCmd, msg, GetServerVirtualSocket(), nServerPort);
	return SendMsg(_msg);
}

bool CServiceCommon::SendToVirtualSocket(const VirtualSocket& vsTo, uint16_t usCmd, const google::protobuf::Message& msg)
{
	CNetworkMessage _msg(usCmd, msg, GetServerVirtualSocket(), vsTo);
	return SendMsg(_msg);
}

bool CServiceCommon::SendMsg(const CNetworkMessage& msg)
{
	__ENTER_FUNCTION
	VirtualSocket vs(msg.GetTo());
	if(GetMessageRoute() && vs.GetServerPort() != 0)
	{
		CMessagePort* pMessagePort = GetMessageRoute()->QueryMessagePort(vs.GetServerPort());
		if(pMessagePort)
		{
			return pMessagePort->SendMsg(msg);
		}
		else
		{
			LOGWARNING("SendMsg To ServerPort:{} {} {}, not find", vs.GetServerPort().GetWorldID(), vs.GetServerPort().GetServiceID(), vs.GetSocketIdx());
		}
		return false;
	}
	else if(vs.GetServerPort() == m_nServerPort && vs.GetSocketIdx() != 0)
	{
		// direct send message
		if(m_pNetworkService)
			return m_pNetworkService->SendSocketMsgByIdx(msg.GetTo().GetSocketIdx(), msg.GetBuf(), msg.GetSize());
		return false;
	}
	else
	{
		LOGWARNING("Message Want Send To Worng: {} {} {}", msg.GetTo().GetServerPort().GetWorldID(), msg.GetTo().GetServerPort().GetServiceID(),
				   msg.GetTo().GetSocketIdx());
#ifdef DEBUG
		DumpStack(CallFrameMap(1));
#endif
		return false;
	}
	__LEAVE_FUNCTION
	return false;
}

bool CServiceCommon::SendBroadcastMsg(const CNetworkMessage& msg)
{
	__ENTER_FUNCTION
	VirtualSocket vs(msg.GetTo());
	if(GetMessageRoute() && vs.GetServerPort() != 0)
	{
		CMessagePort* pMessagePort = GetMessageRoute()->QueryMessagePort(vs.GetServerPort());
		if(pMessagePort)
		{
			return pMessagePort->SendBroadcastMsg(msg);
		}
		else
		{
			LOGWARNING("SendBroadcastMsg:{} {} {}, not find", vs.GetServerPort().GetWorldID(), vs.GetServerPort().GetServiceID(), vs.GetSocketIdx());
		}
		return false;
	}
	else if(vs.GetServerPort() == m_nServerPort)
	{
		// direct send message
		if(m_pNetworkService)
		{
			m_pNetworkService->BrocastMsg(msg.GetBuf(), msg.GetSize());
			return true;
		}
		return false;
	}
	else
	{
		LOGWARNING("Message Want Send To Worng: {} {} {}", msg.GetTo().GetServerPort().GetWorldID(), msg.GetTo().GetServerPort().GetServiceID(),
				   msg.GetTo().GetSocketIdx());
		return false;
	}
	__LEAVE_FUNCTION
	return false;
}
