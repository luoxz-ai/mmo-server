#include "AchievementType.h"

CAchievementTypeSet::CAchievementTypeSet() {}

CAchievementTypeSet::~CAchievementTypeSet() {}

bool CAchievementTypeSet::Init(const char* szFileName)
{
	__ENTER_FUNCTION
	Cfg_Achievement cfg;
	if(pb_util::LoadFromBinaryFile(szFileName, cfg) == false)
	{
		LOGERROR("InitFromFile {} Fail.", szFileName);
		return false;
	}
	for(const auto& iter: cfg.rows())
	{

		auto pData = CAchievementType::CreateNew(iter);
		CHECKF(pData);
		m_setData[pData->GetID()] = pData;
		m_setDataByCheckType[pData->GetCheckData().check_type()].push_back(pData);
		m_setDataByGroupID[pData->GetGroupID()].push_back(pData);
	}
	return true;
	__LEAVE_FUNCTION
	return false;
}

bool CAchievementTypeSet::Reload(const char* szFileName)
{
	__ENTER_FUNCTION
	Destroy();
	return Init(szFileName);
	__LEAVE_FUNCTION
	return false;
}

void CAchievementTypeSet::Destroy()
{
	__ENTER_FUNCTION
	for(auto& [k, v]: m_setData)
	{
		SAFE_DELETE(v);
	}
	m_setData.clear();
	m_setDataByCheckType.clear();
	m_setDataByGroupID.clear();
	__LEAVE_FUNCTION
}

const std::map<uint32_t, CAchievementType*>& CAchievementTypeSet::GetData() const
{
	return m_setData;
}

CAchievementType* CAchievementTypeSet::GetData(uint32_t idType) const
{
	__ENTER_FUNCTION
	auto it = m_setData.find(idType);
	if(it != m_setData.end())
	{
		return it->second;
	}
	__LEAVE_FUNCTION
	return nullptr;
}

const std::vector<CAchievementType*>* CAchievementTypeSet::QueryAchiemenetByGroupID(uint32_t idGroup) const
{
	__ENTER_FUNCTION
	auto it = m_setDataByGroupID.find(idGroup);
	if(it != m_setDataByGroupID.end())
	{
		return &(it->second);
	}
	__LEAVE_FUNCTION
	return nullptr;
}

const std::vector<CAchievementType*>* CAchievementTypeSet::QueryAchiemenetByCheckType(uint32_t idType) const
{
	__ENTER_FUNCTION
	auto it = m_setDataByCheckType.find(idType);
	if(it != m_setDataByCheckType.end())
	{
		return &(it->second);
	}
	__LEAVE_FUNCTION
	return nullptr;
}
