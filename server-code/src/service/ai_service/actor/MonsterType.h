#pragma once


#include "config/Cfg_Monster.pb.h"

#include "ActorAttrib.h"

enum MonsterType
{
	MONSTER_TYPE_NORMAL = 0,
	MONSTER_TYPE_ELIT,
	MONSTER_TYPE_BOSS,
};

class CMonsterType
{
	CMonsterType() {}
public:
	~CMonsterType() {}
	CREATE_NEW_IMPL(CMonsterType);
	bool Init(const Cfg_Monster_Row& row)
	{
		m_Data = row;
		m_ability.load_from(row.attr_data());

		return true;
	}


	using PB_T = Cfg_Monster;
	uint32_t GetID() const { return m_Data.id(); }
	UCHAR GetLevel() const { return  m_Data.level(); }
	uint32_t GetType() const { return  m_Data.monster_type(); }
	uint32_t GetFlag() const { return  m_Data.monster_flag(); }
	uint32_t GetCampID() const { return  m_Data.idcamp(); }
	const std::string& GetName() const {return m_Data.name(); }
	OBJID GetScirptID() const { return  m_Data.scriptid(); }
	uint32_t GetAITypeID() const { return m_Data.aitype(); }
	const CActorAttrib& GetAbility() const	{ return m_ability; }

private:
	Cfg_Monster_Row m_Data;
	CActorAttrib m_ability;

};

typedef CGameDataMap<CMonsterType>	CMonsterTypeSet;

