#include "AIService.h"
#include "NetSocket.h"
#include "tinyxml2/tinyxml2.h"
#include "MessageRoute.h"
#include "EventManager.h"
#include "SettingMap.h"
#include "NetMSGProcess.h"
#include "NetworkMessage.h"
#include "MessageRoute.h"
#include "MessagePort.h"
#include "NetMSGProcess.h"
#include "AIMonster.h"
#include "AIPlayer.h"
#include <functional>

template<>
thread_local CAIService* MyTLSTypePtr<CAIService>::m_pPtr = nullptr;

extern "C" IService* ServiceCreate(uint16_t  idWorld, uint16_t  idService)
{

	CAIService* pService = new CAIService(ServerPort{ idWorld, idService });
	if (pService == nullptr)
		return nullptr;

	if (pService->Create() == false)
	{
		pService->Release();
		return nullptr;
	}

	return pService;
}
__attribute__((visibility("default"))) IService* ServiceCreate(uint16_t  idWorld, uint16_t  idService);


//////////////////////////////////////////////////////////////////////////
CAIService::CAIService(const ServerPort& nServerPort)
	:CServiceCommon(nServerPort,std::string("AI") + std::to_string(nServerPort.GetServiceID())  )
{
	m_tLastDisplayTime.Startup(20);
}

CAIService::~CAIService()
{
	MyTLSTypePtr<CAIService>::set(this);
	scope_guards scope_exit;
	scope_exit += []() {MyTLSTypePtr<CAIService>::set(nullptr); };
	StopLogicThread();

	
	m_pAISceneManager->Destory();
	m_pAIActorManager->Destroy();
}

bool CAIService::Create()
{
	//各种初始化
	scope_guards scope_exit;
	MyTLSTypePtr<CAIService>::set(this);
	scope_exit += []() {MyTLSTypePtr<CAIService>::set(nullptr); };

	BaseCode::SetNdc(GetServiceName() );

	scope_exit += []() {BaseCode::SetNdc(std::string()); };

	extern void export_to_lua(lua_State*, void*);
	m_pScriptManager.reset(CLUAScriptManager::CreateNew(std::string("AIScript") + std::to_string(GetServerPort().GetServiceID()), &export_to_lua, (void*)this, "res/script", false));
	m_pScriptManager->LoadFilesInDir("ai", false);



	m_pMapManager.reset(new CMapManager);
	CHECKF(m_pMapManager->Init(GetZoneID()));

	m_pTargetFAMSet.reset(CTargetFAMSet::CreateNew("res/config/Cfg_TargetFAM.bytes"));
	CHECKF(m_pTargetFAMSet.get());
	m_pSkillFAMSet.reset(CSkillFAMSet::CreateNew("res/config/Cfg_SkillFAM.bytes"));
	CHECKF(m_pSkillFAMSet.get());

	m_pSkillTypeSet.reset(CSkillTypeSet::CreateNew("res/config/Cfg_Skill.bytes"));
	CHECKF(m_pSkillTypeSet.get());

	m_pAITypeSet.reset(CAITypeSet::CreateNew("res/config/Cfg_AIType.bytes"));
	CHECKF(m_pAITypeSet.get());

	m_pMonsterTypeSet.reset(CMonsterTypeSet::CreateNew("res/config/Cfg_Monster.bytes"));
	CHECKF(m_pMonsterTypeSet.get());

	m_pAISceneManager.reset(new CAISceneManager);
	CHECKF(m_pAISceneManager->Init(GetServerPort().GetServiceID()));
	m_pAIActorManager.reset(new CAIActorManager);








	extern void AIServiceMessageHandlerRegister();
	AIServiceMessageHandlerRegister();







	if (CreateService(20) == false)
		return false;

	return true;
}

void CAIService::OnProcessMessage(CNetworkMessage* pNetworkMsg)
{
	switch (pNetworkMsg->GetMsgHead()->usCmd)
	{
	case NETMSG_SCK_CLOSE:
		{
			MSG_SCK_CLOSE* pMsg = (MSG_SCK_CLOSE*)pNetworkMsg->GetBuf();

		}
		break;
	case ServerMSG::MsgID_SceneCreate:
		{
			AISceneManager()->CreateScene(pNetworkMsg);
		}
		break;
	case ServerMSG::MsgID_SceneDestory:
		{
			AISceneManager()->DestoryDynaScene(pNetworkMsg);
		}
		break;
	case ServerMSG::MsgID_ActiveGen:
		{
			ServerMSG::ActiveGen msg;
			if (msg.ParseFromArray(pNetworkMsg->GetMsgBody(), pNetworkMsg->GetBodySize()) == false)
			{
				return;
			}
			CAIScene* pScene = AISceneManager()->QueryScene(msg.scene_id());
			if(pScene)
			{
				pScene->GetMonsterGen().ActiveGen(msg.gen_id(), msg.active());
			}

		}
		break;
	case ServerMSG::MsgID_KillGen:
		{
			ServerMSG::ActiveGen msg;
			if (msg.ParseFromArray(pNetworkMsg->GetMsgBody(), pNetworkMsg->GetBodySize()) == false)
			{
				return;
			}
			CAIScene* pScene = AISceneManager()->QueryScene(msg.scene_id());
			if(pScene)
			{
				pScene->GetMonsterGen().KillGen(msg.gen_id());
			}
		}
		break;
	case ServerMSG::MsgID_ActorCreate:
		{
			ServerMSG::ActorCreate msg;
			if (msg.ParseFromArray(pNetworkMsg->GetMsgBody(), pNetworkMsg->GetBodySize()) == false)
			{
				return;
			}

			CAIScene* pScene = AISceneManager()->QueryScene(msg.scene_id());
			CHECK(pScene);
			CAIActor* pActor = nullptr;
			switch(msg.actortype())
			{
			case ACT_MONSTER:
				{
					pActor = CAIMonster::CreateNew(msg);
				}
				break;
			case ACT_PLAYER:
				{
					pActor = CAIPlayer::CreateNew(msg);
				}
				break;
			case ACT_PET:
				{

				}
				break;
			case ACT_NPC:
				{

				}
				break;
			}
				   
			if(pActor)
			{
				pScene->EnterMap(pActor, msg.posx(),  msg.posy(), msg.face());
				AIActorManager()->AddActor(pActor);
				pActor->OnBorn();
			}
		}
		break;
	case ServerMSG::MsgID_ActorDestory:
		{
			ServerMSG::ActorDestory msg;
			if (msg.ParseFromArray(pNetworkMsg->GetMsgBody(), pNetworkMsg->GetBodySize()) == false)
			{
				return;
			}
			CAIActor* pActor = AIActorManager()->QueryActor(msg.actor_id());
			if(pActor)
			{
				pActor->OnDead();
				if(pActor->GetCurrentScene())
					pActor->GetCurrentScene()->LeaveMap(pActor);
				
				AIActorManager()->DelActor(pActor);
			}
		}
		break;
	case ServerMSG::MsgID_ActorCastSkill_Fail:
		{
			ServerMSG::ActorCastSkill_Fail msg;
			if (msg.ParseFromArray(pNetworkMsg->GetMsgBody(), pNetworkMsg->GetBodySize()) == false)
			{
				return;
			}
			CAIActor* pActor = AIActorManager()->QueryActor(msg.actor_id());
			if(pActor)
			{
				pActor->OnCastSkillFinish();
			}
		}
		break;
	case ServerMSG::MsgID_ActorSetHide:
		{
			ServerMSG::ActorSetHide msg;
			if (msg.ParseFromArray(pNetworkMsg->GetMsgBody(), pNetworkMsg->GetBodySize()) == false)
			{
				return;
			}
			CAIActor* pActor = AIActorManager()->QueryActor(msg.actor_id());
			if(pActor)
			{
				pActor->SetHideCoude(msg.hide_count());
			}
		}
		break;
	default:
		{
			m_pNetMsgProcess->Process(pNetworkMsg);
		}
		break;
	}
}


bool CAIService::SendMsgToZone(uint16_t nCmd, const google::protobuf::Message& msg)
{
	CNetworkMessage _msg(nCmd, msg, GetServerVirtualSocket(), GetZoneServiceVirtualSocket());
	return SendMsg(_msg);
}

void CAIService::OnLogicThreadProc()
{
__ENTER_FUNCTION
	CServiceCommon::OnLogicThreadProc();


	AISceneManager()->OnTimer();


	if(m_tLastDisplayTime.ToNextTime())
	{
		std::string buf = std::string("\n======================================================================")+
		fmt::format(FMT_STRING("\nEvent:{}"), EventManager()->GetEventCount());
		auto pMessagePort = GetMessageRoute()->QueryMessagePort(GetZoneServiceVirtualSocket().GetServerPort(), false);
		if(pMessagePort)
		{
			buf += fmt::format(FMT_STRING("\nMsgPort:{}\tSendBuff:{}"), GetZoneID(), pMessagePort->GetWriteBufferSize());
		}
		LOGMONITOR("{}", buf.c_str());
	}
__LEAVE_FUNCTION
}

void CAIService::OnLogicThreadCreate()
{
	MyTLSTypePtr<CAIService>::set(this);
	CServiceCommon::OnLogicThreadCreate();

}

void CAIService::OnLogicThreadExit()
{

}
