#include "AuthManager.h"
#include <brpc/channel.h>

#include "MD5.h"
#include "SocketService.h"
#include "msg/world_service.pb.h"
#include "server_msg/server_side.pb.h"

const char* AUTH_URL = "https://example.com";



CAuthManager::CAuthManager() {}

CAuthManager::~CAuthManager()
{
    Destory();
}

void CAuthManager::Destory()
{
    m_pAuthChannel.reset();
    m_threadAuth->Stop();
    m_threadAuth->Join();
}

bool CAuthManager::Init(class CSocketService* pWorld)
{
    m_threadAuth = std::make_unique<CWorkerThread>(
        "AuthThread",
        [pWorld]() { SetSocketServicePtr(pWorld); },
        []() { SetSocketServicePtr(nullptr); });

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


bool CAuthManager::IsAuthing(const std::string& openid) const
{
    return m_AuthList.find(openid) != m_AuthList.end();
}

bool CAuthManager::Auth(const std::string& openid, const std::string& auth, const VirtualSocket& vs)
{
    __ENTER_FUNCTION
    brpc::Controller* cntl     = new brpc::Controller;
    cntl->http_request().uri() = AUTH_URL;
    cntl->http_request().set_method(brpc::HTTP_METHOD_POST);
    std::string post_data = fmt::format(FMT_STRING("open_id={}&auth={}"), openid, auth);
    cntl->request_attachment().append(post_data);
    auto call_id       = cntl->call_id().value;
    m_AuthList[openid] = call_id;
    auto& auth_data    = m_AuthDataList[call_id];
    auth_data.open_id  = openid;
    auth_data.from     = vs;

    struct local
    {
        static void _OnAuthResult(brpc::Controller* cntl, CAuthManager* pThis, uint64_t call_id)
        {
            if(cntl->Failed())
            {
                pThis->_AddResult(std::bind(&CAuthManager::_OnAuthFail, pThis, call_id, cntl->ErrorText()));
            }
            else if(cntl->http_response().status_code() != brpc::HTTP_STATUS_OK)
            {
                std::stringstream buf;
                buf << cntl->response_attachment();
                pThis->_AddResult(std::bind(&CAuthManager::_OnAuthFail, pThis, call_id, buf.str()));
            }
            else
            {
                pThis->_AddResult(std::bind(&CAuthManager::_OnAuthSucc, pThis, call_id));
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


void CAuthManager::_AddResult(std::function<void()>&& result_func)
{
    // call by worker thread
    m_ResultList.push(std::move(result_func));
}

void CAuthManager::PorcessResult()
{
    // call by main thread

    std::function<void()> result_func;
    while(m_ResultList.get(result_func))
    {
        result_func();
    }
}



void CAuthManager::_OnAuthFail(uint64_t call_id, std::string str_detail)
{
    auto it = m_AuthDataList.find(call_id);
    if(it == m_AuthDataList.end())
        return;
    auto& auth_data = it->second;

    //发送验证失败消息
    SC_LOGIN msg;
    msg.set_result_code(SC_LOGIN::EC_AUTH);
    msg.set_detail(str_detail);
    SocketService()->SendToVirtualSocket(auth_data.from, msg);

    m_AuthList.erase(auth_data.open_id);
    m_AuthDataList.erase(it);
}

void CAuthManager::_OnAuthSucc(uint64_t call_id)
{
    auto it = m_AuthDataList.find(call_id);
    if(it == m_AuthDataList.end())
        return;
    auto& auth_data = it->second;

    LOGLOGIN("Actor:{} AuthSucc.", auth_data.open_id.c_str());

    ServerMSG::SocketAuth auth_msg;
    auth_msg.set_vs(auth_data.from);
    SocketService()->SendPortMsg(auth_data.from.GetServerPort(), auth_msg);

    SC_LOGIN result_msg;
    result_msg.set_result_code(SC_LOGIN::EC_SUCC);
    std::string md5str =
        md5(auth_data.open_id + std::to_string(TimeGetSecond() / AUTH_KEY_CANUSE_SECS) + AUTH_SERVER_SIGNATURE);
    result_msg.set_last_succ_key(md5str);
    SocketService()->SendToVirtualSocket(auth_data.from, result_msg);

    ServerMSG::SocketLogin login_msg;
    login_msg.set_vs(auth_data.from);
    SocketService()->SendPortMsg(ServerPort(SocketService()->GetWorldID(), WORLD_SERVICE_ID), login_msg);

    m_AuthList.erase(auth_data.open_id);
    m_AuthDataList.erase(it);
}


void CAuthManager::OnAuthThreadCreate() {}

void CAuthManager::OnAuthThreadFinish() {}