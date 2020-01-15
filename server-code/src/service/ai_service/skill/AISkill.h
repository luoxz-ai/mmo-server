#ifndef AISKILL_H
#define AISKILL_H

#include "AIFuzzyLogic.h"
#include "SkillType.h"

class CAISkillData
{
	CAISkillData();

public:
	~CAISkillData();
	CREATE_NEW_IMPL(CAISkillData);
	bool Init(uint32_t idSkill);

public:
	void StartCoolDown();
	bool IsCoolDown();

	double		GetUtilityValue(double dist, double self_hp, double self_mp, double target_hp);
	CSkillType* GetSkillType() const;
	SkillFAM*	GetSkillFAM() const;
	uint32_t	GetSkillTypeID() const;

private:
	uint32_t	m_idSkill	 = 0;
	CSkillType* m_pSkillType = nullptr;
	SkillFAM*	m_pSkillFAM	 = nullptr;
	uint64_t	m_tCoolDown	 = 0;
};

class CAIMonster;
class CAISkillSet
{
public:
	CAISkillSet();
	~CAISkillSet();

	void AddSkill(uint32_t idSkill);

	bool CastSkill(CAISkillData* pSkill);

	CAISkillData* ChooseSkill(double dist, double self_hp, double self_mp, double target_hp);
	void		  OnCastSkill(uint32_t idSkill);

private:
	CAIMonster*									m_pMonster;
	std::unordered_map<uint32_t, CAISkillData*> m_setSkill;
};
#endif /* AISKILL_H */
