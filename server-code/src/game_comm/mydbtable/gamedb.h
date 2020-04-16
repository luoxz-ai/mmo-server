#pragma once
#include <string>
#include "BaseCode.h"
#include "DBField.h"

struct TBLD_COMMONDATA
{
	static constexpr const char* table_name = "tbld_commondata";
	enum FIELD_ENUMS
	{
		ID,//''
		PLAYERID,//'玩家id'
		KEYTYPE,//'数据类型'
		KEYIDX,//'数据编号'
		DATA0,//'数据1'
		DATA1,//'数据2'
		DATA2,//'数据3'
		DATA3,//'数据4'

	};
	static constexpr const char* field_name[] = { "id","playerid","keytype","keyidx","data0","data1","data2","data3" };
	using field_type_t = type_list<int64_t ,int64_t ,int32_t ,int64_t ,int64_t ,int64_t ,int64_t ,int64_t >;
	static constexpr DB_FIELD_TYPES field_type_enum_list[] = { DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_LONG,DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_LONGLONG };
	static constexpr bool pri_key_idx[] = { true,false,false,false,false,false,false,false };
};

		
struct TBLD_COOLDOWN
{
	static constexpr const char* table_name = "tbld_cooldown";
	enum FIELD_ENUMS
	{
		ID,//''
		PLAYERID,//'玩家id'
		KEYTYPE,//'CD类型'
		KEYIDX,//'CD编号'
		EXPIRE_TIME,//'CD结束时间戳'

	};
	static constexpr const char* field_name[] = { "id","playerid","keytype","keyidx","expire_time" };
	using field_type_t = type_list<int64_t ,int64_t ,int32_t ,int64_t ,int64_t >;
	static constexpr DB_FIELD_TYPES field_type_enum_list[] = { DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_LONG,DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_LONGLONG };
	static constexpr bool pri_key_idx[] = { true,false,false,false,false };
};

		
struct TBLD_DATACOUNT
{
	static constexpr const char* table_name = "tbld_datacount";
	enum FIELD_ENUMS
	{
		ID,//''
		PLAYERID,//'玩家id'
		KEYTYPE,//'data类型'
		KEYIDX,//'data编号'
		DATA_NUM,//'累计计数'
		LAST_RESET_TIME,//'最后1次重置的时间戳'

	};
	static constexpr const char* field_name[] = { "id","playerid","keytype","keyidx","data_num","last_reset_time" };
	using field_type_t = type_list<int64_t ,int64_t ,int32_t ,int64_t ,int64_t ,int64_t >;
	static constexpr DB_FIELD_TYPES field_type_enum_list[] = { DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_LONG,DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_LONGLONG };
	static constexpr bool pri_key_idx[] = { true,false,false,false,false,false };
};

		
struct TBLD_GUILD
{
	static constexpr const char* table_name = "tbld_guild";
	enum FIELD_ENUMS
	{
		ID,//''
		LEV,//'帮派等级'
		NAME,//'帮派名称'
		LEADERID,//'帮主id'
		LEADERNAME,//'帮主名称'
		CREATE_TIME,//'创帮时间'
		DEL_TIME,//'删除时间'

	};
	static constexpr const char* field_name[] = { "id","lev","name","leaderid","leadername","create_time","del_time" };
	using field_type_t = type_list<int64_t ,int64_t ,char[255] ,int64_t ,char[32] ,int64_t ,int64_t >;
	static constexpr DB_FIELD_TYPES field_type_enum_list[] = { DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_VARCHAR,DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_VARCHAR,DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_LONGLONG };
	static constexpr bool pri_key_idx[] = { true,false,false,false,false,false,false };
};

		
struct TBLD_ITEM
{
	static constexpr const char* table_name = "tbld_item";
	enum FIELD_ENUMS
	{
		ID,//''
		OWNER_ID,//'所有者id'
		ITEMTYPE,//'道具类型编号'
		POSITION,//'所属包裹类型0:包裹 1:装备槽 100:仓库 101:帮派仓库 102:回购列表 103:邮箱 104:拍卖行'
		GRID,//'所属包裹位置'
		FLAG,//'物品掩码'
		PILENUM,//'堆叠数量'
		EXPIRE_TIME,//'过期时间戳秒'
		DURA,//'当前耐久'
		DURA_LIMIT,//'耐久上限'
		ADDITION_LEV,//'强化等级'
		EXTRA,//'额外信息'

	};
	static constexpr const char* field_name[] = { "id","owner_id","itemtype","position","grid","flag","pilenum","expire_time","dura","dura_limit","addition_lev","extra" };
	using field_type_t = type_list<int64_t ,int64_t ,int32_t ,int32_t ,int32_t ,int32_t ,int32_t ,int32_t ,int32_t ,int32_t ,int32_t ,std::string >;
	static constexpr DB_FIELD_TYPES field_type_enum_list[] = { DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_LONG,DB_FIELD_TYPE_LONG,DB_FIELD_TYPE_LONG,DB_FIELD_TYPE_LONG,DB_FIELD_TYPE_LONG,DB_FIELD_TYPE_LONG,DB_FIELD_TYPE_LONG,DB_FIELD_TYPE_LONG,DB_FIELD_TYPE_LONG,DB_FIELD_TYPE_BLOB };
	static constexpr bool pri_key_idx[] = { true,false,false,false,false,false,false,false,false,false,false,false };
};

		
struct TBLD_MAIL
{
	static constexpr const char* table_name = "tbld_mail";
	enum FIELD_ENUMS
	{
		ID,//''
		OWNERID,//'拥有者id'
		SENDERID,//'拥有者id'
		SENDERNAME,//'发送者名字'
		CREATE_TIME,//'发送时间戳'
		FLAG,//'邮件标志'
		SUBJECT,//'邮件标题'
		CONTENT,//'邮件内容'

	};
	static constexpr const char* field_name[] = { "id","ownerid","senderid","sendername","create_time","flag","subject","content" };
	using field_type_t = type_list<int64_t ,int64_t ,int64_t ,char[255] ,int64_t ,int64_t ,char[255] ,char[1024] >;
	static constexpr DB_FIELD_TYPES field_type_enum_list[] = { DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_VARCHAR,DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_VARCHAR,DB_FIELD_TYPE_VARCHAR };
	static constexpr bool pri_key_idx[] = { true,false,false,false,false,false,false,false };
};

		
struct TBLD_MAIL_ATTACHMENT
{
	static constexpr const char* table_name = "tbld_mail_attachment";
	enum FIELD_ENUMS
	{
		ID,//''
		MAILID,//'邮件id'
		ATTACH_TYPE,//'附件类型'
		ATTACH_DATA,//'数量/具体道具实例ID'

	};
	static constexpr const char* field_name[] = { "id","mailid","attach_type","attach_data" };
	using field_type_t = type_list<int64_t ,int64_t ,int64_t ,int64_t >;
	static constexpr DB_FIELD_TYPES field_type_enum_list[] = { DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_LONGLONG };
	static constexpr bool pri_key_idx[] = { true,false,false,false };
};

		
struct TBLD_PET
{
	static constexpr const char* table_name = "tbld_pet";
	enum FIELD_ENUMS
	{
		ID,//''
		OWNERID,//'所有者玩家id'
		PET_TYPE,//'宠物类型'
		HP,//'当前血量'
		EXP,//'当前经验'

	};
	static constexpr const char* field_name[] = { "id","ownerid","pet_type","hp","exp" };
	using field_type_t = type_list<int64_t ,int64_t ,int32_t ,int32_t ,int64_t >;
	static constexpr DB_FIELD_TYPES field_type_enum_list[] = { DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_LONG,DB_FIELD_TYPE_LONG,DB_FIELD_TYPE_LONGLONG };
	static constexpr bool pri_key_idx[] = { true,false,false,false,false };
};

		
struct TBLD_PLAYER
{
	static constexpr const char* table_name = "tbld_player";
	enum FIELD_ENUMS
	{
		ID,//''
		WORLDID,//'服务器编号'
		ORIWORLDID,//'创建角色时的服务器编号'
		OPENID,//'玩家账号'
		NAME,//'玩家名字'
		PROF,//'玩家职业'
		BASELOOK,//'玩家基本外观'
		LEV,//'玩家等级'
		VIPLEV,//'vip等级'
		RECORD_SCENEID,//'玩家当前所在的地图编号'
		RECORD_X,//'玩家当前所在地图x'
		RECORD_Y,//'玩家当前所在地图y'
		RECORD_FACE,//'玩家当前所在地图face'
		HOME_SCENEID,//'玩家回城点对应的地图编号'
		HOME_X,//'玩家回城点对应的地图x'
		HOME_Y,//'玩家回城点对应的地图y'
		HOME_FACE,//'玩家回城点对应的地图face'
		MATE_ID,//'伴侣ID'
		MATE_NAME,//'伴侣名字'
		TEAMID,//'队伍id'
		GUILDID,//'公会id'
		EXP,//'玩家经验'
		MONEY,//'金币'
		MONEY_BIND,//'绑定金币'
		GOLD,//'元宝'
		GOLD_BIND,//'绑定元宝'
		HP,//'hp'
		MP,//'mp'
		FP,//'fp'
		NP,//'np'
		PKVAL,//'pk值'
		HONOR,//'荣誉值'
		ACHIPOINT,//'成就值'
		BAG_SIZE,//'背包大小'
		STROGE_SIZE,//'仓库大小'
		CREATE_TIME,//'创建时间戳'
		LAST_LOGINTIME,//'最后登录时间戳'
		LAST_LOGOUTTIME,//'最后登出时间戳'
		DEL_TIME,//'删除时间戳'

	};
	static constexpr const char* field_name[] = { "id","worldid","oriworldid","openid","name","prof","baselook","lev","viplev","record_sceneid","record_x","record_y","record_face","home_sceneid","home_x","home_y","home_face","mate_id","mate_name","teamid","guildid","exp","money","money_bind","gold","gold_bind","hp","mp","fp","np","pkval","honor","achipoint","bag_size","stroge_size","create_time","last_logintime","last_logouttime","del_time" };
	using field_type_t = type_list<int64_t ,int32_t ,int32_t ,char[255] ,char[32] ,int32_t ,int32_t ,int32_t ,int32_t ,int64_t ,float ,float ,float ,int64_t ,float ,float ,float ,int64_t ,char[32] ,int64_t ,int64_t ,int64_t ,int64_t ,int64_t ,int64_t ,int64_t ,int32_t ,int32_t ,int32_t ,int32_t ,int32_t ,int32_t ,int32_t ,int32_t ,int32_t ,int64_t ,int64_t ,int64_t ,int64_t >;
	static constexpr DB_FIELD_TYPES field_type_enum_list[] = { DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_LONG,DB_FIELD_TYPE_LONG,DB_FIELD_TYPE_VARCHAR,DB_FIELD_TYPE_VARCHAR,DB_FIELD_TYPE_LONG,DB_FIELD_TYPE_LONG,DB_FIELD_TYPE_LONG,DB_FIELD_TYPE_LONG,DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_FLOAT,DB_FIELD_TYPE_FLOAT,DB_FIELD_TYPE_FLOAT,DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_FLOAT,DB_FIELD_TYPE_FLOAT,DB_FIELD_TYPE_FLOAT,DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_VARCHAR,DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_LONG,DB_FIELD_TYPE_LONG,DB_FIELD_TYPE_LONG,DB_FIELD_TYPE_LONG,DB_FIELD_TYPE_LONG,DB_FIELD_TYPE_LONG,DB_FIELD_TYPE_LONG,DB_FIELD_TYPE_LONG,DB_FIELD_TYPE_LONG,DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_LONGLONG };
	static constexpr bool pri_key_idx[] = { true,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false };
};

		
struct TBLD_PLAYER_GUILDINFO
{
	static constexpr const char* table_name = "tbld_player_guildinfo";
	enum FIELD_ENUMS
	{
		ID,//''
		RANK,//'帮派职位'
		SCORE,//'帮派贡献'
		TOTAL_SCORE,//'帮派贡献累计'
		JOIN_TIME,//'入帮时间'

	};
	static constexpr const char* field_name[] = { "id","rank","score","total_score","join_time" };
	using field_type_t = type_list<int64_t ,int32_t ,int32_t ,int32_t ,int64_t >;
	static constexpr DB_FIELD_TYPES field_type_enum_list[] = { DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_LONG,DB_FIELD_TYPE_LONG,DB_FIELD_TYPE_LONG,DB_FIELD_TYPE_LONGLONG };
	static constexpr bool pri_key_idx[] = { true,false,false,false,false };
};

		
struct TBLD_SKILL
{
	static constexpr const char* table_name = "tbld_skill";
	enum FIELD_ENUMS
	{
		ID,//''
		USERID,//'玩家id'
		SKILLTYPE,//'技能编号'
		LEV,//'技能等级'

	};
	static constexpr const char* field_name[] = { "id","userid","skilltype","lev" };
	using field_type_t = type_list<int64_t ,int64_t ,int32_t ,int32_t >;
	static constexpr DB_FIELD_TYPES field_type_enum_list[] = { DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_LONG,DB_FIELD_TYPE_LONG };
	static constexpr bool pri_key_idx[] = { true,false,false,false };
};

		
struct TBLD_TASK
{
	static constexpr const char* table_name = "tbld_task";
	enum FIELD_ENUMS
	{
		ID,//''
		USERID,//'玩家id'
		TASKID,//'任务id'
		NUM0,//'任务计数1'
		NUM1,//'任务计数2'
		NUM2,//'任务计数3'
		NUM3,//'任务计数4'
		ACCEPT_USERLEV,//'接受任务时的等级'
		ACCEPT_TIME,//'接受任务时间戳'
		FINISH_TIME,//'完成任务时间戳'
		EXPIRE_TIME,//'任务过期时间戳'
		DAYCOUNT,//'本日完成次数'
		DAYCOUNT_MAX,//'本日最大可完成次数'
		STATE,//'任务状态'

	};
	static constexpr const char* field_name[] = { "id","userid","taskid","num0","num1","num2","num3","accept_userlev","accept_time","finish_time","expire_time","daycount","daycount_max","state" };
	using field_type_t = type_list<int64_t ,int64_t ,int32_t ,int32_t ,int32_t ,int32_t ,int32_t ,int32_t ,int64_t ,int64_t ,int64_t ,int32_t ,int32_t ,int32_t >;
	static constexpr DB_FIELD_TYPES field_type_enum_list[] = { DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_LONG,DB_FIELD_TYPE_LONG,DB_FIELD_TYPE_LONG,DB_FIELD_TYPE_LONG,DB_FIELD_TYPE_LONG,DB_FIELD_TYPE_LONG,DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_LONG,DB_FIELD_TYPE_LONG,DB_FIELD_TYPE_LONG };
	static constexpr bool pri_key_idx[] = { true,false,false,false,false,false,false,false,false,false,false,false,false,false };
};

		
struct TBLD_ACHIEVEMENT
{
	static constexpr const char* table_name = "tbld_achievement";
	enum FIELD_ENUMS
	{
		ID,//''
		USERID,//'玩家id'
		ACHIID,//'成就id'
		TAKE,//'成就id'

	};
	static constexpr const char* field_name[] = { "id","userid","achiid","take" };
	using field_type_t = type_list<int64_t ,int64_t ,int32_t ,int32_t >;
	static constexpr DB_FIELD_TYPES field_type_enum_list[] = { DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_LONG,DB_FIELD_TYPE_LONG };
	static constexpr bool pri_key_idx[] = { true,false,false,false };
};

		
struct TBLD_STATUS
{
	static constexpr const char* table_name = "tbld_status";
	enum FIELD_ENUMS
	{
		ID,//''
		USERID,//'玩家id'
		TYPEID,//'状态类型编号'
		LEV,//'状态等级'
		POWER,//'数据'
		SECS,//'持续时间'
		TIMES,//'作用次数'
		LASTSTAMP,//'最后一次作用的时间戳'
		CASTERID,//'施加此状态的角色id'
		PAUSE,//'是否暂停'

	};
	static constexpr const char* field_name[] = { "id","userid","typeid","lev","power","secs","times","laststamp","casterid","pause" };
	using field_type_t = type_list<int64_t ,int64_t ,int32_t ,int32_t ,int32_t ,int32_t ,int32_t ,int32_t ,int64_t ,int64_t >;
	static constexpr DB_FIELD_TYPES field_type_enum_list[] = { DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_LONG,DB_FIELD_TYPE_LONG,DB_FIELD_TYPE_LONG,DB_FIELD_TYPE_LONG,DB_FIELD_TYPE_LONG,DB_FIELD_TYPE_LONG,DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_LONGLONG };
	static constexpr bool pri_key_idx[] = { true,false,false,false,false,false,false,false,false,false };
};

		
struct TBLD_SYSTEMVAR
{
	static constexpr const char* table_name = "tbld_systemvar";
	enum FIELD_ENUMS
	{
		KEYIDX,//''
		NAME,//'系统变量名'
		DATA0,//'数据1'
		DATA1,//'数据2'
		DATA2,//'数据3'
		DATA3,//'数据4'
		STR0,//'文字1'
		STR1,//'文字2'
		STR2,//'文字3'
		STR3,//'文字4'

	};
	static constexpr const char* field_name[] = { "keyidx","name","data0","data1","data2","data3","str0","str1","str2","str3" };
	using field_type_t = type_list<int64_t ,char[255] ,int64_t ,int64_t ,int64_t ,int64_t ,char[255] ,char[255] ,char[255] ,char[255] >;
	static constexpr DB_FIELD_TYPES field_type_enum_list[] = { DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_VARCHAR,DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_LONGLONG,DB_FIELD_TYPE_VARCHAR,DB_FIELD_TYPE_VARCHAR,DB_FIELD_TYPE_VARCHAR,DB_FIELD_TYPE_VARCHAR };
	static constexpr bool pri_key_idx[] = { true,false,false,false,false,false,false,false,false,false };
};

		