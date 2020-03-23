#ifndef GAME_COMMON_DEF_H
#define GAME_COMMON_DEF_H

#include "Common.pb.h"

const char GMCMD_PREFIX = '@';
//多少个账号以后开始排队,缓慢进入
const int32_t _START_WAITING_ACCOUNT_COUNT = 2000;
const int32_t _STOP_WAITING_ACCOUNT_COUNT  = 6000;

const int32_t GUESS_MAX_PLAYER_COUNT = 6000;
const int32_t GUESS_MAX_ACTOR_COUNT  = 30000;

//红名的PK点数
const int32_t RED_NAME_PKVAL    = 1000;
const int32_t YELLOW_NAME_PKVAL = 200;
const int32_t ADD_PKVAL_PERKILL = 100;

//组队人数
const int32_t _MAX_TEAM_MEMBER = 5;
//交互距离
const float MIN_INTERACT_DIS = 5;
//经验分享距离
const float MIN_EXP_SHARED_DIS = 30;

const uint64_t  PLAYER_ID_FACTOR_BYWORLD = 1000000000000000ull;
const uint64_t  PLAYER_ID_FACTOR_BYTYPE  = 400000000000000ull;
inline uint32_t GetWorldIDFromPlayerID(uint64_t idPlayer)
{
    return idPlayer / PLAYER_ID_FACTOR_BYWORLD;
}

inline OBJID GetDefaultPlayerID(uint32_t idWorld)
{
    return idWorld * PLAYER_ID_FACTOR_BYWORLD + PLAYER_ID_FACTOR_BYTYPE;
}

//每个账号最多几个角色
const int32_t _MAX_ROLES_PER_ACCOUNT = 4;

//名字长度
const int32_t _MAX_NAME_SIZE = 32;
const int32_t _MIN_NAME_SIZE = 3;

const float   DEFAULT_MOVE_SPEED = 2.0f; //基准移动单位每秒可以移动几米
const int32_t DEFAULT_MOVE_RADIO = 10000;

constexpr uint32_t TALK_MIN_LEV[] = {
    30,         // CHANNEL_NORMAL = 0,
    30,         // CHANNEL_SCENE = 1,
    30,         // CHANNEL_PRIVATE = 2,
    0,          // CHANNEL_TEAM = 3,
    0,          // CHANNEL_GUILD = 4,
    50,         // CHANNEL_WORLD = 5,
    50,         // CHANNEL_TRUMPET = 6,
    0xFFFFFFFF, // CHANNEL_GLOBAL = 7,
    0xFFFFFFFF, // CHANNEL_SYSTEM = 8,
    0xFFFFFFFF, // CHANNEL_RUMOR = 9,
};

enum ActorType
{
    ACT_UNKNOW  = 0,
    ACT_MONSTER = 1, //怪物
    ACT_NPC     = 2, // NPC
    ACT_PET     = 3, //宠物
    ACT_PLAYER  = 4, //玩家
    ACT_MAPITEM = 5, //掉落物
    ACT_BULLET  = 6, //子弹对象
    ACT_MAX
};

//职业
enum PROFESSION
{
    PROF_NEWIBLE = 0, //新手
    PROF_WARRIOR = 1, //战士
    PROF_PRIEST  = 2, //牧师
    PROF_MAGA    = 4, //法师
};

//阵营
enum CAMP_TYPE
{
    CAMP_MONSTER = 0,
    CAMP_PLAYER  = 1,
};

//伤害类型
enum HIT_TYPE
{
    HIT_WEAPON = 0, //物理
    HIT_MAGE   = 1, //魔法
    HIT_HEAL   = 2, //加血
};

//伤害结果
enum DAMAGE_RESULT
{
    DR_NODAMAGE = 0,
    DR_MISS     = -1,
    DR_NOTARGET = -2,
};

#endif /* GAME_COMMON_DEF_H */
