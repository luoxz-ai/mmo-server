#ifndef AIPHASE_H
#define AIPHASE_H

#include "MonsterGenerator.h"
#include "NetworkMessage.h"
#include "SceneBase.h"

class CAIScene;

export_lua class CAIPhase : public CSceneBase
{
protected:
    CAIPhase();
    bool Init(CAIScene* pScene, const SceneID& idScene, uint64_t idPhase, const PhaseData* pPhaseData);
public:
    CreateNewImpl(CAIPhase);
public:
    virtual ~CAIPhase();

public:
    
    CMonsterGenerator& GetMonsterGen() { return m_MonsterGen; }
    export_lua uint64_t GetPhaseID() const {return m_idPhase;}
private:
    uint64_t m_idPhase;
    CMonsterGenerator m_MonsterGen;
};


#endif /* AIPHASE_H */
