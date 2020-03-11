#include "Monster.h"

#include "Player.h"
#include "Scene.h"
#include "ZoneService.h"
MEMORYHEAP_IMPLEMENTATION(CMonster, s_heap);

CMonster::CMonster() {}

CMonster::~CMonster()
{
	if(GetCurrentScene() != nullptr)
		GetCurrentScene()->LeaveMap(this);
}

bool CMonster::Init(uint32_t idMonsterType, OBJID idOwner, uint32_t idGen, uint32_t idCamp)
{
	__ENTER_FUNCTION
	CMonsterType* pType = MonsterTypeSet()->QueryObj(idMonsterType);
	CHECKF(pType);

	m_idOwner = idOwner;
	m_idGen	  = idGen;
	m_pType	  = pType;
	SetID(ActorManager()->GenMonsterID());
	SetCampID(idCamp);
	CHECKF(CActor::Init());

	m_ActorAttrib = m_pType->GetAbility();

	if(idOwner != 0)
	{
		CPlayer* pPlayer = ActorManager()->QueryPlayer(idOwner);
		if(pPlayer)
		{
			//是否需要根据玩家的属性,放大怪物的属性

			m_idOwner = idOwner;
		}
	}
	RecalcAttrib(true);
	_SetHP(GetHPMax());
	_SetMP(GetMPMax());

	TryExecScript<void>(SCB_MONSTER_ONBORN, this);

	m_pCDSet.reset(CCoolDownSet::CreateNew());
	CHECKF(m_pCDSet.get());

	return true;
	__LEAVE_FUNCTION
	return false;
}

bool CMonster::SendMessage(uint16_t cmd, const google::protobuf::Message& msg) const
{
	__ENTER_FUNCTION
	if(cmd == CMD_SC_SKILL_STUN || cmd == CMD_SC_AOI_UPDATE || cmd == CMD_SC_ATTRIB_CHANGE)
	{
		CNetworkMessage _msg(cmd, msg, ZoneService()->GetServerVirtualSocket(), ZoneService()->GetAIServerVirtualSocket());
		return ZoneService()->SendMsg(_msg);
	}
	return true;
	__LEAVE_FUNCTION
	return false;
}

bool CMonster::CanDamage(CActor* pTarget) const
{
	if(m_idOwner != 0)
	{
		CActor* pOwner = ActorManager()->QueryActor(m_idOwner);
		if(pOwner)
			return pOwner->CanDamage(pTarget);
	}

	return GetCampID() != pTarget->GetCampID();
}

void CMonster::BeKillBy(CActor* pAttacker)
{
	__ENTER_FUNCTION
	CActor::BeKillBy(pAttacker);
	TryExecScript<void>(SCB_MONSTER_ONBEKILL, this);

	CPlayer* pKillerPlayer = nullptr;
	if(pAttacker == nullptr)
	{
		//从HateList取得最高的
		m_HateList.FindIF([this, &pAttacker](ST_HATE_DATA* pHateData) {
			CActor* pActor = ActorManager()->QueryActor(pHateData->idTarget);
			if(pActor == nullptr)
				return false;
			if(GameMath::distance(pActor->GetPos(), GetPos()) > MIN_EXP_SHARED_DIS)
				return false;

			//设置目标
			pAttacker = pActor;
			return true;
		});
	}

	if(pAttacker)
	{
		if(pAttacker->IsPlayer())
		{
			pKillerPlayer = pAttacker->ConvertToDerived<CPlayer>();
		}
		else if(pAttacker->IsPet())
		{
			auto pActor = ActorManager()->QueryActor(pAttacker->GetOwnerID());
			if(pActor)
				pKillerPlayer = pActor->ConvertToDerived<CPlayer>();
		}
	}

	if(pKillerPlayer)
	{
		//产生经验
		uint32_t kill_exp = m_pType->GetKillExp();
		if(pKillerPlayer->GetTeamID() == 0)
		{
			pKillerPlayer->AwardBattleExp(kill_exp, true);
			pKillerPlayer->GetTaskSet()->OnKillMonster(GetTypeID(), true);
		}
		else
		{
			//遍历队员， 在周围的才算
			auto pTeam = TeamManager()->QueryTeam(pKillerPlayer->GetTeamID());
			if(pTeam)
			{
				std::vector<CPlayer*> setMember{pKillerPlayer};
				uint32_t			  nTotalMemberLev = pKillerPlayer->GetLev();
				for(uint32_t i = 0; i < pTeam->GetMemeberAmount(); i++)
				{
					OBJID idMember = pTeam->GetMemberIDByIdx(i);
					if(idMember == pKillerPlayer->GetID())
						continue;
					CActor* pMember = ActorManager()->QueryActor(idMember);
					if(pMember == nullptr)
						continue;
					;
					if(GameMath::distance(pMember->GetPos(), GetPos()) <= MIN_EXP_SHARED_DIS)
					{
						setMember.push_back(pMember->ConvertToDerived<CPlayer>());
						nTotalMemberLev += pMember->GetLev();
					}
				}

				constexpr uint32_t TEAM_EXP_ADJ[] = {
					100, 100, 102, 104, 106, 108, 110,
				};
				//组队加成
				//组队分享
				kill_exp = MulDiv(kill_exp, TEAM_EXP_ADJ[setMember.size()], 100);
				for(CPlayer* pMember: setMember)
				{
					uint32_t nExp = MulDiv(kill_exp, pMember->GetLev(), nTotalMemberLev);
					pMember->AwardBattleExp(nExp, pMember == pKillerPlayer);
					pKillerPlayer->GetTaskSet()->OnKillMonster(GetTypeID(), pMember == pKillerPlayer);
				}
			}
		}

		pKillerPlayer->GetDataCountSet()->AddCount(DATA_ACC_MONSTERKILL, GetTypeID(), 1, UPDATE_FALSE);
		pKillerPlayer->GetDataCountSet()->AddCount(DATA_ACC_SYSTEM, DATA_ACC_SYSTEM_KILL_MONSTER, 1, UPDATE_FALSE);
		if(IsBoss())
			pKillerPlayer->GetDataCountSet()->AddCount(DATA_ACC_SYSTEM, DATA_ACC_SYSTEM_KILL_BOSS, 1, UPDATE_FALSE);
	}

	//前端尸体保留N秒
	GetCurrentScene()->LeaveMap(this);
	DelThis();
	__LEAVE_FUNCTION
}

void CMonster::OnBeAttack(CActor* pAttacker, int32_t nRealDamage)
{
	__ENTER_FUNCTION

	if(pAttacker)
		m_HateList.AddHate(pAttacker->GetID(), nRealDamage);

	CActor::OnBeAttack(pAttacker, nRealDamage);
	TryExecScript<void>(SCB_MONSTER_ONBEATTACK, this, pAttacker, nRealDamage);
	__LEAVE_FUNCTION
}

void CMonster::OnEnterMap(CSceneBase* pScene)
{
	__ENTER_FUNCTION
	CActor::OnEnterMap(pScene);

	ServerMSG::ActorCreate ai_msg;
	ai_msg.set_actor_id(GetID());
	ai_msg.set_scene_id(GetSceneID());
	ai_msg.set_actortype(ACT_MONSTER);
	ai_msg.set_prof(GetTypeID());
	ai_msg.set_lev(GetLev());
	ai_msg.set_campid(GetCampID());
	ai_msg.set_name(GetName());
	ai_msg.set_hp(GetHP());
	ai_msg.set_hpmax(GetHPMax());
	ai_msg.set_mp(GetMP());
	ai_msg.set_mpmax(GetMPMax());
	ai_msg.set_movespd(GetAttrib().get(ATTRIB_MOVESPD));
	ai_msg.set_posx(GetPosX());
	ai_msg.set_posy(GetPosY());
	ai_msg.set_monster_gen_id(m_idGen);
	ai_msg.set_ownerid(GetOwnerID());

	ZoneService()->SendMsgToAIService(ServerMSG::MsgID_ActorCreate, ai_msg);
	__LEAVE_FUNCTION
}

void CMonster::OnLeaveMap(uint64_t idTargetScene)
{
	__ENTER_FUNCTION
	CActor::OnLeaveMap(idTargetScene);

	__LEAVE_FUNCTION
}

bool CMonster::IsEnemy(CSceneObject* pActor) const
{
	__ENTER_FUNCTION
	CHECKF(pActor);
	if(this == pActor)
		return false;

	if(m_idOwner != 0)
	{
		CActor* pOwner = ActorManager()->QueryActor(m_idOwner);
		if(pOwner)
			return pOwner->CanDamage(static_cast<CActor*>(pActor));
	}

	return GetCampID() != static_cast<CActor*>(pActor)->GetCampID();
	__LEAVE_FUNCTION
	return false;
}

void CMonster::MakeShowData(SC_AOI_NEW& msg)
{
	__ENTER_FUNCTION
	msg.set_mapid(GetMapID());
	msg.set_actor_id(GetID());
	msg.set_actortype(ACT_MONSTER);
	msg.set_prof(GetTypeID());
	msg.set_lev(GetLev());

	msg.set_campid(GetCampID());
	msg.set_name(GetName());
	msg.set_hp(GetHP());
	msg.set_hpmax(GetHPMax());
	__LEAVE_FUNCTION
}
