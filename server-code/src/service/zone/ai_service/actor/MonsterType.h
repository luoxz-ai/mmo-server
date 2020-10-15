#ifndef MONSTERTYPE_H
#define MONSTERTYPE_H

#include "ActorAttrib.h"
#include "BaseCode.h"
#include "T_GameDataMap.h"
#include "config/Cfg_Monster.pb.h"

enum MonsterType
{
    MONSTER_TYPE_NORMAL = 0,
    MONSTER_TYPE_ELIT,
    MONSTER_TYPE_BOSS,
};

class CMonsterType : public NoncopyableT<CMonsterType>
{
    CMonsterType() {}
    bool Init(const Cfg_Monster& row)
    {
        m_Data = row;
        CActorAttrib::load_from(m_ability, row.attr_data());

        return true;
    }

public:
    CreateNewImpl(CMonsterType);

public:
    ~CMonsterType() {}

    using PB_T = Cfg_Monster;
    uint32_t            GetID() const { return m_Data.id(); }
    uint8_t             GetLevel() const { return m_Data.level(); }
    uint32_t            GetType() const { return m_Data.monster_type(); }
    uint32_t            GetFlag() const { return m_Data.monster_flag(); }
    uint32_t            GetCampID() const { return m_Data.idcamp(); }
    const std::string&  GetName() const { return m_Data.name(); }
    OBJID               GetScirptID() const { return m_Data.scriptid(); }
    uint32_t            GetAITypeID() const { return m_Data.aitype(); }
    const AttribList_t& GetAbility() const { return m_ability; }

private:
    Cfg_Monster  m_Data;
    AttribList_t m_ability;
};

DEFINE_GAMEMAPDATA(CMonsterTypeSet, CMonsterType);

#endif /* MONSTERTYPE_H */
