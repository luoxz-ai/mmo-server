#ifndef ACTORAI_H
#define ACTORAI_H

#include <functional>
#include <string_view>

#include "AIService.h"
#include "AIType.h"
#include "EventManager.h"
#include "HateList.h"
#include "MapManager.h"
#include "SkillType.h"
//最后还是考虑使用最简答的单层状态机来构建AI
//在攻击决策模块中,适量的使用模糊逻辑,来增加随机性

//状态转移图
// ATT_PRATOL <> ATT_PRATOLWAIT
// VV
// ATT_IDLE <> ATT_RANDMOVE
// VV
// ATT_GOBACK
// VV
// ATT_ATTACK <> ATT_SKILL <> ATT_APPROACH
// VV
// ATT_ESCAPE

enum AITaskType
{
	ATT_IDLE = 0,	// idle
	ATT_ATTACK,		//攻击决策
	ATT_APPROACH,	//移动到距离目标N米处
	ATT_SKILL,		//释放某种技能
	ATT_SKILLWAIT,	// wait for skill finish
	ATT_ESCAPE,		//逃离
	ATT_GOBACK,		//快速移动/无敌移动到目标点
	ATT_PRATOL,		// prtrol from the path
	ATT_PRATOLWAIT, // prtrol from the path
	ATT_RANDMOVE,	// random move
	ATT_MAX,
};

class CActorAI;
struct STATE_DATA
{
	std::string_view name;
	typedef void (CActorAI::*ProcessFunc)();
	ProcessFunc func;
};

class CAIActor;
class CAIPathFinder;
class CActorAI
{
	CActorAI();

public:
	~CActorAI();
	CREATE_NEW_IMPL(CActorAI);
	bool Init(CAIActor* pActor, CAIType* pAIType);

	void OnUnderAttack(OBJID idTarget, int32_t nDamage);
	void OnDead();

	void OrderAttack(OBJID idTarget);

	OBJID SearchEnemy();
	bool  FindNextEnemy();
	void  ClearHateList();
	void  AddHate(OBJID idTarget, int32_t nHate);

	bool FindEnemyInHateList();
	bool ForEachInHateList(std::function<bool(ST_HATE_DATA*)> func);
	void Process();

	uint32_t GetState() const;
	void	 ChangeState(uint32_t val);

	void	 SetAISleep(bool v);
	uint32_t GetCurSkillTypeID() const { return m_nCurSkillTypeID; }
	void	 SetCurSkillTypeID(uint32_t val) { m_nCurSkillTypeID = val; }

	static const STATE_DATA& GetStateData(int32_t nState);

public:
	// 随机移动
	bool ToRandMove();
	// 巡逻路径
	bool ToPratol();
	// 转到idle状态
	bool ToIdle();
	// 攻击决策
	bool ToAttack();
	// 转到前进状态
	bool ToApproach();
	// 转到攻击状态
	bool ToSkill();
	bool ToSkillFinish(uint32_t stun_ms);

	// 转到逃跑状态
	bool ToEscape(OBJID idTarget);

	// 回出生点
	bool ToGoBack();
	// 巡逻等待
	bool ToPatrolWait(uint32_t wait_min, uint32_t wait_max);

protected:
	void ProcessAttack();
	void ProcessRandMove();
	//! 空闲状态处理
	void ProcessIdle();
	//! 前进状态处理
	void ProcessApproach();
	//! 逃跑状态处理
	void ProcessEscape();
	//! 返回出生点处理
	void ProcessGoback();
	//! 攻击状态处理
	void ProcessSkill();
	void ProcessSkillWait();
	//! 巡逻状态处理
	void ProcessPatrol();
	void ProcessPatrolWait();

public:
public:
	OBJID		   GetMainTarget() const;
	void		   SetMainTarget(OBJID val);
	CAIActor*	   GetActor() const;
	CAIType*	   GetAIType() const;
	CAIPathFinder* PathFind() const { return m_pAIPathFinder; }

	const Cfg_AIType_Row&					GetAIData() const;
	const Cfg_Scene_Patrol_Row_patrol_data& GetCurPratolData();

private:
	void AddNextCall(uint32_t ms);

	void SetAutoSearchEnemy();
	void _SearchEnemy_CallBack();

	template<class... Args>
	bool TryExecScript(uint32_t idxCallBackType, Args... args)
	{
		if(GetAIData().script_id() != 0)
			return ScriptManager()->TryExecScript<bool>(GetAIData().script_id(), idxCallBackType, std::forward<Args>(args)...);
		return false;
	}

private:
	uint32_t	   m_nState		   = 0;
	CAIActor*	   m_pActor		   = nullptr;
	CAIType*	   m_pAIType	   = nullptr;
	CAIPathFinder* m_pAIPathFinder = nullptr;
	OBJID		   m_idTarget	   = 0;
	Vector2		   m_posTarget;
	Vector2		   m_posRecord;
	Vector2		   m_posBorn;
	float		   m_fTargetDis = 0.0f;

	int32_t						m_nCurPathNode	  = 0;
	uint32_t					m_nCurSkillTypeID = 0;
	const Cfg_Scene_Patrol_Row* m_pPathData		  = nullptr;

	CEventEntryPtr m_Event;
	CEventEntryPtr m_SearchEnemyEvent;

	CHateList m_HateList;
	bool	  m_bSleep = false;
};
#endif /* ACTORAI_H */
