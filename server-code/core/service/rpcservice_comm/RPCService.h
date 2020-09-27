#ifndef RPCSERVICE_H
#define RPCSERVICE_H

#include "BaseCode.h"
#include "NetworkDefine.h"
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
} // namespace google

class CRPCService
{
protected:
    CRPCService();
    bool Init(const ServiceID& idServiceID);

public:
    virtual ~CRPCService();
    CreateNewImpl(CRPCService);

public:
    bool                       AddRPCService(google::protobuf::Service* pService, const std::string& restful_map = std::string());
    google::protobuf::Service* RemoveRPCService(const std::string& name);
    void                       ClearRPCServices();

    void StopRPCServer();
    bool StartRPCServer(uint16_t                   publish_port,
                        uint16_t                   internal_port      = 0,
                        bool                       bEnableSSL         = true,
                        google::protobuf::Service* pHttpMasterService = nullptr);

private:
    std::unique_ptr<brpc::Server> m_pBRPCServer;
    ServiceID                     m_idServiceID = 0;
};

#endif /* RPCSERVICE_H */
