#ifndef AISERVICE_H
#define AISERVICE_H

#include "BaseCode.h"
#include "IService.h"
#include "MyTimer.h"
#include "NetSocket.h"
#include "ScriptManager.h"
#include "ServiceComm.h"
#include "UIDFactory.h"


class CAIActorManager;
class CAISceneManager;
class CMapManager;

class CAIService : public IService, public CServiceCommon
{
    CAIService();
    bool Init(const ServerPort& nServerPort);
    virtual ~CAIService();
    void Destory();
public:
    

    void Release() override;
    CreateNewRealeaseImpl(CAIService);

    VirtualSocket GetZoneServiceVirtualSocket() const
    {
        return VirtualSocket(ServerPort(GetWorldID(), GetZoneID()), 0);
    }
    uint16_t GetZoneID() const { return GetServiceID() - MIN_ZONE_SERVICE_ID + 1; }

public:
    virtual void OnLogicThreadProc() override;
    virtual void OnLogicThreadCreate() override;
    virtual void OnLogicThreadExit() override;

    virtual void OnProcessMessage(CNetworkMessage*) override;

    //发送消息给AIService
    bool SendMsgToZone(const google::protobuf::Message& msg);
    bool SendMsgToZone(uint16_t nCmd, const google::protobuf::Message& msg);

public:
    CLUAScriptManager* GetScriptManager() const { return m_pScriptManager.get(); }
    CAISceneManager*   GetAISceneManager() const { return m_pAISceneManager.get(); }
    CAIActorManager*   GetAIActorManager() const { return m_pAIActorManager.get(); }
    CMapManager*       GetMapManager() const { return m_pMapManager.get(); }

private:
    CMyTimer m_tLastDisplayTime;

    std::unique_ptr<CLUAScriptManager> m_pScriptManager;
    std::unique_ptr<CAISceneManager>   m_pAISceneManager;
    std::unique_ptr<CAIActorManager>   m_pAIActorManager;
    std::unique_ptr<CMapManager>       m_pMapManager;

public:
    //配置文件



    DEFINE_CONFIG_SET(CTargetFAMSet);
    DEFINE_CONFIG_SET(CSkillFAMSet);
    DEFINE_CONFIG_SET(CAITypeSet);
    DEFINE_CONFIG_SET(CMonsterTypeSet);
    DEFINE_CONFIG_SET(CSkillTypeSet);

};

CAIService* AIService();

inline auto EventManager()
{
    return AIService()->GetEventManager();
}
inline auto ScriptManager()
{
    return AIService()->GetScriptManager();
}
inline auto AISceneManager()
{
    return AIService()->GetAISceneManager();
}
inline auto AIActorManager()
{
    return AIService()->GetAIActorManager();
}
inline auto NetMsgProcess()
{
    return AIService()->GetNetMsgProcess();
}
inline auto TargetFAMSet()
{
    return AIService()->GetCTargetFAMSet();
}
inline auto SkillFAMSet()
{
    return AIService()->GetCSkillFAMSet();
}
inline auto AITypeSet()
{
    return AIService()->GetCAITypeSet();
}
inline auto SkillTypeSet()
{
    return AIService()->GetCSkillTypeSet();
}
inline auto MapManager()
{
    return AIService()->GetMapManager();
}
inline auto MonsterTypeSet()
{
    return AIService()->GetCMonsterTypeSet();
}

#endif /* AISERVICE_H */
