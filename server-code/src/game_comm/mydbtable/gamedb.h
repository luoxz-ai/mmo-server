
#ifndef gamedb_H
#define gamedb_H
#include <string>

#include "BaseCode.h"
#include "DBField.h"

struct TBLD_COMMONDATA
{
    static constexpr const char* table_name() { return "tbld_commondata"; }
    enum FIELD_ENUMS
    {
        ID,       //'id'
        PLAYERID, //'玩家id'
        KEYTYPE,  //'数据类型'
        KEYIDX,   //'数据编号'
        DATA0,    //'数据1'
        DATA1,    //'数据2'
        DATA2,    //'数据3'
        DATA3,    //'数据4'

    };

    static constexpr auto field_info()
    {
        return std::make_tuple(
            std::make_tuple("id", DB_FIELD_TYPE_LONGLONG_UNSIGNED, true, "  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id'"),
            std::make_tuple("playerid",
                            DB_FIELD_TYPE_LONGLONG_UNSIGNED,
                            false,
                            "  `playerid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '玩家id'"),
            std::make_tuple("keytype", DB_FIELD_TYPE_LONG_UNSIGNED, false, "  `keytype` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '数据类型'"),
            std::make_tuple("keyidx", DB_FIELD_TYPE_LONG_UNSIGNED, false, "  `keyidx` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '数据编号'"),
            std::make_tuple("data0", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false, "  `data0` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '数据1'"),
            std::make_tuple("data1", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false, "  `data1` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '数据2'"),
            std::make_tuple("data2", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false, "  `data2` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '数据3'"),
            std::make_tuple("data3", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false, "  `data3` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '数据4'"));
    }

    using field_type_t = type_list<uint64_t, uint64_t, uint32_t, uint32_t, uint64_t, uint64_t, uint64_t, uint64_t>;

    static constexpr size_t field_count() { return 8; }

    static constexpr auto keys_info() { return std::make_tuple(std::make_tuple("idx_player", "playerid"), std::make_tuple("PRIMARY", "id")); }

    static constexpr size_t keys_size() { return 2; }

    static constexpr const char* create_sql()
    {
        return R"##(CREATE TABLE `tbld_commondata` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
  `playerid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '玩家id',
  `keytype` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '数据类型',
  `keyidx` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '数据编号',
  `data0` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '数据1',
  `data1` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '数据2',
  `data2` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '数据3',
  `data3` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '数据4',
  PRIMARY KEY (`id`),
  KEY `idx_player` (`playerid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1)##";
    };
};

struct TBLD_COOLDOWN
{
    static constexpr const char* table_name() { return "tbld_cooldown"; }
    enum FIELD_ENUMS
    {
        ID,          //'id'
        PLAYERID,    //'玩家id'
        KEYTYPE,     //'CD类型'
        KEYIDX,      //'CD编号'
        EXPIRE_TIME, //'CD结束时间戳'

    };

    static constexpr auto field_info()
    {
        return std::make_tuple(
            std::make_tuple("id", DB_FIELD_TYPE_LONGLONG_UNSIGNED, true, "  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id'"),
            std::make_tuple("playerid",
                            DB_FIELD_TYPE_LONGLONG_UNSIGNED,
                            false,
                            "  `playerid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '玩家id'"),
            std::make_tuple("keytype", DB_FIELD_TYPE_LONG_UNSIGNED, false, "  `keytype` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'CD类型'"),
            std::make_tuple("keyidx", DB_FIELD_TYPE_LONG_UNSIGNED, false, "  `keyidx` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'CD编号'"),
            std::make_tuple("expire_time",
                            DB_FIELD_TYPE_LONG_UNSIGNED,
                            false,
                            "  `expire_time` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'CD结束时间戳'"));
    }

    using field_type_t = type_list<uint64_t, uint64_t, uint32_t, uint32_t, uint32_t>;

    static constexpr size_t field_count() { return 5; }

    static constexpr auto keys_info()
    {
        return std::make_tuple(std::make_tuple("idx_player", "playerid"),
                               std::make_tuple("PRIMARY", "id"),
                               std::make_tuple("idx_player_idx", "playerid,keytype,keyidx"));
    }

    static constexpr size_t keys_size() { return 3; }

    static constexpr const char* create_sql()
    {
        return R"##(CREATE TABLE `tbld_cooldown` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
  `playerid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '玩家id',
  `keytype` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'CD类型',
  `keyidx` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'CD编号',
  `expire_time` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'CD结束时间戳',
  PRIMARY KEY (`id`),
  KEY `idx_player_idx` (`playerid`,`keytype`,`keyidx`),
  KEY `idx_player` (`playerid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1)##";
    };
};

struct TBLD_DATACOUNT
{
    static constexpr const char* table_name() { return "tbld_datacount"; }
    enum FIELD_ENUMS
    {
        ID,              //'id'
        PLAYERID,        //'玩家id'
        KEYTYPE,         //'data类型'
        KEYIDX,          //'data编号'
        DATA_NUM,        //'累计计数'
        NEXT_RESET_TIME, //'下一次重置的时间戳'

    };

    static constexpr auto field_info()
    {
        return std::make_tuple(
            std::make_tuple("id", DB_FIELD_TYPE_LONGLONG_UNSIGNED, true, "  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id'"),
            std::make_tuple("playerid",
                            DB_FIELD_TYPE_LONGLONG_UNSIGNED,
                            false,
                            "  `playerid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '玩家id'"),
            std::make_tuple("keytype", DB_FIELD_TYPE_LONG_UNSIGNED, false, "  `keytype` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'data类型'"),
            std::make_tuple("keyidx", DB_FIELD_TYPE_LONG_UNSIGNED, false, "  `keyidx` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'data编号'"),
            std::make_tuple("data_num",
                            DB_FIELD_TYPE_LONGLONG_UNSIGNED,
                            false,
                            "  `data_num` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '累计计数'"),
            std::make_tuple("next_reset_time",
                            DB_FIELD_TYPE_LONG_UNSIGNED,
                            false,
                            "  `next_reset_time` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '下一次重置的时间戳'"));
    }

    using field_type_t = type_list<uint64_t, uint64_t, uint32_t, uint32_t, uint64_t, uint32_t>;

    static constexpr size_t field_count() { return 6; }

    static constexpr auto keys_info()
    {
        return std::make_tuple(std::make_tuple("idx_player", "playerid"),
                               std::make_tuple("PRIMARY", "id"),
                               std::make_tuple("idx_player_idx", "playerid,keytype,keyidx"));
    }

    static constexpr size_t keys_size() { return 3; }

    static constexpr const char* create_sql()
    {
        return R"##(CREATE TABLE `tbld_datacount` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
  `playerid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '玩家id',
  `keytype` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'data类型',
  `keyidx` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'data编号',
  `data_num` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '累计计数',
  `next_reset_time` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '下一次重置的时间戳',
  PRIMARY KEY (`id`),
  KEY `idx_player_idx` (`playerid`,`keytype`,`keyidx`),
  KEY `idx_player` (`playerid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1)##";
    };
};

struct TBLD_ITEM
{
    static constexpr const char* table_name() { return "tbld_item"; }
    enum FIELD_ENUMS
    {
        ID,           //'id'
        OWNER_ID,     //'所有者id'
        ITEMTYPE,     //'道具类型编号'
        POSITION,     //'所属包裹类型0:包裹 1:装备槽 100:仓库 101:帮派仓库 102:回购列表 103:邮箱 104:拍卖行'
        GRID,         //'所属包裹位置'
        FLAG,         //'物品掩码'
        PILENUM,      //'堆叠数量'
        EXPIRE_TIME,  //'过期时间戳秒'
        DURA,         //'当前耐久'
        DURA_LIMIT,   //'耐久上限'
        ADDITION_LEV, //'强化等级'
        EXTRA,        //'额外信息'

    };

    static constexpr auto field_info()
    {
        return std::make_tuple(
            std::make_tuple("id", DB_FIELD_TYPE_LONGLONG_UNSIGNED, true, "  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id'"),
            std::make_tuple("owner_id",
                            DB_FIELD_TYPE_LONGLONG_UNSIGNED,
                            false,
                            "  `owner_id` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '所有者id'"),
            std::make_tuple("itemtype",
                            DB_FIELD_TYPE_LONG_UNSIGNED,
                            false,
                            "  `itemtype` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '道具类型编号'"),
            std::make_tuple("position",
                            DB_FIELD_TYPE_LONG_UNSIGNED,
                            false,
                            "  `position` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '所属包裹类型0:包裹 1:装备槽 100:仓库 101:帮派仓库 "
                            "102:回购列表 103:邮箱 104:拍卖行'"),
            std::make_tuple("grid", DB_FIELD_TYPE_LONG_UNSIGNED, false, "  `grid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '所属包裹位置'"),
            std::make_tuple("flag", DB_FIELD_TYPE_LONG_UNSIGNED, false, "  `flag` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '物品掩码'"),
            std::make_tuple("pilenum", DB_FIELD_TYPE_LONG_UNSIGNED, false, "  `pilenum` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '堆叠数量'"),
            std::make_tuple("expire_time",
                            DB_FIELD_TYPE_LONG_UNSIGNED,
                            false,
                            "  `expire_time` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '过期时间戳秒'"),
            std::make_tuple("dura", DB_FIELD_TYPE_LONG_UNSIGNED, false, "  `dura` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '当前耐久'"),
            std::make_tuple("dura_limit",
                            DB_FIELD_TYPE_LONG_UNSIGNED,
                            false,
                            "  `dura_limit` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '耐久上限'"),
            std::make_tuple("addition_lev",
                            DB_FIELD_TYPE_LONG_UNSIGNED,
                            false,
                            "  `addition_lev` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '强化等级'"),
            std::make_tuple("extra"));
    }

    using field_type_t = type_list<uint64_t, uint64_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t>;

    static constexpr size_t field_count() { return 11; }

    static constexpr auto keys_info()
    {
        return std::make_tuple(std::make_tuple("idx_owner_pack", "owner_id,position"),
                               std::make_tuple("idx_itemtype", "itemtype"),
                               std::make_tuple("PRIMARY", "id"),
                               std::make_tuple("idx_owner", "owner_id"));
    }

    static constexpr size_t keys_size() { return 4; }

    static constexpr const char* create_sql()
    {
        return R"##(CREATE TABLE `tbld_item` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
  `owner_id` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '所有者id',
  `itemtype` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '道具类型编号',
  `position` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '所属包裹类型0:包裹 1:装备槽 100:仓库 101:帮派仓库 102:回购列表 103:邮箱 104:拍卖行',
  `grid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '所属包裹位置',
  `flag` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '物品掩码',
  `pilenum` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '堆叠数量',
  `expire_time` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '过期时间戳秒',
  `dura` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '当前耐久',
  `dura_limit` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '耐久上限',
  `addition_lev` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '强化等级',
  `extra` blob COMMENT '额外信息',
  PRIMARY KEY (`id`),
  KEY `idx_owner` (`owner_id`),
  KEY `idx_itemtype` (`itemtype`),
  KEY `idx_owner_pack` (`owner_id`,`position`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1)##";
    };
};

struct TBLD_MAIL
{
    static constexpr const char* table_name() { return "tbld_mail"; }
    enum FIELD_ENUMS
    {
        ID,          //'id'
        OWNERID,     //'拥有者id'
        SENDERID,    //'拥有者id'
        SENDERNAME,  //'发送者名字'
        CREATE_TIME, //'发送时间戳'
        FLAG,        //'邮件标志'
        SUBJECT,     //'邮件标题'
        CONTENT,     //'邮件内容'

    };

    static constexpr auto field_info()
    {
        return std::make_tuple(
            std::make_tuple("id", DB_FIELD_TYPE_LONGLONG_UNSIGNED, true, "  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id'"),
            std::make_tuple("ownerid",
                            DB_FIELD_TYPE_LONGLONG_UNSIGNED,
                            false,
                            "  `ownerid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '拥有者id'"),
            std::make_tuple("senderid",
                            DB_FIELD_TYPE_LONGLONG_UNSIGNED,
                            false,
                            "  `senderid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '拥有者id'"),
            std::make_tuple("sendername",
                            DB_FIELD_TYPE_VARCHAR,
                            false,
                            "  `sendername` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '发送者名字'"),
            std::make_tuple("create_time",
                            DB_FIELD_TYPE_LONG_UNSIGNED,
                            false,
                            "  `create_time` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '发送时间戳'"),
            std::make_tuple("flag", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false, "  `flag` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '邮件标志'"),
            std::make_tuple("subject",
                            DB_FIELD_TYPE_VARCHAR,
                            false,
                            "  `subject` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '邮件标题'"),
            std::make_tuple("content",
                            DB_FIELD_TYPE_VARCHAR,
                            false,
                            "  `content` varchar(1024) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '邮件内容'"));
    }

    using field_type_t = type_list<uint64_t, uint64_t, uint64_t, char[255], uint32_t, uint64_t, char[255], char[1024]>;

    static constexpr size_t field_count() { return 8; }

    static constexpr auto keys_info()
    {
        return std::make_tuple(std::make_tuple("idx_ownerid", "ownerid"),
                               std::make_tuple("idx_sendid", "senderid"),
                               std::make_tuple("PRIMARY", "id"),
                               std::make_tuple("idx_create_time", "create_time"));
    }

    static constexpr size_t keys_size() { return 4; }

    static constexpr const char* create_sql()
    {
        return R"##(CREATE TABLE `tbld_mail` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
  `ownerid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '拥有者id',
  `senderid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '拥有者id',
  `sendername` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '发送者名字',
  `create_time` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '发送时间戳',
  `flag` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '邮件标志',
  `subject` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '邮件标题',
  `content` varchar(1024) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '邮件内容',
  PRIMARY KEY (`id`),
  KEY `idx_create_time` (`create_time`),
  KEY `idx_ownerid` (`ownerid`),
  KEY `idx_sendid` (`senderid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1)##";
    };
};

struct TBLD_MAIL_ATTACHMENT
{
    static constexpr const char* table_name() { return "tbld_mail_attachment"; }
    enum FIELD_ENUMS
    {
        ID,          //'id'
        MAILID,      //'邮件id'
        ATTACH_TYPE, //'附件类型'
        ATTACH_DATA, //'数量/具体道具实例ID'

    };

    static constexpr auto field_info()
    {
        return std::make_tuple(
            std::make_tuple("id", DB_FIELD_TYPE_LONGLONG_UNSIGNED, true, "  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id'"),
            std::make_tuple("mailid", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false, "  `mailid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '邮件id'"),
            std::make_tuple("attach_type",
                            DB_FIELD_TYPE_LONG_UNSIGNED,
                            false,
                            "  `attach_type` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '附件类型'"),
            std::make_tuple("attach_data",
                            DB_FIELD_TYPE_LONGLONG_UNSIGNED,
                            false,
                            "  `attach_data` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '数量/具体道具实例ID'"));
    }

    using field_type_t = type_list<uint64_t, uint64_t, uint32_t, uint64_t>;

    static constexpr size_t field_count() { return 4; }

    static constexpr auto keys_info() { return std::make_tuple(std::make_tuple("idx_mailid", "mailid"), std::make_tuple("PRIMARY", "id")); }

    static constexpr size_t keys_size() { return 2; }

    static constexpr const char* create_sql()
    {
        return R"##(CREATE TABLE `tbld_mail_attachment` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
  `mailid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '邮件id',
  `attach_type` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '附件类型',
  `attach_data` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '数量/具体道具实例ID',
  PRIMARY KEY (`id`),
  KEY `idx_mailid` (`mailid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1)##";
    };
};

struct TBLD_PET
{
    static constexpr const char* table_name() { return "tbld_pet"; }
    enum FIELD_ENUMS
    {
        ID,       //'id'
        OWNERID,  //'所有者玩家id'
        PET_TYPE, //'宠物类型'
        HP,       //'当前血量'
        EXP,      //'当前经验'

    };

    static constexpr auto field_info()
    {
        return std::make_tuple(
            std::make_tuple("id", DB_FIELD_TYPE_LONGLONG_UNSIGNED, true, "  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id'"),
            std::make_tuple("ownerid",
                            DB_FIELD_TYPE_LONGLONG_UNSIGNED,
                            false,
                            "  `ownerid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '所有者玩家id'"),
            std::make_tuple("pet_type", DB_FIELD_TYPE_LONG_UNSIGNED, false, "  `pet_type` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '宠物类型'"),
            std::make_tuple("hp", DB_FIELD_TYPE_LONG_UNSIGNED, false, "  `hp` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '当前血量'"),
            std::make_tuple("exp", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false, "  `exp` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '当前经验'"));
    }

    using field_type_t = type_list<uint64_t, uint64_t, uint32_t, uint32_t, uint64_t>;

    static constexpr size_t field_count() { return 5; }

    static constexpr auto keys_info()
    {
        return std::make_tuple(std::make_tuple("PRIMARY", "id"),
                               std::make_tuple("idx_ownerid", "ownerid"),
                               std::make_tuple("idx_pet_type", "pet_type"));
    }

    static constexpr size_t keys_size() { return 3; }

    static constexpr const char* create_sql()
    {
        return R"##(CREATE TABLE `tbld_pet` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
  `ownerid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '所有者玩家id',
  `pet_type` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '宠物类型',
  `hp` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '当前血量',
  `exp` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '当前经验',
  PRIMARY KEY (`id`),
  KEY `idx_pet_type` (`pet_type`),
  KEY `idx_ownerid` (`ownerid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1)##";
    };
};

struct TBLD_PLAYER
{
    static constexpr const char* table_name() { return "tbld_player"; }
    enum FIELD_ENUMS
    {
        ID,              //'id'
        WORLDID,         //'服务器编号'
        ORIWORLDID,      //'创建角色时的服务器编号'
        OPENID,          //'玩家账号'
        NAME,            //'玩家名字'
        PROF,            //'玩家职业'
        BASELOOK,        //'玩家基本外观'
        CREATE_TIME,     //'创建时间戳'
        LAST_LOGINTIME,  //'最后登录时间戳'
        LAST_LOGOUTTIME, //'最后登出时间戳'
        DEL_TIME,        //'删除时间戳'
        LEV,             //'玩家等级'
        VIPLEV,          //'vip等级'
        RECORD_SCENEID,  //'玩家当前所在的地图编号'
        RECORD_X,        //'玩家当前所在地图x'
        RECORD_Y,        //'玩家当前所在地图y'
        RECORD_FACE,     //'玩家当前所在地图face'
        HOME_SCENEID,    //'玩家回城点对应的地图编号'
        HOME_X,          //'玩家回城点对应的地图x'
        HOME_Y,          //'玩家回城点对应的地图y'
        HOME_FACE,       //'玩家回城点对应的地图face'
        MATE_ID,         //'伴侣ID'
        MATE_NAME,       //'伴侣名字'
        TEAMID,          //'队伍id'
        GUILDID,         //'公会id'
        EXP,             //'玩家经验'
        MONEY,           //'金币'
        MONEY_BIND,      //'绑定金币'
        GOLD,            //'元宝'
        GOLD_BIND,       //'绑定元宝'
        HP,              //'hp'
        MP,              //'mp'
        FP,              //'fp'
        NP,              //'np'
        PKVAL,           //'pk值'
        HONOR,           //'荣誉值'
        ACHIPOINT,       //'成就值'
        BAG_SIZE,        //'背包大小'
        STROGE_SIZE,     //'仓库大小'

    };

    static constexpr auto field_info()
    {
        return std::make_tuple(
            std::make_tuple("id", DB_FIELD_TYPE_LONGLONG_UNSIGNED, true, "  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id'"),
            std::make_tuple("worldid", DB_FIELD_TYPE_LONG_UNSIGNED, false, "  `worldid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '服务器编号'"),
            std::make_tuple("oriworldid",
                            DB_FIELD_TYPE_LONG_UNSIGNED,
                            false,
                            "  `oriworldid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '创建角色时的服务器编号'"),
            std::make_tuple("openid",
                            DB_FIELD_TYPE_VARCHAR,
                            false,
                            "  `openid` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '玩家账号'"),
            std::make_tuple("name",
                            DB_FIELD_TYPE_VARCHAR,
                            false,
                            "  `name` varchar(32) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '玩家名字'"),
            std::make_tuple("prof", DB_FIELD_TYPE_LONG_UNSIGNED, false, "  `prof` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '玩家职业'"),
            std::make_tuple("baselook",
                            DB_FIELD_TYPE_LONG_UNSIGNED,
                            false,
                            "  `baselook` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '玩家基本外观'"),
            std::make_tuple("create_time",
                            DB_FIELD_TYPE_LONG_UNSIGNED,
                            false,
                            "  `create_time` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '创建时间戳'"),
            std::make_tuple("last_logintime",
                            DB_FIELD_TYPE_LONG_UNSIGNED,
                            false,
                            "  `last_logintime` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '最后登录时间戳'"),
            std::make_tuple("last_logouttime",
                            DB_FIELD_TYPE_LONG_UNSIGNED,
                            false,
                            "  `last_logouttime` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '最后登出时间戳'"),
            std::make_tuple("del_time",
                            DB_FIELD_TYPE_LONG_UNSIGNED,
                            false,
                            "  `del_time` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '删除时间戳'"),
            std::make_tuple("lev", DB_FIELD_TYPE_LONG_UNSIGNED, false, "  `lev` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '玩家等级'"),
            std::make_tuple("viplev", DB_FIELD_TYPE_LONG_UNSIGNED, false, "  `viplev` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'vip等级'"),
            std::make_tuple("record_sceneid",
                            DB_FIELD_TYPE_LONGLONG_UNSIGNED,
                            false,
                            "  `record_sceneid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '玩家当前所在的地图编号'"),
            std::make_tuple("record_x",
                            DB_FIELD_TYPE_FLOAT,
                            false,
                            "  `record_x` float(246) NOT NULL DEFAULT '0.000000' COMMENT '玩家当前所在地图x'"),
            std::make_tuple("record_y",
                            DB_FIELD_TYPE_FLOAT,
                            false,
                            "  `record_y` float(246) NOT NULL DEFAULT '0.000000' COMMENT '玩家当前所在地图y'"),
            std::make_tuple("record_face",
                            DB_FIELD_TYPE_FLOAT,
                            false,
                            "  `record_face` float(246) NOT NULL DEFAULT '0.000000' COMMENT '玩家当前所在地图face'"),
            std::make_tuple("home_sceneid",
                            DB_FIELD_TYPE_LONGLONG_UNSIGNED,
                            false,
                            "  `home_sceneid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '玩家回城点对应的地图编号'"),
            std::make_tuple("home_x",
                            DB_FIELD_TYPE_FLOAT,
                            false,
                            "  `home_x` float(246) NOT NULL DEFAULT '0.000000' COMMENT '玩家回城点对应的地图x'"),
            std::make_tuple("home_y",
                            DB_FIELD_TYPE_FLOAT,
                            false,
                            "  `home_y` float(246) NOT NULL DEFAULT '0.000000' COMMENT '玩家回城点对应的地图y'"),
            std::make_tuple("home_face",
                            DB_FIELD_TYPE_FLOAT,
                            false,
                            "  `home_face` float(246) NOT NULL DEFAULT '0.000000' COMMENT '玩家回城点对应的地图face'"),
            std::make_tuple("mate_id",
                            DB_FIELD_TYPE_LONGLONG_UNSIGNED,
                            false,
                            "  `mate_id` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '伴侣ID'"),
            std::make_tuple("mate_name",
                            DB_FIELD_TYPE_VARCHAR,
                            false,
                            "  `mate_name` varchar(32) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '伴侣名字'"),
            std::make_tuple("teamid", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false, "  `teamid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '队伍id'"),
            std::make_tuple("guildid",
                            DB_FIELD_TYPE_LONGLONG_UNSIGNED,
                            false,
                            "  `guildid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '公会id'"),
            std::make_tuple("exp", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false, "  `exp` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '玩家经验'"),
            std::make_tuple("money", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false, "  `money` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '金币'"),
            std::make_tuple("money_bind",
                            DB_FIELD_TYPE_LONGLONG_UNSIGNED,
                            false,
                            "  `money_bind` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '绑定金币'"),
            std::make_tuple("gold", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false, "  `gold` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '元宝'"),
            std::make_tuple("gold_bind",
                            DB_FIELD_TYPE_LONGLONG_UNSIGNED,
                            false,
                            "  `gold_bind` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '绑定元宝'"),
            std::make_tuple("hp", DB_FIELD_TYPE_LONG_UNSIGNED, false, "  `hp` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'hp'"),
            std::make_tuple("mp", DB_FIELD_TYPE_LONG_UNSIGNED, false, "  `mp` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'mp'"),
            std::make_tuple("fp", DB_FIELD_TYPE_LONG_UNSIGNED, false, "  `fp` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'fp'"),
            std::make_tuple("np", DB_FIELD_TYPE_LONG_UNSIGNED, false, "  `np` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'np'"),
            std::make_tuple("pkval", DB_FIELD_TYPE_LONG_UNSIGNED, false, "  `pkval` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'pk值'"),
            std::make_tuple("honor", DB_FIELD_TYPE_LONG_UNSIGNED, false, "  `honor` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '荣誉值'"),
            std::make_tuple("achipoint", DB_FIELD_TYPE_LONG_UNSIGNED, false, "  `achipoint` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '成就值'"),
            std::make_tuple("bag_size", DB_FIELD_TYPE_LONG_UNSIGNED, false, "  `bag_size` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '背包大小'"),
            std::make_tuple("stroge_size",
                            DB_FIELD_TYPE_LONG_UNSIGNED,
                            false,
                            "  `stroge_size` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '仓库大小'"));
    }

    using field_type_t = type_list<uint64_t,
                                   uint32_t,
                                   uint32_t,
                                   char[255],
                                   char[32],
                                   uint32_t,
                                   uint32_t,
                                   uint32_t,
                                   uint32_t,
                                   uint32_t,
                                   uint32_t,
                                   uint32_t,
                                   uint32_t,
                                   uint64_t,
                                   float,
                                   float,
                                   float,
                                   uint64_t,
                                   float,
                                   float,
                                   float,
                                   uint64_t,
                                   char[32],
                                   uint64_t,
                                   uint64_t,
                                   uint64_t,
                                   uint64_t,
                                   uint64_t,
                                   uint64_t,
                                   uint64_t,
                                   uint32_t,
                                   uint32_t,
                                   uint32_t,
                                   uint32_t,
                                   uint32_t,
                                   uint32_t,
                                   uint32_t,
                                   uint32_t,
                                   uint32_t>;

    static constexpr size_t field_count() { return 39; }

    static constexpr auto keys_info()
    {
        return std::make_tuple(std::make_tuple("idx_create_time", "create_time"),
                               std::make_tuple("idx_worldid", "worldid"),
                               std::make_tuple("idx_last_logouttime", "last_logouttime"),
                               std::make_tuple("idx_last_logintime", "last_logintime"),
                               std::make_tuple("idx_name", "name"),
                               std::make_tuple("idx_openid", "openid"),
                               std::make_tuple("idx_prof", "prof"),
                               std::make_tuple("PRIMARY", "id"),
                               std::make_tuple("idx_lev", "lev"),
                               std::make_tuple("idx_del_time", "del_time"));
    }

    static constexpr size_t keys_size() { return 10; }

    static constexpr const char* create_sql()
    {
        return R"##(CREATE TABLE `tbld_player` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
  `worldid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '服务器编号',
  `oriworldid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '创建角色时的服务器编号',
  `openid` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '玩家账号',
  `name` varchar(32) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '玩家名字',
  `prof` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '玩家职业',
  `baselook` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '玩家基本外观',
  `create_time` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '创建时间戳',
  `last_logintime` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '最后登录时间戳',
  `last_logouttime` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '最后登出时间戳',
  `del_time` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '删除时间戳',
  `lev` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '玩家等级',
  `viplev` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'vip等级',
  `record_sceneid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '玩家当前所在的地图编号',
  `record_x` float(24,6) NOT NULL DEFAULT '0.000000' COMMENT '玩家当前所在地图x',
  `record_y` float(24,6) NOT NULL DEFAULT '0.000000' COMMENT '玩家当前所在地图y',
  `record_face` float(24,6) NOT NULL DEFAULT '0.000000' COMMENT '玩家当前所在地图face',
  `home_sceneid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '玩家回城点对应的地图编号',
  `home_x` float(24,6) NOT NULL DEFAULT '0.000000' COMMENT '玩家回城点对应的地图x',
  `home_y` float(24,6) NOT NULL DEFAULT '0.000000' COMMENT '玩家回城点对应的地图y',
  `home_face` float(24,6) NOT NULL DEFAULT '0.000000' COMMENT '玩家回城点对应的地图face',
  `mate_id` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '伴侣ID',
  `mate_name` varchar(32) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '伴侣名字',
  `teamid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '队伍id',
  `guildid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '公会id',
  `exp` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '玩家经验',
  `money` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '金币',
  `money_bind` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '绑定金币',
  `gold` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '元宝',
  `gold_bind` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '绑定元宝',
  `hp` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'hp',
  `mp` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'mp',
  `fp` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'fp',
  `np` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'np',
  `pkval` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'pk值',
  `honor` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '荣誉值',
  `achipoint` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '成就值',
  `bag_size` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '背包大小',
  `stroge_size` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '仓库大小',
  PRIMARY KEY (`id`),
  UNIQUE KEY `idx_name` (`name`),
  KEY `idx_lev` (`lev`),
  KEY `idx_del_time` (`del_time`),
  KEY `idx_openid` (`openid`),
  KEY `idx_prof` (`prof`),
  KEY `idx_last_logintime` (`last_logintime`),
  KEY `idx_last_logouttime` (`last_logouttime`),
  KEY `idx_worldid` (`worldid`),
  KEY `idx_create_time` (`create_time`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1)##";
    };
};

struct TBLD_SKILL
{
    static constexpr const char* table_name() { return "tbld_skill"; }
    enum FIELD_ENUMS
    {
        ID,        //'id'
        USERID,    //'玩家id'
        SKILLTYPE, //'技能编号'
        LEV,       //'技能等级'

    };

    static constexpr auto field_info()
    {
        return std::make_tuple(
            std::make_tuple("id", DB_FIELD_TYPE_LONGLONG_UNSIGNED, true, "  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id'"),
            std::make_tuple("userid", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false, "  `userid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '玩家id'"),
            std::make_tuple("skilltype",
                            DB_FIELD_TYPE_LONG_UNSIGNED,
                            false,
                            "  `skilltype` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '技能编号'"),
            std::make_tuple("lev", DB_FIELD_TYPE_LONG_UNSIGNED, false, "  `lev` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '技能等级'"));
    }

    using field_type_t = type_list<uint64_t, uint64_t, uint32_t, uint32_t>;

    static constexpr size_t field_count() { return 4; }

    static constexpr auto keys_info() { return std::make_tuple(std::make_tuple("idx_userid", "userid"), std::make_tuple("PRIMARY", "id")); }

    static constexpr size_t keys_size() { return 2; }

    static constexpr const char* create_sql()
    {
        return R"##(CREATE TABLE `tbld_skill` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
  `userid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '玩家id',
  `skilltype` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '技能编号',
  `lev` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '技能等级',
  PRIMARY KEY (`id`),
  KEY `idx_userid` (`userid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1)##";
    };
};

struct TBLD_TASK
{
    static constexpr const char* table_name() { return "tbld_task"; }
    enum FIELD_ENUMS
    {
        ID,             //'id'
        USERID,         //'玩家id'
        TASKID,         //'任务id'
        NUM0,           //'任务计数1'
        NUM1,           //'任务计数2'
        NUM2,           //'任务计数3'
        NUM3,           //'任务计数4'
        ACCEPT_USERLEV, //'接受任务时的等级'
        ACCEPT_TIME,    //'接受任务时间戳'
        FINISH_TIME,    //'完成任务时间戳'
        EXPIRE_TIME,    //'任务过期时间戳'
        DAYCOUNT,       //'本日完成次数'
        DAYCOUNT_MAX,   //'本日最大可完成次数'
        STATE,          //'任务状态'

    };

    static constexpr auto field_info()
    {
        return std::make_tuple(
            std::make_tuple("id", DB_FIELD_TYPE_LONGLONG_UNSIGNED, true, "  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id'"),
            std::make_tuple("userid", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false, "  `userid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '玩家id'"),
            std::make_tuple("taskid", DB_FIELD_TYPE_LONG_UNSIGNED, false, "  `taskid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '任务id'"),
            std::make_tuple("num0", DB_FIELD_TYPE_LONG_UNSIGNED, false, "  `num0` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '任务计数1'"),
            std::make_tuple("num1", DB_FIELD_TYPE_LONG_UNSIGNED, false, "  `num1` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '任务计数2'"),
            std::make_tuple("num2", DB_FIELD_TYPE_LONG_UNSIGNED, false, "  `num2` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '任务计数3'"),
            std::make_tuple("num3", DB_FIELD_TYPE_LONG_UNSIGNED, false, "  `num3` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '任务计数4'"),
            std::make_tuple("accept_userlev",
                            DB_FIELD_TYPE_LONG_UNSIGNED,
                            false,
                            "  `accept_userlev` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '接受任务时的等级'"),
            std::make_tuple("accept_time",
                            DB_FIELD_TYPE_LONG_UNSIGNED,
                            false,
                            "  `accept_time` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '接受任务时间戳'"),
            std::make_tuple("finish_time",
                            DB_FIELD_TYPE_LONG_UNSIGNED,
                            false,
                            "  `finish_time` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '完成任务时间戳'"),
            std::make_tuple("expire_time",
                            DB_FIELD_TYPE_LONG_UNSIGNED,
                            false,
                            "  `expire_time` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '任务过期时间戳'"),
            std::make_tuple("daycount",
                            DB_FIELD_TYPE_LONG_UNSIGNED,
                            false,
                            "  `daycount` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '本日完成次数'"),
            std::make_tuple("daycount_max",
                            DB_FIELD_TYPE_LONG_UNSIGNED,
                            false,
                            "  `daycount_max` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '本日最大可完成次数'"),
            std::make_tuple("state", DB_FIELD_TYPE_LONG_UNSIGNED, false, "  `state` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '任务状态'"));
    }

    using field_type_t = type_list<uint64_t,
                                   uint64_t,
                                   uint32_t,
                                   uint32_t,
                                   uint32_t,
                                   uint32_t,
                                   uint32_t,
                                   uint32_t,
                                   uint32_t,
                                   uint32_t,
                                   uint32_t,
                                   uint32_t,
                                   uint32_t,
                                   uint32_t>;

    static constexpr size_t field_count() { return 14; }

    static constexpr auto keys_info()
    {
        return std::make_tuple(std::make_tuple("idx_userid", "userid"), std::make_tuple("PRIMARY", "id"), std::make_tuple("idx_taskid", "taskid"));
    }

    static constexpr size_t keys_size() { return 3; }

    static constexpr const char* create_sql()
    {
        return R"##(CREATE TABLE `tbld_task` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
  `userid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '玩家id',
  `taskid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '任务id',
  `num0` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '任务计数1',
  `num1` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '任务计数2',
  `num2` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '任务计数3',
  `num3` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '任务计数4',
  `accept_userlev` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '接受任务时的等级',
  `accept_time` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '接受任务时间戳',
  `finish_time` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '完成任务时间戳',
  `expire_time` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '任务过期时间戳',
  `daycount` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '本日完成次数',
  `daycount_max` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '本日最大可完成次数',
  `state` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '任务状态',
  PRIMARY KEY (`id`),
  KEY `idx_taskid` (`taskid`),
  KEY `idx_userid` (`userid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1)##";
    };
};

struct TBLD_ACHIEVEMENT
{
    static constexpr const char* table_name() { return "tbld_achievement"; }
    enum FIELD_ENUMS
    {
        ID,     //'id'
        USERID, //'玩家id'
        ACHIID, //'成就id'
        TAKE,   //'成就id'

    };

    static constexpr auto field_info()
    {
        return std::make_tuple(
            std::make_tuple("id", DB_FIELD_TYPE_LONGLONG_UNSIGNED, true, "  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id'"),
            std::make_tuple("userid", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false, "  `userid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '玩家id'"),
            std::make_tuple("achiid", DB_FIELD_TYPE_LONG_UNSIGNED, false, "  `achiid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '成就id'"),
            std::make_tuple("take", DB_FIELD_TYPE_LONG_UNSIGNED, false, "  `take` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '成就id'"));
    }

    using field_type_t = type_list<uint64_t, uint64_t, uint32_t, uint32_t>;

    static constexpr size_t field_count() { return 4; }

    static constexpr auto keys_info()
    {
        return std::make_tuple(std::make_tuple("idx_userid", "userid"), std::make_tuple("PRIMARY", "id"), std::make_tuple("idx_achiid", "achiid"));
    }

    static constexpr size_t keys_size() { return 3; }

    static constexpr const char* create_sql()
    {
        return R"##(CREATE TABLE `tbld_achievement` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
  `userid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '玩家id',
  `achiid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '成就id',
  `take` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '成就id',
  PRIMARY KEY (`id`),
  KEY `idx_achiid` (`achiid`),
  KEY `idx_userid` (`userid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1)##";
    };
};

struct TBLD_STATUS
{
    static constexpr const char* table_name() { return "tbld_status"; }
    enum FIELD_ENUMS
    {
        ID,        //'id'
        USERID,    //'玩家id'
        TYPEID,    //'状态类型编号'
        LEV,       //'状态等级'
        POWER,     //'数据'
        SECS,      //'持续时间'
        TIMES,     //'作用次数'
        LASTSTAMP, //'最后一次作用的时间戳'
        CASTERID,  //'施加此状态的角色id'
        PAUSE,     //'是否暂停'
        STATUSID,  //'状态等级'

    };

    static constexpr auto field_info()
    {
        return std::make_tuple(
            std::make_tuple("id", DB_FIELD_TYPE_LONGLONG_UNSIGNED, true, "  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id'"),
            std::make_tuple("userid", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false, "  `userid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '玩家id'"),
            std::make_tuple("typeid", DB_FIELD_TYPE_LONG_UNSIGNED, false, "  `typeid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '状态类型编号'"),
            std::make_tuple("lev", DB_FIELD_TYPE_LONG_UNSIGNED, false, "  `lev` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '状态等级'"),
            std::make_tuple("power", DB_FIELD_TYPE_LONG_UNSIGNED, false, "  `power` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '数据'"),
            std::make_tuple("secs", DB_FIELD_TYPE_LONG_UNSIGNED, false, "  `secs` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '持续时间'"),
            std::make_tuple("times", DB_FIELD_TYPE_LONG_UNSIGNED, false, "  `times` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '作用次数'"),
            std::make_tuple("laststamp",
                            DB_FIELD_TYPE_LONG_UNSIGNED,
                            false,
                            "  `laststamp` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '最后一次作用的时间戳'"),
            std::make_tuple("casterid",
                            DB_FIELD_TYPE_LONGLONG_UNSIGNED,
                            false,
                            "  `casterid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '施加此状态的角色id'"),
            std::make_tuple("pause", DB_FIELD_TYPE_LONG_UNSIGNED, false, "  `pause` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '是否暂停'"),
            std::make_tuple("statusid", DB_FIELD_TYPE_LONG_UNSIGNED, false, "  `statusid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '状态等级'"));
    }

    using field_type_t = type_list<uint64_t, uint64_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint64_t, uint32_t, uint32_t>;

    static constexpr size_t field_count() { return 11; }

    static constexpr auto keys_info()
    {
        return std::make_tuple(std::make_tuple("idx_userid", "userid"),
                               std::make_tuple("PRIMARY", "id"),
                               std::make_tuple("idx_statusid", "typeid,lev"));
    }

    static constexpr size_t keys_size() { return 3; }

    static constexpr const char* create_sql()
    {
        return R"##(CREATE TABLE `tbld_status` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
  `userid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '玩家id',
  `typeid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '状态类型编号',
  `lev` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '状态等级',
  `power` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '数据',
  `secs` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '持续时间',
  `times` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '作用次数',
  `laststamp` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '最后一次作用的时间戳',
  `casterid` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '施加此状态的角色id',
  `pause` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '是否暂停',
  `statusid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '状态等级',
  PRIMARY KEY (`id`),
  KEY `idx_statusid` (`typeid`,`lev`),
  KEY `idx_userid` (`userid`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1)##";
    };
};

struct TBLD_SYSTEMVAR
{
    static constexpr const char* table_name() { return "tbld_systemvar"; }
    enum FIELD_ENUMS
    {
        KEYIDX, //'id'
        NAME,   //'系统变量名'
        DATA0,  //'数据1'
        DATA1,  //'数据2'
        DATA2,  //'数据3'
        DATA3,  //'数据4'
        STR0,   //'文字1'
        STR1,   //'文字2'
        STR2,   //'文字3'
        STR3,   //'文字4'

    };

    static constexpr auto field_info()
    {
        return std::make_tuple(
            std::make_tuple("keyidx", DB_FIELD_TYPE_LONGLONG_UNSIGNED, true, "  `keyidx` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id'"),
            std::make_tuple("name",
                            DB_FIELD_TYPE_VARCHAR,
                            false,
                            "  `name` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '系统变量名'"),
            std::make_tuple("data0", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false, "  `data0` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '数据1'"),
            std::make_tuple("data1", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false, "  `data1` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '数据2'"),
            std::make_tuple("data2", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false, "  `data2` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '数据3'"),
            std::make_tuple("data3", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false, "  `data3` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '数据4'"),
            std::make_tuple("str0", DB_FIELD_TYPE_VARCHAR, false, "  `str0` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '文字1'"),
            std::make_tuple("str1", DB_FIELD_TYPE_VARCHAR, false, "  `str1` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '文字2'"),
            std::make_tuple("str2", DB_FIELD_TYPE_VARCHAR, false, "  `str2` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '文字3'"),
            std::make_tuple("str3", DB_FIELD_TYPE_VARCHAR, false, "  `str3` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '文字4'"));
    }

    using field_type_t = type_list<uint64_t, char[255], uint64_t, uint64_t, uint64_t, uint64_t, char[255], char[255], char[255], char[255]>;

    static constexpr size_t field_count() { return 10; }

    static constexpr auto keys_info() { return std::make_tuple(std::make_tuple("PRIMARY", "keyidx")); }

    static constexpr size_t keys_size() { return 1; }

    static constexpr const char* create_sql()
    {
        return R"##(CREATE TABLE `tbld_systemvar` (
  `keyidx` bigint(20) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
  `name` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '系统变量名',
  `data0` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '数据1',
  `data1` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '数据2',
  `data2` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '数据3',
  `data3` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '数据4',
  `str0` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '文字1',
  `str1` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '文字2',
  `str2` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '文字3',
  `str3` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '' COMMENT '文字4',
  PRIMARY KEY (`keyidx`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1)##";
    };
};

using GAMEDB_TABLE_LIST = type_list<TBLD_COMMONDATA,
                                    TBLD_COOLDOWN,
                                    TBLD_DATACOUNT,
                                    TBLD_ITEM,
                                    TBLD_MAIL,
                                    TBLD_MAIL_ATTACHMENT,
                                    TBLD_PET,
                                    TBLD_PLAYER,
                                    TBLD_SKILL,
                                    TBLD_TASK,
                                    TBLD_ACHIEVEMENT,
                                    TBLD_STATUS,
                                    TBLD_SYSTEMVAR>;

#endif
