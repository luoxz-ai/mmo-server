#ifndef AIPHASE_H
#define AIPHASE_H

#include <memory>

#include "MonsterGenerator.h"
#include "NetworkMessage.h"
#include "SceneBase.h"

class CAIScene;
class CAIGroupManager;

export_lua class CAIPhase : public CSceneBase
{
protected:
    CAIPhase();
    bool Init(CAIScene* pScene, const SceneIdx& idxScene, uint64_t idPhase, const PhaseData* pPhaseData);

public:
    CreateNewImpl(CAIPhase);

public:
    virtual ~CAIPhase();

public:
    CMonsterGenerator&  GetMonsterGen() { return m_MonsterGen; }
    export_lua uint64_t GetPhaseID() const { return m_idPhase; }
    export_lua CAIGroupManager* GetAIGroupMgr() const { return m_pAIGroupManager.get(); }

private:
    uint64_t                         m_idPhase;
    CMonsterGenerator                m_MonsterGen;
    std::unique_ptr<CAIGroupManager> m_pAIGroupManager;
};

#endif /* AIPHASE_H */
