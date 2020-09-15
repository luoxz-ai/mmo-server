#include "ServiceDefine.h"

std::string GetServiceName(uint32_t nServiceID)
{
    uint8_t nServiceType = 0;
    uint8_t nSerivceIdx = 0;
    SplitUINT16(nServiceID, nServiceType, nSerivceIdx);
    switch(nSerivceType)
    {
        case SOCKET_SERVICE:
        {
            return std::string("Socket_") + std::to_string(nServiceIdx);
        }
        break;
        case WORLD_SERVICE:
        {
            return "World";
        }
        break;
        case SCENE_SERVICE:
        {
            return std::string("Scene_") + std::to_string(nServiceIdx);
        }
        break;
        case SHARED_SCENE_SERVICE:
        {
            return std::string("SharedScene_") + std::to_string(nServiceIdx);
        }
        break;
        case AI_SERVICE:
        {
            return std::string("AI_") + std::to_string(nServiceIdx);
        }
        break;
        case MARKET_SERVICE:
        {
            return "Market";
        }
        break;
        case GM_SERVICE:
        {
            return "GM";
        }
        break;
        case AUTH_SERVICE:
        {
            return std::string("Auth_") + std::to_string(nServiceIdx);
        }
        break;
        case GLOBAL_ROUTE_SERVICE:
        {
            return "Route";
        }
        break;
        case GM_PROXY_SERVICE:
        {
            return std::string("GMProxy_") + std::to_string(nServiceIdx);
        }
        break;
        default:
            return "unknow_service";
        break;
    }
}