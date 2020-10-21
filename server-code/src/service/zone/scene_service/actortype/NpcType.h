#ifndef NPCTYPE_H
#define NPCTYPE_H

#include "ActorAttrib.h"
#include "T_GameDataMap.h"
#include "config/Cfg_Npc.pb.h"

enum NpcTypeFlag
{
    NPC_TYPE_FLAG_TASK     = 0x00, //任务NPC
    NPC_TYPE_FLAG_SHOP     = 0x01, //商店NPC
    NPC_TYPE_FLAG_INTERACT = 0x02, //交互型NPC
};

class CNpcType : public NoncopyableT<CNpcType>
{
    CNpcType() {}
    bool Init(const Cfg_Npc& row)
    {
        m_Data = row;
        CActorAttrib::load_from(m_ability, row.attr_data());
        return true;
    }

public:
    CreateNewImpl(CNpcType);

public:
    ~CNpcType() {}

    using PB_T = Cfg_Npc;
    uint32_t GetID() const { return m_Data.id(); }
    uint8_t  GetLevel() const { return m_Data.level(); }
    uint32_t GetTypeFlag() const { return m_Data.type_flag(); }

    uint32_t           GetCampID() const { return m_Data.idcamp(); }
    uint32_t           GetShopID() const { return m_Data.shop_id(); }
    const std::string& GetName() const { return m_Data.name(); }
    const std::string& GetShopLinkName() const { return m_Data.shop_linkname(); }
    const std::string& GetDialogText() const { return m_Data.dialog_txt(); }
    float              GetHigh() const { return m_Data.high(); }
    float              GetVolume() const { return m_Data.volume(); }

    OBJID               GetScriptID() const { return m_Data.scriptid(); }
    uint16_t            GetMapID() const { return m_Data.mapid(); }
    uint64_t            GetPhaseID() const { return m_Data.phase_id(); }
    float               GetPosX() const { return m_Data.posx(); }
    float               GetPosY() const { return m_Data.posy(); }
    float               GetFace() const { return m_Data.face(); }
    const AttribList_t& GetAbility() const { return m_ability; }

private:
    Cfg_Npc      m_Data;
    AttribList_t m_ability;
};

//////////////////////////////////////////////////////////////////////
class CNpcTypeSet : public CGameDataContainer<CNpcType>
{
    CNpcTypeSet();

public:
    CreateNewImpl(CNpcTypeSet);

public:
    virtual ~CNpcTypeSet();

public:
    using NpcTypeList     = std::vector<CNpcType*>;
    using PhaseNpcTypeSet = std::unordered_map<uint16_t, NpcTypeList>;
    virtual void Clear() override;
    virtual void BuildIndex() override;

    const CNpcType*    QueryObj(uint32_t idType) const;
    const NpcTypeList* QueryObjByMapID(uint16_t idMap, uint16_t idPhaseType) const;

protected:
    std::unordered_map<uint32_t, CNpcType*>       m_setData;
    std::unordered_map<uint16_t, PhaseNpcTypeSet> m_setDataByMapID;
};

#endif /* NPCTYPE_H */
