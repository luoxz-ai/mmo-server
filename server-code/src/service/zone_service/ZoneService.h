#ifndef ZoneService_h__
#define ZoneService_h__



#include "MyTimer.h"
#include "NetSocket.h"
#include "IService.h"
#include "ScriptManager.h"
#include "ServiceComm.h"
#include "game_common_def.h"


class CMysqlConnection;
class CMapManager;
class CSystemVarSet;
class CActorManager;
class CSceneManager;
class CLoadingThread;
class CGMManager;
class CMonitorMgr;
class CTeamInfoManager;


export_lua class CZoneService : public IService, public CServiceCommon
{
    CZoneService();
    bool Init(const ServerPort& nServerPort);
    virtual ~CZoneService();
    void Destory();
public:
    

    
    void Release() override;
    CreateNewRealeaseImpl(CZoneService);
    
    uint16_t GetZoneID() const { return GetServiceID() - MIN_ZONE_SERVICE_ID + 1; }
public:
    virtual void OnLogicThreadProc() override;
    virtual void OnLogicThreadCreate() override;
    virtual void OnLogicThreadExit() override;

    virtual void OnProcessMessage(CNetworkMessage*) override;

public:
    void                          CreateSocketMessagePool(const VirtualSocket& vs);
    void                          DelSocketMessagePool(const VirtualSocket& vs);
    void                          PushMsgToMessagePool(const VirtualSocket& vs, CNetworkMessage* pMsg);
    std::deque<CNetworkMessage*>& GetMessagePoolRef(const VirtualSocket& vs);
    bool                          PopMsgFromMessagePool(const VirtualSocket& vs, CNetworkMessage*& pMsg);
    //发送消息给World
    export_lua bool SendMsgToWorld(uint16_t idWorld, uint16_t nCmd, const google::protobuf::Message& msg)const;
    //通过World转发消息
    export_lua bool TransmiteMsgFromWorldToOther(uint16_t                         idWorld,
                                                 uint16_t                         idService,
                                                 uint16_t                         nCmd,
                                                 const google::protobuf::Message& msg)const;
    //转发消息给其他的zone
    export_lua bool BroadcastToZone(uint16_t nCmd, const google::protobuf::Message& msg)const;
    //广播消息给所有的玩家
    export_lua bool BroadcastToAllPlayer(const google::protobuf::Message& msg)const;
    export_lua bool BroadcastToAllPlayer(uint16_t nCmd, const google::protobuf::Message& msg)const;
    //发送消息给玩家
    export_lua bool SendMsgToPlayer(const VirtualSocket& vs, const google::protobuf::Message& msg)const;
    export_lua bool SendMsgToPlayer(const VirtualSocket& vs, uint16_t nCmd, const google::protobuf::Message& msg)const;
    //发送消息给AIService
    export_lua bool SendPortMsgToAIService(const google::protobuf::Message& msg)const;
    export_lua bool SendMsgToAIService(uint16_t nCmd, const google::protobuf::Message& msg)const;

    //发送广播包给玩家
    void _ID2VS(OBJID id, VirtualSocketMap_t& VSMap)const override;

public:
    std::unique_ptr<CMysqlConnection> ConnectGlobalDB();

    export_lua CMysqlConnection* GetGameDB(uint16_t nWorldID);
    void                         ReleaseGameDB(uint16_t nWorldID);
    
    CMysqlConnection* _ConnectGameDB(uint16_t nWorldID, CMysqlConnection* pGlobalDB);

    export_lua CLUAScriptManager* GetScriptManager() const { return m_pScriptManager.get(); }
    export_lua CMapManager* GetMapManager() const { return m_pMapManager.get(); }
    export_lua CSystemVarSet* GetSystemVarSet() const { return m_pSystemVarSet.get(); }

    export_lua CActorManager* GetActorManager()const { return m_pActorManager.get(); }
    export_lua CSceneManager* GetSceneManager()const { return m_pSceneManager.get(); }
    export_lua CLoadingThread* GetLoadingThread()const { return m_pLoadingThread.get(); }
    
    export_lua CGMManager* GetGMManager()const { return m_pGMManager.get(); }
    export_lua CTeamInfoManager* GetTeamInfoManager()const { return m_pTeamInfoManager.get(); }

private:
    void ProcessPortMessage();

private:
    std::unique_ptr<CMysqlConnection> m_pGlobalDB;
    std::unordered_map<uint16_t, std::unique_ptr<CMysqlConnection>> m_GameDBMap;

    CMyTimer m_tLastDisplayTime;
    size_t   m_nMessageProcess = 0;

    std::unique_ptr<CActorManager>  m_pActorManager;
    std::unique_ptr<CSceneManager>  m_pSceneManager;
    std::unique_ptr<CLoadingThread> m_pLoadingThread;

    std::unordered_map<uint64_t, std::deque<CNetworkMessage*>> m_MessagePoolBySocket;

    std::unique_ptr<CGMManager>        m_pGMManager;
    std::unique_ptr<CLUAScriptManager> m_pScriptManager;
    std::unique_ptr<CMapManager>       m_pMapManager;
    std::unique_ptr<CSystemVarSet>     m_pSystemVarSet;

    std::unique_ptr<CTeamInfoManager>  m_pTeamInfoManager;

public:
    //配置文件
    DEFINE_CONFIG_SET(CStatusTypeSet);
    DEFINE_CONFIG_SET(CUserAttrSet);
    DEFINE_CONFIG_SET(CDataCountLimitSet);
    DEFINE_CONFIG_SET(CSkillTypeSet);
    DEFINE_CONFIG_SET(CSkillAttachStatusDataSet);
    DEFINE_CONFIG_SET(CSkillDetachStatusDataSet);
    DEFINE_CONFIG_SET(CMonsterTypeSet);
    DEFINE_CONFIG_SET(CBulletTypeSet);
    DEFINE_CONFIG_SET(CItemTypeSet);
    DEFINE_CONFIG_SET(CItemAdditionSet);
    DEFINE_CONFIG_SET(CItemFormulaDataSet);
    DEFINE_CONFIG_SET(CItemUpgradeDataSet);
    DEFINE_CONFIG_SET(CSuitEquipSet);
    DEFINE_CONFIG_SET(CTaskTypeSet);
    DEFINE_CONFIG_SET(CAchievementTypeSet);
    DEFINE_CONFIG_SET(CNpcTypeSet);
    DEFINE_CONFIG_SET(CPetTypeSet);
};

export_lua CZoneService* ZoneService();
void                     SetZoneServicePtr(CZoneService*);

export_lua inline auto EventManager()
{
    return ZoneService()->GetEventManager();
}
export_lua inline auto NetMsgProcess()
{
    return ZoneService()->GetNetMsgProcess();
}
export_lua inline auto ScriptManager()
{
    return ZoneService()->GetScriptManager();
}
export_lua inline auto ActorManager()
{
    return ZoneService()->GetActorManager();
}
export_lua inline auto SceneManager()
{
    return ZoneService()->GetSceneManager();
}
export_lua inline auto MapManager()
{
    return ZoneService()->GetMapManager();
}
export_lua inline auto SystemVarSet()
{
    return ZoneService()->GetSystemVarSet();
}
export_lua inline auto MonitorMgr()
{
    return ZoneService()->GetMonitorMgr();
}
export_lua inline auto GMManager()
{
    return ZoneService()->GetGMManager();
}
export_lua inline auto TeamManager()
{
    return ZoneService()->GetTeamInfoManager();
}

export_lua inline auto StatusTypeSet()
{
    return ZoneService()->GetCStatusTypeSet();
}
export_lua inline auto UserAttrSet()
{
    return ZoneService()->GetCUserAttrSet();
}
export_lua inline auto DataCountLimitSet()
{
    return ZoneService()->GetCDataCountLimitSet();
}
export_lua inline auto SkillTypeSet()
{
    return ZoneService()->GetCSkillTypeSet();
}
export_lua inline auto SkillAttachStatusDataSet()
{
    return ZoneService()->GetCSkillAttachStatusDataSet();
}
export_lua inline auto SkillDetachStatusDataSet()
{
    return ZoneService()->GetCSkillDetachStatusDataSet();
}
export_lua inline auto MonsterTypeSet()
{
    return ZoneService()->GetCMonsterTypeSet();
}
export_lua inline auto BulletTypeSet()
{
    return ZoneService()->GetCBulletTypeSet();
}
export_lua inline auto ItemTypeSet()
{
    return ZoneService()->GetCItemTypeSet();
}
export_lua inline auto ItemAdditionSet()
{
    return ZoneService()->GetCItemAdditionSet();
}
export_lua inline auto ItemFormulaDataSet()
{
    return ZoneService()->GetCItemFormulaDataSet();
}
export_lua inline auto ItemUpgradeDataSet()
{
    return ZoneService()->GetCItemUpgradeDataSet();
}
export_lua inline auto SuitEquipSet()
{
    return ZoneService()->GetCSuitEquipSet();
}
export_lua inline auto TaskTypeSet()
{
    return ZoneService()->GetCTaskTypeSet();
}
export_lua inline auto AchievementTypeSet()
{
    return ZoneService()->GetCAchievementTypeSet();
}
export_lua inline auto NpcTypeSet()
{
    return ZoneService()->GetCNpcTypeSet();
}
export_lua inline auto PetTypeSet()
{
    return ZoneService()->GetCPetTypeSet();
}

#endif // ZoneService_h__
