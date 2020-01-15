#ifndef PET_H
#define PET_H


#include "Actor.h"
#include "PetType.h"

class CPetSet;
export_lua class CPet : public CActor
{
  protected:
	CPet();

  public:
	virtual ~CPet();

	CREATE_NEW_IMPL(CPet);
	bool Init(CPetSet* pPetSet, CDBRecordPtr&& pRecord);

  public:
	export_lua virtual ActorType GetActorType() const { return ActorType::ACT_PET; }
	export_lua static ActorType	 GetActorTypeStatic() { return ActorType::ACT_PET; }
	export_lua virtual uint32_t	 GetHP() const override { return m_nHP; };
	export_lua virtual uint32_t	 GetMP() const override { return m_nMP; };
	export_lua virtual uint32_t	 GetFP() const override { return m_nFP; };
	export_lua virtual uint32_t	 GetNP() const override { return m_nNP; };
	export_lua virtual OBJID	 GetOwnerID() const override { return m_pRecord->Field(TBLD_PET::OWNERID); }
	export_lua uint32_t			 GetPetTypeID() const { return m_pRecord->Field(TBLD_PET::PET_TYPE); }

	export_lua virtual void _SetHP(uint32_t v) override { m_nHP = v; };
	export_lua virtual void _SetMP(uint32_t v) override { m_nMP = v; };
	export_lua virtual void _SetFP(uint32_t v) override { m_nFP = v; };
	export_lua virtual void _SetNP(uint32_t v) override { m_nNP = v; };

	export_lua virtual bool CanDamage(CActor* pTarget) const override;
	export_lua virtual void BeKillBy(CActor* pAttacker) override;
	export_lua virtual bool IsEnemy(CSceneObject* pTarget) const override;
	virtual void			MakeShowData(SC_AOI_NEW& msg) override;
	virtual void			OnEnterMap(CSceneBase* pScene) override;
	virtual void			OnLeaveMap(uint64_t idTargetScene) override;

	export_lua void CallOut();
	export_lua void CallBack();

	void Save();

  public:
	MEMORYHEAP_DECLARATION(s_heap);

  private:
	CPetSet*	 m_pPetSet;
	CPetType*	 m_pType;
	uint32_t	 m_nHP = 0;
	uint32_t	 m_nMP = 0;
	uint32_t	 m_nFP = 0;
	uint32_t	 m_nNP = 0;
	CDBRecordPtr m_pRecord;
};
#endif /* PET_H */
