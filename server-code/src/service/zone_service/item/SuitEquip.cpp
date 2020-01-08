#include "SuitEquip.h"
#include "ProtobuffUtil.h"
CSuitEquipSet::CSuitEquipSet()
{

}

CSuitEquipSet::~CSuitEquipSet()
{
__ENTER_FUNCTION
	for (auto& [k,list] : m_setSuitType)
	{
		for(auto& v : list)
		{
			SAFE_DELETE(v);
		}
	}
	m_setSuitType.clear();
__LEAVE_FUNCTION
}


bool CSuitEquipSet::Init(const char* szFileName)
{
__ENTER_FUNCTION
	Cfg_Suit cfg;
	if (pb_util::LoadFromBinaryFile(szFileName, cfg) == false)
	{
		LOGERROR("InitFromFile {} Fail.", szFileName);
		return false;
	}
	for (const auto &iter : cfg.rows())
	{
		auto& refList = m_setSuitType[iter.id()];
		auto pData = CSuitEquipData::CreateNew(iter);
		CHECKF(pData);
		refList.insert(refList.begin(), pData);
	}
	return true;
__LEAVE_FUNCTION
	return false;
}

bool CSuitEquipSet::Reload(const char* szFileName)
{
__ENTER_FUNCTION
	Destroy();
	return Init(szFileName);
__LEAVE_FUNCTION
	return false;
}

void CSuitEquipSet::Destroy()
{
__ENTER_FUNCTION
	for(auto it_map = m_setSuitType.begin(); it_map != m_setSuitType.end(); it_map++)
	{
		auto& refList = it_map->second;
		for(auto& pData : refList)
		{
			SAFE_DELETE(pData);
		}
		refList.clear();
	}
	m_setSuitType.clear();
__LEAVE_FUNCTION
}


struct SUITTYPE_ORDER
{
	uint32_t	m_nNum;
	SUITTYPE_ORDER(uint32_t nNum) : m_nNum(nNum)	{}
	bool operator()(CSuitEquipData* lft, CSuitEquipData* rht) const
	{
		if (nullptr != lft && nullptr != rht)
			return lft->GetEquipNum() > rht->GetEquipNum();
		else if (nullptr != lft)
			return lft->GetEquipNum() > m_nNum;
		else if (nullptr != rht)
			return m_nNum > rht->GetEquipNum();
		return false;
	}
};

const CSuitEquipData* CSuitEquipSet::QuerySuitEquip(uint32_t idSuit, uint32_t nEquipNum)
{
__ENTER_FUNCTION

	CHECKF (idSuit!=ID_NONE && nEquipNum > 0);

	auto it=m_setSuitType.find(idSuit);
	if (it == m_setSuitType.end())
		return nullptr;

	
	const auto& typeSet = it->second;

	
	SUITTYPE_ORDER order(nEquipNum);
	auto itFind = std::lower_bound(typeSet.begin(), typeSet.end(), nullptr, order);
	if (itFind != typeSet.end())
		return *itFind;
	
__LEAVE_FUNCTION
	return nullptr;
}



