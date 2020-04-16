#ifndef PETTYPE_H
#define PETTYPE_H

#include "ActorAttrib.h"
#include "config/Cfg_Pet.pb.h"

enum PetType
{
    PET_TYPE_NORMAL = 0,
};

class CPetType : public NoncopyableT<CPetType>
{
    CPetType() {}
    bool Init(const Cfg_Pet_Row& row)
    {
        m_Data = row;
        CActorAttrib::load_from(m_ability, row.attr_data());

        return true;
    }
public:
    CreateNewImpl(CPetType);
public:
    ~CPetType() {}
     
    

    using PB_T = Cfg_Pet;
    uint32_t               GetID() const { return m_Data.id(); }
    uint32_t               GetType() const { return m_Data.pet_type(); }
    uint32_t               GetFlag() const { return m_Data.pet_flag(); }
    float                  GetHigh() const { return m_Data.high(); }
    float                  GetVolume() const { return m_Data.volume(); }
    const std::string&     GetName() const { return m_Data.name(); }
    OBJID                  GetScirptID() const { return m_Data.scriptid(); }
    const AttribList_t&    GetAbility() const { return m_ability; }
    const AttribDataProto& getData() const { return m_Data.attr_data(); }

private:
    Cfg_Pet_Row  m_Data;
    AttribList_t m_ability;
};

DEFINE_GAMEMAPDATA(CPetTypeSet,CPetType);

#endif /* PETTYPE_H */
