#ifndef MONSTERTYPE_H
#define MONSTERTYPE_H

#include "ActorAttrib.h"
#include "config/Cfg_Monster.pb.h"

enum MonsterType
{
    MONSTER_TYPE_NORMAL = 0,
    MONSTER_TYPE_ELIT,
    MONSTER_TYPE_BOSS,
};

class CMonsterType: public Noncopyable<CMonsterType>
{
    CMonsterType() {}
public:
    CreateNewImpl(CMonsterType);
public:
    ~CMonsterType() {}
    
    bool Init(const Cfg_Monster_Row& row)
    {
        m_Data = row;
        m_ability.load_from(row.attr_data());

        return true;
    }

    using PB_T = Cfg_Monster;
    uint32_t            GetID() const { return m_Data.id(); }
    uint8_t             GetLevel() const { return m_Data.level(); }
    uint32_t            GetType() const { return m_Data.monster_type(); }
    uint32_t            GetFlag() const { return m_Data.monster_flag(); }
    uint32_t            GetCampID() const { return m_Data.idcamp(); }
    uint32_t            GetKillExp() const { return m_Data.kill_exp(); }
    float               GetHigh() const { return m_Data.high(); }
    float               GetVolume() const { return m_Data.volume(); }
    const std::string&  GetName() const { return m_Data.name(); }
    OBJID               GetScirptID() const { return m_Data.scriptid(); }
    const CActorAttrib& GetAbility() const { return m_ability; }

private:
    Cfg_Monster_Row m_Data;
    CActorAttrib    m_ability;
};

typedef CGameDataMap<CMonsterType> CMonsterTypeSet;

#endif /* MONSTERTYPE_H */
