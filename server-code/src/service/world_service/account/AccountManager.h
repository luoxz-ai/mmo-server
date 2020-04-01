#ifndef ACCOUNTMANAGER_H
#define ACCOUNTMANAGER_H

#include <queue>
#include <unordered_map>
#include <unordered_set>

#include "BaseCode.h"
#include "MysqlConnection.h"
#include "NetworkDefine.h"
#include "Thread.h"

namespace brpc
{
class Channel;
};
class CNetworkMessage;
class CMysqlConnection;
class CAccount;

constexpr const char* AUTH_SERVER_SIGNATURE = "test";
constexpr int32_t     AUTH_KEY_CANUSE_SECS  = 180;

class CAccountManager: public Noncopyable<CAccountManager>
{
    CAccountManager();
    bool Init(class CWorldService* pWorld);
public:
    CreateNewImpl(CAccountManager);
public:
  
    ~CAccountManager();

    
    //验证
    bool IsAuthing(const std::string& openid) const;
    bool Auth(const std::string& openid, const std::string& auth, const VirtualSocket& vs);
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
    size_t GetAccountSize() const;
    size_t GetWaitAccountSize() const;
    void   Destory();
    CAccount* QueryAccountBySocket(const VirtualSocket& vs) const;
private:
    //等待认证列表
    std::unordered_map<std::string, uint64_t> m_AuthList;
    struct ST_AUTH_DATA
    {
        std::string   open_id;
        VirtualSocket from;
    };
    std::unordered_map<uint64_t, ST_AUTH_DATA> m_AuthDataList;
    MPSCQueue<std::function<void()>>           m_ResultList;
    uint64_t                                   m_CallIdx = 0;

    //已经认证的Account
    std::unordered_map<std::string, CAccount*>   m_setAccount;
    std::unordered_map<VirtualSocket, CAccount*> m_setAccountBySocket;
    std::deque<CAccount*>                        m_setWaitAccount;
    std::unique_ptr<CWorkerThread>               m_threadAuth;

    std::unique_ptr<brpc::Channel> m_pAuthChannel;
};
#endif /* ACCOUNTMANAGER_H */
