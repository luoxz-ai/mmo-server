#ifndef SERVICEDEFINE_H
#define SERVICEDEFINE_H

#include <atomic>
#include <unordered_map>

#include "BaseType.h"
#include "Misc.h"
#include "CheckUtil.h"
#include "export_lua.h"

export_lua enum SERVICE_TYPE {
    WORLD_SERVICE        = 1, //单服单个, 重要节点,奔溃需重启整组服务, 登录服务, 单服内全局事件, 排行榜, 公会, 组队
    SCENE_SERVICE        = 2, //单服多个, 游戏服务,负载场景
    SHARED_SCENE_SERVICE = 3, //全区多个共用, 游戏服务,负载场景
    AI_SERVICE           = 4, //单服多个, 与zone联动, ai服务,每个ai服务对应一个zone
    MARKET_SERVICE       = 5, //单服单个, 拍卖行
    GM_SERVICE           = 6, //单服单个, 无状态, 后台调用, 充值调用
    SOCKET_SERVICE       = 6, //单服多个, 连接服务, 网关, 广播, 组播
    AUTH_SERVICE         = 8, //单服多个/单个, 无状态, 验证登录

    GLOBAL_ROUTE_SERVICE = 10, //全局单个, 无状态, 所有服务均会连接该服务,用来同步特殊的消息
    GM_PROXY_SERVICE     = 11, //全局多个, 无状态, 充值回调, 后台回调, http服务, 收到后进行验证,验证后,转发给对应服的GM_SERVICE
};

enum SERVICE_UID_TYPE
{
    WORLD_SERVICE_UID  = 1,
    MARKET_SERVICE_UID = 2,
    GM_SERVICE_UID     = 3,
    SCENE_SERVICE_UID  = 4,
};

export_lua inline std::string GetServiceName(uint32_t nServiceID);

#endif /* SERVICEDEFINE_H */
