#ifndef MONSTER_H
#define MONSTER_H

#include "Actor.h"
#include "HateList.h"
#include "MonsterType.h"

export_lua class CMonster : public CActor
{
protected:
	CMonster();

public:
	virtual ~CMonster();

	CREATE_NEW_IMPL(CMonster);
	bool Init(uint32_t idMonsterType, OBJID idOwner, uint32_t idGen, uint32_t idCamp);

public:
	export_lua virtual bool SendMessage(uint16_t cmd, const google::protobuf::Message& msg) const override;

	export_lua virtual ActorType GetActorType() const override{ return ActorType::ACT_MONSTER; }
	export_lua static ActorType	 GetActorTypeStatic() { return ActorType::ACT_MONSTER; }

	export_lua virtual uint32_t			  GetLev() const override { return m_pType->GetLevel(); }
	export_lua virtual const std::string& GetName() const override { return m_pType->GetName(); }
	export_lua uint32_t					  GetTypeID() const { return m_pType->GetID(); }

	export_lua virtual uint32_t GetHP() const override { return m_nHP; };
	export_lua virtual uint32_t GetMP() const override { return m_nMP; };
	export_lua virtual OBJID	GetOwnerID() const override { return m_idOwner; }

	export_lua virtual void _SetHP(uint32_t v) override { m_nHP = v; };
	export_lua virtual void _SetMP(uint32_t v) override { m_nMP = v; };
	export_lua virtual bool CanDamage(CActor* pTarget) const override;
	export_lua virtual void BeKillBy(CActor* pAttacker) override;
	virtual void			OnBeAttack(CActor* pAttacker, int nRealDamage) override;

	virtual void OnEnterMap(CSceneBase* pScene) override;
	virtual void OnLeaveMap(uint64_t idTargetScene) override;

	export_lua bool IsBoss() const { return m_pType->GetType() == MONSTER_TYPE_BOSS; }
	export_lua bool IsElit() const { return m_pType->GetType() == MONSTER_TYPE_ELIT; }

	export_lua virtual bool IsEnemy(CSceneObject* pActor) const override;
	void					SetIsAISleep(bool bVal) {}
	virtual void			MakeShowData(SC_AOI_NEW& msg) override;
	template<typename RVal, typename... Args>
	RVal TryExecScript(uint32_t idxCallBackType, Args&&... args)
	{
		if(m_pType->GetScirptID() == 0)
			return RVal();
		else
			return ScriptManager()->TryExecScript<RVal>(m_pType->GetScirptID(), idxCallBackType, std::forward<Args>(args)...);
	}

public:
	MEMORYHEAP_DECLARATION(s_heap);

private:
	uint32_t m_nHP = 0;
	uint32_t m_nMP = 0;

	uint32_t	  m_idGen;
	OBJID		  m_idOwner;
	CMonsterType* m_pType = nullptr;
	CHateList	  m_HateList;
};
#endif /* MONSTER_H */
