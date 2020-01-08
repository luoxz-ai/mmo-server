#pragma once


#include "config/Cfg_Pet.pb.h"

#include "ActorAttrib.h"

enum PetType
{
	PET_TYPE_NORMAL = 0,
};

class CPetType
{
	CPetType() {}
public:
	~CPetType() {}
	CREATE_NEW_IMPL(CPetType);
	bool Init(const Cfg_Pet_Row& row)
	{
		m_Data = row;
		m_ability.load_from(row.attr_data());

		return true;
	}


	using PB_T = Cfg_Pet;
	uint32_t GetID() const { return m_Data.id(); }
	uint32_t GetType() const { return  m_Data.pet_type(); }
	uint32_t GetFlag() const { return  m_Data.pet_flag(); }
	float GetHigh() const { return  m_Data.high(); }
	float GetVolume() const { return  m_Data.volume(); }
	const std::string& GetName() const {return m_Data.name(); }
	OBJID GetScirptID() const { return  m_Data.scriptid(); }
	const CActorAttrib& GetAbility() const	{ return m_ability; }

private:
	Cfg_Pet_Row m_Data;
	CActorAttrib m_ability;

};

typedef CGameDataMap<CPetType>	CPetTypeSet;

