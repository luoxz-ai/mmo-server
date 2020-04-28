#ifndef NetworkDefine_h__
#define NetworkDefine_h__

#include <atomic>
#include <unordered_map>

#include "BaseType.h"
#include "Misc.h"
#include "export_lua.h"

const int32_t _MAX_MSGSIZE = 1024 * 4;

export_lua enum SERVICE_TYPE {
    SOCKET_SERVICE = 1, //单服多个, 连接服务, 网关, 广播, 组播
    WORLD_SERVICE  = 2, //单服单个, 重要节点,奔溃需重启整组服务, 登录服务, 单服内全局事件, 排行榜, 公会, 组队
    ZONE_SERVICE   = 3, //单服多个, 游戏服务,负载场景
    AI_SERVICE     = 4, //单服多个, 与zone联动, ai服务,每个ai服务对应一个zone
    MARKET_SERVICE = 5, //单服单个, 拍卖行
    GM_SERVICE     = 6, //单服单个, 无状态, 后台调用, 充值调用

    SERVICECTRL_SERVICE = 8, //全局单个, 无状态, 所有服务均会连接该服务,用来同步特殊的消息
    GLOBAL_ROUTE_SERVICE =
        9, //全局多个, 无状态, 充值回调, 后台回调, http服务, 收到后进行验证,验证后,转发给对应服的GM_SERVICE
};

export_lua enum {
    WORLD_SERVICE_ID  = 1,
    MARKET_SERVICE_ID = 3,
    GM_SERVICE_ID     = 4,

    MIN_ZONE_SERVICE_ID      = 11,
    MAX_ZONE_SERVICE_ID      = 15,
    MIN_SHAREZONE_SERVICE_ID = 16,
    MAX_SHAREZONE_SERVICE_ID = 20,

    MIN_AI_SERVICE_ID = 21,
    MAX_AI_SERVICE_ID = 30,

    MIN_SOCKET_SERVICE_ID = 31,
    MAX_SOCKET_SERVICE_ID = 35,

    MIN_GM_PROYX_SERVICE_ID = 41,
    MAX_GM_PROYX_SERVICE_ID = 45,

    ROUTE_SERVICE_ID = 60,
    MAX_SERVICE_ID   = 63,
};

struct ServiceNameRegister
{
    ServiceNameRegister();
    std::string                               GetServiceName(uint32_t nServiceID) const;
    std::unordered_map<uint32_t, std::string> s_ServiceName;
};

export_lua inline std::string GetServiceName(uint32_t nServiceID)
{
    static ServiceNameRegister s_instance;
    return s_instance.GetServiceName(nServiceID);
}

export_lua class ServerPort
{
public:
    export_lua ServerPort(uint32_t nServerPort = 0)
        : m_Data32(nServerPort)
    {
    }

    export_lua ServerPort(uint16_t idWorld, uint16_t idService)
        : m_Data{idWorld, idService}
    {
    }
    export_lua ServerPort(const ServerPort& rht)
        : m_Data(rht.m_Data)
    {
    }

    export_lua bool IsVaild() const { return m_Data32 != 0; }

    operator uint32_t() const { return m_Data32; }

    export_lua bool operator==(const ServerPort& rht) const { return m_Data32 == rht.m_Data32; }

    export_lua bool operator<(const ServerPort& rht) const { return m_Data32 < rht.m_Data32; }

    export_lua uint16_t GetWorldID() const { return m_Data.idWorld; }
    export_lua void     SetWorldID(uint16_t idWorld) { m_Data.idWorld = idWorld; }
    export_lua uint16_t GetServiceID() const { return m_Data.idService; }
    export_lua void     SetServiceID(uint16_t idService) { m_Data.idService = idService; }
    export_lua uint32_t GetData() const { return m_Data32; }
    export_lua void     SetData(uint32_t _Data32) { m_Data32 = _Data32; }

private:
    struct ST_SERVERPORT
    {
        uint16_t idWorld   = 0; //实际取值范围0~0x1FFF 8191
        uint16_t idService = 0; //实际取值范围0~0x3F 63
    };
    union {
        ST_SERVERPORT m_Data;
        uint32_t      m_Data32 = 0;
    };
};

// custom specialization of std::hash can be injected in namespace std
namespace std
{
    template<>
    struct hash<ServerPort>
    {
        typedef ServerPort  argument_type;
        typedef std::size_t result_type;
        result_type         operator()(argument_type const& s) const
        {
            std::hash<uint32_t> hasher;
            return hasher(s.GetData());
        }
    };
} // namespace std

export_lua class VirtualSocket
{
public:
    export_lua VirtualSocket(uint64_t nVirtualSocket = 0)
        : m_Data64(nVirtualSocket)
    {
    }

    export_lua VirtualSocket(const ServerPort& nServerPort)
        : m_Data{nServerPort.GetWorldID(), nServerPort.GetServiceID(), 0, 0}
    {
    }

    export_lua VirtualSocket(const ServerPort& nServerPort, uint16_t nSocketIdx)
        : m_Data{nServerPort.GetWorldID(), nServerPort.GetServiceID(), nSocketIdx, 0}
    {
    }
    export_lua VirtualSocket(const VirtualSocket& rht)
        : m_Data64(rht.m_Data64)
    {
    }

    export_lua static VirtualSocket CreateVirtualSocket(const ServerPort& nServerPort, uint16_t nSocketIdx)
    {
        return VirtualSocket{nServerPort, nSocketIdx};
    }

    export_lua bool IsVaild() const { return m_Data64 != 0; }

    operator uint64_t() const { return m_Data64; }

    export_lua bool operator==(const VirtualSocket& rht) const { return m_Data64 == rht.m_Data64; }

    export_lua bool operator<(const VirtualSocket& rht) const { return m_Data64 < rht.m_Data64; }

    export_lua ServerPort GetServerPort() const { return ServerPort{m_Data.m_idWorld, m_Data.m_idService}; }
    export_lua void       SetServerPort(const ServerPort& val)
    {
        m_Data.m_idWorld   = val.GetWorldID();
        m_Data.m_idService = val.GetServiceID();
    }
    export_lua uint16_t GetSocketIdx() const { return m_Data.m_nSocketIdx; }
    export_lua void     SetSocketIdx(uint16_t val) { m_Data.m_nSocketIdx = val; }
    export_lua uint64_t GetData64() const { return m_Data64; }

private:
    struct ST_VIRTUALSOCKET
    {
        uint16_t m_idWorld    = 0; //实际取值范围0~0x1FFF 8191
        uint16_t m_idService  = 0; //实际取值范围0~0x3F 63
        uint16_t m_nSocketIdx = 0; //实际取值范围1~0xFFFF 65535
        uint16_t nRevert      = 0;
    };
    union {
        ST_VIRTUALSOCKET m_Data;
        uint64_t         m_Data64 = 0;
    };
};

namespace std
{
    template<>
    struct hash<VirtualSocket>
    {
        typedef VirtualSocket argument_type;
        typedef std::size_t   result_type;
        result_type           operator()(argument_type const& s) const
        {
            std::hash<uint64_t> hasher;
            return hasher(s.operator uint64_t());
        }
    };
} // namespace std

struct ServerAddrInfo
{
    ServerAddrInfo()                      = default;
    ServerAddrInfo(const ServerAddrInfo&) = default;
    ServerAddrInfo& operator=(const ServerAddrInfo&) = default;

    uint16_t    idWorld;
    uint16_t    idService;
    std::string lib_name;
    std::string bind_addr;
    std::string route_addr;
    uint16_t    route_port;
    std::string publish_addr;
    uint16_t    publish_port;
    uint16_t    debug_port;
};

const uint16_t CLIENT_MSG_ID_BEGIN = 1;
const uint16_t CLIENT_MSG_ID_END   = 10000;

//常用内部消息通信及消息结构
enum MSGTYPE_INTERNAL
{
    COMMON_CMD_BEGIN  = 0,
    COMMON_CMD_SC_KEY = 1,
    COMMON_CMD_PING   = 2,
    COMMON_CMD_PONG   = 3,
    COMMON_CMD_INTERRUPT  = 9999,

    NETMSG_INTERNAL                   = 10001, //内网消息传输
    NETMSG_INTERNAL_FORWARD           = 10002, //转发包
    NETMSG_INTERNAL_BROCAST_ALL       = 10003, //广播包
    NETMSG_INTERNAL_BROCAST_BYVS      = 10004, //组播包
    NETMSG_INTERNAL_BROCAST_BYID      = 10005, //组播包
    NETMSG_INTERNAL_BROCAST_BYGROUPID = 10006, //组播包

};

#pragma pack(push) //保存对齐状态
#pragma pack(1)    // 1 bytes对齐

struct MSG_HEAD
{
    unsigned short usSize;
    unsigned short usCmd;
};

struct MSG_INTERNAL_MSG_HEAD : public MSG_HEAD
{
    uint64_t nFrom = 0;
    uint64_t nTo   = 0;
};

struct MSG_INTERNAL_FORWARD_MSG_HEAD : public MSG_HEAD
{
    uint64_t nFrom    = 0;
    uint64_t nTo      = 0;
    uint64_t nForward = 0;
};

constexpr size_t BROCAST_MSG_MAX_SOCKET =
    (_MAX_MSGSIZE - sizeof(MSG_HEAD) - sizeof(uint32_t) - sizeof(uint64_t)) / sizeof(uint64_t);
struct MSG_INTERNAL_BROCAST_MSG_HEAD : public MSG_HEAD
{
    uint32_t nAmount = 0;
    uint64_t nFrom   = 0;
    uint64_t setTo[BROCAST_MSG_MAX_SOCKET];
    MSG_INTERNAL_BROCAST_MSG_HEAD() { array_setzero(setTo); }
    uint16_t      GetSize() { return sizeof(MSG_HEAD) + sizeof(nFrom) + sizeof(nAmount) + nAmount * sizeof(uint64_t); }
    static size_t GetMaxAmount() { return BROCAST_MSG_MAX_SOCKET; }
};

constexpr size_t BROCAST_BYID_MSG_MAX_ID =
    (_MAX_MSGSIZE - sizeof(MSG_HEAD) - sizeof(uint32_t) - sizeof(uint64_t)) / sizeof(OBJID);
struct MSG_INTERNAL_BROCAST_BYID_MSG_HEAD : public MSG_HEAD
{
    uint32_t nAmount = 0;
    uint64_t nFrom;
    OBJID    setTo[BROCAST_BYID_MSG_MAX_ID];
    MSG_INTERNAL_BROCAST_BYID_MSG_HEAD() { array_setzero(setTo); }
    uint16_t      GetSize() { return sizeof(MSG_HEAD) + sizeof(nFrom) + sizeof(nAmount) + nAmount * sizeof(OBJID); }
    static size_t GetMaxAmount() { return BROCAST_BYID_MSG_MAX_ID; }
};

constexpr size_t BROCAST_BYGROUPID_MSG_MAX_ID =
    (_MAX_MSGSIZE - sizeof(MSG_HEAD) - sizeof(uint32_t) - sizeof(uint64_t)) / sizeof(OBJID);
struct MSG_INTERNAL_BROCAST_BYGROUPID_MSG_HEAD : public MSG_HEAD
{

    uint32_t nAmount = 0;
    uint64_t nFrom   = 0;
    OBJID    setTo[BROCAST_BYGROUPID_MSG_MAX_ID];

    MSG_INTERNAL_BROCAST_BYGROUPID_MSG_HEAD() { array_setzero(setTo); }
    uint16_t      GetSize() { return sizeof(MSG_HEAD) + sizeof(nFrom) + sizeof(nAmount) + nAmount * sizeof(OBJID); }
    static size_t GetMaxAmount() { return BROCAST_BYGROUPID_MSG_MAX_ID; }
};

struct MSG_INTERNAL_TRANSMITE : public MSG_HEAD
{
};

struct MSG_PING : public MSG_HEAD
{
    MSG_PING()
    {
        usCmd  = COMMON_CMD_PING;
        usSize = sizeof(MSG_PING);
    }
};

struct MSG_PONG : public MSG_HEAD
{
    MSG_PONG()
    {
        usCmd  = COMMON_CMD_PONG;
        usSize = sizeof(MSG_PONG);
    }
};

#pragma pack(pop) //恢复对齐状态

#endif // NetworkDefine_h__
