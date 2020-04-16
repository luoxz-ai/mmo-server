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
    for(auto& pair_val: m_mapScene)
    {
        LOGDEBUG("AIScene {} Destroy", pair_val.first.GetMapID());
        SAFE_DELETE(pair_val.second);
    }
    m_mapScene.clear();
}

CAIScene* CAISceneManager::CreateScene(const SceneID& idScene)
{
    auto pMap    = MapManager()->QueryMap(idScene.GetMapID());
    CHECKF(pMap);

    CAIScene* pScene = CAIScene::CreateNew(idScene);
    CHECKF(pScene);

    m_mapScene[idScene.GetStaticPhaseSceneID()] = pScene;

    LOGDEBUG("AIScene {} Created", idScene.GetMapID());
    m_nStaticScene++;
    return pScene;
}


CAIScene* CAISceneManager::QueryScene(const SceneID& idScene)
{
    auto itFind = m_mapScene.find(idScene.GetStaticPhaseSceneID());
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

    return pScene->QueryPhaseBySceneID(idScene);
}

void CAISceneManager::OnTimer() {}