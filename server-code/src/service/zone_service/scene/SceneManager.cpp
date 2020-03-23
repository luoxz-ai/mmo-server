#include "SceneManager.h"

#include "DynaScene.h"
#include "Scene.h"
#include "ZoneService.h"

CSceneManager::CSceneManager()
    : m_nStaticScene(0)
{
}

CSceneManager::~CSceneManager()
{
    Destory();
}

bool CSceneManager::Init(uint32_t idZone)
{
    //将所有归属于本Zone的非副本地图进行加载
    MapManager()->ForEach([pThis = this, idZone](CGameMap* pMap) {
        //如果不是本Zone的Map, 不会读取MapData,  副本地图也不需要创建
        if(pMap->IsZoneMap(idZone) == false || pMap->IsDynaMap() == true)
            return;

        pThis->_CreateStaticScene(pMap->GetMapID());
    });

    return true;
}

void CSceneManager::Destory()
{
    for(auto& [k, v]: m_mapScene)
    {
        SAFE_DELETE(v);
    }
    m_mapScene.clear();
}

CScene* CSceneManager::_CreateStaticScene(uint16_t idMap)
{
    CGameMap* pMap = MapManager()->QueryMap(idMap);
    CHECKF(pMap && pMap->IsDynaMap() == false);

    SceneID idScene(ZoneService()->GetServiceID(), idMap, 0);
    CScene* pScene = CScene::CreateNew(idScene);

    m_mapScene[idScene] = pScene;

    LOGMESSAGE("StaticScene {} Created", idMap);
    m_nStaticScene++;
    return pScene;
}

CScene* CSceneManager::CreateDynaScene(uint16_t idMap)
{
    CGameMap* pMap = MapManager()->QueryMap(idMap);
    CHECKF(pMap && pMap->IsDynaMap() == true);

    auto&    refPool    = m_setDynaSceneIDPool[idMap];
    uint32_t idxDynaMap = 0;
    if(refPool.m_IDPool.empty() == false)
    {
        idxDynaMap = refPool.m_IDPool.front();
        refPool.m_IDPool.pop_front();
    }
    else
    {
        refPool.m_lastID++;
        if(refPool.m_lastID == 0)
        {
            // id回滚了，这不可能发生，有大楼子
            // log error
            return nullptr;
        }
        idxDynaMap = refPool.m_lastID;
    }

    SceneID     idScene(ZoneService()->GetServiceID(), idMap, idxDynaMap);
    CDynaScene* pScene = CDynaScene::CreateNew(idScene);

    m_mapScene[idScene] = pScene;

    return pScene;
}

void CSceneManager::DestoryDynaScene(const SceneID& idScene)
{
    if(idScene.GetDynaMapIdx() == 0)
        return;

    auto itFind = m_mapScene.find(idScene);
    if(itFind == m_mapScene.end())
        return;

    CScene* pScene = itFind->second;
}

CScene* CSceneManager::QueryScene(const SceneID& idScene)
{
    auto itFind = m_mapScene.find(idScene);
    if(itFind == m_mapScene.end())
        return nullptr;

    CScene* pScene = itFind->second;
    if(pScene->GetSceneState() == SCENESTATE_WAIT_DESTORY)
    {
        return nullptr;
    }

    return pScene;
}

CScene* CSceneManager::QueryStaticScene(uint16_t idMap)
{
    return QueryScene(SceneID(ZoneService()->GetServiceID(), idMap, 0));
}

size_t CSceneManager::GetSceneCount()
{
    return m_nStaticScene;
}

size_t CSceneManager::GetDynaSceneCount()
{

    return m_mapScene.size() - m_nStaticScene;
}

void CSceneManager::ForEach(const std::function<void(CScene*)>& func)
{
    for(const auto& [k, v]: m_mapScene)
    {
        func(v);
    }
}

void CSceneManager::OnTimer()
{
    for(auto it = m_mapScene.begin(); it != m_mapScene.end();)
    {
        CScene* pScene = it->second;
        if(pScene->NeedDestory() && pScene->CanDestory())
        {
            auto& refPool = m_setDynaSceneIDPool[pScene->GetMapID()];
            refPool.m_IDPool.push_back(pScene->GetSceneID().GetDynaMapIdx());

            // send msg to AI
            ServerMSG::SceneDestory msg;
            msg.set_scene_id(pScene->GetSceneID());
            ZoneService()->SendMsgToAIService(ServerMSG::MsgID_SceneDestory, msg);

            SAFE_DELETE(pScene);
            it = m_mapScene.erase(it);
        }
        else
        {
            it++;
        }
    }
}
