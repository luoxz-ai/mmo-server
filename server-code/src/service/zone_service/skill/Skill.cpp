#include "Skill.h"

#include "Player.h"
#include "ZoneService.h"
CUserSkillData::CUserSkillData() {}

CUserSkillData::~CUserSkillData() {}

bool CUserSkillData::Init(CActor* pOwner, uint32_t idSkillSort, uint32_t nLev)
{
	__ENTER_FUNCTION
	auto pDB = ZoneService()->GetGameDB(pOwner->GetWorldID());
	CHECKF(pDB);
	m_pData								  = pDB->MakeRecord(TBLD_SKILL::table_name);
	m_pData->Field(TBLD_SKILL::ID)		  = ZoneService()->CreateUID();
	m_pData->Field(TBLD_SKILL::USERID)	  = pOwner->GetID();
	m_pData->Field(TBLD_SKILL::SKILLTYPE) = idSkillSort;
	m_pData->Field(TBLD_SKILL::LEV)		  = nLev;
	CHECKF(m_pData->Update(true));
	return true;
	__LEAVE_FUNCTION
	return false;
}

bool CUserSkillData::Init(CActor* pOwner, CDBRecordPtr&& pRow)
{
	__ENTER_FUNCTION
	m_pData.reset(pRow.release());
	return true;
	__LEAVE_FUNCTION
	return false;
}

void CUserSkillData::SetSkillLev(uint32_t nLev, bool bUpdate /*= true*/)
{
	__ENTER_FUNCTION
	m_pData->Field(TBLD_SKILL::LEV) = nLev;
	if(bUpdate)
		m_pData->Update();
	__LEAVE_FUNCTION
}

void CUserSkillData::Update()
{
	__ENTER_FUNCTION
	m_pData->Update();
	__LEAVE_FUNCTION
}

//////////////////////////////////////////////////////////////////////////
CUserSkillManager::CUserSkillManager() {}

CUserSkillManager::~CUserSkillManager()
{
	__ENTER_FUNCTION
	for(auto& [k, v]: m_setSkillData)
	{
		SAFE_DELETE(v);
	}
	m_setSkillData.clear();
	__LEAVE_FUNCTION
}

bool CUserSkillManager::Init(CPlayer* pOwner)
{
	__ENTER_FUNCTION
	m_pOwner = pOwner;
	auto pDB = ZoneService()->GetGameDB(m_pOwner->GetWorldID());
	CHECKF(pDB);
	auto pResult = pDB->Query(TBLD_SKILL::table_name, fmt::format(FMT_STRING("SELECT * FROM {} WHERE userid={}"), TBLD_SKILL::table_name, m_pOwner->GetID()));
	if(pResult)
	{
		for(size_t i = 0; i < pResult->get_num_row(); i++)
		{
			auto			row	  = pResult->fetch_row(true);
			CUserSkillData* pData = CUserSkillData::CreateNew(m_pOwner, std::move(row));
			if(pData)
			{
				m_setSkillData[pData->GetSkillSort()] = pData;
				CSkillType* pSkillType				  = SkillTypeSet()->QueryObj(CSkillType::MakeID(pData->GetSkillSort(), pData->GetSkillLev()));
				if(pSkillType && pSkillType->GetSkillType() == SKILLTYPE_PASSIVE)
				{
					const auto& refList = pSkillType->GetAttrib();
					m_pOwner->GetAttrib().Store(refList);
				}
			}
			else
			{
				// logerror
			}
		}
	}
	return true;
	__LEAVE_FUNCTION
	return false;
}

bool CUserSkillManager::LearnSkill(uint32_t idSkillSort)
{
	__ENTER_FUNCTION
	auto pSkillData = _QuerySkill(idSkillSort);
	if(pSkillData != nullptr)
		return false;

	CSkillType* pSkillType = SkillTypeSet()->QueryObj(CSkillType::MakeID(idSkillSort, 1));
	if(pSkillType == nullptr)
		return false;

	//检查各种学习需求

	//学习技能
	CUserSkillData* pData = CUserSkillData::CreateNew(m_pOwner, idSkillSort, 1);
	if(pData)
	{
		m_setSkillData[pData->GetSkillSort()] = pData;
		if(pSkillType && pSkillType->GetSkillType() == SKILLTYPE_PASSIVE)
		{
			const auto& refList = pSkillType->GetAttrib();
			m_pOwner->GetAttrib().Store(refList);
		}

		m_pOwner->GetAchievement()->CheckAchiCondition(CONDITION_SKILL_LEARN, idSkillSort, 1);

		// lua call onSkillLearn

		//通知前端
		return true;
	}
	else
	{
		// logerror
		return false;
	}

	__LEAVE_FUNCTION
	return false;
}

bool CUserSkillManager::CastSkill(uint32_t idSkillSort, OBJID idTarget, const Vector2& pos)
{
	__ENTER_FUNCTION
	auto pSkillData = _QuerySkill(idSkillSort);
	if(pSkillData != nullptr)
		return false;

	return m_pOwner->_CastSkill(CSkillType::MakeID(idSkillSort, pSkillData->GetSkillLev()), idTarget, pos);
	__LEAVE_FUNCTION
	return false;
}

bool CUserSkillManager::UpgradeSkill(uint32_t idSkillSort)
{
	__ENTER_FUNCTION
	auto pSkillData = _QuerySkill(idSkillSort);
	if(pSkillData == nullptr)
		return false;

	CSkillType* pSkillType = SkillTypeSet()->QueryObj(CSkillType::MakeID(idSkillSort, pSkillData->GetSkillLev() + 1));
	if(pSkillType == nullptr)
		return false;

	//检查各种学习需求

	//属性
	if(pSkillType->GetSkillType() == SKILLTYPE_PASSIVE)
	{
		CSkillType* pOldSkillType = SkillTypeSet()->QueryObj(CSkillType::MakeID(idSkillSort, pSkillData->GetSkillLev()));
		if(pOldSkillType)
		{
			const auto& refList = pOldSkillType->GetAttrib();
			m_pOwner->GetAttrib().Remove(refList);
		}
	}

	pSkillData->SetSkillLev(pSkillData->GetSkillLev() + 1, UPDATE_TRUE);

	if(pSkillType && pSkillType->GetSkillType() == SKILLTYPE_PASSIVE)
	{
		const auto& refList = pSkillType->GetAttrib();
		m_pOwner->GetAttrib().Store(refList);
	}

	m_pOwner->GetAchievement()->CheckAchiCondition(CONDITION_SKILL_LEARN, idSkillSort, pSkillData->GetSkillLev());
	return true;
	__LEAVE_FUNCTION
	return false;
}

CUserSkillData* CUserSkillManager::_QuerySkill(uint32_t idSkillSort) const
{
	__ENTER_FUNCTION
	auto it = m_setSkillData.find(idSkillSort);
	if(it == m_setSkillData.end())
		return nullptr;
	return it->second;
	__LEAVE_FUNCTION
	return nullptr;
}
