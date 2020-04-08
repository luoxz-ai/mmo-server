#include "Player.h"

#include "GameEventDef.h"
#include "NetMSGProcess.h"
#include "Npc.h"
#include "Scene.h"
#include "ZoneService.h"
MEMORYHEAP_IMPLEMENTATION(CPlayer, s_heap);

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

    auto pDB = ZoneService()->GetGameDB(GetWorldIDFromPlayerID(idPlayer));
    CHECKF(pDB);
    auto result_ptr =
        pDB->Query(TBLD_PLAYER::table_name,
                   fmt::format(FMT_STRING("SELECT * FROM {} WHERE id={} LIMIT 1"), TBLD_PLAYER::table_name, idPlayer));
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

    m_pUserSkillManager.reset(CUserSkillManager::CreateNew(this));
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

bool CPlayer::SendMsg(uint16_t cmd, const google::protobuf::Message& msg) const
{
    __ENTER_FUNCTION
    CNetworkMessage _msg(cmd, msg, ZoneService()->GetServerVirtualSocket(), GetSocket());
    MonitorMgr()->AddSendInfo(cmd, _msg.GetSize());
    return ZoneService()->SendMsg(_msg);
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

void CPlayer::OnLogin(bool bLogin, const SceneID& idScene, float fPosX, float fPosY, float fRange, float fFace)
{
    __ENTER_FUNCTION

    if(bLogin)
        LOGLOGIN("CPlayer::OnLogin: {}", GetID());
    else
        LOGLOGIN("CPlayer::OnLoginChangeZone: {}", GetID());

    if(bLogin)
    {

        //登陆相关的操作， 只需要做1次
        SystemVarSet()->SyncToClient(this);

        uint32_t now            = TimeGetSecond();
        uint32_t lastLogoutTime = m_pRecord->Field(TBLD_PLAYER::LAST_LOGOUTTIME);
        uint32_t offline_time   = 0;
        if(lastLogoutTime < now)
            offline_time = now - lastLogoutTime;
        CCommonData* pCommonData = GetCommonDataSet()->QueryData(COMMON_DATA_CONTINUELOGIN);
        if(pCommonData)
        {
            int32_t nDateDiff = DateDiffLocal(pCommonData->GetData(0), now);
            if(nDateDiff >= 1)
            {
                if(nDateDiff == 1)
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
    CScene* pCurScene = SceneManager()->QueryScene(idScene);
    if(pCurScene == nullptr)
    {
        pCurScene = SceneManager()->QueryScene(GetRecordSceneID());
        if(pCurScene == nullptr)
        {
            pCurScene = SceneManager()->QueryScene(GetHomeSceneID());
            if(pCurScene == nullptr)
            {
                // log error, notify client
                LOGERROR("CPlayer::OnLogin FindMap Error: {}", GetID());
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

    SendMsg(CMD_SC_LOADMAP, msg);

    EventManager()->ScheduleEvent(0, std::bind(&CPlayer::OnTimer, this), 100, true, m_pEventOnTimer);
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

    SendMsg(CMD_SC_PLAYERINFO, msg);
    __LEAVE_FUNCTION
}

void CPlayer::SendAttribToClient()
{
    __ENTER_FUNCTION
    SC_ACTORATTRIB msg;
    for(int32_t i = 0; i < ATTRIB_MAX; i++)
    {
        msg.add_attrib_list(m_ActorAttrib.get(i));
    }

    SendMsg(CMD_SC_ACTORATTRIB, msg);
    __LEAVE_FUNCTION
}

void CPlayer::RecalcAttrib(bool bClearCache /*= false*/)
{
    __ENTER_FUNCTION
    if(bClearCache == true)
    {
        // lev给予的基础属性
        auto pData = UserAttrSet()->QueryObj(CUserAttrData::MakeID(GetProf(), GetLev()));
        CHECK(pData);
        m_ActorAttrib.SetBase(pData->GetAbility());
    }

    CActor::RecalcAttrib(bClearCache);

    SendAttribToClient();

    __LEAVE_FUNCTION
}

void CPlayer::OnLevUp(uint32_t nLev)
{
    __ENTER_FUNCTION
    //调用系统脚本
    ScriptManager()->_ExecScript<void>("OnPlayerLevUp", this, nLev);
    __LEAVE_FUNCTION
}

void CPlayer::BrodacastShow()
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
    CHECKF_M(pLeaveData,
             fmt::format(FMT_STRING("Can't Find LeaveMap {} On Map {}"), GetMapID(), nLeavePointIdx).c_str());

    CHECKF(GetPos().distance(Vector2(pLeaveData->x(), pLeaveData->y())) > pLeaveData->range());

    auto pGameMap = MapManager()->QueryMap(pLeaveData->dest_map_id());
    CHECKF_M(pGameMap,
             fmt::format(FMT_STRING("Can't Find Map {} When LeaveMap {} On Map {}"),
                         pLeaveData->dest_map_id(),
                         GetMapID(),
                         nLeavePointIdx)
                 .c_str());

    //检查所有通行检查
    auto pEnterData = pGameMap->GetEnterPointByIdx(pLeaveData->dest_enter_point_idx());
    CHECKF_M(pEnterData,
             fmt::format(FMT_STRING("Can't Find EnterPoint {} On Map {} When LeaveMap {} On Map {}"),
                         pLeaveData->dest_enter_point_idx(),
                         pLeaveData->dest_map_id(),
                         GetMapID(),
                         nLeavePointIdx)
                 .c_str());

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

    return FlyMap(pEnterData->idmap(), pEnterData->x(), pEnterData->y(), pEnterData->range(), pEnterData->face());
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
    SendMsg(CMD_SC_TALK, msg);
}

void CPlayer::SetPKMode(uint32_t val)
{
    __ENTER_FUNCTION
    switch(val)
    {
        case PKMODE_PEACE:
        case PKMODE_REDNAME:
        case PKMODE_TEAM:
        case PKMODE_GUILD:
        case PKMODE_CAMP:
        case PKMODE_ALL:
        {
            m_nPKMode = val;
        }
        break;
        default:
            return;
            break;
    }

    SC_CHANGE_PKMODE msg;
    msg.set_pkmode(m_nPKMode);
    SendMsg(CMD_SC_CHANGE_PKMODE, msg);
    __LEAVE_FUNCTION
}

void CPlayer::_SetProperty(uint32_t nType, uint32_t nVal, uint32_t nSync /*= SYNC_TRUE*/)
{
    __ENTER_FUNCTION
    switch(nType)
    {
        case PROP_LEVEL:
        {
            m_pRecord->Field(TBLD_PLAYER::LEV) = nVal;
            m_pRecord->Update();
            GetAchievement()->CheckAchiCondition(CONDITION_LEVEL, nVal);
        }
        break;
        case PROP_EXP:
        {
            m_pRecord->Field(TBLD_PLAYER::EXP) = nVal;
        }
        break;
        case PROP_MONEY:
        {
            m_pRecord->Field(TBLD_PLAYER::MONEY) = nVal;
            m_pRecord->Update();
            GetAchievement()->CheckAchiCondition(CONDITION_MONEY, nVal);
        }
        break;
        case PROP_MONEYBIND:
        {
            m_pRecord->Field(TBLD_PLAYER::MONEY_BIND) = nVal;
            m_pRecord->Update();
        }
        break;
        case PROP_GOLD:
        {
            m_pRecord->Field(TBLD_PLAYER::GOLD) = nVal;
            m_pRecord->Update();
        }
        break;
        case PROP_GOLDBIND:
        {
            m_pRecord->Field(TBLD_PLAYER::GOLD_BIND) = nVal;
            m_pRecord->Update();
        }
        break;
        case PROP_PKVAL:
        {
            m_pRecord->Field(TBLD_PLAYER::PKVAL) = nVal;
        }
        break;
        case PROP_HONOR:
        {
            m_pRecord->Field(TBLD_PLAYER::HONOR) = nVal;
            m_pRecord->Update();
        }
        break;
        case PROP_ACHIPOINT:
        {
            m_pRecord->Field(TBLD_PLAYER::ACHIPOINT) = nVal;
            m_pRecord->Update();
            GetAchievement()->CheckAchiCondition(CONDITION_ACHIPOINT, nVal);
        }
        break;
        case PROP_VIPLEV:
        {
            m_pRecord->Field(TBLD_PLAYER::VIPLEV) = nVal;
            m_pRecord->Update();
        }
        break;
        default:
            break;
    }

    CActor::_SetProperty(nType, nVal, nSync);
    __LEAVE_FUNCTION
}

uint32_t CPlayer::GetPropertyMax(uint32_t nType) const
{
    __ENTER_FUNCTION
    switch(nType)
    {
        case PROP_EXP:
        {
            return 0xFFFFFFFF;
        }
        break;
        case PROP_MONEY:
        {
            return 0xFFFFFFFF;
        }
        break;
        case PROP_MONEYBIND:
        {
            return 0xFFFFFFFF;
        }
        break;
        case PROP_GOLD:
        {
            return 0xFFFFFFFF;
        }
        break;
        case PROP_GOLDBIND:
        {
            return 0xFFFFFFFF;
        }
        break;
        case PROP_PKVAL:
        {
            return 99999999;
        }
        break;
        case PROP_HONOR:
        {
            return 999999999;
        }
        break;
        case PROP_ACHIPOINT:
        {
            return 999999999;
        }
        break;
        case PROP_VIPLEV:
        {
            return 255;
        }
        break;
        default:
            break;
    }

    return CActor::GetPropertyMax(nType);
    __LEAVE_FUNCTION
    return 0;
}

uint32_t CPlayer::GetProperty(uint32_t nType) const
{
    __ENTER_FUNCTION
    switch(nType)
    {
        case PROP_EXP:
        {
            return GetExp();
        }
        break;
        case PROP_MONEY:
        {
            return GetMoney();
        }
        break;
        case PROP_MONEYBIND:
        {
            return GetMoneyBind();
        }
        break;
        case PROP_GOLD:
        {
            return GetGold();
        }
        break;
        case PROP_GOLDBIND:
        {
            return GetGoldBind();
        }
        break;
        case PROP_PKVAL:
        {
            return GetPKVal();
        }
        break;
        case PROP_HONOR:
        {
            return GetHonor();
        }
        break;
        case PROP_ACHIPOINT:
        {
            return GetAchiPoint();
        }
        break;
        case PROP_VIPLEV:
        {
            return GetAchiPoint();
        }
        break;
        default:
            break;
    }

    return CActor::GetProperty(nType);
    __LEAVE_FUNCTION
    return 0;
}

bool CPlayer::CanDamage(CActor* pTarget) const
{
    __ENTER_FUNCTION
    CHECKF(pTarget);
    CHECKF(pTarget->GetCurrentScene());
    CHECKF(pTarget->GetCurrentScene() == GetCurrentScene());

    if(this == pTarget)
        return false;

    CPlayer* pTargetPlayer = nullptr;
    if(pTarget->IsMonster())
    {
        if(pTarget->GetOwnerID() != 0)
            pTargetPlayer = ActorManager()->QueryPlayer(pTarget->GetOwnerID());
        else
            return true;
    }
    else if(pTarget->IsPet())
    {
        pTargetPlayer = ActorManager()->QueryPlayer(pTarget->GetOwnerID());
    }
    if(pTarget->IsPlayer())
    {
        pTargetPlayer = pTarget->CastTo<CPlayer>();
    }

    if(pTargetPlayer)
    {
        //根据pk规则
        if(GetCurrentScene()->GetMap()->IsPvPDisable(pTarget->GetPos().x, pTarget->GetPos().y) == true)
            return false;
        if(GetCurrentScene()->GetMap()->IsPvPDisable(GetPos().x, GetPos().y) == true)
            return false;

        //新手保护
        constexpr uint32_t NEWIBLE_PK_PROTECTED_LEV = 40;
        if(GetCurrentScene()->GetMap()->HasMapFlag(MAPFLAG_DISABLE_PKPROTECTED) == false &&
           pTargetPlayer->GetLev() < NEWIBLE_PK_PROTECTED_LEV)
            return false;

        switch(GetPKMode())
        {
            case PKMODE_PEACE:
            {
                return false;
            }
            break;
            case PKMODE_REDNAME:
            {
                return pTargetPlayer->GetStatus()->TestStatusByType(STATUSTYPE_CRIME) ||
                       pTargetPlayer->GetPKVal() >= RED_NAME_PKVAL;
            }
            break;
            case PKMODE_TEAM:
            {
                return (GetTeamID() == 0 || pTargetPlayer->GetTeamID() != GetTeamID());
            }
            break;
            case PKMODE_GUILD:
            {
                return (GetGuildID() == 0 || pTargetPlayer->GetGuildID() != GetGuildID());
            }
            break;
            case PKMODE_CAMP:
            {
                return (pTargetPlayer->GetCampID() != GetCampID());
            }
            break;
            case PKMODE_ALL:
            {
                return true;
            }
            break;
        }
    }

    return true;
    __LEAVE_FUNCTION
    return false;
}

void CPlayer::BeKillBy(CActor* pAttacker)
{
    CActor::BeKillBy(pAttacker);

    if(pAttacker && pAttacker->IsPlayer())
    {
        CPlayer* pPlayer = pAttacker->CastTo<CPlayer>();
        if(GetCurrentScene()->IsPvPFree(GetPos().x, GetPos().y) == false)
        {
            //计算PK值
            pPlayer->GetDataCountSet()->AddCount(DATA_ACC_SYSTEM, DATA_ACC_SYSTEM_KILLPLAYER, 1, UPDATE_FALSE);
            pPlayer->AddProperty(PROP_PKVAL, ADD_PKVAL_PERKILL, SYNC_ALL_DELAY);
        }
    }

    GetDataCountSet()->AddCount(DATA_ACC_SYSTEM, DATA_ACC_SYSTEM_BEKILL_BYPLAYER, 1, UPDATE_FALSE);
}

bool CPlayer::IsEnemy(CSceneObject* pTarget) const
{
    __ENTER_FUNCTION
    if(this == pTarget)
        return false;
    CActor* pTargetActor = static_cast<CActor*>(pTarget);
    if(pTargetActor->IsMonster())
    {
        if(pTargetActor->GetOwnerID() == GetID())
            return false;

        return pTargetActor->GetCampID() != GetCampID();
    }
    else if(pTargetActor->IsPet())
    {
        if(pTargetActor->GetOwnerID() == GetID())
            return false;

        return pTargetActor->GetCampID() != GetCampID();
    }
    else if(pTargetActor->IsPlayer())
    {
        return pTargetActor->GetCampID() != GetCampID();
    }
    __LEAVE_FUNCTION
    return false;
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

CPackage* CPlayer::QueryPackage(uint32_t nPosition)
{
    __ENTER_FUNCTION
    if(nPosition == ITEMPOSITION_BAG)
        return GetBag();
    else if(nPosition == ITEMPOSITION_STORAGE)
        return GetStroagePackage();
    else
        return nullptr;
    __LEAVE_FUNCTION
    return nullptr;
}

CStoragePackage* CPlayer::GetStroagePackage()
{
    __ENTER_FUNCTION
    if(m_pStoragePackage == nullptr)
    {
        m_pStoragePackage.reset(CStoragePackage::CreateNew(this, ITEMPOSITION_STORAGE, GetStrogeMaxSize()));
        CHECKF(m_pStoragePackage);
        m_pStoragePackage->SendAllItemInfo();
    }
    return m_pStoragePackage.get();
    __LEAVE_FUNCTION
    return nullptr;
}

bool CPlayer::CheckMoney(uint32_t nMoneyType, uint32_t nVal)
{
    __ENTER_FUNCTION
    switch(nMoneyType)
    {
        case MT_MONEY:
        {
            return GetMoney() >= nVal;
        }
        break;
        case MT_MONEYBIND:
        {
            return GetMoneyBind() >= nVal;
        }
        break;
        case MT_GOLD:
        {
            return GetGold() >= nVal;
        }
        break;
        case MT_GOLDBIND:
        {
            return GetGoldBind() >= nVal;
        }
        break;
        default:
            break;
    }
    __LEAVE_FUNCTION
    return false;
}

bool CPlayer::SpendMoney(uint32_t nMoneyType, uint32_t nVal)
{
    __ENTER_FUNCTION
    switch(nMoneyType)
    {
        case MT_MONEY:
        {
            if(GetMoney() >= nVal)
                SetProperty(PROP_MONEY, GetMoney() - nVal, SYNC_TRUE);
            else
                return false;
        }
        break;
        case MT_MONEYBIND:
        {
            if(GetMoneyBind() >= nVal)
            {
                SetProperty(PROP_MONEYBIND, GetMoneyBind() - nVal, SYNC_TRUE);
            }
            else if((uint64_t)GetMoneyBind() + (uint64_t)GetMoney() >= nVal)
            {
                AddProperty(PROP_MONEY, -(nVal - GetMoneyBind()), SYNC_TRUE);
                SetProperty(PROP_MONEYBIND, 0, SYNC_TRUE);
            }
            else
                return false;
        }
        break;
        case MT_GOLD:
        {
            if(GetGold() >= nVal)
                SetProperty(PROP_GOLD, GetGold() - nVal, SYNC_TRUE);
            else
                return false;
        }
        break;
        case MT_GOLDBIND:
        {
            if(GetGoldBind() >= nVal)
            {
                SetProperty(PROP_GOLDBIND, GetGoldBind() - nVal, SYNC_TRUE);
            }
            else if((uint64_t)GetGoldBind() + (uint64_t)GetGold() >= nVal)
            {
                AddProperty(PROP_GOLD, -(nVal - GetGoldBind()), SYNC_TRUE);
                SetProperty(PROP_GOLDBIND, 0, SYNC_TRUE);
            }
            else
                return false;
        }
        break;
        default:
            break;
    }

    //记录日志

    //即刻回写
    m_pRecord->Update(true);
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CPlayer::AwardMeony(uint32_t nMoneyType, uint32_t nVal)
{
    __ENTER_FUNCTION
    switch(nMoneyType)
    {
        case MT_MONEY:
        {
            AddProperty(PROP_MONEY, nVal, SYNC_TRUE);
        }
        break;
        case MT_MONEYBIND:
        {
            AddProperty(PROP_MONEYBIND, nVal, SYNC_TRUE);
        }
        break;
        case MT_GOLD:
        {
            AddProperty(PROP_GOLD, nVal, SYNC_TRUE);
        }
        break;
        case MT_GOLDBIND:
        {
            AddProperty(PROP_GOLDBIND, nVal, SYNC_TRUE);
        }
        break;
        default:
            break;
    }

    //记录日志

    //即刻回写
    m_pRecord->Update(true);

    return true;
    __LEAVE_FUNCTION
    return false;
}

void CPlayer::AwardExp(uint32_t nExp)
{
    __ENTER_FUNCTION
    nExp = MulDiv(nExp, GetAttrib().get(ATTRIB_EXP_ADJ) + 10000, 10000);

    uint64_t nCurExp = GetExp() + (uint64_t)nExp;
    //计算升级
    auto pData = UserAttrSet()->QueryObj(CUserAttrData::MakeID(GetProf(), GetLev()));
    while(pData && nCurExp > pData->GetLevUpNeedExp())
    {
        nCurExp -= pData->GetLevUpNeedExp();
        AddProperty(PROP_LEVEL, 1, SYNC_ALL_DELAY);
        OnLevUp(GetLev());
        pData = UserAttrSet()->QueryObj(CUserAttrData::MakeID(GetProf(), GetLev()));
    }

    SetProperty(PROP_EXP, (uint32_t)nCurExp, SYNC_TRUE);

    __LEAVE_FUNCTION
}

void CPlayer::AwardBattleExp(uint32_t nExp, bool bKillBySelf)
{
    AwardExp(nExp);
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
    auto& refPool = ZoneService()->GetMessagePoolRef(GetSocket());
    if(refPool.empty() == true)
        return;

    auto pProcesser = ZoneService()->GetNetMsgProcess();
    //每个人每次最多处理10条消息
    constexpr int32_t MAX_PROCESS_PER_TIME = 10;
    int32_t           nProcessCount        = 0;
    while(refPool.empty() == false && nProcessCount < MAX_PROCESS_PER_TIME)
    {
        CNetworkMessage* pMsg = refPool.front();
        refPool.pop_front();
        if(pMsg)
        {
            MonitorMgr()->AddConnStart(pMsg->GetCmd());
            pProcesser->Process(pMsg);
            MonitorMgr()->AddConnEnd(pMsg->GetCmd());
            SAFE_DELETE(pMsg);
        }
    }
    if(refPool.size() > 100)
    {
        LOGWARNING("Player{} Have Too Mony Msg Need Process.", GetID());
    }
    __LEAVE_FUNCTION
}

bool CPlayer::DialogBegin(const std::string& title)
{
    __ENTER_FUNCTION
    m_dialog_msg.Clear();
    m_dialog_msg.set_dialog_id(ZoneService()->CreateUID());
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CPlayer::DialogAddText(const std::string& txt)
{
    __ENTER_FUNCTION
    m_dialog_msg.add_dialog_txt(txt);
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CPlayer::DialogAddLink(uint32_t           nLinkType,
                            const std::string& link_txt,
                            uint32_t           idFuncType,
                            uint64_t           idData,
                            const std::string& callback_func,
                            uint64_t           idNpc)
{
    __ENTER_FUNCTION
    m_dialog_callback[m_dialog_msg.dialog_id()].push_back({idFuncType, idData, callback_func, idNpc});
    auto pLinkButton = m_dialog_msg.add_dialog_link_list();
    pLinkButton->set_style(nLinkType);
    pLinkButton->set_txt(link_txt);
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CPlayer::DialogSend(uint32_t nDialogType /*=0*/)
{
    __ENTER_FUNCTION
    m_dialog_msg.set_dialog_type(nDialogType);
    SendMsg(CMD_SC_DIALOG, m_dialog_msg);
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CPlayer::OnDialogClick(uint64_t idDialog, uint32_t nIdx)
{
    __ENTER_FUNCTION
    auto it = m_dialog_callback.find(idDialog);
    if(it == m_dialog_callback.end())
    {
        return false;
    }

    const auto& refList = it->second;
    auto        v       = refList[nIdx % refList.size()];
    m_dialog_callback.erase(it);
    switch(v.idFuncType)
    {
        case DIALOG_FUNC_ACCEPTTASK:
        {
            return GetTaskSet()->AcceptTask(v.idData, true, false);
        }
        break;
        case DIALOG_FUNC_SUBMITTASK:
        {
            if(GetTaskSet()->SubmitTask(GetLowFromU64(v.idData), GetHighFromU64(v.idData)) == true)
            {
                if(v.idNpc != 0)
                {
                    return ActiveNpc(v.idNpc);
                }
            }
            return true;
        }
        break;
        case DIALOG_FUNC_QUICKFINISHTASK:
        {
            if(GetTaskSet()->QuickFinish(GetLowFromU64(v.idData)))
            {
                if(v.idNpc != 0)
                {
                    return ActiveNpc(v.idNpc);
                }
            }
            return true;
        }
        break;
        case DIALOG_FUNC_SHOWTASK:
        {
            return GetTaskSet()->ShowTaskDialog(GetLowFromU64(v.idData), v.idNpc);
        }
        break;
        case DIALOG_FUNC_OPENSHOP:
        {
            DialogBegin("");
            DialogSend(GetLowFromU64(v.idData));
            return true;
        }
        break;
        case DIALOG_FUNC_SCRIPT:
        {
            ScriptManager()->ExecScript<void>(v.idData, v.callback_func.c_str(), this);
            return true;
        }
        break;
        default:
            break;
    }

    return false;
    __LEAVE_FUNCTION
    return false;
}

bool CPlayer::ActiveNpc(OBJID idNpc)
{
    __ENTER_FUNCTION
    CActor* pActor = ActorManager()->QueryActor(idNpc);
    if(pActor->IsNpc() == false)
        return false;

    if(GameMath::distance(pActor->GetPos(), GetPos()) < MIN_INTERACT_DIS)
        return false;

    CNpc* pNpc = pActor->CastTo<CNpc>();
    pNpc->ActiveNpc(this);
    return true;
    __LEAVE_FUNCTION
    return false;
}