#pragma once
#include <string>

#include "BaseCode.h"
#include "DBField.h"

struct TBLD_COMMONDATA
{
    static constexpr const char* table_name() { return "tbld_commondata"; }
    enum FIELD_ENUMS
    {
        ID,       //''
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
        return std::make_tuple(std::make_tuple("id", DB_FIELD_TYPE_LONGLONG_UNSIGNED, true),
                               std::make_tuple("playerid", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
                               std::make_tuple("keytype", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("keyidx", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
                               std::make_tuple("data0", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
                               std::make_tuple("data1", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
                               std::make_tuple("data2", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
                               std::make_tuple("data3", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false));
    }

    using field_type_t = type_list<uint64_t, uint64_t, uint32_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t>;

    static constexpr size_t field_count() { return 8; }
};

struct TBLD_COOLDOWN
{
    static constexpr const char* table_name() { return "tbld_cooldown"; }
    enum FIELD_ENUMS
    {
        ID,          //''
        PLAYERID,    //'玩家id'
        KEYTYPE,     //'CD类型'
        KEYIDX,      //'CD编号'
        EXPIRE_TIME, //'CD结束时间戳'

    };

    static constexpr auto field_info()
    {
        return std::make_tuple(std::make_tuple("id", DB_FIELD_TYPE_LONGLONG_UNSIGNED, true),
                               std::make_tuple("playerid", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
                               std::make_tuple("keytype", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("keyidx", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
                               std::make_tuple("expire_time", DB_FIELD_TYPE_LONG_UNSIGNED, false));
    }

    using field_type_t = type_list<uint64_t, uint64_t, uint32_t, uint64_t, uint32_t>;

    static constexpr size_t field_count() { return 5; }
};

struct TBLD_DATACOUNT
{
    static constexpr const char* table_name() { return "tbld_datacount"; }
    enum FIELD_ENUMS
    {
        ID,              //''
        PLAYERID,        //'玩家id'
        KEYTYPE,         //'data类型'
        KEYIDX,          //'data编号'
        DATA_NUM,        //'累计计数'
        LAST_RESET_TIME, //'最后1次重置的时间戳'

    };

    static constexpr auto field_info()
    {
        return std::make_tuple(std::make_tuple("id", DB_FIELD_TYPE_LONGLONG_UNSIGNED, true),
                               std::make_tuple("playerid", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
                               std::make_tuple("keytype", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("keyidx", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
                               std::make_tuple("data_num", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
                               std::make_tuple("last_reset_time", DB_FIELD_TYPE_LONG_UNSIGNED, false));
    }

    using field_type_t = type_list<uint64_t, uint64_t, uint32_t, uint64_t, uint64_t, uint32_t>;

    static constexpr size_t field_count() { return 6; }
};

struct TBLD_GUILD
{
    static constexpr const char* table_name() { return "tbld_guild"; }
    enum FIELD_ENUMS
    {
        ID,          //''
        LEV,         //'帮派等级'
        NAME,        //'帮派名称'
        LEADERID,    //'帮主id'
        LEADERNAME,  //'帮主名称'
        CREATE_TIME, //'创帮时间'
        DEL_TIME,    //'删除时间'

    };

    static constexpr auto field_info()
    {
        return std::make_tuple(std::make_tuple("id", DB_FIELD_TYPE_LONGLONG_UNSIGNED, true),
                               std::make_tuple("lev", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
                               std::make_tuple("name", DB_FIELD_TYPE_VARCHAR, false),
                               std::make_tuple("leaderid", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
                               std::make_tuple("leadername", DB_FIELD_TYPE_VARCHAR, false),
                               std::make_tuple("create_time", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("del_time", DB_FIELD_TYPE_LONG_UNSIGNED, false));
    }

    using field_type_t = type_list<uint64_t, uint64_t, char[255], uint64_t, char[32], uint32_t, uint32_t>;

    static constexpr size_t field_count() { return 7; }
};

struct TBLD_ITEM
{
    static constexpr const char* table_name() { return "tbld_item"; }
    enum FIELD_ENUMS
    {
        ID,       //''
        OWNER_ID, //'所有者id'
        ITEMTYPE, //'道具类型编号'
        POSITION, //'所属包裹类型0:包裹 1:装备槽 100:仓库 101:帮派仓库 102:回购列表 103:邮箱 104:拍卖行'
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
        return std::make_tuple(std::make_tuple("id", DB_FIELD_TYPE_LONGLONG_UNSIGNED, true),
                               std::make_tuple("owner_id", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
                               std::make_tuple("itemtype", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("position", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("grid", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("flag", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("pilenum", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("expire_time", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("dura", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("dura_limit", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("addition_lev", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("extra", DB_FIELD_TYPE_BLOB, false));
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
                                   std::string>;

    static constexpr size_t field_count() { return 12; }
};

struct TBLD_MAIL
{
    static constexpr const char* table_name() { return "tbld_mail"; }
    enum FIELD_ENUMS
    {
        ID,          //''
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
        return std::make_tuple(std::make_tuple("id", DB_FIELD_TYPE_LONGLONG_UNSIGNED, true),
                               std::make_tuple("ownerid", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
                               std::make_tuple("senderid", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
                               std::make_tuple("sendername", DB_FIELD_TYPE_VARCHAR, false),
                               std::make_tuple("create_time", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("flag", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
                               std::make_tuple("subject", DB_FIELD_TYPE_VARCHAR, false),
                               std::make_tuple("content", DB_FIELD_TYPE_VARCHAR, false));
    }

    using field_type_t = type_list<uint64_t, uint64_t, uint64_t, char[255], uint32_t, uint64_t, char[255], char[1024]>;

    static constexpr size_t field_count() { return 8; }
};

struct TBLD_MAIL_ATTACHMENT
{
    static constexpr const char* table_name() { return "tbld_mail_attachment"; }
    enum FIELD_ENUMS
    {
        ID,          //''
        MAILID,      //'邮件id'
        ATTACH_TYPE, //'附件类型'
        ATTACH_DATA, //'数量/具体道具实例ID'

    };

    static constexpr auto field_info()
    {
        return std::make_tuple(std::make_tuple("id", DB_FIELD_TYPE_LONGLONG_UNSIGNED, true),
                               std::make_tuple("mailid", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
                               std::make_tuple("attach_type", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("attach_data", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false));
    }

    using field_type_t = type_list<uint64_t, uint64_t, uint32_t, uint64_t>;

    static constexpr size_t field_count() { return 4; }
};

struct TBLD_PET
{
    static constexpr const char* table_name() { return "tbld_pet"; }
    enum FIELD_ENUMS
    {
        ID,       //''
        OWNERID,  //'所有者玩家id'
        PET_TYPE, //'宠物类型'
        HP,       //'当前血量'
        EXP,      //'当前经验'

    };

    static constexpr auto field_info()
    {
        return std::make_tuple(std::make_tuple("id", DB_FIELD_TYPE_LONGLONG_UNSIGNED, true),
                               std::make_tuple("ownerid", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
                               std::make_tuple("pet_type", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("hp", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("exp", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false));
    }

    using field_type_t = type_list<uint64_t, uint64_t, uint32_t, uint32_t, uint64_t>;

    static constexpr size_t field_count() { return 5; }
};

struct TBLD_PLAYER
{
    static constexpr const char* table_name() { return "tbld_player"; }
    enum FIELD_ENUMS
    {
        ID,              //''
        WORLDID,         //'服务器编号'
        ORIWORLDID,      //'创建角色时的服务器编号'
        OPENID,          //'玩家账号'
        NAME,            //'玩家名字'
        PROF,            //'玩家职业'
        BASELOOK,        //'玩家基本外观'
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
        CREATE_TIME,     //'创建时间戳'
        LAST_LOGINTIME,  //'最后登录时间戳'
        LAST_LOGOUTTIME, //'最后登出时间戳'
        DEL_TIME,        //'删除时间戳'

    };

    static constexpr auto field_info()
    {
        return std::make_tuple(std::make_tuple("id", DB_FIELD_TYPE_LONGLONG_UNSIGNED, true),
                               std::make_tuple("worldid", DB_FIELD_TYPE_SHORT_UNSIGNED, false),
                               std::make_tuple("oriworldid", DB_FIELD_TYPE_SHORT_UNSIGNED, false),
                               std::make_tuple("openid", DB_FIELD_TYPE_VARCHAR, false),
                               std::make_tuple("name", DB_FIELD_TYPE_VARCHAR, false),
                               std::make_tuple("prof", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("baselook", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("lev", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("viplev", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("record_sceneid", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
                               std::make_tuple("record_x", DB_FIELD_TYPE_FLOAT, false),
                               std::make_tuple("record_y", DB_FIELD_TYPE_FLOAT, false),
                               std::make_tuple("record_face", DB_FIELD_TYPE_FLOAT, false),
                               std::make_tuple("home_sceneid", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
                               std::make_tuple("home_x", DB_FIELD_TYPE_FLOAT, false),
                               std::make_tuple("home_y", DB_FIELD_TYPE_FLOAT, false),
                               std::make_tuple("home_face", DB_FIELD_TYPE_FLOAT, false),
                               std::make_tuple("mate_id", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
                               std::make_tuple("mate_name", DB_FIELD_TYPE_VARCHAR, false),
                               std::make_tuple("teamid", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
                               std::make_tuple("guildid", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
                               std::make_tuple("exp", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
                               std::make_tuple("money", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
                               std::make_tuple("money_bind", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
                               std::make_tuple("gold", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
                               std::make_tuple("gold_bind", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
                               std::make_tuple("hp", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("mp", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("fp", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("np", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("pkval", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("honor", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("achipoint", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("bag_size", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("stroge_size", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("create_time", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("last_logintime", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("last_logouttime", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("del_time", DB_FIELD_TYPE_LONG_UNSIGNED, false));
    }

    using field_type_t = type_list<uint64_t,
                                   uint16_t,
                                   uint16_t,
                                   char[255],
                                   char[32],
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
                                   uint32_t,
                                   uint32_t,
                                   uint32_t,
                                   uint32_t,
                                   uint32_t>;

    static constexpr size_t field_count() { return 39; }
};

struct TBLD_PLAYER_GUILDINFO
{
    static constexpr const char* table_name() { return "tbld_player_guildinfo"; }
    enum FIELD_ENUMS
    {
        ID,          //''
        RANK,        //'帮派职位'
        SCORE,       //'帮派贡献'
        TOTAL_SCORE, //'帮派贡献累计'
        JOIN_TIME,   //'入帮时间'

    };

    static constexpr auto field_info()
    {
        return std::make_tuple(std::make_tuple("id", DB_FIELD_TYPE_LONGLONG_UNSIGNED, true),
                               std::make_tuple("rank", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("score", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("total_score", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("join_time", DB_FIELD_TYPE_LONG_UNSIGNED, false));
    }

    using field_type_t = type_list<uint64_t, uint32_t, uint32_t, uint32_t, uint32_t>;

    static constexpr size_t field_count() { return 5; }
};

struct TBLD_SKILL
{
    static constexpr const char* table_name() { return "tbld_skill"; }
    enum FIELD_ENUMS
    {
        ID,        //''
        USERID,    //'玩家id'
        SKILLTYPE, //'技能编号'
        LEV,       //'技能等级'

    };

    static constexpr auto field_info()
    {
        return std::make_tuple(std::make_tuple("id", DB_FIELD_TYPE_LONGLONG_UNSIGNED, true),
                               std::make_tuple("userid", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
                               std::make_tuple("skilltype", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("lev", DB_FIELD_TYPE_LONG_UNSIGNED, false));
    }

    using field_type_t = type_list<uint64_t, uint64_t, uint32_t, uint32_t>;

    static constexpr size_t field_count() { return 4; }
};

struct TBLD_TASK
{
    static constexpr const char* table_name() { return "tbld_task"; }
    enum FIELD_ENUMS
    {
        ID,             //''
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
        return std::make_tuple(std::make_tuple("id", DB_FIELD_TYPE_LONGLONG_UNSIGNED, true),
                               std::make_tuple("userid", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
                               std::make_tuple("taskid", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("num0", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("num1", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("num2", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("num3", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("accept_userlev", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("accept_time", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("finish_time", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("expire_time", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("daycount", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("daycount_max", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("state", DB_FIELD_TYPE_LONG_UNSIGNED, false));
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
};

struct TBLD_ACHIEVEMENT
{
    static constexpr const char* table_name() { return "tbld_achievement"; }
    enum FIELD_ENUMS
    {
        ID,     //''
        USERID, //'玩家id'
        ACHIID, //'成就id'
        TAKE,   //'成就id'

    };

    static constexpr auto field_info()
    {
        return std::make_tuple(std::make_tuple("id", DB_FIELD_TYPE_LONGLONG_UNSIGNED, true),
                               std::make_tuple("userid", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
                               std::make_tuple("achiid", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("take", DB_FIELD_TYPE_LONG_UNSIGNED, false));
    }

    using field_type_t = type_list<uint64_t, uint64_t, uint32_t, uint32_t>;

    static constexpr size_t field_count() { return 4; }
};

struct TBLD_STATUS
{
    static constexpr const char* table_name() { return "tbld_status"; }
    enum FIELD_ENUMS
    {
        ID,        //''
        USERID,    //'玩家id'
        TYPEID,    //'状态类型编号'
        LEV,       //'状态等级'
        POWER,     //'数据'
        SECS,      //'持续时间'
        TIMES,     //'作用次数'
        LASTSTAMP, //'最后一次作用的时间戳'
        CASTERID,  //'施加此状态的角色id'
        PAUSE,     //'是否暂停'

    };

    static constexpr auto field_info()
    {
        return std::make_tuple(std::make_tuple("id", DB_FIELD_TYPE_LONGLONG_UNSIGNED, true),
                               std::make_tuple("userid", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
                               std::make_tuple("typeid", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("lev", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("power", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("secs", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("times", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("laststamp", DB_FIELD_TYPE_LONG_UNSIGNED, false),
                               std::make_tuple("casterid", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
                               std::make_tuple("pause", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false));
    }

    using field_type_t =
        type_list<uint64_t, uint64_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint64_t, uint64_t>;

    static constexpr size_t field_count() { return 10; }
};

struct TBLD_SYSTEMVAR
{
    static constexpr const char* table_name() { return "tbld_systemvar"; }
    enum FIELD_ENUMS
    {
        KEYIDX, //''
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
        return std::make_tuple(std::make_tuple("keyidx", DB_FIELD_TYPE_LONGLONG_UNSIGNED, true),
                               std::make_tuple("name", DB_FIELD_TYPE_VARCHAR, false),
                               std::make_tuple("data0", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
                               std::make_tuple("data1", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
                               std::make_tuple("data2", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
                               std::make_tuple("data3", DB_FIELD_TYPE_LONGLONG_UNSIGNED, false),
                               std::make_tuple("str0", DB_FIELD_TYPE_VARCHAR, false),
                               std::make_tuple("str1", DB_FIELD_TYPE_VARCHAR, false),
                               std::make_tuple("str2", DB_FIELD_TYPE_VARCHAR, false),
                               std::make_tuple("str3", DB_FIELD_TYPE_VARCHAR, false));
    }

    using field_type_t = type_list<uint64_t,
                                   char[255],
                                   uint64_t,
                                   uint64_t,
                                   uint64_t,
                                   uint64_t,
                                   char[255],
                                   char[255],
                                   char[255],
                                   char[255]>;

    static constexpr size_t field_count() { return 10; }
};
