#include "AIActor.h"
#include "AIActorManager.h"
#include "AIMonster.h"
#include "AIService.h"
#include "NetMSGProcess.h"

void On_CMD_SC_AOI_NEW(CNetworkMessage* pNetworkMsg) {}

void On_CMD_SC_AOI_REMOVE(CNetworkMessage* pNetworkMsg) {}

void On_CMD_SC_AOI_UPDATE(CNetworkMessage* pNetworkMsg)
{
	SC_AOI_UPDATE msg;
	if(msg.ParseFromArray(pNetworkMsg->GetMsgBody(), pNetworkMsg->GetBodySize()) == false)
	{
		return;
	}

	CAIActor* pActor = AIActorManager()->QueryActor(msg.actor_id());
	CHECK(pActor);
	CHECK(pActor->GetCurrentScene());
	CHECK(pActor->GetCurrentScene()->GetMapID() == msg.mapid());

	pActor->SetPos(Vector2(msg.posx(), msg.posy()));
	pActor->UpdateViewList();
	LOGDEBUG("Actor:{} MoveTo {} {:.2f}, {:.2f}", pActor->GetID(), pActor->GetCurrentScene()->GetMapID(), pActor->GetPosX(), pActor->GetPosY());
}

void On_CMD_SC_ATTRIB_CHANGE(CNetworkMessage* pNetworkMsg)
{
	SC_ATTRIB_CHANGE msg;
	if(msg.ParseFromArray(pNetworkMsg->GetMsgBody(), pNetworkMsg->GetBodySize()) == false)
	{
		return;
	}

	CAIActor* pActor = AIActorManager()->QueryActor(msg.actor_id());
	CHECK(pActor);
	for(int32_t i = 0; i < msg.datalist_size(); i++)
	{
		const auto& data = msg.datalist(i);
		pActor->SetProperty(data.actype(), data.val());
	}
}

void On_CMD_SC_CASTSKILL(CNetworkMessage* pNetworkMsg)
{
	SC_CASTSKILL msg;
	if(msg.ParseFromArray(pNetworkMsg->GetMsgBody(), pNetworkMsg->GetBodySize()) == false)
	{
		return;
	}

	CAIActor* pActor = AIActorManager()->QueryActor(msg.actor_id());
	CHECK(pActor);
	pActor->OnCastSkill(msg.skill_id());
}

void On_CMD_SC_SKILL_STUN(CNetworkMessage* pNetworkMsg)
{
	SC_SKILL_STUN msg;
	if(msg.ParseFromArray(pNetworkMsg->GetMsgBody(), pNetworkMsg->GetBodySize()) == false)
	{
		return;
	}

	CAIActor* pActor = AIActorManager()->QueryActor(msg.actor_id());
	CHECK(pActor);
	pActor->OnCastSkillFinish(msg.stun_ms());
}

void On_CMD_SC_SKILL_BREAK(CNetworkMessage* pNetworkMsg)
{
	SC_SKILL_BREAK msg;
	if(msg.ParseFromArray(pNetworkMsg->GetMsgBody(), pNetworkMsg->GetBodySize()) == false)
	{
		return;
	}

	CAIActor* pActor = AIActorManager()->QueryActor(msg.actor_id());
	CHECK(pActor);
	pActor->OnCastSkillFinish(0);
}

void On_CMD_SC_DAMAGE(CNetworkMessage* pNetworkMsg)
{
	SC_DAMAGE msg;
	if(msg.ParseFromArray(pNetworkMsg->GetMsgBody(), pNetworkMsg->GetBodySize()) == false)
	{
		return;
	}

	CAIActor* pActor = AIActorManager()->QueryActor(msg.actor_id());
	CHECK(pActor);
	pActor->OnUnderAttack(msg.attacker_id(), msg.damage());
}

void AIServiceMessageHandlerRegister()
{
	auto pNetMsgProcess = AIService()->GetNetMsgProcess();
	using namespace std::placeholders;
	pNetMsgProcess->Register(CMD_SC_ATTRIB_CHANGE, &On_CMD_SC_ATTRIB_CHANGE);
	pNetMsgProcess->Register(CMD_SC_AOI_UPDATE, &On_CMD_SC_AOI_UPDATE);
	pNetMsgProcess->Register(CMD_SC_AOI_REMOVE, &On_CMD_SC_AOI_REMOVE);
	pNetMsgProcess->Register(CMD_SC_AOI_NEW, &On_CMD_SC_AOI_NEW);
	pNetMsgProcess->Register(CMD_SC_CASTSKILL, &On_CMD_SC_CASTSKILL);
	pNetMsgProcess->Register(CMD_SC_SKILL_STUN, &On_CMD_SC_SKILL_STUN);
	pNetMsgProcess->Register(CMD_SC_SKILL_BREAK, &On_CMD_SC_SKILL_BREAK);
	pNetMsgProcess->Register(CMD_SC_DAMAGE, &On_CMD_SC_DAMAGE);
}
