#include "NpcType.h"
#include "ProtobuffUtil.h"
CNpcTypeSet::CNpcTypeSet()
{

}

CNpcTypeSet::~CNpcTypeSet()
{
__ENTER_FUNCTION
	for(auto&[k, v] : m_setData)
	{
		SAFE_DELETE(v);
	}
	m_setData.clear();
	m_setDataByMapID.clear();
__LEAVE_FUNCTION
}


bool CNpcTypeSet::Init(const char* szFileName)
{
__ENTER_FUNCTION
	Cfg_Npc cfg;
	if (pb_util::LoadFromBinaryFile(szFileName, cfg) == false)
	{
		LOGERROR("InitFromFile {} Fail.", szFileName);
		return false;
	}
	for (const auto &iter : cfg.rows())
	{
		auto pData = CNpcType::CreateNew(iter);
		CHECKF(pData);
		m_setData[pData->GetID()] = pData;
		m_setDataByMapID[pData->GetMapID()].push_back(pData);
	}
	return true;
__LEAVE_FUNCTION
	return false;
}

bool CNpcTypeSet::Reload(const char* szFileName)
{
__ENTER_FUNCTION
	Destroy();
	return Init(szFileName);
__LEAVE_FUNCTION
	return false;
}

void CNpcTypeSet::Destroy()
{
__ENTER_FUNCTION
	for(auto& [k,v]: m_setData)
	{
		SAFE_DELETE(v);
	}
	m_setData.clear();
	m_setDataByMapID.clear();
__LEAVE_FUNCTION
}

CNpcType* CNpcTypeSet::QueryObj(uint32_t idType)
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

const std::vector<CNpcType*>* CNpcTypeSet::QueryObjByMapID(uint32_t idMap)
{
__ENTER_FUNCTION
	auto it = m_setDataByMapID.find(idMap);
	if(it != m_setDataByMapID.end())
	{
		return &(it->second);
	}
__LEAVE_FUNCTION
	return nullptr;

}


