
#ifndef globaldb_H
#define globaldb_H
#include <string>

#include "BaseCode.h"
#include "DBField.h"

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

struct TBLD_GUILD
{
    static constexpr const char* table_name() { return "tbld_guild"; }
    enum FIELD_ENUMS
    {
        ID,           //'id'
        LEV,          //'帮派等级'
        NAME,         //'帮派名称'
        LEADER_ID,    //'帮主id'
        LEADER_NAME,  //'帮主名称'
        CREATOR_ID,   //'创建人ID'
        CREATOR_NAME, //'创建人名称'
        CREATE_TIME,  //'创帮时间'
        DEL_TIME,     //'删除时间'
        SLOGAN,       //'口号'
        INFO1,        //'信息'
        INFO2,        //'信息'
        INFO3,        //'信息'
        FLAG,         //'设置掩码'

    };

    static constexpr auto field_info()
    {
        return std::make_tuple(
            std::make_tuple("id", DB_FIELD_TYPE_LONGLONG_UNSIGNED, true, "  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id'"),
            std::make_tuple("lev", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false, "  `lev` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '帮派等级'"),
            std::make_tuple("name",
                            DB_FIELD_TYPE_VARCHAR,
                            false,
                            "  `name` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '帮派名称'"),
            std::make_tuple("leader_id",
                            DB_FIELD_TYPE_LONGLONG_UNSIGNED,
                            false,
                            "  `leader_id` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '帮主id'"),
            std::make_tuple("leader_name",
                            DB_FIELD_TYPE_VARCHAR,
                            false,
                            "  `leader_name` varchar(32) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '帮主名称'"),
            std::make_tuple("creator_id",
                            DB_FIELD_TYPE_LONGLONG_UNSIGNED,
                            false,
                            "  `creator_id` bigint(32) unsigned NOT NULL DEFAULT '0' COMMENT '创建人ID'"),
            std::make_tuple("creator_name",
                            DB_FIELD_TYPE_VARCHAR,
                            false,
                            "  `creator_name` varchar(32) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '创建人名称'"),
            std::make_tuple("create_time",
                            DB_FIELD_TYPE_LONG_UNSIGNED,
                            false,
                            "  `create_time` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '创帮时间'"),
            std::make_tuple("del_time", DB_FIELD_TYPE_LONG_UNSIGNED, false, "  `del_time` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '删除时间'"),
            std::make_tuple("slogan",
                            DB_FIELD_TYPE_VARCHAR,
                            false,
                            "  `slogan` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '口号'"),
            std::make_tuple("info1",
                            DB_FIELD_TYPE_VARCHAR,
                            false,
                            "  `info1` varchar(1024) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '信息'"),
            std::make_tuple("info2",
                            DB_FIELD_TYPE_VARCHAR,
                            false,
                            "  `info2` varchar(1024) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '信息'"),
            std::make_tuple("info3",
                            DB_FIELD_TYPE_VARCHAR,
                            false,
                            "  `info3` varchar(1024) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '信息'"),
            std::make_tuple("flag", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false, "  `flag` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '设置掩码'"));
    }

    using field_type_t = type_list<uint64_t,
                                   uint64_t,
                                   char[255],
                                   uint64_t,
                                   char[32],
                                   uint64_t,
                                   char[32],
                                   uint32_t,
                                   uint32_t,
                                   char[255],
                                   char[1024],
                                   char[1024],
                                   char[1024],
                                   uint64_t>;

    static constexpr size_t field_count() { return 14; }

    static constexpr auto keys_info() { return std::make_tuple(std::make_tuple("PRIMARY", "id")); }

    static constexpr size_t keys_size() { return 1; }

    static constexpr const char* create_sql()
    {
        return R"##(CREATE TABLE `tbld_guild` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
  `lev` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '帮派等级',
  `name` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '帮派名称',
  `leader_id` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '帮主id',
  `leader_name` varchar(32) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '帮主名称',
  `creator_id` bigint(32) unsigned NOT NULL DEFAULT '0' COMMENT '创建人ID',
  `creator_name` varchar(32) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '创建人名称',
  `create_time` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '创帮时间',
  `del_time` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '删除时间',
  `slogan` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '口号',
  `info1` varchar(1024) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '信息',
  `info2` varchar(1024) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '信息',
  `info3` varchar(1024) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '信息',
  `flag` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '设置掩码',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1)##";
    };
};

struct TBLD_GUILD_MEMBERINFO
{
    static constexpr const char* table_name() { return "tbld_guild_memberinfo"; }
    enum FIELD_ENUMS
    {
        ID,               //'id'
        GUILDID,          //'guildid'
        RANK,             //'帮派职位'
        SCORE,            //'帮派贡献'
        TOTAL_SCORE,      //'帮派贡献累计'
        JOIN_TIME,        //'入帮时间'
        LEVEL,            //'等级'
        NAME,             //'名字'
        LAST_LOGIN_TIME,  //'最后登录时间'
        LAST_LOGOUT_TIME, //'最后登出时间'

    };

    static constexpr auto field_info()
    {
        return std::make_tuple(
            std::make_tuple("id", DB_FIELD_TYPE_LONGLONG_UNSIGNED, true, "  `id` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT 'id'"),
            std::make_tuple("guildid",
                            DB_FIELD_TYPE_LONGLONG_UNSIGNED,
                            false,
                            "  `guildid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT 'guildid'"),
            std::make_tuple("rank", DB_FIELD_TYPE_LONG_UNSIGNED, false, "  `rank` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '帮派职位'"),
            std::make_tuple("score", DB_FIELD_TYPE_LONG_UNSIGNED, false, "  `score` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '帮派贡献'"),
            std::make_tuple("total_score",
                            DB_FIELD_TYPE_LONG_UNSIGNED,
                            false,
                            "  `total_score` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '帮派贡献累计'"),
            std::make_tuple("join_time",
                            DB_FIELD_TYPE_LONG_UNSIGNED,
                            false,
                            "  `join_time` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '入帮时间'"),
            std::make_tuple("level", DB_FIELD_TYPE_LONG_UNSIGNED, false, "  `level` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '等级'"),
            std::make_tuple("name", DB_FIELD_TYPE_LONG_UNSIGNED, false, "  `name` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '名字'"),
            std::make_tuple("last_login_time",
                            DB_FIELD_TYPE_LONG_UNSIGNED,
                            false,
                            "  `last_login_time` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '最后登录时间'"),
            std::make_tuple("last_logout_time",
                            DB_FIELD_TYPE_LONG_UNSIGNED,
                            false,
                            "  `last_logout_time` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '最后登出时间'"));
    }

    using field_type_t = type_list<uint64_t, uint64_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t>;

    static constexpr size_t field_count() { return 10; }

    static constexpr auto keys_info() { return std::make_tuple(std::make_tuple("PRIMARY", "id")); }

    static constexpr size_t keys_size() { return 1; }

    static constexpr const char* create_sql()
    {
        return R"##(CREATE TABLE `tbld_guild_memberinfo` (
  `id` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT 'id',
  `guildid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT 'guildid',
  `rank` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '帮派职位',
  `score` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '帮派贡献',
  `total_score` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '帮派贡献累计',
  `join_time` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '入帮时间',
  `level` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '等级',
  `name` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '名字',
  `last_login_time` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '最后登录时间',
  `last_logout_time` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '最后登出时间',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1)##";
    };
};

using GLOBALDB_TABLE_LIST = type_list<TBLD_GMLIST, TBLD_GLOBALVAR, TBLD_GUILD, TBLD_GUILD_MEMBERINFO>;

#endif
