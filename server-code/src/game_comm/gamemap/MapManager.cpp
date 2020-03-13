#include "MapManager.h"

#include "CallStackDumper.h"
#include "ProtobuffUtil.h"

CMapManager::CMapManager() {}

CMapManager::~CMapManager() 
{
	for(auto& [k,v] : m_vecMapData)
	{
		SAFE_DELETE(v);
	}
	for(auto& [k,v] : m_vecMap)
	{
		SAFE_DELETE(v);
	}
	
}

bool CMapManager::Init(uint16_t idZone)
{
	//读取配置文件
	{
		Cfg_Scene cfg;
		if(pb_util::LoadFromBinaryFile("res/config/Cfg_Scene.bytes", cfg) == false)
		{
			LOGERROR("InitFromFile res/config/Cfg_Scene.bytes Fail");
			return false;
		}

		for(const auto& iter: cfg.rows())
		{
			CMapData* pMapData = nullptr;
			if(idZone == 0 || iter.idzone() == 0 || iter.idzone() == idZone)
			{
				auto itFind = m_vecMapData.find(iter.idmapdata());
				if(itFind != m_vecMapData.end())
				{
					pMapData = itFind->second;
				}
				else
				{
					pMapData = CMapData::CreateNew(iter.idmapdata());
					CHECKF(pMapData);
					m_vecMapData[pMapData->GetMapTemplateID()] = pMapData;
					
				}
			}

			CGameMap* pGameMap = new CGameMap(this, iter, pMapData);
			if(pGameMap == nullptr)
			{
				return false;
			}

			m_vecMap[pGameMap->GetMapID()] = pGameMap;
		}
	}

	//进入点
	{
		Cfg_Scene_EnterPoint cfg;
		if(pb_util::LoadFromBinaryFile("res/config/Cfg_Scene_EnterPoint.bytes", cfg) == false)
		{
			LOGERROR("InitFromFile res/config/Cfg_Scene_EnterPoint.bytes Fail");
			return false;
		}

		for(const auto& iter: cfg.rows())
		{
			CGameMap* pGameMap = m_vecMap[iter.idmap()];
			if(pGameMap == nullptr)
				continue;

			pGameMap->_setEnterPoint(iter);
		}

		LOGMESSAGE("Cfg_Scene_EnterPoint LoadSucc.");
	}

	//进入点
	{
		Cfg_Scene_LeavePoint cfg;
		if(pb_util::LoadFromBinaryFile("res/config/Cfg_Scene_LeavePoint.bytes", cfg) == false)
		{
			LOGERROR("InitFromFile res/config/Cfg_Scene_LeavePoint.bytes Fail");
			return false;
		}

		for(const auto& iter: cfg.rows())
		{
			CGameMap* pGameMap = m_vecMap[iter.idmap()];
			if(pGameMap == nullptr)
				continue;

			pGameMap->_setLeavePoint(iter);
		}

		LOGMESSAGE("Cfg_Scene_LeavePoint LoadSucc.");
	}

	//刷怪点
	{
		Cfg_Scene_MonsterGenerator cfg;
		if(pb_util::LoadFromBinaryFile("res/config/Cfg_Scene_MonsterGenerator.bytes", cfg) == false)
		{
			LOGERROR("InitFromFile res/config/Cfg_Scene_MonsterGenerator.bytes Fail");
			return false;
		}

		for(const auto& iter: cfg.rows())
		{
			CGameMap* pGameMap = m_vecMap[iter.idmap()];
			if(pGameMap == nullptr)
				continue;

			pGameMap->_AddMonsterGenerator(iter);
		}

		LOGMESSAGE("Cfg_Scene_MonsterGenerator LoadSucc.");
	}

	//巡逻路径
	{
		/*Cfg_Scene_Patrol cfg;
		if (pb_util::LoadFromBinaryFile("res/config/Cfg_Scene_Patrol.bytes", cfg) == false)
		{
			LOGERROR("InitFromFile res/config/Cfg_Scene_Patrol.bytes Fail");
			return false;
		}

		for (const auto &iter : cfg.rows())
		{
			CGameMap* pGameMap = m_vecMap[iter.idmap()];
			if(pGameMap == nullptr)
				continue;

			pGameMap->_AddPatrol(iter);
		}

		LOGMESSAGE("Cfg_Scene_Patrol LoadSucc.");*/

	}

	// rebornData
	{
		Cfg_Scene_Reborn cfg;
		if(pb_util::LoadFromBinaryFile("res/config/Cfg_Scene_Reborn.bytes", cfg) == false)
		{
			LOGERROR("InitFromFile res/config/Cfg_Scene_Reborn.bytes Fail");
			return false;
		}

		for(const auto& iter: cfg.rows())
		{
			CGameMap* pGameMap = m_vecMap[iter.idmap()];
			if(pGameMap == nullptr)
				continue;

			pGameMap->_AddRebornData(iter);
		}

		LOGMESSAGE("Cfg_Scene_Reborn LoadSucc.");
	}

	return true;
}

void CMapManager::ForEach(const std::function<void(CGameMap*)>& func)
{
	for(const auto& v: m_vecMap)
	{
		func(v.second);
	}
}

CGameMap* CMapManager::QueryMap(uint16_t idMap)
{
	auto it_find = m_vecMap.find(idMap);
	if(it_find != m_vecMap.end())
		return it_find->second;
	return nullptr;
}

CMapData* CMapManager::QueryMapData(uint16_t idMapTemplate)
{
	auto it_find = m_vecMapData.find(idMapTemplate);
	if(it_find != m_vecMapData.end())
		return it_find->second;
	return nullptr;
}
