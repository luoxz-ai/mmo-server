#include "ItemAddition.h"

#include "ProtobuffUtil.h"
CItemAdditionSet::CItemAdditionSet() {}

CItemAdditionSet::~CItemAdditionSet()
{
	__ENTER_FUNCTION
	for(auto& [k, list]: m_setAddition)
	{
		for(auto& v: list)
		{
			SAFE_DELETE(v);
		}
	}
	m_setAddition.clear();
	__LEAVE_FUNCTION
}

bool CItemAdditionSet::Init(const char* szFileName)
{
	__ENTER_FUNCTION
	Cfg_ItemAddition cfg;
	if(pb_util::LoadFromBinaryFile(szFileName, cfg) == false)
	{
		LOGERROR("InitFromFile {} Fail.", szFileName);
		return false;
	}
	for(const auto& iter: cfg.rows())
	{
		auto& refList = m_setAddition[iter.id()];
		auto  pData	  = CItemAdditionData::CreateNew(iter);
		CHECKF(pData);
		refList.push_back(pData);
	}
	return true;
	__LEAVE_FUNCTION
	return false;
}

bool CItemAdditionSet::Reload(const char* szFileName)
{
	__ENTER_FUNCTION
	Destroy();
	return Init(szFileName);
	__LEAVE_FUNCTION
	return false;
}

void CItemAdditionSet::Destroy()
{
	__ENTER_FUNCTION
	for(auto it_map = m_setAddition.begin(); it_map != m_setAddition.end(); it_map++)
	{
		auto& refList = it_map->second;
		for(auto& pData: refList)
		{
			SAFE_DELETE(pData);
		}
		refList.clear();
	}
	m_setAddition.clear();
	__LEAVE_FUNCTION
}

const CItemAdditionData* CItemAdditionSet::QueryItemAddition(uint32_t idType, int nLevel)
{
	__ENTER_FUNCTION
	auto it_map = m_setAddition.find(idType);
	if(it_map == m_setAddition.end())
		return nullptr;
	const auto& refList = it_map->second;
	if(nLevel > (int)refList.size())
		return nullptr;
	return refList[nLevel - 1];
	__LEAVE_FUNCTION
	return nullptr;
}
