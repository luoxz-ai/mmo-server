#pragma once

#include <unordered_map>
#include <unordered_set>
#include <queue>
#include "BaseCode.h"
#include "Thread.h"
#include "NetworkDefine.h"
#include "MysqlConnection.h"

namespace brpc
{
	class Channel;
};
class CNetworkMessage;
class CMysqlConnection;
class CAccount;
class CAccountManager
{
public:
	CAccountManager();
	~CAccountManager();

	CREATE_NEW_IMPL(CAccountManager);
	bool Init(class CWorldService* pWorld);



	void RegisterMessageHandler();
	//验证
	bool Auth(CNetworkMessage* pMsg);
	void _OnAuthFail(uint64_t call_id, std::string str_detail);
	void _OnAuthSucc(uint64_t call_id);
	void _AddResult(std::function<void()>&& result_func);
	void PorcessResult();
	//将Account加入列表,通知前端登录成功,并下发角色信息
	void Login(const std::string& openid, CAccount* pAccount);
	void Logout(const VirtualSocket& vs);

	void OnAuthThreadCreate();
	void OnAuthThreadFinish();
	
	
	void OnTimer();
public:
	bool OnMsgCreateActor(CNetworkMessage* pMsg);
	bool OnMsgSelectActor(CNetworkMessage* pMsg);
	size_t GetAccountSize() const;
	size_t GetWaitAccountSize() const;
	void Destory();
private:
	//等待认证列表
	std::unordered_map<std::string, uint64_t> m_AuthList;
	struct ST_AUTH_DATA
	{
		std::string open_id;
		VirtualSocket from;
	};
	std::unordered_map<uint64_t, ST_AUTH_DATA> m_AuthDataList;
	MPSCQueue<std::function<void()>> m_ResultList;
	uint64_t m_CallIdx = 0;

	//已经认证的Account
	std::unordered_map<std::string, CAccount*> m_setAccount;
	std::unordered_map<VirtualSocket, CAccount*> m_setAccountBySocket;
	std::deque<CAccount*> m_setWaitAccount;
	std::unique_ptr<CWorkerThread> m_threadAuth;

	
	brpc::Channel* m_pAuthChannel = nullptr;
};