
#ifndef serverinfodb_H
#define serverinfodb_H
#include <string>

#include "BaseCode.h"
#include "DBField.h"

struct TBLD_DBINFO
{
    static constexpr const char* table_name() { return "tbld_dbinfo"; }
    enum FIELD_ENUMS
    {
        WORLDID, //'游戏服编号'
        DB_URL,  //'游戏数据mysql地址'

    };

    static constexpr auto field_info()
    {
        return std::make_tuple(
            std::make_tuple("worldid", "  `worldid` int unsigned NOT NULL DEFAULT '0' COMMENT '游戏服编号'", DB_FIELD_TYPE_LONG_UNSIGNED, true),
            std::make_tuple(
                "db_url",
                "  `db_url` varchar(2048) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '游戏数据mysql地址'",
                DB_FIELD_TYPE_VARCHAR,
                false));
    }

    using field_type_t = type_list<uint32_t, char[2048]>;

    static constexpr size_t field_count() { return 2; }

    static constexpr auto keys_info() { return std::make_tuple(std::make_tuple("PRIMARY", "worldid")); }

    static constexpr size_t keys_size() { return 1; }

    static constexpr const char* create_sql()
    {
        return R"##(CREATE TABLE `tbld_dbinfo` (
  `worldid` int unsigned NOT NULL DEFAULT '0' COMMENT '游戏服编号',
  `db_url` varchar(2048) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '游戏数据mysql地址',
  PRIMARY KEY (`worldid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci)##";
    };
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
        return std::make_tuple(
            std::make_tuple("worldid", "  `worldid` int unsigned NOT NULL DEFAULT '0' COMMENT '游戏服编号'", DB_FIELD_TYPE_LONG_UNSIGNED, true),
            std::make_tuple("server_name",
                            "  `server_name` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '服务器名'",
                            DB_FIELD_TYPE_VARCHAR,
                            false),
            std::make_tuple("open_time",
                            "  `open_time` bigint unsigned NOT NULL DEFAULT '0' COMMENT '开服时间戳'",
                            DB_FIELD_TYPE_LONGLONG_UNSIGNED,
                            false),
            std::make_tuple("open_status",
                            "  `open_status` int unsigned NOT NULL DEFAULT '0' COMMENT '开放状态'",
                            DB_FIELD_TYPE_LONG_UNSIGNED,
                            false),
            std::make_tuple("publish_addr",
                            "  `publish_addr` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '入口地址'",
                            DB_FIELD_TYPE_VARCHAR,
                            false),
            std::make_tuple("mergeto",
                            "  `mergeto` smallint unsigned NOT NULL DEFAULT '0' COMMENT '已经被合服到哪个服了'",
                            DB_FIELD_TYPE_SHORT_UNSIGNED,
                            false));
    }

    using field_type_t = type_list<uint32_t, char[255], uint64_t, uint32_t, char[255], uint16_t>;

    static constexpr size_t field_count() { return 6; }

    static constexpr auto keys_info() { return std::make_tuple(std::make_tuple("PRIMARY", "worldid")); }

    static constexpr size_t keys_size() { return 1; }

    static constexpr const char* create_sql()
    {
        return R"##(CREATE TABLE `tbld_serverlist` (
  `worldid` int unsigned NOT NULL DEFAULT '0' COMMENT '游戏服编号',
  `server_name` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '服务器名',
  `open_time` bigint unsigned NOT NULL DEFAULT '0' COMMENT '开服时间戳',
  `open_status` int unsigned NOT NULL DEFAULT '0' COMMENT '开放状态',
  `publish_addr` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '入口地址',
  `mergeto` smallint unsigned NOT NULL DEFAULT '0' COMMENT '已经被合服到哪个服了',
  PRIMARY KEY (`worldid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci)##";
    };
};

struct TBLD_SERVICEDETAIL
{
    static constexpr const char* table_name() { return "tbld_servicedetail"; }
    enum FIELD_ENUMS
    {
        WORLDID,      //'游戏服编号'
        SERVICE_TYPE, //'服务类型'
        SERVICE_IDX,  //'服务编号'
        LIB_NAME,     //'lib库名'
        ROUTE_ADDR,   //'内网通讯地址'
        ROUTE_PORT,   //'内网通讯端口'
        PUBLISH_ADDR, //'外网地址'
        PUBLISH_PORT, //'外网端口'
        DEBUG_PORT,   //'debug端口'
        BIND_ADDR,    //'监听绑定地址'

    };

    static constexpr auto field_info()
    {
        return std::make_tuple(
            std::make_tuple("worldid", "  `worldid` int unsigned NOT NULL DEFAULT '0' COMMENT '游戏服编号'", DB_FIELD_TYPE_LONG_UNSIGNED, true),
            std::make_tuple("service_type",
                            "  `service_type` int unsigned NOT NULL DEFAULT '0' COMMENT '服务类型'",
                            DB_FIELD_TYPE_LONG_UNSIGNED,
                            true),
            std::make_tuple("service_idx", "  `service_idx` int unsigned NOT NULL DEFAULT '0' COMMENT '服务编号'", DB_FIELD_TYPE_LONG_UNSIGNED, true),
            std::make_tuple("lib_name",
                            "  `lib_name` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT 'lib库名'",
                            DB_FIELD_TYPE_VARCHAR,
                            false),
            std::make_tuple("route_addr",
                            "  `route_addr` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '内网通讯地址'",
                            DB_FIELD_TYPE_VARCHAR,
                            false),
            std::make_tuple("route_port",
                            "  `route_port` int unsigned NOT NULL DEFAULT '0' COMMENT '内网通讯端口'",
                            DB_FIELD_TYPE_LONG_UNSIGNED,
                            false),
            std::make_tuple("publish_addr",
                            "  `publish_addr` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '外网地址'",
                            DB_FIELD_TYPE_VARCHAR,
                            false),
            std::make_tuple("publish_port",
                            "  `publish_port` int unsigned NOT NULL DEFAULT '0' COMMENT '外网端口'",
                            DB_FIELD_TYPE_LONG_UNSIGNED,
                            false),
            std::make_tuple("debug_port", "  `debug_port` int unsigned NOT NULL DEFAULT '0' COMMENT 'debug端口'", DB_FIELD_TYPE_LONG_UNSIGNED, false),
            std::make_tuple("bind_addr",
                            "  `bind_addr` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '监听绑定地址'",
                            DB_FIELD_TYPE_VARCHAR,
                            false));
    }

    using field_type_t = type_list<uint32_t, uint32_t, uint32_t, char[255], char[255], uint32_t, char[255], uint32_t, uint32_t, char[255]>;

    static constexpr size_t field_count() { return 10; }

    static constexpr auto keys_info() { return std::make_tuple(std::make_tuple("PRIMARY", "worldid,service_type,service_idx")); }

    static constexpr size_t keys_size() { return 1; }

    static constexpr const char* create_sql()
    {
        return R"##(CREATE TABLE `tbld_servicedetail` (
  `worldid` int unsigned NOT NULL DEFAULT '0' COMMENT '游戏服编号',
  `service_type` int unsigned NOT NULL DEFAULT '0' COMMENT '服务类型',
  `service_idx` int unsigned NOT NULL DEFAULT '0' COMMENT '服务编号',
  `lib_name` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT 'lib库名',
  `route_addr` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '内网通讯地址',
  `route_port` int unsigned NOT NULL DEFAULT '0' COMMENT '内网通讯端口',
  `publish_addr` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '外网地址',
  `publish_port` int unsigned NOT NULL DEFAULT '0' COMMENT '外网端口',
  `debug_port` int unsigned NOT NULL DEFAULT '0' COMMENT 'debug端口',
  `bind_addr` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '监听绑定地址',
  PRIMARY KEY (`worldid`,`service_type`,`service_idx`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci)##";
    };
};

using SERVERINFODB_TABLE_LIST = type_list<TBLD_DBINFO, TBLD_SERVERLIST, TBLD_SERVICEDETAIL>;

#endif
