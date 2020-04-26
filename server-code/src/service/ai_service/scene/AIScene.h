#ifndef AISCENE_H
#define AISCENE_H

#include <unordered_map>

#include "MapManager.h"
#include "NetworkMessage.h"

class CAIPhase;
class PhaseData;
class CAISceneManager;
class CAIPhase;

export_lua class CAIScene
{
protected:
    CAIScene();
    bool Init(const SceneID& idScene);

public:
    CreateNewImpl(CAIScene);

public:
    virtual ~CAIScene();

public:
    export_lua CAIPhase* CreatePhase(const SceneID& idScene, uint64_t idPhase);
    export_lua CAIPhase* CreatePhase(const SceneID& idScene, uint64_t idPhase, const PhaseData* pPhaseData);

    export_lua bool DestoryPhase(uint64_t idPhase);
    export_lua bool DestoryPhaseByIdx(uint32_t idxPhase);

    export_lua CAIPhase* QueryPhaseByIdx(uint32_t idxPhase) const;
    export_lua CAIPhase* QueryPhaseByID(uint64_t idPhase) const;

private:
    SceneID                                                 m_SceneID;
    std::unordered_map<uint64_t, std::unique_ptr<CAIPhase>> m_pPhaseSet;
    std::unordered_map<uint32_t, CAIPhase*>                 m_pPhaseSetByIdx;
};

#endif /* AISCENE_H */
