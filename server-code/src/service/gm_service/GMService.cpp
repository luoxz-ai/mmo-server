#include "GMService.h"
#include "NetSocket.h"
#include "tinyxml2/tinyxml2.h"
#include "event2/http.h"
#include "event2/keyvalq_struct.h"
 #include <sys/queue.h>
#include "MessageRoute.h"
#include "EventManager.h"
#include "SettingMap.h"
#include "NetMSGProcess.h"
#include "NetworkMessage.h"
#include "MessageRoute.h"
#include "MessagePort.h"
#include "NetMSGProcess.h"
#include <functional>
#include <unordered_map>
#include "gm_service.pb.h"
#include "brpc/server.h"

namespace Game
{
	// Service with static path.
	class GM_ServiceImpl : public GM_Service 
	{
	CGMService* m_pService;
	public:
		GM_ServiceImpl(CGMService* pService)
			:m_pService(pService)
		{
		};
		virtual ~GM_ServiceImpl() {};
		virtual void SetGM(google::protobuf::RpcController* cntl_base,
							const SetGMRequest* request,
							SetGMResponse* response,
							google::protobuf::Closure* done) override
		{
			// This object helps you to call done->Run() in RAII style. If you need
			// to process the request asynchronously, pass done_guard.release().
			brpc::ClosureGuard done_guard(done);
        
			brpc::Controller* cntl = static_cast<brpc::Controller*>(cntl_base);

			LOGMESSAGE_NOFMT(fmt::format("Received SetGM[log_id={}]: open_id={} gm_level={} sign={}", 
					   cntl->log_id(), 
					   request->open_id(), 
					   request->gm_level(), 
					   request->sign()) );
		}
		virtual void BlockLogin(google::protobuf::RpcController* cntl_base,
							const BlockLoginRequest* request,
							BlockLoginResponse* response,
							google::protobuf::Closure* done) override
		{
			// This object helps you to call done->Run() in RAII style. If you need
			// to process the request asynchronously, pass done_guard.release().
			brpc::ClosureGuard done_guard(done);
        
			brpc::Controller* cntl = static_cast<brpc::Controller*>(cntl_base);

			LOGMESSAGE_NOFMT(fmt::format("Received BlockLogin[log_id={}]: server_id={} open_id={} actor_id={} block_timestamp={} sign={}", 
					   cntl->log_id(), 
					   request->server_id(), 
					   request->open_id(), 
					   request->actor_id(), 
					   request->block_timestamp(), 
					   request->sign()) );



			//send to server_x:gm_service
			
			//wait for response
		}
		virtual void MuteChat(google::protobuf::RpcController* cntl_base,
							const MuteChatRequest* request,
							MuteChatResponse* response,
							google::protobuf::Closure* done) override
		{
			// This object helps you to call done->Run() in RAII style. If you need
			// to process the request asynchronously, pass done_guard.release().
			brpc::ClosureGuard done_guard(done);
        
			brpc::Controller* cntl = static_cast<brpc::Controller*>(cntl_base);

			LOGMESSAGE_NOFMT(fmt::format("Received MuteChat[log_id={}]: server_id={} open_id={} actor_id={} mute_timestamp={} sign={}", 
					   cntl->log_id(), 
					   request->server_id(), 
					   request->open_id(), 
					   request->actor_id(), 
					   request->mute_timestamp(), 
					   request->sign()) );
		}
	};
}

template<>
thread_local CGMService* MyTLSTypePtr<CGMService>::m_pPtr = nullptr;

extern "C" IService* ServiceCreate(uint16_t  idWorld, uint16_t  idService)
{

	CGMService* pService = new CGMService(ServerPort{ idWorld, idService });
	if (pService == nullptr)
		return nullptr;

	if (pService->Create() == false)
	{
		pService->Release();
		return nullptr;
	}

	return pService;
}
__attribute__((visibility("default"))) IService* ServiceCreate(uint16_t  idWorld, uint16_t  idService);










//////////////////////////////////////////////////////////////////////////
CGMService::CGMService(const ServerPort& nServerPort)
	:CServiceCommon(nServerPort,"GM")
{
	
}

CGMService::~CGMService()
{
	MyTLSTypePtr<CGMService>::set(this);
	scope_guards scope_exit;
	scope_exit += []() {MyTLSTypePtr<CGMService>::set(nullptr); };
	
	StopRPCServer();

	auto pService = RemoveRPCService("GM_Service");
	SAFE_DELETE(pService);
	
}

bool CGMService::Create()
{
	//各种初始化
	scope_guards scope_exit;
	MyTLSTypePtr<CGMService>::set(this);
	scope_exit += []() {MyTLSTypePtr<CGMService>::set(nullptr); };

	BaseCode::SetNdc(GetServiceName());
	scope_exit += []() {BaseCode::SetNdc(std::string()); };


	if(CreateService(20) == false)
		return false;

	{
		const ServerAddrInfo* pAddrInfo = GetMessageRoute()->QueryServiceInfo(GetServerPort());
		if(pAddrInfo == nullptr)
		{
			LOGFATAL("CGMService QueryServerInfo {} fail", GetServerPort().GetServiceID());
			return false;
		}

		CHECKF(AddRPCService(new Game::GM_ServiceImpl(this)));
		CHECKF(StartRPCServer(pAddrInfo->publish_port,pAddrInfo->debug_port));

	}
	LOGMESSAGE("GMService {} Create", GetServerPort().GetServiceID());

	return true;
}

void CGMService::SendServiceReady()
{
	MSG_INTERNAL_SERVICE_READY msg;
	msg.idWorld = GetWorldID();
	msg.bReady = true;
	SendPortMsg(ServerPort(0,SERVICECTRL_SERVICE_ID), (byte*)&msg, sizeof(msg));
}

void CGMService::SendServiceUnReady()
{
	MSG_INTERNAL_SERVICE_READY msg;
	msg.idWorld = GetWorldID();
	msg.bReady = false;
	SendPortMsg(ServerPort(0,SERVICECTRL_SERVICE_ID), (byte*)&msg, sizeof(msg));
}


void CGMService::OnProcessMessage(CNetworkMessage* pNetworkMsg)
{
	switch (pNetworkMsg->GetMsgHead()->usCmd)
	{
	case NETMSG_SCK_CLOSE:
		{
			MSG_SCK_CLOSE* pMsg = (MSG_SCK_CLOSE*)pNetworkMsg->GetBuf();

		}
		break;
	case NETMSG_SERVICE_READY:
		{
			//发送一条消息给ServiceCtrl,通知有一个新的服被开启了, 要求ServiceCtrl通知所有GlobalRoute服,更新路由信息
			MSG_INTERNAL_SERVICE_REGISTER msg;
			msg.idWorld = GetWorldID();
			msg.update_time = TimeGetSecond();
			SendPortMsg(ServerPort(0,SERVICECTRL_SERVICE_ID), (byte*)&msg, sizeof(msg));
			LOGMESSAGE("WorldReady: {}", GetWorldID());
			EventManager()->ScheduleEvent(0, [this]()
			{
				SendServiceReady();
			}, 30*1000, true);
		}
		break;
	case ServerMSG::MsgID_ServiceHttpRequest:
		{
			ServerMSG::ServiceHttpRequest msg;
			if(msg.ParseFromArray(pNetworkMsg->GetMsgBody(),pNetworkMsg->GetBodySize()	) == false)
				return;
			LOGMESSAGE("recv_httprequest:{}", msg.uid());

			auto it = m_HttpRequestHandle.find(msg.kvmap().at("mothed"));
			if(it == m_HttpRequestHandle.end())
			{
				ServerMSG::ServiceHttpResponse send_msg;
				send_msg.set_uid(msg.uid());
				send_msg.set_response_code(HTTP_BADMETHOD);
				SendPortMsg(pNetworkMsg->GetFrom().GetServerPort(), ServerMSG::MsgID_ServiceHttpResponse, send_msg);
				return;
			}
			it->second(pNetworkMsg->GetFrom().GetServerPort(), msg);
			LOGMESSAGE("finish_httprequest:{}", msg.uid());
		}
		break;
	default:
		{
			m_pNetMsgProcess->Process(pNetworkMsg);
		}
		break;
	}
}

void CGMService::OnLogicThreadProc()
{

	CServiceCommon::OnLogicThreadProc();


}

void CGMService::OnLogicThreadCreate()
{
	MyTLSTypePtr<CGMService>::set(this);
	CServiceCommon::OnLogicThreadCreate();
}

void CGMService::OnLogicThreadExit()
{

}


