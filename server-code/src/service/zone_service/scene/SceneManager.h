#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "BaseCode.h"
#include "MapManager.h"

class CScene;
export_lua class CSceneManager : public Noncopyable<CSceneManager>
{
    CSceneManager();
public:
    CreateNewImpl(CSceneManager);
public:
    ~CSceneManager();
    

    bool       Init(uint32_t idZone);
    void       Destory();
    export_lua CScene* _CreateStaticScene(uint16_t idMap);
    export_lua CScene* CreateDynaScene(uint16_t idMap);
    export_lua void    DestoryDynaScene(const SceneID& idScene);

    export_lua CScene* QueryScene(const SceneID& idScene);
    export_lua CScene* QueryStaticScene(uint16_t idMap);
    export_lua size_t  GetSceneCount();
    export_lua size_t  GetDynaSceneCount();

    export_lua void ForEach(const std::function<void(CScene*)>& func);
    void            OnTimer();

protected:
private:
    std::unordered_map<SceneID, CScene*> m_mapScene;
    struct DynaSceneIDPool
    {
        std::deque<uint32_t> m_IDPool;
        uint32_t             m_lastID;
    };
    std::unordered_map<uint16_t, DynaSceneIDPool> m_setDynaSceneIDPool;
    size_t                                        m_nStaticScene;
};
#endif /* SCENEMANAGER_H */
