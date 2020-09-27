#include "RPCService.h"

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

CRPCService::CRPCService() {}

CRPCService::~CRPCService()
{
    if(m_pBRPCServer)
    {
        StopRPCServer();
        m_pBRPCServer->ClearServices();
    }
}

bool CRPCService::Init(const ServiceID& idServiceID)
{
    if(m_pBRPCServer == nullptr)
    {
        m_idServiceID = idServiceID;
        m_pBRPCServer = std::make_unique<brpc::Server>();
        return true;
    }

    return false;
}

void CRPCService::ClearRPCServices()
{
    if(m_pBRPCServer)
    {
        m_pBRPCServer->ClearServices();
    }
}

bool CRPCService::AddRPCService(google::protobuf::Service* pService, const std::string& restful_map)
{
    if(m_pBRPCServer == nullptr)
    {
        m_pBRPCServer = std::make_unique<brpc::Server>();
    }
    if(pService == nullptr)
        return false;
    if(restful_map.empty())
    {
        if(m_pBRPCServer->AddService(pService, brpc::SERVER_OWNS_SERVICE) != 0)
        {
            LOGFATAL("Fail to AddRPCService");
            return false;
        }
    }
    else
    {
        if(m_pBRPCServer->AddService(pService, brpc::SERVER_OWNS_SERVICE, restful_map) != 0)
        {
            LOGFATAL("Fail to AddRPCService");
            return false;
        }
    }

    return true;
}

google::protobuf::Service* CRPCService::RemoveRPCService(const std::string& name)
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

void CRPCService::StopRPCServer()
{
    if(m_pBRPCServer)
    {
        m_pBRPCServer->Stop(0);
        m_pBRPCServer->Join();
    }
}

bool CRPCService::StartRPCServer(uint16_t publish_port, uint16_t internal_port, bool bEnableSSL, google::protobuf::Service* pHttpMasterService)
{
    if(m_pBRPCServer == nullptr)
        return false;
    brpc::ServerOptions options;
    options.idle_timeout_sec = -1;
    if(internal_port == 0)
        options.internal_port = 32000 + m_idServiceID;
    else
        options.internal_port = internal_port;
    if(bEnableSSL)
    {
        options.mutable_ssl_options()->default_cert.certificate = "cert.pem";
        options.mutable_ssl_options()->default_cert.private_key = "key.pem";
        options.mutable_ssl_options()->ciphers                  = "";
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