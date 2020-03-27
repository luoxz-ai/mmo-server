#include "NetworkDefine.h"


ServiceNameRegister::ServiceNameRegister()
{
    s_ServiceName[WORLD_SERVICE_ID] = "World";
    s_ServiceName[MARKET_SERVICE_ID] = "Market";
    s_ServiceName[GM_SERVICE_ID] = "GM";
    s_ServiceName[SERVICECTRL_SERVICE_ID] = "ServiceCtrl";    
    for(int32_t i = MIN_ZONE_SERVICE_ID; i <= MAX_ZONE_SERVICE_ID; i++ )
    {
        s_ServiceName[i] = std::string("Zone_") + std::to_string(i-MIN_ZONE_SERVICE_ID+1);
    }
    for(int32_t i = MIN_SHAREZONE_SERVICE_ID; i <= MAX_SHAREZONE_SERVICE_ID; i++ )
    {
        s_ServiceName[i] = std::string("SharedZone_") + std::to_string(i-MIN_SHAREZONE_SERVICE_ID+1);
    }
    for(int32_t i = MIN_AI_SERVICE_ID; i <= MAX_AI_SERVICE_ID; i++ )
    {
        s_ServiceName[i] = std::string("AI_") + std::to_string(i-MIN_AI_SERVICE_ID+1);
    }
    for(int32_t i = MIN_SOCKET_SERVICE_ID; i <= MAX_SOCKET_SERVICE_ID; i++ )
    {
        s_ServiceName[i] = std::string("Socket") + std::to_string(i-MAX_SOCKET_SERVICE_ID+1);
    }
    for(int32_t i = MIN_GLOBAL_ROUTE_SERVICE_ID; i <= MAX_GLOBAL_ROUTE_SERVICE_ID; i++ )
    {
        s_ServiceName[i] = std::string("GlobalRoute") + std::to_string(i-MIN_GLOBAL_ROUTE_SERVICE_ID+1);
    }

}

std::string ServiceNameRegister::GetServiceName(uint32_t nServiceID) const
{
    auto it = s_ServiceName.find(nServiceID);
    if(it == s_ServiceName.end())
    {
        return "unknown";
    }
    return it->second;
}
