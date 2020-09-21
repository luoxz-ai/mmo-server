
#ifndef globaldb_H
#define globaldb_H
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
        return std::make_tuple(
            std::make_tuple("worldid", DB_FIELD_TYPE_LONG_UNSIGNED, true, "  `worldid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '游戏服编号'"),
            std::make_tuple("db_ip",
                            DB_FIELD_TYPE_VARCHAR,
                            false,
                            "  `db_ip` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '游戏数据mysql地址'"),
            std::make_tuple("db_port",
                            DB_FIELD_TYPE_SHORT_UNSIGNED,
                            false,
                            "  `db_port` smallint(11) unsigned NOT NULL DEFAULT '0' COMMENT '游戏数据mysql端口'"),
            std::make_tuple("db_name",
                            DB_FIELD_TYPE_VARCHAR,
                            false,
                            "  `db_name` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '游戏数据mysql表名'"),
            std::make_tuple("db_user",
                            DB_FIELD_TYPE_VARCHAR,
                            false,
                            "  `db_user` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '游戏数据mysql用户'"),
            std::make_tuple("db_passwd",
                            DB_FIELD_TYPE_VARCHAR,
                            false,
                            "  `db_passwd` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '游戏数据mysql密码'"));
    }

    using field_type_t = type_list<uint32_t, char[255], uint16_t, char[255], char[255], char[255]>;

    static constexpr size_t field_count() { return 6; }

    static constexpr auto keys_info() { return std::make_tuple(std::make_tuple("PRIMARY", "worldid")); }

    static constexpr size_t keys_size() { return 1; }

    static constexpr const char* create_sql()
    {
        return R"##(CREATE TABLE `tbld_dbinfo` (
  `worldid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '游戏服编号',
  `db_ip` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '游戏数据mysql地址',
  `db_port` smallint(11) unsigned NOT NULL DEFAULT '0' COMMENT '游戏数据mysql端口',
  `db_name` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '游戏数据mysql表名',
  `db_user` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '游戏数据mysql用户',
  `db_passwd` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '游戏数据mysql密码',
  PRIMARY KEY (`worldid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1)##";
    };
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
        return std::make_tuple(
            std::make_tuple("key", DB_FIELD_TYPE_LONG_UNSIGNED, true, "  `key` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'key'"),
            std::make_tuple("value0", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false, "  `value0` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT 'value'"),
            std::make_tuple("value1", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false, "  `value1` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT 'value'"),
            std::make_tuple("value2", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false, "  `value2` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT 'value'"),
            std::make_tuple("value3", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false, "  `value3` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT 'value'"),
            std::make_tuple("value_str0",
                            DB_FIELD_TYPE_VARCHAR,
                            false,
                            "  `value_str0` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT 'value_str'"),
            std::make_tuple("value_str1",
                            DB_FIELD_TYPE_VARCHAR,
                            false,
                            "  `value_str1` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT 'value_str'"),
            std::make_tuple("value_str2",
                            DB_FIELD_TYPE_VARCHAR,
                            false,
                            "  `value_str2` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT 'value_str'"),
            std::make_tuple("value_str3",
                            DB_FIELD_TYPE_VARCHAR,
                            false,
                            "  `value_str3` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT 'value_str'"));
    }

    using field_type_t = type_list<uint32_t, uint64_t, uint64_t, uint64_t, uint64_t, char[255], char[255], char[255], char[255]>;

    static constexpr size_t field_count() { return 9; }

    static constexpr auto keys_info() { return std::make_tuple(std::make_tuple("PRIMARY", "key")); }

    static constexpr size_t keys_size() { return 1; }

    static constexpr const char* create_sql()
    {
        return R"##(CREATE TABLE `tbld_globalvar` (
  `key` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'key',
  `value0` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT 'value',
  `value1` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT 'value',
  `value2` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT 'value',
  `value3` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT 'value',
  `value_str0` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT 'value_str',
  `value_str1` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT 'value_str',
  `value_str2` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT 'value_str',
  `value_str3` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT 'value_str',
  PRIMARY KEY (`key`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1)##";
    };
};

struct TBLD_GMLIST
{
    static constexpr const char* table_name() { return "tbld_gmlist"; }
    enum FIELD_ENUMS
    {
        ID,     //'id'
        OPENID, //'openid'
        GMLEV,  //'gm等级'

    };

    static constexpr auto field_info()
    {
        return std::make_tuple(
            std::make_tuple("id", DB_FIELD_TYPE_LONGLONG_UNSIGNED, true, "  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id'"),
            std::make_tuple("openid",
                            DB_FIELD_TYPE_VARCHAR,
                            false,
                            "  `openid` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT 'openid'"),
            std::make_tuple("gmlev", DB_FIELD_TYPE_LONG_UNSIGNED, false, "  `gmlev` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'gm等级'"));
    }

    using field_type_t = type_list<uint64_t, char[255], uint32_t>;

    static constexpr size_t field_count() { return 3; }

    static constexpr auto keys_info() { return std::make_tuple(std::make_tuple("PRIMARY", "id")); }

    static constexpr size_t keys_size() { return 1; }

    static constexpr const char* create_sql()
    {
        return R"##(CREATE TABLE `tbld_gmlist` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
  `openid` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT 'openid',
  `gmlev` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'gm等级',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1)##";
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
        return std::make_tuple(std::make_tuple("worldid",
                                               DB_FIELD_TYPE_SHORT_UNSIGNED,
                                               true,
                                               "  `worldid` smallint(11) unsigned NOT NULL DEFAULT '0' COMMENT '游戏服编号'"),
                               std::make_tuple("server_name",
                                               DB_FIELD_TYPE_VARCHAR,
                                               false,
                                               "  `server_name` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '服务器名'"),
                               std::make_tuple("open_time",
                                               DB_FIELD_TYPE_LONG_UNSIGNED,
                                               false,
                                               "  `open_time` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '开服时间戳'"),
                               std::make_tuple("open_status",
                                               DB_FIELD_TYPE_LONG_UNSIGNED,
                                               false,
                                               "  `open_status` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '开放状态'"),
                               std::make_tuple("publish_addr",
                                               DB_FIELD_TYPE_VARCHAR,
                                               false,
                                               "  `publish_addr` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '入口地址'"),
                               std::make_tuple("mergeto",
                                               DB_FIELD_TYPE_SHORT_UNSIGNED,
                                               false,
                                               "  `mergeto` smallint(11) unsigned NOT NULL DEFAULT '0' COMMENT '已经被合服到哪个服了'"));
    }

    using field_type_t = type_list<uint16_t, char[255], uint32_t, uint32_t, char[255], uint16_t>;

    static constexpr size_t field_count() { return 6; }

    static constexpr auto keys_info() { return std::make_tuple(std::make_tuple("PRIMARY", "worldid")); }

    static constexpr size_t keys_size() { return 1; }

    static constexpr const char* create_sql()
    {
        return R"##(CREATE TABLE `tbld_serverlist` (
  `worldid` smallint(11) unsigned NOT NULL DEFAULT '0' COMMENT '游戏服编号',
  `server_name` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '服务器名',
  `open_time` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '开服时间戳',
  `open_status` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '开放状态',
  `publish_addr` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '入口地址',
  `mergeto` smallint(11) unsigned NOT NULL DEFAULT '0' COMMENT '已经被合服到哪个服了',
  PRIMARY KEY (`worldid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1)##";
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
        return std::make_tuple(std::make_tuple("worldid",
                                               DB_FIELD_TYPE_SHORT_UNSIGNED,
                                               true,
                                               "  `worldid` smallint(11) unsigned NOT NULL DEFAULT '0' COMMENT '游戏服编号'"),
                               std::make_tuple("service_type",
                                               DB_FIELD_TYPE_TINY_UNSIGNED,
                                               true,
                                               "  `service_type` tinyint(11) unsigned NOT NULL DEFAULT '0' COMMENT '服务类型'"),
                               std::make_tuple("service_idx",
                                               DB_FIELD_TYPE_TINY_UNSIGNED,
                                               true,
                                               "  `service_idx` tinyint(11) unsigned NOT NULL DEFAULT '0' COMMENT '服务编号'"),
                               std::make_tuple("lib_name",
                                               DB_FIELD_TYPE_VARCHAR,
                                               false,
                                               "  `lib_name` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT 'lib库名'"),
                               std::make_tuple("route_addr",
                                               DB_FIELD_TYPE_VARCHAR,
                                               false,
                                               "  `route_addr` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '内网通讯地址'"),
                               std::make_tuple("route_port",
                                               DB_FIELD_TYPE_SHORT_UNSIGNED,
                                               false,
                                               "  `route_port` smallint(11) unsigned NOT NULL DEFAULT '0' COMMENT '内网通讯端口'"),
                               std::make_tuple("publish_addr",
                                               DB_FIELD_TYPE_VARCHAR,
                                               false,
                                               "  `publish_addr` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '外网地址'"),
                               std::make_tuple("publish_port",
                                               DB_FIELD_TYPE_SHORT_UNSIGNED,
                                               false,
                                               "  `publish_port` smallint(11) unsigned NOT NULL DEFAULT '0' COMMENT '外网端口'"),
                               std::make_tuple("debug_port",
                                               DB_FIELD_TYPE_SHORT_UNSIGNED,
                                               false,
                                               "  `debug_port` smallint(11) unsigned NOT NULL DEFAULT '0' COMMENT 'debug端口'"),
                               std::make_tuple("bind_addr",
                                               DB_FIELD_TYPE_VARCHAR,
                                               false,
                                               "  `bind_addr` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '监听绑定地址'"));
    }

    using field_type_t = type_list<uint16_t, uint8_t, uint8_t, char[255], char[255], uint16_t, char[255], uint16_t, uint16_t, char[255]>;

    static constexpr size_t field_count() { return 10; }

    static constexpr auto keys_info() { return std::make_tuple(std::make_tuple("PRIMARY", "worldid,service_type,service_idx")); }

    static constexpr size_t keys_size() { return 1; }

    static constexpr const char* create_sql()
    {
        return R"##(CREATE TABLE `tbld_servicedetail` (
  `worldid` smallint(11) unsigned NOT NULL DEFAULT '0' COMMENT '游戏服编号',
  `service_type` tinyint(11) unsigned NOT NULL DEFAULT '0' COMMENT '服务类型',
  `service_idx` tinyint(11) unsigned NOT NULL DEFAULT '0' COMMENT '服务编号',
  `lib_name` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT 'lib库名',
  `route_addr` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '内网通讯地址',
  `route_port` smallint(11) unsigned NOT NULL DEFAULT '0' COMMENT '内网通讯端口',
  `publish_addr` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '外网地址',
  `publish_port` smallint(11) unsigned NOT NULL DEFAULT '0' COMMENT '外网端口',
  `debug_port` smallint(11) unsigned NOT NULL DEFAULT '0' COMMENT 'debug端口',
  `bind_addr` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '监听绑定地址',
  PRIMARY KEY (`worldid`,`service_type`,`service_idx`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1)##";
    };
};

using GLOBALDB_TABLE_LIST = type_list<TBLD_DBINFO, TBLD_GLOBALVAR, TBLD_GMLIST, TBLD_SERVERLIST, TBLD_SERVICEDETAIL>;

#endif
