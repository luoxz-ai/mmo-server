#ifndef AISERVICE_H
#define AISERVICE_H
#include "AIActorManager.h"
#include "AIFuzzyLogic.h"
#include "AIScene.h"
#include "AISkill.h"
#include "AIType.h"
#include "BaseCode.h"
#include "IService.h"
#include "MonsterType.h"
#include "MyTimer.h"
#include "NetSocket.h"
#include "ScriptManager.h"
#include "ServiceComm.h"
#include "UIDFactory.h"
#include "msg/server_side.pb.h"
#include "msg/ts_cmd.pb.h"
#include "msg/zone_service.pb.h"

struct event;
class CNetMSGProcess;
class CAIService : public IService, public CServiceCommon
{
  public:
	CAIService(const ServerPort& nServerPort);
	virtual ~CAIService();
	void Release() { delete this; }
	bool Create();

	VirtualSocket GetZoneServiceVirtualSocket() const
	{
		return VirtualSocket(ServerPort(GetWorldID(), GetZoneID()), 0);
	}
	uint16_t GetZoneID() const { return GetServiceID() - 10; }

  public:
	virtual void OnLogicThreadProc() override;
	virtual void OnLogicThreadCreate() override;
	virtual void OnLogicThreadExit() override;

	virtual void OnProcessMessage(CNetworkMessage*) override;

	//发送消息给AIService
	bool SendMsgToZone(uint16_t nCmd, const google::protobuf::Message& msg);

  public:
	CLUAScriptManager* GetScriptManager() const { return m_pScriptManager.get(); }
	CTargetFAMSet*	   GetTargetFAMSet() const { return m_pTargetFAMSet.get(); }
	CSkillFAMSet*	   GetSkillFAMSet() const { return m_pSkillFAMSet.get(); }
	CAITypeSet*		   GetAITypeSet() const { return m_pAITypeSet.get(); }
	CMonsterTypeSet*   GetMonsterTypeSet() const { return m_pMonsterTypeSet.get(); }
	CAISceneManager*   GetAISceneManager() const { return m_pAISceneManager.get(); }
	CAIActorManager*   GetAIActorManager() const { return m_pAIActorManager.get(); }
	CSkillTypeSet*	   GetSkillTypeSet() const { return m_pSkillTypeSet.get(); }

	CMapManager* GetMapManager() const { return m_pMapManager.get(); }

  private:
	CMyTimer m_tLastDisplayTime;

	std::unique_ptr<CLUAScriptManager> m_pScriptManager;
	std::unique_ptr<CTargetFAMSet>	   m_pTargetFAMSet;
	std::unique_ptr<CSkillFAMSet>	   m_pSkillFAMSet;
	std::unique_ptr<CAITypeSet>		   m_pAITypeSet;
	std::unique_ptr<CMonsterTypeSet>   m_pMonsterTypeSet;
	std::unique_ptr<CAISceneManager>   m_pAISceneManager;
	std::unique_ptr<CAIActorManager>   m_pAIActorManager;
	std::unique_ptr<CSkillTypeSet>	   m_pSkillTypeSet;
	std::unique_ptr<CMapManager>	   m_pMapManager;
};

#define AIService()		 MyTLSTypePtr<CAIService>::get()
#define EventManager()	 AIService()->GetEventManager()
#define ScriptManager()	 AIService()->GetScriptManager()
#define NetMsgProcess()	 AIService()->GetNetMsgProcess()
#define TargetFAMSet()	 AIService()->GetTargetFAMSet()
#define SkillFAMSet()	 AIService()->GetSkillFAMSet()
#define AITypeSet()		 AIService()->GetAITypeSet()
#define AISceneManager() AIService()->GetAISceneManager()
#define AIActorManager() AIService()->GetAIActorManager()
#define SkillTypeSet()	 AIService()->GetSkillTypeSet()
#define MapManager()	 AIService()->GetMapManager()
#define MonsterTypeSet() AIService()->GetMonsterTypeSet()

#endif /* AISERVICE_H */
