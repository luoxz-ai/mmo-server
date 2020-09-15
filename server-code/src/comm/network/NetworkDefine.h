#ifndef NetworkDefine_h__
#define NetworkDefine_h__

#include <atomic>
#include <unordered_map>

#include "BaseType.h"
#include "Misc.h"
#include "CheckUtil.h"
#include "export_lua.h"


const int32_t _MAX_MSGSIZE = 1024 * 4;

export_lua class ServiceID
{
public:
    export_lua ServiceID(uint16_t nServerID = 0)
        : m_Data16(nServerID)
    {
    }

    export_lua ServiceID(uint8_t idServiceType, uint8_t idServiceIdx)
        : m_Data{idServiceType, idServiceIdx}
    {
    }

    export_lua ServiceID(const ServiceID& rht)
        : m_Data(rht.m_Data)
    {
    }

    export_lua bool IsVaild() const { return m_Data16 != 0; }

    operator uint32_t() const { return m_Data16; }

    export_lua bool operator==(const ServiceID& rht) const { return m_Data16 == rht.m_Data16; }

    export_lua bool operator<(const ServiceID& rht) const { return m_Data16 < rht.m_Data16; }

    export_lua uint16_t GetServiceID() const { return m_Data16; }
    export_lua void     SetServiceID(uint16_t idService) { m_Data16 = idService; }
    export_lua uint8_t GetServiceType() const { return m_Data.idServiceType; }
    export_lua void     SetServiceType(uint8_t idServiceType) { m_Data.idServiceType = idServiceType; }
    export_lua uint8_t GetServiceIdx() const { return m_Data.idServiceIdx; }
    export_lua void     SetServiceIdx(uint8_t idServiceIdx) { m_Data.idServiceIdx = idServiceIdx; }
    export_lua uint32_t GetData() const { return m_Data16; }
    export_lua void     SetData(uint32_t _Data16) { m_Data16 = _Data16; }

private:
    struct ST_SERVERID
    {
        uint8_t  m_idServiceType  = 0; //实际取值范围0~0xFF 255
        uint8_t  m_idServiceIdx   = 0; //实际取值范围0~0xFF 255
    };
    union {
        ST_SERVERID m_Data;
        uint16_t      m_Data16 = 0;
    };
};

export_lua class ServerPort
{
public:
    export_lua ServerPort(uint32_t nServerPort = 0)
        : m_Data32(nServerPort)
    {
    }

    export_lua ServerPort(uint16_t idWorld, uint8_t idServiceType, uint8_t idServiceIdx)
        : m_Data{idWorld, idServiceType, idServiceIdx}
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
    export_lua uint16_t GetServiceID() const { return MakeUINT16(m_Data.m_idServiceType,m_Data.m_idServiceIdx); }
    export_lua void     SetServiceID(uint16_t idService) { SplitUINT16(idService, m_Data.idServiceType, m_Data.idServiceIdx); }
    export_lua uint8_t GetServiceType() const { return m_Data.idServiceType; }
    export_lua void     SetServiceType(uint8_t idServiceType) { m_Data.idServiceType = idServiceType; }
    export_lua uint8_t GetServiceIdx() const { return m_Data.idServiceIdx; }
    export_lua void     SetServiceIdx(uint8_t idServiceIdx) { m_Data.idServiceIdx = idServiceIdx; }
    export_lua uint32_t GetData() const { return m_Data32; }
    export_lua void     SetData(uint32_t _Data32) { m_Data32 = _Data32; }

private:
    struct ST_SERVERPORT
    {
        uint16_t m_idWorld        = 0; //实际取值范围0~0xFFFF 65535
        uint8_t  m_idServiceType  = 0; //实际取值范围0~0xFF 255
        uint8_t  m_idServiceIdx   = 0; //实际取值范围0~0xFF 255
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

template<>
struct fmt::formatter<ServerPort> : public fmt::formatter<uint32_t>
{
    template<typename FormatContext>
    auto format(const ServerPort& port, FormatContext& ctx)
    {
        // ctx.out() is an output iterator to write to.
        return fmt::format_to(ctx.out(), "({}:{})", port.GetWorldID(), port.GetServiceID());
    }
};

export_lua class VirtualSocket
{
public:
    export_lua VirtualSocket(uint64_t nVirtualSocket = 0)
        : m_Data64(nVirtualSocket)
    {
    }

    export_lua VirtualSocket(const ServerPort& nServerPort)
        : m_Data{nServerPort.GetWorldID(), nServerPort.GetServiceType(), nServerPort.GetServiceIdx(), 0, 0}
    {
    }

    export_lua VirtualSocket(const ServerPort& nServerPort, uint16_t nSocketIdx)
        : m_Data{nServerPort.GetWorldID(), nServerPort.GetServiceType(), nServerPort.GetServiceIdx(), nSocketIdx, 0}
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

    export_lua ServerPort GetServerPort() const { return ServerPort{m_Data.m_idWorld, m_Data.m_idServiceType, m_Data.m_idService}; }
    export_lua void       SetServerPort(const ServerPort& val)
    {
        m_Data.m_idWorld   = val.GetWorldID();
        m_Data.m_idServiceType = val.GetServiceType();
        m_Data.m_idServiceIdx = val.GetServiceIdx();
    }
    export_lua uint16_t GetWorldID() const { return m_Data.m_idWorld; }
    export_lua uint16_t GetServiceID() const { return MakeUINT16(m_Data.m_idServiceType,m_Data.m_idServiceIdx); }
    export_lua uint8_t GetServiceType() const { return m_Data.m_idServiceType; }
    export_lua uint8_t GetServiceIdx() const { return m_Data.m_idServiceIdx; }

    export_lua uint16_t GetSocketIdx() const { return m_Data.m_nSocketIdx; }
    export_lua void     SetSocketIdx(uint16_t val) { m_Data.m_nSocketIdx = val; }
    export_lua uint64_t GetData64() const { return m_Data64; }

private:
    struct ST_VIRTUALSOCKET
    {
        uint16_t m_idWorld        = 0; //实际取值范围0~0xFFFF 65535
        uint8_t  m_idServiceType  = 0; //实际取值范围0~0xFF 255
        uint8_t  m_idServiceIdx   = 0; //实际取值范围0~0xFF 255
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

template<>
struct fmt::formatter<VirtualSocket>: public fmt::formatter<uint64_t>
{
    template<typename FormatContext>
    auto format(const VirtualSocket& vs, FormatContext& ctx)
    {
        // ctx.out() is an output iterator to write to.
        return fmt::format_to(ctx.out(), "({}:{}:{})", vs.GetWorldID(), vs.GetServiceID(), vs.GetSocketIdx());
    }
};

struct ServerAddrInfo
{
    ServerAddrInfo()                      = default;
    ServerAddrInfo(const ServerAddrInfo&) = default;
    ServerAddrInfo& operator=(const ServerAddrInfo&) = default;

    uint16_t    idWorld;
    uint8_t     idServiceType;
    uint8_t     idServiceIdx;
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
    COMMON_CMD_BEGIN     = 0,
    COMMON_CMD_SC_KEY    = 1,
    COMMON_CMD_PING      = 2,
    COMMON_CMD_PONG      = 3,
    COMMON_CMD_INTERRUPT = 9999,

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
