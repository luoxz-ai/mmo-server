#ifndef PETSET_H
#define PETSET_H


#include <unordered_map>

#include "Pet.h"
class CPetSet
{
	CPetSet();

  public:
	~CPetSet();
	CREATE_NEW_IMPL(CPetSet);
	bool Init(CPlayer* pOwner);

	void  SyncAll();
	void  Save();
	CPet* CallPet(OBJID idPet);
	void  CallBack();

  public:
	//对宠物的一些升级,合成操作等
	void AddPet(OBJID idPetType);
	void DelPet(OBJID idPet);

  private:
	CPlayer*						 m_pOwner;
	CPet*							 m_pCurPet;
	std::unordered_map<OBJID, CPet*> m_setPet;
};
#endif /* PETSET_H */
