#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "BaseCode.h"
#include "MapManager.h"

class CScene;
export_lua class CSceneManager : public NoncopyableT<CSceneManager>
{
    CSceneManager();
public:
    CreateNewImpl(CSceneManager);
public:
    ~CSceneManager();
    

    bool       Init(uint32_t idZone);
    void       Destory();
    export_lua CScene* _CreateStaticScene(uint16_t idMap);
    export_lua CScene* CreateDynaScene(uint16_t idMap, uint64_t idMainPhase);
    export_lua CPhase* CreatePhase(uint16_t idMap, uint64_t idPhase);
    
    export_lua CScene* _QueryScene(const SceneID& idScene);
    export_lua CPhase* QueryPhase(const SceneID& idScene);
    export_lua CScene* QueryStaticScene(uint16_t idMap);
    export_lua size_t  GetSceneCount();
    export_lua size_t  GetDynaSceneCount();

    export_lua void ForEach(const std::function<void(CScene*)>& func);
    void            OnTimer();

protected:
private:
    std::unordered_map<uint32_t, CScene*> m_mapScene;
    size_t                                        m_nStaticScene;
};
#endif /* SCENEMANAGER_H */
