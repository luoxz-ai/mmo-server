#ifndef WorldService_h__
#define WorldService_h__


#include "MyTimer.h"
#include "NetMSGProcess.h"
#include "NetSocket.h"
#include "IService.h"
#include "ServiceComm.h"

class CMysqlConnection;
class CAccountManager;
class CUserManager;
class CSystemVarSet;
class CGMManager;
class CTeamManager;
class CMapManager;
class CUserAttrSet;
class CBornPosSet;



class CWorldService : public IService, public CServiceCommon
{
    CWorldService();
    bool Init(const ServerPort& nServerPort);
    virtual ~CWorldService();
    void Destory();
public:
    

   
    void Release() override { Destory();delete this; }
    
    CreateNewRealeaseImpl(CWorldService);
public:
    virtual void OnLogicThreadProc() override;
    virtual void OnLogicThreadCreate() override;
    virtual void OnLogicThreadExit() override;

    virtual void OnProcessMessage(CNetworkMessage*) override;

public:
    uint64_t CreatePlayerID();
    void     RecyclePlayerID(OBJID idPlayer);
    void     SetServiceReady(uint16_t idService);

    bool CheckProgVer(const std::string& prog_ver);
    bool BroadcastToZone(uint16_t nCmd, const google::protobuf::Message& msg);
    bool BroadcastToAllPlayer(uint16_t nCmd, const google::protobuf::Message& msg);
    //发送广播包给玩家
    void _ID2VS(OBJID id, VirtualSocketMap_t& VSMap)override;
public:
    CAccountManager*  GetAccountManager() const { return m_pAccountManager.get(); }
    CUserManager*     GetUserManager() const { return m_pUserManager.get(); }
    CMapManager*      GetMapManager() const { return m_pMapManager.get(); }
    CMysqlConnection* GetGameDB() const { return m_pGameDB.get(); }
    CSystemVarSet*    GetSystemVarSet() const { return m_pSystemVarSet.get(); }
    CGMManager*       GetGMManager() const { return m_pGMManager.get(); }
    CTeamManager*     GetTeamManager() const { return m_pTeamManager.get(); }
    std::unique_ptr<CMysqlConnection> ConnectGlobalDB();
private:
    uint64_t                     m_nCurPlayerMaxID;
    std::deque<OBJID>            m_setPlayerIDPool;
    std::unordered_set<uint16_t> m_setServiceNeedReady;

    std::unique_ptr<CMysqlConnection> m_pGameDB;

    std::unique_ptr<CUserManager>    m_pUserManager;
    std::unique_ptr<CAccountManager> m_pAccountManager;
    std::unique_ptr<CGMManager>      m_pGMManager;

    std::unique_ptr<CUserAttrSet>  m_pUserAttrSet;
    std::unique_ptr<CBornPosSet>   m_pBornPosSet;
    std::unique_ptr<CMapManager>   m_pMapManager;
    std::unique_ptr<CSystemVarSet> m_pSystemVarSet;
    std::unique_ptr<CTeamManager>  m_pTeamManager;

    CMyTimer m_tLastDisplayTime;

    DEFINE_CONFIG_SET(CUserAttrSet);
    DEFINE_CONFIG_SET(CBornPosSet);

};

CWorldService* WorldService();
void           SetWorldServicePtr(CWorldService* ptr);

inline auto GMManager()
{
    return WorldService()->GetGMManager();
}
inline auto EventManager()
{
    return WorldService()->GetEventManager();
}
inline auto NetMsgProcess()
{
    return WorldService()->GetNetMsgProcess();
}
inline auto AccountManager()
{
    return WorldService()->GetAccountManager();
}
inline auto UserManager()
{
    return WorldService()->GetUserManager();
}
inline auto UserAttrSet()
{
    return WorldService()->GetCUserAttrSet();
}
inline auto BornPosSet()
{
    return WorldService()->GetCBornPosSet();
}
inline auto MapManager()
{
    return WorldService()->GetMapManager();
}
inline auto SystemVarSet()
{
    return WorldService()->GetSystemVarSet();
}
inline auto TeamManager()
{
    return WorldService()->GetTeamManager();
}

#endif // WorldService_h__
