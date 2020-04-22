#ifndef SCENE_H
#define SCENE_H

#include "EventEntry.h"
#include "SceneBase.h"
#include "ZoneService.h"
#include "IDGenPool.h"
#include <functional>

class CActor;
class CMonster;
class CNpc;
class CPlayer;
class CPhase;

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
  
    export_lua CPhase* CreatePhase(uint64_t idPhase);
    export_lua CPhase* CreatePhase(uint64_t idPhase, const PhaseData* pPhaseData);
    export_lua CPhase* QueryPhase(uint64_t idPhase) const;
    export_lua CPhase* _QueryPhase(uint64_t idPhase) const;
    export_lua CPhase* QueryPhaseByIdx(uint32_t idxPhase) const;
    export_lua bool DestoryPhase(uint64_t idPhase);
    export_lua void ForEach(std::function<void(const CPhase*)> func)const;
private:
    SceneID m_idSceneID;
    std::unordered_map<uint64_t, std::unique_ptr<CPhase>> m_setPhase;
    std::unordered_map<uint64_t, CPhase*> m_setPhaseByIdx;
    
    IDGenPool<uint32_t> m_DynaIDPool;
};
#endif /* SCENE_H */
