#ifndef AIMONSTER_H
#define AIMONSTER_H

#include "AIActor.h"
#include "AISkill.h"
#include "AIType.h"
#include "MonsterType.h"
#include "msg/server_side.pb.h"
class CActorAI;
class CAIMonster : public CAIActor
{
  public:
	CAIMonster();
	~CAIMonster();
	CREATE_NEW_IMPL(CAIMonster);
	bool Init(const ServerMSG::ActorCreate& msg);

	virtual ActorType GetActorType() const { return ActorType::ACT_MONSTER; }
	static ActorType  GetActorTypeStatic() { return ActorType::ACT_MONSTER; }

	void		 SetIsAISleep(bool bSleep);
	virtual void OnDead() override;
	virtual void OnBorn() override;
	virtual void OnUnderAttack(OBJID idTarget, int32_t nDamage) override;
	virtual bool IsEnemy(CSceneObject* pTarget) const override;

	uint32_t	  GetGenID() const { return m_idGen; }
	CMonsterType* Type() const { return m_pType; }

	virtual void OnCastSkillFinish(uint32_t stun_ms) override;

	OBJID GetOwnerID() const { return m_idOwner; }
	void  SetOwnerID(OBJID val) { m_idOwner = val; }

  public:
	MEMORYHEAP_DECLARATION(s_heap);

  private:
	uint32_t	  m_idGen;
	OBJID		  m_idOwner;
	CMonsterType* m_pType;
	CAIType*	  m_pAIType;
	CActorAI*	  m_pAI;
};
#endif /* AIMONSTER_H */
