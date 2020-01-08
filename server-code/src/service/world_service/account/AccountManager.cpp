#include "Account.h"
#include "AccountManager.h"
#include "WorldService.h"
#include "md5.h"
#include <brpc/channel.h>
const char* AUTH_URL = "https://example.com";

static const std::string AUTH_SERVER_SIGNATURE = "test";
static const int AUTH_KEY_CANUSE_SECS = 180;
CAccountManager::CAccountManager()
{
	
}

CAccountManager::~CAccountManager()
{
	Destory();
}

void CAccountManager::Destory()
{
	SAFE_DELETE(m_pAuthChannel);
	m_threadAuth->Stop();
	m_threadAuth->Join();
	for (auto& pair_v : m_setAccount)
	{
		SAFE_DELETE(pair_v.second);
	}
	m_setAccount.clear();
	m_setAccountBySocket.clear();
	m_setWaitAccount.clear();
}


bool CAccountManager::Init(class CWorldService* pWorld)
{
	m_threadAuth.reset(new CWorkerThread("AuthThread", [pWorld]()
	{
		MyTLSTypePtr<CWorldService>::set(pWorld);	
		BaseCode::SetNdc("AuthThread");
		LOGMESSAGE("ThreadID:{}", get_cur_thread_id());
	}));

	m_pAuthChannel = new brpc::Channel;
	brpc::ChannelOptions options;
	options.protocol = brpc::PROTOCOL_HTTP;
	//options.connection_type = brpc::CONNECTION_TYPE_POOLED;
	options.timeout_ms = 50000/*milliseconds*/;  //5s
	options.max_retry = 3;

	//options.ssl_options.enable = true;
	if (m_pAuthChannel->Init(AUTH_URL, "", &options) != 0) 
	{
		LOGERROR("Fail to initialize AuthUrl:{}", AUTH_URL);
		return false;
	}

	return true;
}

void CAccountManager::RegisterMessageHandler()
{
	WorldService()->GetNetMsgProcess()->Register(CMD_CS_LOGIN, std::bind(&CAccountManager::Auth, this, std::placeholders::_1));
	WorldService()->GetNetMsgProcess()->Register(CMD_CS_CREATEACTOR, std::bind(&CAccountManager::OnMsgCreateActor, this, std::placeholders::_1));
	WorldService()->GetNetMsgProcess()->Register(CMD_CS_SELECTACTOR, std::bind(&CAccountManager::OnMsgSelectActor, this, std::placeholders::_1));
}

bool CAccountManager::Auth(CNetworkMessage* pMsg)
{
__ENTER_FUNCTION
	CS_LOGIN msg;
	if (msg.ParseFromArray(pMsg->GetMsgBody(), pMsg->GetBodySize()) == false)
	{
		return false;
	}



	auto itFind = m_AuthList.find(msg.openid());
	if (itFind != m_AuthList.end())
	{
		LOGLOGIN("Actor:{} IsAleardyInAuth.", msg.openid().c_str());

		//当前正在验证，通知客户端
		SC_LOGIN result_msg;
		result_msg.set_result_code(SC_LOGIN::EC_WAIT_AUTH);
		WorldService()->SendToVirtualSocket(pMsg->GetFrom(), CMD_SC_LOGIN, result_msg);
		return false;
	}
	auto nGMLev = GMManager()->GetGMLevel(msg.openid());
	//校验程序版本号
	if (nGMLev == 0 && ( msg.prog_ver().empty() || WorldService()->CheckProgVer(msg.prog_ver()) == false) )
	{
		//发送错误给前端
		LOGLOGIN("Actor:{} CheckProgVerFail.", msg.openid().c_str());
		SC_LOGIN result_msg;
		result_msg.set_result_code(SC_LOGIN::EC_PROG_VER);
		WorldService()->SendToVirtualSocket(pMsg->GetFrom(), CMD_SC_LOGIN, result_msg);
		return false;
	}

	if(msg.auth().empty())
	{
		if(msg.last_succ_key().empty() == false)
		{
			//曾经验证成功过， 检查2次校验串
			std::string md5str = md5(msg.openid() + std::to_string(TimeGetSecond() / AUTH_KEY_CANUSE_SECS) + AUTH_SERVER_SIGNATURE);
			if (nGMLev == 0 && msg.last_succ_key() != md5str)
			{
				//发送错误给前端
				LOGLOGIN("Actor:{} MD5CHECKFail.", msg.openid().c_str());

				SC_LOGIN result_msg;
				result_msg.set_result_code(SC_LOGIN::EC_LAST_KEY);
				WorldService()->SendToVirtualSocket(pMsg->GetFrom(), CMD_SC_LOGIN, result_msg);
				return false;
			}
			else
			{
				LOGLOGIN("Actor:{} LoginAuth.", msg.openid().c_str());
				//可以直接登陆了
				MSG_SCK_AUTH auth_msg;
				auth_msg.vs = pMsg->GetFrom();
				WorldService()->SendPortMsg(pMsg->GetFrom().GetServerPort(), (byte*)&auth_msg, sizeof(auth_msg) );


				SC_LOGIN result_msg;
				result_msg.set_result_code(SC_LOGIN::EC_SUCC);
				result_msg.set_last_succ_key(md5str);
				WorldService()->SendToVirtualSocket(pMsg->GetFrom(), CMD_SC_LOGIN, result_msg);
			

				CAccount* pAccount = CAccount::CreateNew(msg.openid(), pMsg->GetFrom());
				//将准备好的Account放入Result列表,等待主线程处理
				Login(msg.openid(), pAccount);


				return true;
			}
		}
	}
	else
	{
		SC_LOGIN result_msg;
		result_msg.set_result_code(SC_LOGIN::EC_AUTH);
		WorldService()->SendToVirtualSocket(pMsg->GetFrom(), CMD_SC_LOGIN, result_msg);
		return false;
	}





	LOGLOGIN("Actor:{} StartAuth.", msg.openid().c_str());
	
	brpc::Controller* cntl = new brpc::Controller;;
	cntl->http_request().uri() = AUTH_URL;
	cntl->http_request().set_method(brpc::HTTP_METHOD_POST);
	std::string post_data = fmt::format("open_id={}&auth={}", msg.openid(), msg.auth());
	cntl->request_attachment().append(post_data);
	auto call_id = cntl->call_id().value;
	m_AuthList[msg.openid()] = call_id;
	auto& auth_data = m_AuthDataList[call_id];
	auth_data.open_id = msg.openid();
	auth_data.from = pMsg->GetFrom();

	struct __local
	{
		static void _OnAuthResult(brpc::Controller* cntl, CAccountManager* pThis, uint64_t call_id)
		{
			if(cntl->Failed())
			{
				pThis->_AddResult(std::bind(&CAccountManager::_OnAuthFail, pThis, call_id, cntl->ErrorText()));
			}
			else if(cntl->http_response().status_code() != brpc::HTTP_STATUS_OK)
			{
				std::stringstream buf;
				buf <<  cntl->response_attachment();
				pThis->_AddResult(std::bind(&CAccountManager::_OnAuthFail, pThis, call_id,buf.str()));
			}
			else
			{
				pThis->_AddResult( std::bind(&CAccountManager::_OnAuthSucc, pThis, call_id) );
			}
		}

	};
	google::protobuf::Closure* done = brpc::NewCallback(&__local::_OnAuthResult, cntl, this, call_id );

	m_pAuthChannel->CallMethod(NULL, cntl, NULL, NULL, done);
	if (cntl->Failed()) 
	{
		LOGERROR("Fail to callMethod Auth:{}", AUTH_URL);
		return false;
	}
	
	


	return true;
__LEAVE_FUNCTION
	return false;
}

void CAccountManager::_AddResult(std::function<void()>&& result_func)
{
	//call by worker thread
	m_ResultList.push(std::move(result_func));
}

void CAccountManager::PorcessResult()
{
	//call by main thread

	std::function<void()> result_func;
	while(m_ResultList.get(result_func))
	{
		result_func();
	}
}

void CAccountManager::_OnAuthFail(uint64_t call_id, std::string str_detail)
{
	auto it = m_AuthDataList.find(call_id);
	if(it == m_AuthDataList.end())
		return;
	auto& auth_data = it->second;

	//发送验证失败消息
	SC_LOGIN msg;
	msg.set_result_code(SC_LOGIN::EC_AUTH);
	msg.set_detail(str_detail);
	WorldService()->SendToVirtualSocket(auth_data.from, CMD_SC_LOGIN, msg);
	
	m_AuthList.erase(auth_data.open_id);
	m_AuthDataList.erase(it);
}

void CAccountManager::_OnAuthSucc(uint64_t call_id)
{
	auto it = m_AuthDataList.find(call_id);
	if(it == m_AuthDataList.end())
		return;
	auto& auth_data = it->second;

	CAccount* pAccount = CAccount::CreateNew(auth_data.open_id,auth_data.from);
	CHECK(pAccount);
	LOGLOGIN("Actor:{} AuthSucc.", auth_data.open_id.c_str());

	MSG_SCK_AUTH auth_msg;
	auth_msg.vs = pAccount->GetSocket();
	WorldService()->SendPortMsg(pAccount->GetSocket().GetServerPort(), (byte*)&auth_msg, sizeof(auth_msg) );

	SC_LOGIN result_msg;
	result_msg.set_result_code(SC_LOGIN::EC_SUCC);
	std::string md5str = md5(auth_data.open_id + std::to_string(TimeGetSecond() / AUTH_KEY_CANUSE_SECS) + AUTH_SERVER_SIGNATURE);
	result_msg.set_last_succ_key(md5str);
	WorldService()->SendToVirtualSocket(pAccount->GetSocket(), CMD_SC_LOGIN, result_msg);
	
	Login(auth_data.open_id, pAccount);
	m_AuthList.erase(auth_data.open_id);
	m_AuthDataList.erase(it);

}

void CAccountManager::Login(const std::string& openid, CAccount* pAccount)
{
__ENTER_FUNCTION
	//判断当前是否已经有账号登陆了？ 如果已经有账号登陆了， 先将前一个Account踢下线
	bool bKicked = false;
	auto itFind = m_setAccount.find(pAccount->GetOpenID());
	if (itFind != m_setAccount.end())
	{
		//kick out
		CAccount* pOldAccount = itFind->second;
		pOldAccount->KickOut();
		m_setAccountBySocket.erase(pOldAccount->GetSocket());
		m_setAccount.erase(itFind);
		if (pOldAccount->IsWait())
		{
			auto it_waitAccount = std::find(m_setWaitAccount.begin(), m_setWaitAccount.end(), pOldAccount);
			if (it_waitAccount != m_setWaitAccount.end())
			{
				m_setWaitAccount.erase(it_waitAccount);
			}
		}
		

		SAFE_DELETE(pOldAccount);
		bKicked = true;
	}

	m_setAccount[pAccount->GetOpenID()] = pAccount;
	m_setAccountBySocket[pAccount->GetSocket()] = pAccount;

	if (bKicked || 	GMManager()->GetGMLevel(pAccount->GetOpenID()) > 0 || m_setAccount.size() - m_setWaitAccount.size() < _START_WAITING_ACCOUNT_COUNT)
	{
		//通知前端，登陆成功
		LOGLOGIN("ActorSucc:{}.", pAccount->GetOpenID().c_str());
		pAccount->SetWait(false);
		pAccount->SendActorInfo();
	}
	else
	{
		//放入等待登陆队列
		LOGLOGIN("ActorWait:{}.", pAccount->GetOpenID().c_str());

		pAccount->SetWait(true);
		m_setWaitAccount.push_back(pAccount);
		pAccount->SendWaitInfo();
	}

__LEAVE_FUNCTION
}

void CAccountManager::Logout(const VirtualSocket& vs)
{
__ENTER_FUNCTION

	auto itFind = m_setAccountBySocket.find(vs);
	if (itFind == m_setAccountBySocket.end())
		return;


	CAccount* pAccount = itFind->second;
	LOGLOGIN("ActorLogout:{}.", pAccount->GetOpenID().c_str());

	if (pAccount->IsWait() == false)
	{
		pAccount->KickOut();
	}

	m_setAccount.erase(pAccount->GetOpenID());
	m_setAccountBySocket.erase(itFind);
	if (pAccount->IsWait())
	{
		auto it_waitAccount = std::find(m_setWaitAccount.begin(), m_setWaitAccount.end(), pAccount);
		if (it_waitAccount != m_setWaitAccount.end())
		{
			m_setWaitAccount.erase(it_waitAccount);
		}
	}
	SAFE_DELETE(pAccount);
__LEAVE_FUNCTION
}

void CAccountManager::OnAuthThreadCreate()
{
}

void CAccountManager::OnAuthThreadFinish()
{
}

void CAccountManager::OnTimer()
{
__ENTER_FUNCTION

	PorcessResult();

	//检查等待登陆队列， 如果等待登陆队列不为空
	//每次最多放行5个
	static const int MAX_PROCESS_PER_TIMERS = 20;
	for (int i = 0; i < MAX_PROCESS_PER_TIMERS; i++)
	{
		if(m_setWaitAccount.empty())
			break;
		if(m_setAccount.size() - m_setWaitAccount.size() >= _STOP_WAITING_ACCOUNT_COUNT)
			break;

		CAccount* pWaitAccount = m_setWaitAccount.front();
		m_setWaitAccount.pop_front();
		LOGLOGIN("ActorSucc:{}.", pWaitAccount->GetOpenID().c_str());
		pWaitAccount->SetWait(false);
		//通知前端
		pWaitAccount->SendActorInfo();
	}

__LEAVE_FUNCTION
}

bool CAccountManager::OnMsgCreateActor(CNetworkMessage* pMsg)
{
__ENTER_FUNCTION

	CS_CREATEACTOR msg;
	if (msg.ParseFromArray(pMsg->GetMsgBody(), pMsg->GetBodySize()) == false)
	{
		return false;
	}

	auto itFind = m_setAccountBySocket.find(pMsg->GetFrom());
	if (itFind == m_setAccountBySocket.end())
		return true;
	CAccount* pAccount = itFind->second;
	CHECKF(pAccount->IsWait() == false);
	
	pAccount->CreateActor(msg.name(), msg.prof(), msg.baselook());

	return true;
__LEAVE_FUNCTION
	return false;

}

bool CAccountManager::OnMsgSelectActor(CNetworkMessage* pMsg)
{
__ENTER_FUNCTION
	CS_SELECTACTOR msg;
	if (msg.ParseFromArray(pMsg->GetMsgBody(), pMsg->GetBodySize()) == false)
	{
		return false;
	}

	auto itFind = m_setAccountBySocket.find(pMsg->GetFrom());
	if (itFind == m_setAccountBySocket.end())
		return false;
	CAccount* pAccount = itFind->second;
	CHECKF(pAccount->IsWait() == false);
	pAccount->SelectActor(msg.actor_idx());

	return true;
__LEAVE_FUNCTION
	return false;
}

size_t CAccountManager::GetAccountSize() const
{
	return m_setAccount.size();
}

size_t CAccountManager::GetWaitAccountSize() const
{
	return m_setWaitAccount.size();
}


