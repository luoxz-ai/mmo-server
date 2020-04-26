#include"AISceneManagr.h"
#include"MapManager.h"
#include"AIScene.h"
#include"AIPhase.h"
#include"AIService.h"

CAISceneManager::CAISceneManager() {}

CAISceneManager::~CAISceneManager()
{
    Destory();
}

bool CAISceneManager::Init(uint32_t idZone)
{
    return true;
}

void CAISceneManager::Destory()
{
    for(auto& [k,v]: m_mapScene)
    {
        SAFE_DELETE(v);
        LOGDEBUG("AIScene {} Destroy", k);
    }
    m_mapScene.clear();
}

CAIScene* CAISceneManager::CreateScene(const SceneID& idScene)
{
    uint16_t idMap = idScene.GetMapID();
    auto pMap    = MapManager()->QueryMap(idMap);
    CHECKF(pMap);

    CAIScene* pScene = CAIScene::CreateNew(idScene);
    CHECKF(pScene);

    m_mapScene[idMap] = pScene;

    LOGINFO("AIScene {} Created", idMap);
    m_nStaticScene++;
    return pScene;
}


CAIScene* CAISceneManager::QueryScene(const SceneID& idScene)
{
    auto itFind = m_mapScene.find(idScene.GetMapID());
    if(itFind == m_mapScene.end())
        return nullptr;

    CAIScene* pScene = itFind->second;
    return pScene;
}

CAIPhase* CAISceneManager::QueryPhase(const SceneID& idScene)
{
    CAIScene* pScene = QueryScene(idScene);
    if(pScene == nullptr)
    {
        return nullptr;
    }

    return pScene->QueryPhaseByIdx(idScene.GetPhaseIdx());
}

void CAISceneManager::OnTimer() {}