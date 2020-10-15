#ifndef __USERATTR_H__
#define __USERATTR_H__

#include "ActorAttrib.h"
#include "BaseCode.h"
#include "DBRecord.h"
#include "ProtobuffUtil.h"
#include "T_GameDataMap.h"
#include "config/Cfg_UserAttr.pb.h"
#include "gamedb.h"
//////////////////////////////////////////////////////////////////////
//
class CUserAttrData : public NoncopyableT<CUserAttrData>
{
    CUserAttrData() {}
    bool Init(const Cfg_UserAttr& row)
    {
        m_dwProfession = row.prof();
        m_nLevel       = row.level();

        CActorAttrib::load_from(m_ability, row.attr_data());

        return true;
    }

public:
    CreateNewImpl(CUserAttrData);

    using PB_T = Cfg_UserAttr;
    virtual ~CUserAttrData() {}

public:
    uint32_t GetID() { return CUserAttrData::MakeID(m_dwProfession, m_nLevel); }

public:
    const AttribList_t& GetAbility() const { return m_ability; }
    uint32_t            GetLevUpNeedExp() const { return m_nLevUpExp; }

public:
    static uint32_t MakeID(uint16_t dwProfession, uint32_t ucLevel) { return ((uint32_t)(dwProfession) << 16) | (uint32_t)(ucLevel & 0xFFFF); }

protected:
    uint16_t m_dwProfession;
    uint16_t m_nLevel;

    uint32_t     m_nLevUpExp;
    AttribList_t m_ability;
};

//////////////////////////////////////////////////////////////////////
DEFINE_GAMEMAPDATA(CUserAttrSet, CUserAttrData);

#endif //__USERATTR_H__
