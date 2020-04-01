#include "ZoneService.h"

#include <functional>

#include "Actor.h"
#include "EventManager.h"
#include "MessagePort.h"
#include "MessageRoute.h"
#include "Monster.h"
#include "NetMSGProcess.h"
#include "NetSocket.h"
#include "NetworkMessage.h"
#include "Player.h"
#include "Scene.h"
#include "SettingMap.h"
#include "globaldb.h"
#include "server_msg/server_side.pb.h"

static thread_local CZoneService* tls_pService = nullptr;
CZoneService*                     ZoneService()
{
    return tls_pService;
}

void SetZoneServicePtr(CZoneService* pZone)
{
    tls_pService = pZone;
}

extern "C" __attribute__((visibility("default"))) IService* ServiceCreate(uint16_t idWorld, uint16_t idService)
{
    return CZoneService::CreateNew(ServerPort{idWorld, idService});
}

//////////////////////////////////////////////////////////////////////////
CZoneService::CZoneService()
{
   
}

CZoneService::~CZoneService()
{
   
}

void CZoneService::Destory()
{
    tls_pService = this;
    scope_guards scope_exit;
    scope_exit += []() {
        tls_pService = nullptr;
    };
    StopLogicThread();
    if(m_pLoadingThread)
        m_pLoadingThread->Destory();
    m_pLoadingThread.reset();
    if(GetSceneManager())
        GetSceneManager()->Destory();
    m_pSceneManager.reset();
    if(GetActorManager())
        GetActorManager()->Destory();
    m_pActorManager.reset();
    for(auto& [k, refQueue]: m_MessagePoolBySocket)
    {
        for(auto& msg: refQueue)
        {
            SAFE_DELETE(msg);
        }
    }
    m_MessagePoolBySocket.clear();
    DestoryServiceCommon();
    LOGMESSAGE("{} Close", GetServiceName());
}

bool CZoneService::Init(const ServerPort& nServerPort)
{
    __ENTER_FUNCTION

    //各种初始化
    tls_pService = this;
    scope_guards scope_exit;
    scope_exit += []() {
        tls_pService = nullptr;
    };
    CServiceCommon::Init(nServerPort);
    auto oldNdc = BaseCode::SetNdc(GetServiceName());
    scope_exit += [oldNdc]() {
        BaseCode::SetNdc(oldNdc);
        ;
    };
    
    m_MessagePoolBySocket.reserve(GUESS_MAX_PLAYER_COUNT);
    m_tLastDisplayTime.Startup(20);
    const auto& settings = GetMessageRoute()->GetSettingMap();
    {
        const auto& settingGlobalDB = settings["GlobalMYSQL"][0];

        auto pDB = std::make_unique<CMysqlConnection>();

        if(pDB->Connect(settingGlobalDB.Query("host"),
                        settingGlobalDB.Query("user"),
                        settingGlobalDB.Query("passwd"),
                        settingGlobalDB.Query("dbname"),
                        settingGlobalDB.QueryULong("port")) == false)
        {
            return false;
        }
        m_pGlobalDB.reset(pDB.release());

        if(GetWorldID() != 0)
        {
            GetGameDB(GetWorldID());
        }
    }

    //配置读取
#define DEFINE_CONFIG_LOAD(T, path)   \
    m_p##T.reset(T::CreateNew(path)); \
    CHECKF(m_p##T.get());
    DEFINE_CONFIG_LOAD(CStatusTypeSet, "res/config/Cfg_Status.bytes");
    DEFINE_CONFIG_LOAD(CUserAttrSet, "res/config/Cfg_UserAttr.bytes");
    DEFINE_CONFIG_LOAD(CDataCountLimitSet, "res/config/Cfg_DataCountLimit.bytes");
    DEFINE_CONFIG_LOAD(CSkillTypeSet, "res/config/Cfg_Skill.bytes");
    DEFINE_CONFIG_LOAD(CSkillAttachStatusDataSet, "res/config/Cfg_SkillAttachStatus.bytes");
    DEFINE_CONFIG_LOAD(CSkillDetachStatusDataSet, "res/config/Cfg_SkillDetachStatus.bytes");
    DEFINE_CONFIG_LOAD(CMonsterTypeSet, "res/config/Cfg_Monster.bytes");
    DEFINE_CONFIG_LOAD(CBulletTypeSet, "res/config/Cfg_Bullet.bytes");
    DEFINE_CONFIG_LOAD(CItemTypeSet, "res/config/Cfg_Item.bytes");
    DEFINE_CONFIG_LOAD(CItemAdditionSet, "res/config/Cfg_ItemAddition.bytes");
    DEFINE_CONFIG_LOAD(CItemFormulaDataSet, "res/config/Cfg_ItemFormula.bytes");
    DEFINE_CONFIG_LOAD(CItemUpgradeDataSet, "res/config/Cfg_ItemUpgrade.bytes");
    DEFINE_CONFIG_LOAD(CSuitEquipSet, "res/config/Cfg_Suit.bytes");
    DEFINE_CONFIG_LOAD(CTaskTypeSet, "res/config/Cfg_Task.bytes");
    DEFINE_CONFIG_LOAD(CAchievementTypeSet, "res/config/Cfg_Achievement.bytes");
    DEFINE_CONFIG_LOAD(CNpcTypeSet, "res/config/Cfg_Npc.bytes");
#undef DEFINE_CONFIG_LOAD

    m_pMapManager.reset(CMapManager::CreateNew(GetServerPort().GetServiceID()));
    CHECKF(m_pMapManager.get());
    m_pActorManager.reset(CActorManager::CreateNew());
    CHECKF(m_pActorManager.get());
    m_pTeamInfoManager.reset(CTeamInfoManager::CreateNew());
    CHECKF(m_pTeamInfoManager.get());
    m_pGMManager.reset(CGMManager::CreateNew());
    CHECKF(m_pGMManager.get());

    //脚本加载
    extern void export_to_lua(lua_State*, void*);
    m_pScriptManager.reset(
        CLUAScriptManager::CreateNew(std::string("ZoneScript") + std::to_string(GetServerPort().GetServiceID()),
                                     &export_to_lua,
                                     (void*)this,
                                     "res/script/zone_service"));

    //必须要晚于MapManger和ActorManager
    m_pSceneManager.reset(CSceneManager::CreateNew(GetServerPort().GetServiceID()));
    CHECKF(m_pSceneManager.get());

    extern void ZoneMessageHandlerRegister();
    ZoneMessageHandlerRegister();

    if(GetWorldID() != 0)
    {
        //共享型Zone是没有自己的数据的

        m_pSystemVarSet.reset(CSystemVarSet::CreateNew());
        CHECKF(m_pSystemVarSet.get());
    }

    m_pLoadingThread.reset(CLoadingThread::CreateNew(this));
    CHECKF(m_pLoadingThread.get());

    if(CreateService(20) == false)
        return false;

    // send message to world, notify zone ready
    if(GetWorldID() != 0)
    {
        ServerMSG::ServiceReady msg;
        msg.set_serverport(GetServerPort());
        SendMsgToWorld(GetWorldID(), ServerMSG::MsgID_ServiceReady, msg);
    }

    return true;
    __LEAVE_FUNCTION
    return false;
}

void CZoneService::OnProcessMessage(CNetworkMessage* pNetworkMsg)
{
    __ENTER_FUNCTION

    switch(pNetworkMsg->GetMsgHead()->usCmd)
    {
        case NETMSG_SCK_CONNECT:
        {
            // socket
        }
        break;
        case NETMSG_SCK_CLOSE:
        {
            MSG_SCK_CLOSE* pMsg = (MSG_SCK_CLOSE*)pNetworkMsg->GetBuf();
        }
        break;
        case NETMSG_INTERNAL_SERVICE_REGISTER:
        {
            MSG_INTERNAL_SERVICE_REGISTER* pMsg = (MSG_INTERNAL_SERVICE_REGISTER*)pNetworkMsg->GetBuf();
            GetMessageRoute()->SetWorldReady(pMsg->idWorld, true);
            GetMessageRoute()->ReloadServiceInfo(pMsg->update_time, pMsg->idWorld);
        }
        break;
        case NETMSG_INTERNAL_SERVICE_READY:
        {
            MSG_INTERNAL_SERVICE_READY* pMsg = (MSG_INTERNAL_SERVICE_READY*)pNetworkMsg->GetBuf();
            GetMessageRoute()->SetWorldReady(pMsg->idWorld, pMsg->bReady);
        }
        break;
        default:
        {
            //如果是玩家的消息
            if(pNetworkMsg->GetCmd() >= CLIENT_MSG_ID_BEGIN && pNetworkMsg->GetCmd() <= CLIENT_MSG_ID_END)
            {
                //玩家的消息，先丢到玩家身上去，等待后续处理
                PushMsgToMessagePool(pNetworkMsg->GetFrom(), pNetworkMsg);
            }
            else
            {
                //服务器间的消息，现在就可以处理了
                m_pNetMsgProcess->Process(pNetworkMsg);
            }
        }
        break;
    }

    __LEAVE_FUNCTION
}

void CZoneService::CreateSocketMessagePool(const VirtualSocket& vs)
{
    m_MessagePoolBySocket[vs].clear();
}

void CZoneService::DelSocketMessagePool(const VirtualSocket& vs)
{
    m_MessagePoolBySocket.erase(vs);
}

void CZoneService::PushMsgToMessagePool(const VirtualSocket& vs, CNetworkMessage* pMsg)
{
    __ENTER_FUNCTION

    auto itFind = m_MessagePoolBySocket.find(vs);
    if(itFind == m_MessagePoolBySocket.end())
        return;

    auto& refList = itFind->second;

    CNetworkMessage* pStoreMsg = new CNetworkMessage(*pMsg);
    pStoreMsg->CopyBuffer();
    refList.push_back(pStoreMsg);

    constexpr int32_t MAX_USER_HOLD_MESSAGE = 500;
    if(refList.size() > MAX_USER_HOLD_MESSAGE)
    {
        // logerror
        LOGERROR("Player:{} {} Hold Too Many Message", vs.GetServerPort().GetServiceID(), vs.GetSocketIdx());
        // kick user
        MSG_SCK_CLOSE msg;
        msg.vs = vs;
        SendPortMsg(vs.GetServerPort(), (byte*)&msg, sizeof(msg));
    }

    __LEAVE_FUNCTION
}

std::deque<CNetworkMessage*>& CZoneService::GetMessagePoolRef(const VirtualSocket& vs)
{
    return m_MessagePoolBySocket[vs];
}

bool CZoneService::PopMsgFromMessagePool(const VirtualSocket& vs, CNetworkMessage*& pMsg)
{
    __ENTER_FUNCTION
    auto itFind = m_MessagePoolBySocket.find(vs);
    if(itFind == m_MessagePoolBySocket.end())
        return false;

    auto& refList = itFind->second;
    if(refList.empty())
        return false;

    pMsg = refList.front();
    refList.pop_front();
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CZoneService::SendMsgToWorld(uint16_t idWorld, uint16_t nCmd, const google::protobuf::Message& msg)
{
    CNetworkMessage _msg(nCmd, msg, GetServerVirtualSocket(), ServerPort(idWorld, WORLD_SERVICE_ID), 0);
    return SendMsg(_msg);
}

bool CZoneService::TransmiteMsgFromWorldToOther(uint16_t                         idWorld,
                                                uint16_t                         idService,
                                                uint16_t                         nCmd,
                                                const google::protobuf::Message& msg)
{
    CNetworkMessage _msg(nCmd,
                         msg,
                         GetServerVirtualSocket(),
                         ServerPort(idWorld, WORLD_SERVICE_ID),
                         ServerPort(GetWorldID(), idService));
    return SendMsg(_msg);
}

bool CZoneService::BroadcastToZone(uint16_t nCmd, const google::protobuf::Message& msg)
{
    __ENTER_FUNCTION

    for(uint32_t i = MIN_ZONE_SERVICE_ID; i <= MAX_ZONE_SERVICE_ID; i++)
    {
        if(i == GetServiceID())
            continue;

        CNetworkMessage _msg(nCmd, msg, GetServerVirtualSocket(), VirtualSocket(ServerPort(GetWorldID(), i), 0));
        SendMsg(_msg);
    }
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CZoneService::BroadcastToAllPlayer(uint16_t nCmd, const google::protobuf::Message& msg)
{
    __ENTER_FUNCTION
    CNetworkMessage _msg(nCmd, msg, GetServerVirtualSocket(), 0);
    m_MonitorMgr.AddSendInfo_broad(nCmd, _msg.GetSize());

    if(GetWorldID() != 0)
    {
        for(uint32_t i = MIN_SOCKET_SERVICE_ID; i <= MAX_SOCKET_SERVICE_ID; i++)
        {
            _msg.SetTo(VirtualSocket(ServerPort(GetWorldID(), i), 0));
            SendBroadcastMsg(_msg);
        }
    }
    else
    {
        //需要遍历所有的玩家
        auto func_callback = [](auto pair_val) -> OBJID {
            auto pPlayer = pair_val.second;
            if(pPlayer)
                return pPlayer->GetID();
            return 0;
        };
        auto func = std::bind(&CActorManager::ForeachPlayer, GetActorManager(), std::move(func_callback));
        BroadcastMessageToPlayer(func, nCmd, msg);
    }

    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CZoneService::SendMsgToPlayer(const VirtualSocket& vs, uint16_t nCmd, const google::protobuf::Message& msg)
{
    CNetworkMessage _msg(nCmd, msg, GetServerVirtualSocket(), vs);
    m_MonitorMgr.AddSendInfo(nCmd, _msg.GetSize());
    return SendMsg(_msg);
}

bool CZoneService::SendMsgToAIService(uint16_t nCmd, const google::protobuf::Message& msg)
{
    CNetworkMessage _msg(nCmd, msg, GetServerVirtualSocket(), GetAIServerVirtualSocket());
    return SendMsg(_msg);
}

void CZoneService::_ID2VS(OBJID id, CZoneService::VSMap_t& VSMap)
{
    __ENTER_FUNCTION
    CActor* pActor = GetActorManager()->QueryActor(id);
    if(pActor && pActor->IsPlayer())
    {
        CPlayer* pPlayer = pActor->ConvertToDerived<CPlayer>();
        VSMap[pPlayer->GetSocket().GetServerPort()].push_back(pPlayer->GetSocket());
    }
    __LEAVE_FUNCTION
}

void CZoneService::ReleaseGameDB(uint16_t nWorldID)
{
    __ENTER_FUNCTION
    auto itFind = m_GameDBMap.find(nWorldID);
    if(itFind != m_GameDBMap.end())
    {
        m_GameDBMap.erase(itFind);
    }
    __LEAVE_FUNCTION
}

CMysqlConnection* CZoneService::GetGameDB(uint16_t nWorldID)
{
    __ENTER_FUNCTION
    auto itFind = m_GameDBMap.find(nWorldID);
    if(itFind != m_GameDBMap.end())
    {
        return itFind->second.get();
    }
    else
    {
        //通过globaldb查询localdb
        auto result = m_pGlobalDB->Query(
            TBLD_DBINFO::table_name,
            fmt::format(FMT_STRING("SELECT * FROM {} WHERE worldid={} LIMIT 1"), TBLD_DBINFO::table_name, nWorldID));
        if(result)
        {
            auto row = result->fetch_row(false);
            if(row)
            {
                std::string host   = row->Field(TBLD_DBINFO::DB_IP);
                uint32_t    port   = row->Field(TBLD_DBINFO::DB_PORT);
                std::string user   = row->Field(TBLD_DBINFO::DB_USER);
                std::string passwd = row->Field(TBLD_DBINFO::DB_PASSWD);
                std::string dbname = row->Field(TBLD_DBINFO::DB_NAME);

                auto pDB = std::make_unique<CMysqlConnection>();
                if(pDB->Connect(host, user, passwd, dbname, port) == false)
                {
                    return nullptr;
                }
                m_GameDBMap[nWorldID].reset(pDB.release());
                return m_GameDBMap[nWorldID].get();
            }
        }
    }
    LOGERROR("can find dbinfo worldid:{}", nWorldID);
    __LEAVE_FUNCTION
    return nullptr;
}

void CZoneService::OnLogicThreadProc()
{
    __ENTER_FUNCTION
    //处理消息
    TICK_EVAL(CServiceCommon::OnLogicThreadProc());

    //处理登陆
    TICK_EVAL(GetLoadingThread()->OnMainThreadExec());

    // lua step gc
    TICK_EVAL(m_pScriptManager->OnTimer(TimeGetMonotonic()));

    if(m_tLastDisplayTime.ToNextTime())
    {
        std::string buf = std::string("\n======================================================================") +
                          fmt::format(FMT_STRING("\nMessageProcess:{}"), GetMessageProcess()) +
                          fmt::format(FMT_STRING("\nEvent:{}\t"), EventManager()->GetEventCount()) +
                          fmt::format(FMT_STRING("\nUser:{}\tMonster:{}"),
                                      ActorManager()->GetUserCount(),
                                      ActorManager()->GetMonsterCount()) +
                          fmt::format(FMT_STRING("\nLoading:{}\tSaveing:{}\tReady:{}"),
                                      GetLoadingThread()->GetLoadingCount(),
                                      GetLoadingThread()->GetSaveingCount(),
                                      GetLoadingThread()->GetReadyCount()) +
                          fmt::format(FMT_STRING("\nScene:{}\tDynaScene:{}"),
                                      SceneManager()->GetSceneCount(),
                                      SceneManager()->GetDynaSceneCount());
        SceneManager()->ForEach([&buf](CScene* pScene) {
            if(pScene->IsDynaScene() == false)
            {
                buf += fmt::format(FMT_STRING("\nScene{}\tPlayer:{}\tActor:{}"),
                                   pScene->GetMapID(),
                                   pScene->GetPlayerCount(),
                                   pScene->GetActorCount());
            }
        });
        static const uint16_t ServiceID[] = {WORLD_SERVICE_ID, uint16_t(GetServiceID() + 10), 31, 32, 33, 34, 35};

        for(size_t i = 0; i < sizeOfArray(ServiceID); i++)
        {
            auto pMessagePort = GetMessageRoute()->QueryMessagePort(ServerPort(GetWorldID(), ServiceID[i]), false);
            if(pMessagePort)
            {
                buf += fmt::format(FMT_STRING("\nMsgPort:{}\tSendBuff:{}"),
                                   ServiceID[i],
                                   pMessagePort->GetWriteBufferSize());
            }
        }

        LOGMONITOR("{}", buf.c_str());
        m_MonitorMgr.Print();
        SetMessageProcess(0);
    }

    __LEAVE_FUNCTION
}

void CZoneService::OnLogicThreadCreate()
{
    tls_pService = this;
    CServiceCommon::OnLogicThreadCreate();
}

void CZoneService::OnLogicThreadExit()
{
    CServiceCommon::OnLogicThreadExit();
}
