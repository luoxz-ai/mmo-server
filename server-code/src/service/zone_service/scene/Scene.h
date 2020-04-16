#ifndef SCENE_H
#define SCENE_H

#include "EventEntry.h"
#include "SceneBase.h"
#include "ZoneService.h"
#include "MapVal.h"
#include "Phase.h"
#include "IDGenPool.h"

class CActor;
class CMonster;
class CNpc;
export_lua class CScene
{
protected:
    CScene();
public:
    CreateNewImpl(CScene);
public:
    virtual ~CScene();
    

public:
    bool            Init(const SceneID& idScene, uint64_t idMainPhase);

    export_lua void KickAllPlayer(const char* pszReason = "");
    export_lua void _KickPlayer(const char* pszReason, CPlayer* pPlayer);

   
    export_lua CPhase* CreatePhase(uint64_t idPhase);
    export_lua CPhase* CreatePhase(uint64_t idPhase, PhaseData* pPhaseData);
    export_lua CPhase* QueryPhase(uint64_t idPhase) const;
    export_lua bool DestoryPhase(uint64_t idPhase);
    export_lua void ForEach(std::function<void(CPhase*)>&& func);
private:
    SceneID m_idSceneID;
    std::unordered_map<uint64_t, std::unique_ptr<CPhase>> m_setPhase;
    
    IDGenPool<uint32_t> m_DynaIDPool;
};
#endif /* SCENE_H */
