#pragma once

#include "BaseCode.h"
#include "DBRecord.h"
#include "SkillType.h"
#include "gamedb.h"
//每一条技能记录
export_lua class CUserSkillData
{
public:
	CUserSkillData();
	~CUserSkillData();
public:
	CREATE_NEW_IMPL(CUserSkillData);
	bool	Init(CActor* pOwner, uint32_t idSkillSort, uint32_t nLev);
	bool	Init(CActor* pOwner, CDBRecordPtr&& pRow);

	export_lua uint32_t GetSkillSort() const {return m_pData->Field(TBLD_SKILL::SKILLTYPE);}
	export_lua uint32_t GetSkillLev() const {return m_pData->Field(TBLD_SKILL::LEV);}
	export_lua void SetSkillLev(uint32_t nLev, bool bUpdate = true);
	export_lua void Update();
private:
	CDBRecordPtr m_pData;
};

//玩家的技能表
class CPlayer;
export_lua class CUserSkillManager
{
public:
	CUserSkillManager();
	~CUserSkillManager();
public:
	CREATE_NEW_IMPL(CUserSkillManager);
	bool	Init(CPlayer* pOwner);

	export_lua bool	LearnSkill(uint32_t idSkillSort);
	export_lua bool	CastSkill(uint32_t idSkillSort, OBJID idTarget, const Vector2& pos);
	export_lua bool	UpgradeSkill(uint32_t idSkillSort);
	export_lua CUserSkillData* _QuerySkill(uint32_t idSkillSort) const;
	void	OnRecalcAttrib(CActorAttribCalc& calc) const;
private:
	
	CPlayer* m_pOwner = nullptr;
	std::map<uint32_t, CUserSkillData*> m_setSkillData;
};