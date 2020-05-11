#include "MapManager.h"

#include "CallStackDumper.h"
#include "ProtobuffUtil.h"
#include "game_common_def.h"

CMapManager::CMapManager() {}

CMapManager::~CMapManager() {}

bool CMapManager::Init(uint16_t idZone)
{
    //读取配置文件
    {
        Cfg_Scene cfg;
        if(pb_util::LoadFromBinaryFile(GetCfgFilePath() + "Cfg_Scene.bytes", cfg) == false)
        {
            LOGERROR("InitFromFile Cfg_Scene.bytes Fail");
            return false;
        }

        for(const auto& iter: cfg.rows())
        {
            CMapData* pMapData = nullptr;
            //只有本zone需要用到的map，才会加载mapdata
            if(idZone == 0 || iter.idzone() == 0 || iter.idzone() == idZone)
            {
                auto      itFind   = m_vecMapData.find(iter.idmapdata());
                if(itFind != m_vecMapData.end())
                {
                    pMapData = itFind->second.get();
                }
                else
                {
                    pMapData = CMapData::CreateNew(iter.idmapdata());
                    CHECKF(pMapData);
                    m_vecMapData[pMapData->GetMapTemplateID()].reset(pMapData);
                }    
            }
    
            CGameMap* pGameMap = CGameMap::CreateNew(this, iter, pMapData);
            if(pGameMap == nullptr)
            {
                return false;
            }

            m_vecMap[pGameMap->GetMapID()].reset(pGameMap);
        }
    }

    //进入点
    {
        Cfg_Scene_EnterPoint cfg;
        if(pb_util::LoadFromBinaryFile(GetCfgFilePath() + "Cfg_Scene_EnterPoint.bytes", cfg) == false)
        {
            LOGERROR("InitFromFile Cfg_Scene_EnterPoint.bytes Fail");
            return false;
        }

        for(const auto& iter: cfg.rows())
        {
            CGameMap* pGameMap = _QueryMap(iter.idmap());
            if(pGameMap == nullptr)
                continue;

            pGameMap->_setEnterPoint(iter);
        }

        LOGDEBUG("Cfg_Scene_EnterPoint LoadSucc.");
    }

    //进入点
    {
        Cfg_Scene_LeavePoint cfg;
        if(pb_util::LoadFromBinaryFile(GetCfgFilePath() + "Cfg_Scene_LeavePoint.bytes", cfg) == false)
        {
            LOGERROR("InitFromFile Cfg_Scene_LeavePoint.bytes Fail");
            return false;
        }

        for(const auto& iter: cfg.rows())
        {
            CGameMap* pGameMap = _QueryMap(iter.idmap());
            if(pGameMap == nullptr)
                continue;

            pGameMap->_setLeavePoint(iter);
        }

        LOGDEBUG("Cfg_Scene_LeavePoint LoadSucc.");
    }

    //刷怪点
    {
        Cfg_Scene_MonsterGenerator cfg;
        if(pb_util::LoadFromBinaryFile(GetCfgFilePath() + "Cfg_Scene_MonsterGenerator.bytes", cfg) == false)
        {
            LOGERROR("InitFromFile Cfg_Scene_MonsterGenerator.bytes Fail");
            return false;
        }

        for(const auto& iter: cfg.rows())
        {
            CGameMap* pGameMap = _QueryMap(iter.idmap());
            if(pGameMap == nullptr || pGameMap->GetMapData() == nullptr)
                continue;

            pGameMap->_AddMonsterGenerator(iter);
        }

        LOGDEBUG("Cfg_Scene_MonsterGenerator LoadSucc.");
    }

    //巡逻路径
    if(false)
    {
        Cfg_Scene_Patrol cfg;
        if(pb_util::LoadFromBinaryFile(GetCfgFilePath() + "Cfg_Scene_Patrol.bytes", cfg) == false)
        {
            LOGERROR("InitFromFile Cfg_Scene_Patrol.bytes Fail");
            return false;
        }

        for(const auto& iter: cfg.rows())
        {
            CGameMap* pGameMap = _QueryMap(iter.idmap());
            if(pGameMap == nullptr || pGameMap->GetMapData() == nullptr)
                continue;

            pGameMap->_AddPatrol(iter);
        }

        LOGDEBUG("Cfg_Scene_Patrol LoadSucc.");
    }

    // rebornData
    {
        Cfg_Scene_Reborn cfg;
        if(pb_util::LoadFromBinaryFile(GetCfgFilePath() + "Cfg_Scene_Reborn.bytes", cfg) == false)
        {
            LOGERROR("InitFromFile Cfg_Scene_Reborn.bytes Fail");
            return false;
        }

        for(const auto& iter: cfg.rows())
        {
            CGameMap* pGameMap = _QueryMap(iter.idmap());
            if(pGameMap == nullptr)
                continue;

            pGameMap->_AddRebornData(iter);
        }

        LOGDEBUG("Cfg_Scene_Reborn LoadSucc.");
    }

    LOGDEBUG("MapManager LoadSucc.");
    return true;
}

void CMapManager::ForEach(const std::function<void(CGameMap*)>& func) const
{
    for(const auto& v: m_vecMap)
    {
        func(v.second.get());
    }
}

const CGameMap* CMapManager::QueryMap(uint16_t idMap) const
{
    auto it_find = m_vecMap.find(idMap);
    if(it_find != m_vecMap.end())
        return it_find->second.get();
    return nullptr;
}

CGameMap* CMapManager::_QueryMap(uint16_t idMap) const
{
    auto it_find = m_vecMap.find(idMap);
    if(it_find != m_vecMap.end())
        return it_find->second.get();
    return nullptr;
}

const CMapData* CMapManager::QueryMapData(uint16_t idMapTemplate) const
{
    auto it_find = m_vecMapData.find(idMapTemplate);
    if(it_find != m_vecMapData.end())
        return it_find->second.get();
    return nullptr;
}
