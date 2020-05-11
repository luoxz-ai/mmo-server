#pragma once
#include <string>

#include "BaseCode.h"
#include "DBField.h"

struct TBLD_DBINFO
{
    static constexpr const char* table_name() { return "tbld_dbinfo"; }
    enum FIELD_ENUMS
    {
        WORLDID,   //'游戏服编号'
        DB_IP,     //'游戏数据mysql地址'
        DB_PORT,   //'游戏数据mysql端口'
        DB_NAME,   //'游戏数据mysql表名'
        DB_USER,   //'游戏数据mysql用户'
        DB_PASSWD, //'游戏数据mysql密码'

    };

    static constexpr auto field_info()
    {
        return std::make_tuple(std::make_tuple("worldid", DB_FIELD_TYPE_LONG_UNSIGNED, true),
                               std::make_tuple("db_ip", DB_FIELD_TYPE_VARCHAR, false),
                               std::make_tuple("db_port", DB_FIELD_TYPE_SHORT_UNSIGNED, false),
                               std::make_tuple("db_name", DB_FIELD_TYPE_VARCHAR, false),
                               std::make_tuple("db_user", DB_FIELD_TYPE_VARCHAR, false),
                               std::make_tuple("db_passwd", DB_FIELD_TYPE_VARCHAR, false));
    }

    using field_type_t = type_list<uint32_t, char[255], uint16_t, char[255], char[255], char[255]>;

    static constexpr size_t field_count() { return 6; }
};

struct TBLD_GLOBALVAR
{
    static constexpr const char* table_name() { return "tbld_globalvar"; }
    enum FIELD_ENUMS
    {
        KEY,        //'key'
        VALUE0,     //'value'
        VALUE1,     //'value'
        VALUE2,     //'value'
        VALUE3,     //'value'
        VALUE_STR0, //'value_str'
        VALUE_STR1, //'value_str'
        VALUE_STR2, //'value_str'
        VALUE_STR3, //'value_str'

    };

    static constexpr auto field_info()
    {
        return std::make_tuple(std::make_tuple("key", DB_FIELD_TYPE_LONG_UNSIGNED, true),
                               std::make_tuple("value0", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
                               std::make_tuple("value1", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
                               std::make_tuple("value2", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
                               std::make_tuple("value3", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
                               std::make_tuple("value_str0", DB_FIELD_TYPE_VARCHAR, false),
                               std::make_tuple("value_str1", DB_FIELD_TYPE_VARCHAR, false),
                               std::make_tuple("value_str2", DB_FIELD_TYPE_VARCHAR, false),
                               std::make_tuple("value_str3", DB_FIELD_TYPE_VARCHAR, false));
    }

    using field_type_t =
        type_list<uint32_t, uint64_t, uint64_t, uint64_t, uint64_t, char[255], char[255], char[255], char[255]>;

    static constexpr size_t field_count() { return 9; }
};

struct TBLD_GMLIST
{
    static constexpr const char* table_name() { return "tbld_gmlist"; }
    enum FIELD_ENUMS
    {
        ID,     //''
        OPENID, //'openid'
        GMLEV,  //'gm等级'

    };

    static constexpr auto field_info()
    {
        return std::make_tuple(std::make_tuple("id", DB_FIELD_TYPE_LONGLONG_UNSIGNED, true),
                               std::make_tuple("openid", DB_FIELD_TYPE_VARCHAR, false),
                               std::make_tuple("gmlev", DB_FIELD_TYPE_LONG_UNSIGNED, false));
    }

    using field_type_t = type_list<uint64_t, char[255], uint32_t>;

    static constexpr size_t field_count() { return 3; }
};

struct TBLD_SERVERLIST
{
    static constexpr const char* table_name() { return "tbld_serverlist"; }
    enum FIELD_ENUMS
    {
        WORLDID,      //'游戏服编号'
        SERVER_NAME,  //'服务器名'
        OPEN_TIME,    //'开服时间戳'
        OPEN_STATUS,  //'开放状态'
        PUBLISH_ADDR, //'入口地址'
        MERGETO,      //'已经被合服到哪个服了'

    };

    static constexpr auto field_info()
    {
        return std::make_tuple(std::make_tuple("worldid", DB_FIELD_TYPE_SHORT_UNSIGNED, true),
                               std::make_tuple("server_name", DB_FIELD_TYPE_VARCHAR, false),
                               std::make_tuple("open_time", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("open_status", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("publish_addr", DB_FIELD_TYPE_VARCHAR, false),
                               std::make_tuple("mergeto", DB_FIELD_TYPE_SHORT_UNSIGNED, false));
    }

    using field_type_t = type_list<uint16_t, char[255], uint32_t, uint32_t, char[255], uint16_t>;

    static constexpr size_t field_count() { return 6; }
};

struct TBLD_SERVICEDETAIL
{
    static constexpr const char* table_name() { return "tbld_servicedetail"; }
    enum FIELD_ENUMS
    {
        WORLDID,      //'游戏服编号'
        SERVICEID,    //'服务编号'
        SERVICE_TYPE, //'lib库名'
        ROUTE_ADDR,   //'内网通讯地址'
        ROUTE_PORT,   //'内网通讯端口'
        PUBLISH_ADDR, //'外网地址'
        PUBLISH_PORT, //'外网端口'
        DEBUG_PORT,   //'debug端口'
        BIND_ADDR,    //'监听绑定地址'

    };

    static constexpr auto field_info()
    {
        return std::make_tuple(std::make_tuple("worldid", DB_FIELD_TYPE_SHORT_UNSIGNED, true),
                               std::make_tuple("serviceid", DB_FIELD_TYPE_SHORT_UNSIGNED, true),
                               std::make_tuple("service_type", DB_FIELD_TYPE_VARCHAR, false),
                               std::make_tuple("route_addr", DB_FIELD_TYPE_VARCHAR, false),
                               std::make_tuple("route_port", DB_FIELD_TYPE_SHORT_UNSIGNED, false),
                               std::make_tuple("publish_addr", DB_FIELD_TYPE_VARCHAR, false),
                               std::make_tuple("publish_port", DB_FIELD_TYPE_SHORT_UNSIGNED, false),
                               std::make_tuple("debug_port", DB_FIELD_TYPE_SHORT_UNSIGNED, false),
                               std::make_tuple("bind_addr", DB_FIELD_TYPE_VARCHAR, false));
    }

    using field_type_t =
        type_list<uint16_t, uint16_t, char[255], char[255], uint16_t, char[255], uint16_t, uint16_t, char[255]>;

    static constexpr size_t field_count() { return 9; }
};
