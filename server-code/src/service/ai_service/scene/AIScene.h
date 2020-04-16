#ifndef AISCENE_H
#define AISCENE_H

#include "NetworkMessage.h"
#include <unordered_map>
#include "MapManager.h"

class CAIPhase;
class PhaseData;
class CAISceneManager;
class CAIPhase;

class CAIScene
{
protected:
    CAIScene();
    bool         Init(const SceneID& idScene);
public:
    CreateNewImpl(CAIScene);
public:
    virtual ~CAIScene();

public:
    
    CAIPhase* CreatePhase(const SceneID& idScene, uint64_t idPhase);
    CAIPhase* CreatePhase(const SceneID& idScene, uint64_t idPhase, const PhaseData* pPhaseData);

    bool DestoryPhase(uint64_t idPhase);
    bool DestoryPhaseBySceneID(const SceneID& idScene);

    CAIPhase* QueryPhaseBySceneID(const SceneID& idScene) const;
    CAIPhase* QueryPhaseByID(uint64_t idPhase) const;
private:
    SceneID m_SceneID;
    std::unordered_map<uint64_t, std::unique_ptr<CAIPhase>> m_pPhaseSet;
    std::unordered_map<uint32_t, CAIPhase*> m_pPhaseSetByIdx;
};

#endif /* AISCENE_H */
