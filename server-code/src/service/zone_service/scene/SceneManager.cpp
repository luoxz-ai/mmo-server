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
    MapManager()->ForEach([pThis = this, idZone](CGameMap* pMap) 
    {
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

CScene* CSceneManager::CreatePhase(uint16_t idMap, uint64_t idMainPhase)
{
    const CGameMap* pMap = MapManager()->QueryMap(idMap);
    CHECKF(pMap && pMap->IsZoneMap(ZoneService()->GetZoneID()));
    CScene* pScene = QueryScene(idMap);
    if(pScene)
    {
        return pScene->CreatePhase(idMainPhase);
    }

    SceneID idScene(ZoneService()->GetServiceID(), idMap, 0);
    CScene* pScene = CScene::CreateNew(idScene, idMainPhase);

    m_mapScene[idScene.GetSceneID()] = pScene;

    LOGMESSAGE("DynaScene {} Created", idMap);
   
    return pScene->QueryPhase(idMainPhase);
}

CScene* CSceneManager::_CreateStaticScene(uint16_t idMap)
{
    const CGameMap* pMap = MapManager()->QueryMap(idMap);
    CHECKF(pMap && pMap->IsDynaMap() == false);

    SceneID idScene(ZoneService()->GetServiceID(), idMap, 0);
    CScene* pScene = CScene::CreateNew(idScene, 0);

    m_mapScene[idScene.GetSceneID()] = pScene;

    LOGMESSAGE("StaticScene {} Created", idMap);
    m_nStaticScene++;
    return pScene;
}

CScene* CSceneManager::_QueryScene(const SceneID& idScene)
{
    auto itFind = m_mapScene.find(idScene.GetSceneID());
    if(itFind != m_mapScene.end())
        return itFind->second;
    return nullptr;
}

CPhase* CSceneManager::QueryPhase(const SceneID& idScene)
{
    CScene* pScene = _QueryScene(idScene); 
    if(pScene == nullptr)
    {
        return nullptr;
    }
    auto pPhase = pScene->QueryPhaseByIdx(idScene.GetPhaseIdx());
    return pPhase;
}

CScene* CSceneManager::QueryScene(uint16_t idMap)
{
    return _QueryScene(SceneID(ZoneService()->GetServiceID(), idMap, 0));
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
