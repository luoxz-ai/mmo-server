#pragma once
#include <string>

#include "BaseCode.h"
#include "DBField.h"

struct TBLD_DBINFO
{
    static constexpr const char* table_name = "tbld_dbinfo";
    enum FIELD_ENUMS
    {
        WORLDID,   //'游戏服编号'
        DB_IP,     //'游戏数据mysql地址'
        DB_PORT,   //'游戏数据mysql端口'
        DB_NAME,   //'游戏数据mysql表名'
        DB_USER,   //'游戏数据mysql用户'
        DB_PASSWD, //'游戏数据mysql密码'

    };
    static constexpr const char* field_name[] = {"worldid", "db_ip", "db_port", "db_name", "db_user", "db_passwd"};
    using field_type_t = type_list<uint16_t, char[255], uint32_t, char[255], char[255], char[255]>;
    static constexpr DB_FIELD_TYPES field_type_enum_list[] = {DB_FIELD_TYPE_SHORT_UNSIGNED,
                                                              DB_FIELD_TYPE_VARCHAR,
                                                              DB_FIELD_TYPE_LONG_UNSIGNED,
                                                              DB_FIELD_TYPE_VARCHAR,
                                                              DB_FIELD_TYPE_VARCHAR,
                                                              DB_FIELD_TYPE_VARCHAR};
    static constexpr bool           pri_key_idx[]          = {true, false, false, false, false, false};
};

struct TBLD_GLOBALVAR
{
    static constexpr const char* table_name = "tbld_globalvar";
    enum FIELD_ENUMS
    {
        ID,        //'id'
        KEY,       //'key'
        VALUE,     //'value'
        VALUE_STR, //'value_str'

    };
    static constexpr const char* field_name[]              = {"id", "key", "value", "value_str"};
    using field_type_t                                     = type_list<uint64_t, char[255], uint32_t, char[1024]>;
    static constexpr DB_FIELD_TYPES field_type_enum_list[] = {DB_FIELD_TYPE_LONGLONG_UNSIGNED,
                                                              DB_FIELD_TYPE_VARCHAR,
                                                              DB_FIELD_TYPE_LONG_UNSIGNED,
                                                              DB_FIELD_TYPE_VARCHAR};
    static constexpr bool           pri_key_idx[]          = {true, false, false, false};
};

struct TBLD_GMLIST
{
    static constexpr const char* table_name = "tbld_gmlist";
    enum FIELD_ENUMS
    {
        ID,     //'id'
        OPENID, //'openid'
        GMLEV,  //'gm等级'

    };
    static constexpr const char* field_name[]              = {"id", "openid", "gmlev"};
    using field_type_t                                     = type_list<uint64_t, char[255], uint32_t>;
    static constexpr DB_FIELD_TYPES field_type_enum_list[] = {DB_FIELD_TYPE_LONGLONG_UNSIGNED,
                                                              DB_FIELD_TYPE_VARCHAR,
                                                              DB_FIELD_TYPE_LONG_UNSIGNED};
    static constexpr bool           pri_key_idx[]          = {true, false, false};
};

struct TBLD_SERVERLIST
{
    static constexpr const char* table_name = "tbld_serverlist";
    enum FIELD_ENUMS
    {
        WORLDID,      //'游戏服编号'
        SERVER_NAME,  //'服务器名'
        OPEN_TIME,    //'开服时间戳'
        OPEN_STATUS,  //'开放状态'
        PUBLISH_ADDR, //'入口地址'
        MERGETO,      //'已经被合服到哪个服了'

    };
    static constexpr const char* field_name[] =
        {"worldid", "server_name", "open_time", "open_status", "publish_addr", "mergeto"};
    using field_type_t = type_list<uint16_t, char[255], int64_t, uint32_t, char[1024], uint16_t>;
    static constexpr DB_FIELD_TYPES field_type_enum_list[] = {DB_FIELD_TYPE_SHORT_UNSIGNED,
                                                              DB_FIELD_TYPE_VARCHAR,
                                                              DB_FIELD_TYPE_LONGLONG,
                                                              DB_FIELD_TYPE_LONG_UNSIGNED,
                                                              DB_FIELD_TYPE_VARCHAR,
                                                              DB_FIELD_TYPE_SHORT_UNSIGNED};
    static constexpr bool           pri_key_idx[]          = {true, false, false, false, false, false};
};

struct TBLD_SERVICEDETAIL
{
    static constexpr const char* table_name = "tbld_servicedetail";
    enum FIELD_ENUMS
    {
        WORLDID,      //'服编号'
        SERVICEID,    //'服务编号'
        SERVICE_TYPE, //'lib库名'
        ROUTE_ADDR,   //'内网通讯地址'
        ROUTE_PORT,   //'内网通讯端口'
        PUBLISH_ADDR, //'外网地址'
        PUBLISH_PORT, //'外网端口'
        DEBUG_PORT,   //'debug端口'
        BIND_ADDR,    //'监听绑定地址'

    };
    static constexpr const char* field_name[] = {"worldid",
                                                 "serviceid",
                                                 "service_type",
                                                 "route_addr",
                                                 "route_port",
                                                 "publish_addr",
                                                 "publish_port",
                                                 "debug_port",
                                                 "bind_addr"};
    using field_type_t =
        type_list<uint16_t, uint16_t, char[255], char[255], uint16_t, char[255], uint16_t, uint16_t, char[255]>;
    static constexpr DB_FIELD_TYPES field_type_enum_list[] = {DB_FIELD_TYPE_SHORT_UNSIGNED,
                                                              DB_FIELD_TYPE_SHORT_UNSIGNED,
                                                              DB_FIELD_TYPE_VARCHAR,
                                                              DB_FIELD_TYPE_VARCHAR,
                                                              DB_FIELD_TYPE_SHORT_UNSIGNED,
                                                              DB_FIELD_TYPE_VARCHAR,
                                                              DB_FIELD_TYPE_SHORT_UNSIGNED,
                                                              DB_FIELD_TYPE_SHORT_UNSIGNED,
                                                              DB_FIELD_TYPE_VARCHAR};
    static constexpr bool           pri_key_idx[] = {true, true, false, false, false, false, false, false, false};
};
