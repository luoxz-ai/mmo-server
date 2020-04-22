#include "AccountManager.h"

#include <brpc/channel.h>

#include "Account.h"
#include "MD5.h"
#include "WorldService.h"
#include "msg/world_service.pb.h"
#include "GMManager.h"

const char* AUTH_URL = "https://example.com";


CAccountManager::CAccountManager()
{
}

CAccountManager::~CAccountManager()
{
    Destory();
}

void CAccountManager::Destory()
{
    m_pAuthChannel.reset();
    m_threadAuth->Stop();
    m_threadAuth->Join();
    for(auto& pair_v: m_setAccount)
    {
        SAFE_DELETE(pair_v.second);
    }
    m_setAccount.clear();
    m_setAccountBySocket.clear();
    m_setWaitAccount.clear();
}

bool CAccountManager::Init(class CWorldService* pWorld)
{
    m_threadAuth = std::make_unique<CWorkerThread>(
        "AuthThread",
        [pWorld]() {
            SetWorldServicePtr(pWorld);
            BaseCode::SetNdc("AuthThread");
            LOGMESSAGE("Create ThreadID:{}", get_cur_thread_id());
        },
        []() {
            SetWorldServicePtr(nullptr);
            LOGMESSAGE("Exit ThreadID:{}", get_cur_thread_id());
            BaseCode::ClearNdc();
            ;
        });

    m_pAuthChannel = std::make_unique<brpc::Channel>();
    brpc::ChannelOptions options;
    options.protocol = brpc::PROTOCOL_HTTP;
    // options.connection_type = brpc::CONNECTION_TYPE_POOLED;
    options.timeout_ms = 50000 /*milliseconds*/; // 5s
    options.max_retry  = 3;

    // options.ssl_options.enable = true;
    if(m_pAuthChannel->Init(AUTH_URL, "", &options) != 0)
    {
        LOGERROR("Fail to initialize AuthUrl:{}", AUTH_URL);
        return false;
    }

    return true;
}

bool CAccountManager::IsAuthing(const std::string& openid) const
{
    return  m_AuthList.find(openid) != m_AuthList.end();
}

bool CAccountManager::Auth(const std::string& openid, const std::string& auth, const VirtualSocket& vs)
{
    __ENTER_FUNCTION
    brpc::Controller* cntl = new brpc::Controller;
    cntl->http_request().uri() = AUTH_URL;
    cntl->http_request().set_method(brpc::HTTP_METHOD_POST);
    std::string post_data = fmt::format(FMT_STRING("open_id={}&auth={}"), openid, auth);
    cntl->request_attachment().append(post_data);
    auto call_id             = cntl->call_id().value;
    m_AuthList[openid] = call_id;
    auto& auth_data          = m_AuthDataList[call_id];
    auth_data.open_id        = openid;
    auth_data.from           = vs;

    struct local
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
                buf << cntl->response_attachment();
                pThis->_AddResult(std::bind(&CAccountManager::_OnAuthFail, pThis, call_id, buf.str()));
            }
            else
            {
                pThis->_AddResult(std::bind(&CAccountManager::_OnAuthSucc, pThis, call_id));
            }
        }
    };
    google::protobuf::Closure* done = brpc::NewCallback(&local::_OnAuthResult, cntl, this, call_id);

    m_pAuthChannel->CallMethod(NULL, cntl, NULL, NULL, done);
    if(cntl->Failed())
    {
        LOGERROR("Fail to callMethod Auth:{}", AUTH_URL);
        return false;
    }

    return true;
    __LEAVE_FUNCTION
    return false;
}

CAccount* CAccountManager::QueryAccountBySocket(const VirtualSocket& vs) const
{
    auto it = m_setAccountBySocket.find(vs);
    if(it == m_setAccountBySocket.end())
    {
        return it->second;
    }
    return nullptr;
}

void CAccountManager::_AddResult(std::function<void()>&& result_func)
{
    // call by worker thread
    m_ResultList.push(std::move(result_func));
}

void CAccountManager::PorcessResult()
{
    // call by main thread

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
    WorldService()->SendToVirtualSocket(auth_data.from, msg);

    m_AuthList.erase(auth_data.open_id);
    m_AuthDataList.erase(it);
}

void CAccountManager::_OnAuthSucc(uint64_t call_id)
{
    auto it = m_AuthDataList.find(call_id);
    if(it == m_AuthDataList.end())
        return;
    auto& auth_data = it->second;

    CAccount* pAccount = CAccount::CreateNew(auth_data.open_id, auth_data.from);
    CHECK(pAccount);
    LOGLOGIN("Actor:{} AuthSucc.", auth_data.open_id.c_str());

    MSG_SCK_AUTH auth_msg;
    auth_msg.vs = pAccount->GetSocket();
    WorldService()->SendPortMsg(pAccount->GetSocket().GetServerPort(), (byte*)&auth_msg, sizeof(auth_msg));

    SC_LOGIN result_msg;
    result_msg.set_result_code(SC_LOGIN::EC_SUCC);
    std::string md5str =
        md5(auth_data.open_id + std::to_string(TimeGetSecond() / AUTH_KEY_CANUSE_SECS) + AUTH_SERVER_SIGNATURE);
    result_msg.set_last_succ_key(md5str);
    WorldService()->SendToVirtualSocket(pAccount->GetSocket(), result_msg);

    Login(auth_data.open_id, pAccount);
    m_AuthList.erase(auth_data.open_id);
    m_AuthDataList.erase(it);
}

void CAccountManager::Login(const std::string& openid, CAccount* pAccount)
{
    __ENTER_FUNCTION
    //判断当前是否已经有账号登陆了？ 如果已经有账号登陆了， 先将前一个Account踢下线
    bool bKicked = false;
    auto itFind  = m_setAccount.find(pAccount->GetOpenID());
    if(itFind != m_setAccount.end())
    {
        // kick out
        CAccount* pOldAccount = itFind->second;
        pOldAccount->KickOut();
        m_setAccountBySocket.erase(pOldAccount->GetSocket());
        m_setAccount.erase(itFind);
        if(pOldAccount->IsWait())
        {
            auto it_waitAccount = std::find(m_setWaitAccount.begin(), m_setWaitAccount.end(), pOldAccount);
            if(it_waitAccount != m_setWaitAccount.end())
            {
                m_setWaitAccount.erase(it_waitAccount);
            }
        }

        SAFE_DELETE(pOldAccount);
        bKicked = true;
    }

    m_setAccount[pAccount->GetOpenID()]         = pAccount;
    m_setAccountBySocket[pAccount->GetSocket()] = pAccount;

    if(bKicked || GMManager()->GetGMLevel(pAccount->GetOpenID()) > 0 ||
       m_setAccount.size() - m_setWaitAccount.size() < _START_WAITING_ACCOUNT_COUNT)
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
    if(itFind == m_setAccountBySocket.end())
        return;

    CAccount* pAccount = itFind->second;
    LOGLOGIN("ActorLogout:{}.", pAccount->GetOpenID().c_str());

    if(pAccount->IsWait() == false)
    {
        pAccount->KickOut();
    }

    m_setAccount.erase(pAccount->GetOpenID());
    m_setAccountBySocket.erase(itFind);
    if(pAccount->IsWait())
    {
        auto it_waitAccount = std::find(m_setWaitAccount.begin(), m_setWaitAccount.end(), pAccount);
        if(it_waitAccount != m_setWaitAccount.end())
        {
            m_setWaitAccount.erase(it_waitAccount);
        }
    }
    SAFE_DELETE(pAccount);
    __LEAVE_FUNCTION
}

void CAccountManager::OnAuthThreadCreate() {}

void CAccountManager::OnAuthThreadFinish() {}

void CAccountManager::OnTimer()
{
    __ENTER_FUNCTION

    PorcessResult();

    //检查等待登陆队列， 如果等待登陆队列不为空
    //每次最多放行5个
    constexpr int32_t MAX_PROCESS_PER_TIMERS = 20;
    for(int32_t i = 0; i < MAX_PROCESS_PER_TIMERS; i++)
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


size_t CAccountManager::GetAccountSize() const
{
    return m_setAccount.size();
}

size_t CAccountManager::GetWaitAccountSize() const
{
    return m_setWaitAccount.size();
}
