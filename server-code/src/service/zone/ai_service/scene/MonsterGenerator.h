#ifndef MONSTERGENERATOR_H
#define MONSTERGENERATOR_H

#include <unordered_map>
#include <unordered_set>

#include "BaseCode.h"
#include "EventManager.h"
#include "SceneID.h"

class CAIMonster;
class Cfg_Scene_MonsterGenerator;
struct MonsterGenData
{
    MonsterGenData(const Cfg_Scene_MonsterGenerator& _gen_data);
    const Cfg_Scene_MonsterGenerator& gen_data;
    uint32_t                          nIdxGen;
    uint32_t                          nCurGen;
    CEventEntryPtr                    m_pEvent;
    std::unordered_set<CAIMonster*>   m_setMonster;
};

class CAIPhase;
class CGameMap;
class CMonsterGenerator : public NoncopyableT<CMonsterGenerator>
{
    CMonsterGenerator();

    bool Init(CAIPhase* pScene);

public:
    CreateNewImpl(CMonsterGenerator);
    ~CMonsterGenerator();

public:
    void            Generator();
    void            ActiveAll(bool bActive);
    void            ActiveGen(uint32_t idGen, bool bActive);
    void            KillAllGen();
    void            KillGen(uint32_t idGen);
    void            GenOnce(uint32_t idGen, uint64_t idPhase = 0);
    MonsterGenData* QueryGen(uint32_t idGen);
    void            OnMonsterBorn(CAIMonster* pMonster);
    void            OnMonsterDead(CAIMonster* pMonster);

private:
    void OnGenTimer(MonsterGenData* pData);
    void _GenMonster(MonsterGenData* pData, uint64_t idPhase = 0);

    void StartGen(MonsterGenData* pData, bool bCheckRunning = true);
    void StopGen(MonsterGenData* pData);

private:
    SceneIdx        m_idxScene;
    const CGameMap* m_pMap;

    std::unordered_map<uint32_t, MonsterGenData*> m_setGen;
};

#endif /* MONSTERGENERATOR_H */
