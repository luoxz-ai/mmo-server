#pragma once


#include "config/Cfg_Bullet.pb.h"


enum BULLET_EMIT_TYPE
{
	EMIT_NUM,			//直接发射对应数量个
	EMIT_PRETARGET_ONE,	//每个目标1个
	EMIT_PRETARGET_NUM,	//每个目标对应数量个
};

enum BULLET_TARGET_TYPE
{
	BULLET_TARGET_ACTOR,	//追踪目标
	BULLET_TARGET_POS,		//追踪点
};

enum BULLET_MOVE_TYPE
{
	BULLET_MOVE_LINE,	//直线行进
	BULLET_MOVE_SIN,	//sin曲线行进
};

enum BULLET_EXPIRE_TYPE
{
	BULLET_EXPIRE_ARRIVE,	//到达目的地就死亡,同时作用1次
	BULLET_EXPIRE_APPLY,	//作用1次就死亡
	BULLET_EXPIRE_TIME,		//到达时间才死亡
};

class CBulletType
{
	CBulletType() {}
public:
	~CBulletType() {}
	CREATE_NEW_IMPL(CBulletType);
	bool Init(const Cfg_Bullet_Row& row)
	{
		m_Data = row;
		return true;
	}


	using PB_T = Cfg_Bullet;
	uint32_t GetID() const { return m_Data.id(); }
	uint32_t GetNum() const { return  m_Data.num(); }
	uint32_t GetEmitType() const { return  m_Data.emit_type(); }
	uint32_t GetTargetType() const { return  m_Data.target_type(); }
	uint32_t GetApplyTimes() const { return  m_Data.apply_times(); }
	uint32_t GetApplyMS() const { return  m_Data.apply_ms(); }
	uint32_t GetApplyAdjMS() const { return  m_Data.apply_adj_ms(); }
	uint32_t  GetMoveSPD() const  { return  m_Data.move_spd(); }
	uint32_t GetMoveType() const  { return  m_Data.move_type(); }
	uint32_t GetMoveParam() const  { return  m_Data.move_param(); }
	uint32_t GetExpireType() const  { return  m_Data.expire_type(); }
	uint32_t GetExpireMS() const  { return  m_Data.expire_ms(); }
	uint32_t GetSkillID() const   { return  m_Data.skill_id(); }


private:
	Cfg_Bullet_Row m_Data;

};

typedef CGameDataMap<CBulletType>	CBulletTypeSet;

