#include "Player.h"

#include "GameEventDef.h"
#include "ItemType.h"
#include "MapManager.h"
#include "MonitorMgr.h"
#include "NetMSGProcess.h"
#include "Npc.h"
#include "Phase.h"
#include "Scene.h"
#include "SceneManager.h"
#include "ActorManager.h"
#include "LoadingThread.h"
#include "SystemVars.h"
#include "SceneService.h"

OBJECTHEAP_IMPLEMENTATION(CPlayer, s_heap);

CPlayer::CPlayer()
{
    //默认阵营
    SetCampID(CAMP_PLAYER);
}

CPlayer::~CPlayer()
{
    __ENTER_FUNCTION
    for(auto& pMsg: m_MessageList)
    {
        SAFE_DELETE(pMsg);
    }
    m_MessageList.clear();
    if(GetCurrentScene() != nullptr)
        GetCurrentScene()->LeaveMap(this);

    __LEAVE_FUNCTION
}

bool CPlayer::Init(OBJID idPlayer, const VirtualSocket& socket)
{
    __ENTER_FUNCTION
    //这个函数实在Loading线程中调用的, 不要发送消息,不要处理各种问题,只负责读取数据库
    SetID(idPlayer);
    m_Socket = socket;

    auto pDB = SceneService()->GetGameDB(GetWorldIDFromPlayerID(idPlayer));
    CHECKF(pDB);
    auto result_ptr = pDB->QueryKeyLimit<TBLD_PLAYER, TBLD_PLAYER::ID>(idPlayer,1);
    CHECKF(result_ptr);

    m_pRecord = result_ptr->fetch_row(true);
    CHECKF(m_pRecord);

    CHECKF(CActor::Init());

    //初始化其他模块
    m_pCommonDataSet.reset(CCommonDataSet::CreateNew(this));
    CHECKF(m_pCommonDataSet.get());

    m_pDataCountSet.reset(CDataCountSet::CreateNew(this));
    CHECKF(m_pDataCountSet.get());

    m_pPetSet.reset(CPetSet::CreateNew(this));
    CHECKF(m_pPetSet.get());

    m_pUserSkillManager.reset(CPlayerSkillManager::CreateNew(this));
    CHECKF(m_pUserSkillManager.get());

    m_pCDSet.reset(CPlayerCoolDownSet::CreateNew(this));
    CHECKF(m_pCDSet.get());

    m_pBag.reset(CPackage::CreateNew(this, ITEMPOSITION_BAG, GetBagMaxSize()));
    CHECKF(m_pBag.get());

    m_pEquipmentSet.reset(CEquipment::CreateNew(this));
    CHECKF(m_pEquipmentSet.get());

    m_pTaskSet.reset(CPlayerTask::CreateNew(this));
    CHECKF(m_pTaskSet.get());

    m_pAchievement.reset(CPlayerAchievement::CreateNew(this));
    CHECKF(m_pAchievement.get());

    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CPlayer::SendMsg(const google::protobuf::Message& msg) const
{
    return SendMsg(to_sc_cmd(msg), msg);
}

bool CPlayer::SendMsg(uint16_t cmd, const google::protobuf::Message& msg) const
{
    __ENTER_FUNCTION
    CNetworkMessage _msg(cmd, msg, SceneService()->GetServerVirtualSocket(), GetSocket());
    MonitorMgr()->AddSendInfo(cmd, _msg.GetSize());
    return SceneService()->SendMsgToPort(_msg);
    __LEAVE_FUNCTION
    return false;
}

uint32_t CPlayer::GetBagMaxSize() const
{
    return DEFAULT_BAG_SIZE + m_pRecord->Field(TBLD_PLAYER::BAG_SIZE).get<uint32_t>();
}

uint32_t CPlayer::GetStrogeMaxSize() const
{
    return DEFAULT_STROGE_SIZE + m_pRecord->Field(TBLD_PLAYER::STROGE_SIZE).get<uint32_t>();
}

void CPlayer::OnTimer()
{
    __ENTER_FUNCTION
    ProcessMsg();
    auto now = TimeGetSecond();
    m_pBag->CheckItemExpire(now);
    if(m_pStoragePackage)
        m_pStoragePackage->CheckItemExpire(now);
    m_pEquipmentSet->CheckItemExpire(now);
    __LEAVE_FUNCTION
}


void CPlayer::OnLogout()
{
    __ENTER_FUNCTION
    //从当前场景离开
    if(m_pScene != nullptr)
    {
        // log error
        m_pStatus->OnLogout();
        m_pScene->LeaveMap(this);
    }

    uint32_t now = TimeGetSecond();
    m_pRecord->Field(TBLD_PLAYER::LAST_LOGOUTTIME) = now;


    SceneService()->DelSocketMessagePool(GetSocket());
    //处理好数据
    LOGLOGIN("Player:{} Logout", GetName().c_str());
    ActorManager()->DelActor(this, false);
    m_pEventOnTimer.Clear();

    ST_LOADINGTHREAD_PROCESS_DATA data;
    data.nPorcessType = LPT_SAVE;
    data.idPlayer     = GetID();
    data.bChangeZone  = false;
    data.socket       = GetSocket();
    data.idTargetScene= 0;
    data.fPosX        = 0.0f;
    data.fPosY        = 0.0f;
    data.fRange       = 0.0f;
    data.fFace        = 0.0f;
    data.pPlayer      = this;
    SceneService()->GetLoadingThread()->AddClosePlayer(std::move(data));

    __LEAVE_FUNCTION
}


void CPlayer::OnLogin(bool bLogin, const SceneIdx& idxScene, float fPosX, float fPosY, float fRange, float fFace)
{
    __ENTER_FUNCTION

    if(bLogin)
        LOGLOGIN("CPlayer::OnLogin: {}", GetID());
    else
        LOGLOGIN("CPlayer::OnLoginChangeZone: {}", GetID());

    if(bLogin)
    {
        uint32_t now = TimeGetSecond();
        m_pRecord->Field(TBLD_PLAYER::LAST_LOGINTIME) = now;
        m_pRecord->Update();
        //登陆相关的操作， 只需要做1次
        SystemVarSet()->SyncToClient(this);

        
        uint32_t lastLogoutTime = m_pRecord->Field(TBLD_PLAYER::LAST_LOGOUTTIME);

        uint32_t offline_time   = 0;
        if(lastLogoutTime < now)
            offline_time = now - lastLogoutTime;
        CCommonData* pCommonData = GetCommonDataSet()->QueryData(COMMON_DATA_CONTINUELOGIN);
        if(pCommonData)
        {
            int32_t nDayDiff = DayDiffLocal(pCommonData->GetData(0), now);
            if(nDayDiff >= 1)
            {
                if(nDayDiff == 1)
                {
                    //记录连续登录
                    pCommonData->AddData(1, 1, UPDATE_FALSE, SYNC_FALSE);
                }
                //离线时长超过1小时，记录离线时间
                if(offline_time > 3600)
                    pCommonData->AddData(2, offline_time, UPDATE_FALSE, SYNC_FALSE);

                pCommonData->SetData(0, now, UPDATE_TRUE, SYNC_FALSE);
                //增加总累计登录
                GetDataCountSet()->AddCount(DATA_ACC_SYSTEM, DATA_ACC_SYSTEM_LOGINDAY, 1, UPDATE_FALSE);
            }
        }

        //同步各种数据给前端
        m_pCommonDataSet->SyncAll();
        m_pDataCountSet->SyncAll();
        m_pCDSet->SyncAll();
        m_pPetSet->SyncAll();
        m_pBag->SendAllItemInfo();
        m_pEquipmentSet->SendInfo();
        m_pTaskSet->SendTaskInfo();
        m_pAchievement->SyncAll();

        // 组队信息
        // if (GetTeamID() != ID_NONE)
        //	QueryTeamMember()->OnLogin();

        
    }
    m_pStatus->OnLogin();
    m_pStatus->SyncTo(this);

    //其他的换线操作

    //找到对应的场景
    CPhase* pCurScene = SceneManager()->QueryPhase(idxScene);
    if(pCurScene == nullptr)
    {
        pCurScene = SceneManager()->QueryPhase(idxScene.GetStaticPhaseSceneIdx());
        if(pCurScene == nullptr)
        {
            pCurScene = SceneManager()->QueryPhase(GetRecordSceneIdx());
            if(pCurScene == nullptr)
            {
                pCurScene = SceneManager()->QueryPhase(GetHomeSceneIdx());
                if(pCurScene == nullptr)
                {
                    // log error, notify client
                    LOGERROR("CPlayer[{}]::OnLogin FindMap Error: id:{} record:{} home{} ", GetID(), idxScene, GetRecordSceneIdx(), GetHomeSceneIdx());
                    return;
                }
                else
                {
                    fPosX = GetHomePosX();
                    fPosY = GetHomePosY();
                }
            }
            else
            {
                fPosX = GetRecordPosX();
                fPosY = GetRecordPosY();
            }
        }
    }

    Vector2 findpos = pCurScene->FindPosNearby(Vector2(fPosX, fPosY), fRange);

    m_idLoadingScene = pCurScene->GetID();
    m_fLoadingPosX   = findpos.x;
    m_fLoadingPosY   = findpos.y;
    m_fLoadingFace   = fFace;

    SendPlayerInfoToClient();
    RecalcAttrib(true);

    //通知前端读取场景
    //等待前端loading完成
    SC_LOADMAP msg;
    msg.set_mapid(pCurScene->GetMapID());
    msg.set_posx(m_fLoadingPosX);
    msg.set_posy(m_fLoadingPosY);
    msg.set_face(m_fLoadingFace);

    SendMsg(msg);

    CEventEntryCreateParam param;
    param.evType = 0;
    param.cb     = std::bind(&CPlayer::OnTimer, this);
    param.tWaitTime = 100;
    param.bPersist  = true;
    EventManager()->ScheduleEvent(param, m_pEventOnTimer);
    __LEAVE_FUNCTION
}

void CPlayer::SendPlayerInfoToClient()
{
    __ENTER_FUNCTION
    SC_PLAYERINFO msg;
    msg.set_playerid(GetID());
    msg.set_worldid(GetWorldID());
    msg.set_name(GetName());
    msg.set_prof(GetProf());
    msg.set_baselook(GetBaseLook());
    msg.set_exp(GetExp());
    msg.set_level(GetLev());
    msg.set_viplev(GetVipLev());
    msg.set_pkval(GetPKVal());
    msg.set_honor(GetHonor());
    msg.set_campid(GetCampID());

    msg.set_money(GetMoney());
    msg.set_money_bind(GetMoneyBind());
    msg.set_gold(GetGold());
    msg.set_gold_bind(GetGoldBind());

    msg.set_bag_size(GetBagMaxSize());
    msg.set_stroge_size(GetStrogeMaxSize());
    msg.set_achi_point(GetAchiPoint());

    msg.set_hp(GetHP());
    msg.set_mp(GetMP());
    msg.set_fp(GetFP());
    msg.set_np(GetNP());

    SendMsg(msg);
    __LEAVE_FUNCTION
}

void CPlayer::OnLevUp(uint32_t nLev)
{
    __ENTER_FUNCTION
    //调用系统脚本
    ScriptManager()->_ExecScript<void>("OnPlayerLevUp", this, nLev);
    __LEAVE_FUNCTION
}

void CPlayer::BroadcastShow()
{
    AddDelaySendShowToAllViewPlayer();
}

bool CPlayer::IsTalkEnable(uint32_t nTalkChannel)
{
    __ENTER_FUNCTION
    if(GetLev() < TALK_MIN_LEV[nTalkChannel])
        return false;
    if(GetCDSet()->IsCoolDown(COOLDOWN_TYPE_TALK, nTalkChannel) == false)
        return false;

    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CPlayer::TryChangeMap(uint32_t nLeavePointIdx)
{
    __ENTER_FUNCTION
    auto pLeaveData = GetCurrentScene()->GetMap()->GetLeavePointByIdx(nLeavePointIdx);
    CHECKF_FMT(pLeaveData, "Can't Find LeaveMap {} On Map {}", GetMapID(), nLeavePointIdx);

    CHECKF(GetPos().distance(Vector2(pLeaveData->x(), pLeaveData->y())) > pLeaveData->range());

    auto pGameMap = MapManager()->QueryMap(pLeaveData->dest_map_id());
    CHECKF_FMT(pGameMap,
               "Can't Find Map {} When LeaveMap {} On Map {}",
               pLeaveData->dest_map_id(),
               GetMapID(),
               nLeavePointIdx);

    //检查所有通行检查
    auto pEnterData = pGameMap->GetEnterPointByIdx(pLeaveData->dest_enter_point_idx());
    CHECKF_FMT(pEnterData,
               "Can't Find EnterPoint {} On Map {} When LeaveMap {} On Map {}",
               pLeaveData->dest_enter_point_idx(),
               pLeaveData->dest_map_id(),
               GetMapID(),
               nLeavePointIdx);

    if(GetTeamMemberCount() < pEnterData->team_req())
    {
        // send errmsg
        return false;
    }

    if(GetGuildLev() < pEnterData->guild_req())
    {
        // send errmsg
        return false;
    }

    if(GetLev() < pEnterData->lev_req())
    {
        return false;
    }

    if(GetVipLev() < pEnterData->vip_lev_req())
    {
        return false;
    }

    return FlyMap(pEnterData->idmap(),
                  pEnterData->idphase(),
                  pEnterData->x(),
                  pEnterData->y(),
                  pEnterData->range(),
                  pEnterData->face());
    __LEAVE_FUNCTION
    return false;
}

uint32_t CPlayer::GetTeamMemberCount() const
{
    return 0;
}

uint32_t CPlayer::GetGuildLev() const
{
    return 0;
}

void CPlayer::SendTalkMsg(uint32_t nTalkChannel, const std::string& txt)
{
    // MsgTalk
    SC_TALK msg;
    msg.set_words(txt);
    msg.set_channel(TalkChannel(nTalkChannel));
    msg.set_send_time(TimeGetSecond());
    SendMsg(msg);
}

void CPlayer::MakeShowData(SC_AOI_NEW& msg)
{
    __ENTER_FUNCTION
    msg.set_mapid(GetMapID());

    msg.set_actor_id(GetID());
    msg.set_actortype(ACT_PLAYER);
    msg.set_campid(GetCampID());
    msg.set_prof(GetProf());
    msg.set_lev(GetLev());
    msg.set_baselook(GetBaseLook());
    msg.set_name(GetName());
    msg.set_hp(GetHP());
    msg.set_hpmax(GetHPMax());

    msg.set_weaponid(m_pEquipmentSet->GetWeaponTypeID());
    msg.set_armorid(m_pEquipmentSet->GetArmorTypeID());

    // msg.set_guildid(GetGuildID());
    // msg.set_guildname(GetGuildName());

    __LEAVE_FUNCTION
}

void CPlayer::SaveInfo()
{
    __ENTER_FUNCTION

    //每个模块回写
    m_pCommonDataSet->Save();
    m_pDataCountSet->Save();
    m_pPetSet->Save();
    m_pCDSet->Save();
    m_pBag->SaveAll();
    m_pEquipmentSet->SaveAll();
    m_pTaskSet->SaveInfo();
    m_pAchievement->SaveAll();
    //本身回写
    m_pRecord->Update(true);

    LOGLOGIN("CPlayer::SaveInfo: {}", GetID());
    __LEAVE_FUNCTION
}

void CPlayer::ProcessMsg()
{
    __ENTER_FUNCTION
    auto& refPool = SceneService()->GetMessagePoolRef(GetSocket());
    if(refPool.empty() == true)
        return;

    auto pProcesser = SceneService()->GetNetMsgProcess();
    //每个人每次最多处理10条消息
    constexpr int32_t MAX_PROCESS_PER_TIME = 10;
    int32_t           nProcessCount        = 0;
    while(refPool.empty() == false && nProcessCount < MAX_PROCESS_PER_TIME)
    {
        CNetworkMessage* pMsg = refPool.front();
        refPool.pop_front();
        if(pMsg)
        {
            MonitorMgr()->CmdProcessStart(pMsg->GetCmd());
            pProcesser->Process(pMsg);
            MonitorMgr()->CmdProcessEnd(pMsg->GetCmd());
            SAFE_DELETE(pMsg);
        }
    }
    if(refPool.size() > 100)
    {
        LOGWARNING("Player{} Have Too Mony Msg Need Process.", GetID());
    }
    __LEAVE_FUNCTION
}
