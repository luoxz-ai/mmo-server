#include "PetSet.h"

#include "Player.h"
CPetSet::CPetSet()
	: m_pOwner(nullptr)
	, m_pCurPet(nullptr)
{
}

CPetSet::~CPetSet()
{
	__ENTER_FUNCTION
	CallBack();
	for(auto& [k, v]: m_setPet)
	{
		SAFE_DELETE(v);
	}
	m_setPet.clear();
	__LEAVE_FUNCTION
}

bool CPetSet::Init(CPlayer* pOwner)
{
	__ENTER_FUNCTION
	CHECKF(pOwner);
	m_pOwner = pOwner;

	auto* pDB = ZoneService()->GetGameDB(m_pOwner->GetWorldID());
	CHECKF(pDB);
	auto result_ptr =
		pDB->Query(TBLD_PET::table_name, fmt::format(FMT_STRING("SELECT * FROM {} WHERE ownerid={} LIMIT 1"), TBLD_PET::table_name, m_pOwner->GetOwnerID()));
	if(result_ptr)
	{
		for(size_t i = 0; i < result_ptr->get_num_row(); i++)
		{
			auto  row				= result_ptr->fetch_row(true);
			CPet* pPet				= CPet::CreateNew(this, std::move(row));
			m_setPet[pPet->GetID()] = pPet;
		}
	}

	return true;
	__LEAVE_FUNCTION
	return false;
}

void CPetSet::SyncAll() {}

void CPetSet::Save() {}

CPet* CPetSet::CallPet(OBJID idPet)
{
	__ENTER_FUNCTION

	auto it = m_setPet.find(idPet);
	if(it != m_setPet.end())
	{
		CPet* pPet = it->second;
		if(pPet != m_pCurPet)
			pPet->CallOut();
		return pPet;
	}
	__LEAVE_FUNCTION
	return nullptr;
}

void CPetSet::CallBack()
{
	__ENTER_FUNCTION
	if(m_pCurPet)
	{
		m_pCurPet->CallBack();
		m_pCurPet = nullptr;
	}
	__LEAVE_FUNCTION
}

void CPetSet::AddPet(OBJID idPetType) {}

void CPetSet::DelPet(OBJID idPet) {}
