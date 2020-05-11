#include "AIPhase.h"
#include "AIScene.h"
#include "AISceneManagr.h"
#include "AIService.h"
#include "MapManager.h"

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
    for(auto& [k, v]: m_mapScene)
    {
        SAFE_DELETE(v);
        LOGDEBUG("AIScene {} Destroy", k);
    }
    m_mapScene.clear();
}

CAIScene* CAISceneManager::CreateScene(uint16_t idMap)
{
    auto     pMap  = MapManager()->QueryMap(idMap);
    CHECKF(pMap);

    CAIScene* pScene = CAIScene::CreateNew(idMap);
    CHECKF(pScene);

    m_mapScene[idMap] = pScene;

    LOGINFO("AIScene {} Created", idMap);
    m_nStaticScene++;
    return pScene;
}

CAIScene* CAISceneManager::QueryScene(const SceneIdx& idxScene)
{
    auto itFind = m_mapScene.find(idxScene.GetMapID());
    if(itFind == m_mapScene.end())
        return nullptr;

    CAIScene* pScene = itFind->second;
    return pScene;
}

CAIPhase* CAISceneManager::QueryPhase(const SceneIdx& idxScene)
{
    CAIScene* pScene = QueryScene(idxScene);
    if(pScene == nullptr)
    {
        return nullptr;
    }

    return pScene->QueryPhaseByIdx(idxScene.GetPhaseIdx());
}

void CAISceneManager::OnTimer() {}