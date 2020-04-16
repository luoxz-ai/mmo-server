#ifndef MONSTERGENERATOR_H
#define MONSTERGENERATOR_H

#include <unordered_map>
#include <unordered_set>

#include "BaseCode.h"
#include "Cfg_Scene_MonsterGenerator.pb.h"
#include "EventManager.h"
#include "MapManager.h"

class CAIMonster;
struct MonsterGenData
{
    MonsterGenData(const Cfg_Scene_MonsterGenerator_Row& _gen_data)
        : gen_data(_gen_data)
        , nIdxGen(_gen_data.idx())
        , nCurGen(0)
    {
    }
    const Cfg_Scene_MonsterGenerator_Row& gen_data;
    uint32_t                              nIdxGen;
    uint32_t                              nCurGen;
    CEventEntryPtr                        m_pEvent;
    std::unordered_set<CAIMonster*>       m_setMonster;
};

class CAIPhase;
class CMonsterGenerator
{
public:
    CMonsterGenerator();
    ~CMonsterGenerator();

public:
    void            Init(CAIPhase* pScene);
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
    SceneID         m_idScene;
    const CGameMap* m_pMap;

    std::unordered_map<uint32_t, MonsterGenData*> m_setGen;
};

#endif /* MONSTERGENERATOR_H */
