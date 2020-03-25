#include "GameEventDef.h"
#include "NetMSGProcess.h"
#include "Player.h"
#include "Scene.h"
#include "ZoneService.h"

bool CPlayer::EnterDynaScene(CDynaScene* pScene, float fPosX, float fPosY, float fRange, float fFace)
{
    __ENTER_FUNCTION
    return EventManager()->ScheduleEvent(
        EVENTID_FLYMAP,
        std::bind(&CPlayer::_FlyMap, this, pScene->GetSceneID(), fPosX, fPosY, fRange, fFace),
        0,
        false,
        GetEventMapRef());
    __LEAVE_FUNCTION
    return false;
}

bool CPlayer::FlyMap(uint32_t idMap, float fPosX, float fPosY, float fRange, float fFace)
{
    __ENTER_FUNCTION

    CHECKF(GetCurrentScene());
    if(GetCurrentScene()->GetMap()->HasMapFlag(MAPFLAG_DISABLE_FLYMAP) == true)
        return false;

    CScene* pScene = SceneManager()->QueryStaticScene(idMap);
    if(pScene)
    {
        //场景在本地
        //延迟调用真正的FlyMap
        EventManager()->ScheduleEvent(
            EVENTID_FLYMAP,
            std::bind(&CPlayer::_FlyMap, this, pScene->GetSceneID(), fPosX, fPosY, fRange, fFace),
            0,
            false,
            GetEventMapRef());

        return true;
    }
    else
    {
        //通过原始Mapid查找所在Zone
        const CGameMap* pMap = MapManager()->QueryMap(idMap);
        CHECKF_M(pMap, fmt::format(FMT_STRING("can't find map {}"), idMap).c_str())

        uint16_t idZone = pMap->GetZoneID();
        if(idZone != ZoneService()->GetServiceID())
        {
            //切换zone
            EventManager()->ScheduleEvent(
                EVENTID_FLYMAP,
                std::bind(&CPlayer::_ChangeZone, this, SceneID(idZone, idMap, 0), fPosX, fPosY, fRange, fFace),
                0,
                false,
                GetEventMapRef());

            return true;
        }
        else
        {
            //应该在本zone，但是本zone上又没找到， 这就是大篓子
            // logerror

            return false;
        }
    }

    __LEAVE_FUNCTION
    return false;
}

void CPlayer::_ChangeZone(const SceneID& idScene, float fPosX, float fPosY, float fRange, float fFace)
{
    __ENTER_FUNCTION
    //从当前场景离开
    if(m_pScene == nullptr)
    {
        // log error
    }
    else
    {
        //从老地图离开
        m_pScene->LeaveMap(this, idScene);
    }

    //删除
    ActorManager()->DelActor(this, false);
    m_pEventOnTimer.Clear();
    ZoneService()->GetLoadingThread()->AddClosePlayer(this, idScene, fPosX, fPosY, fRange, fFace);
    __LEAVE_FUNCTION
}

void CPlayer::OnChangeZoneSaveFinish(const SceneID& idScene, float fPosX, float fPosY, float fRange, float fFace)
{
    __ENTER_FUNCTION
    //已经Save结束
    //发送ChangeZone消息给World， 由World转发该消息给对应的Zone
    ServerMSG::PlayerChangeZone msg;
    msg.set_socket(GetSocket());
    msg.set_idplayer(GetID());
    msg.set_idscene(idScene);
    msg.set_posx(fPosX);
    msg.set_posy(fPosY);
    msg.set_range(fRange);
    msg.set_face(fFace);

    ZoneService()->SendMsgToWorld(GetWorldID(), ServerMSG::MsgID_PlayerChangeZone, msg);

    SendGameData(idScene);
    ZoneService()->DelSocketMessagePool(GetSocket());
    __LEAVE_FUNCTION
}

void CPlayer::OnLoadMapSucc()
{
    __ENTER_FUNCTION
    CHECK(m_idLoadingScene != 0);
    CHECK(GetCurrentScene() == nullptr);
    CScene* pScene = SceneManager()->QueryScene(m_idLoadingScene);
    CHECK(pScene);
    LOGLOGIN("CPlayer::OnLoadMapSucc: {}", GetID());

    pScene->EnterMap(this, m_fLoadingPosX, m_fLoadingPosY, m_fLoadingFace);
    __LEAVE_FUNCTION
}

void CPlayer::SendGameData(const SceneID& idScene)
{
    __ENTER_FUNCTION
    ServerMSG::PlayerChangeZone_Data msg;

    //通过World发送
    ZoneService()->SendMsgToWorld(GetWorldID(), ServerMSG::MsgID_PlayerChangeZone_Data, msg);
    __LEAVE_FUNCTION
}
void CPlayer::OnRecvGameData(CNetworkMessage* pMsg)
{
    __ENTER_FUNCTION

    __LEAVE_FUNCTION
}

void CPlayer::OnEnterMap(CSceneBase* pScene)
{
    __ENTER_FUNCTION

    CHECK(GetCurrentScene() == nullptr);
    LOGLOGIN("Player OnEnterMapStart:{} {} pos:{:.2f} {:.2f}", GetID(), pScene->GetMapID(), GetPosX(), GetPosY());
    CActor::OnEnterMap(pScene);

    m_idLoadingScene = 0;
    m_fLoadingPosX   = 0.0f;
    m_fLoadingPosY   = 0.0f;
    m_fLoadingFace   = 0.0f;

    if(pScene->GetMap()->HasMapFlag(MAPFLAG_RECORD_DISABLE) == false)
    {
        m_pRecord->Field(TBLD_PLAYER::RECORD_SCENEID) = pScene->GetSceneID().data64;
        m_pRecord->Field(TBLD_PLAYER::RECORD_X)       = GetPosX();
        m_pRecord->Field(TBLD_PLAYER::RECORD_Y)       = GetPosY();
        m_pRecord->Field(TBLD_PLAYER::RECORD_FACE)    = GetFace();
    }
    //进入地图了， 保存一下当前的一些修改
    m_pRecord->Update(true);

    ServerMSG::ActorCreate ai_msg;
    ai_msg.set_actor_id(GetID());
    ai_msg.set_scene_id(GetSceneID());
    ai_msg.set_actortype(ACT_PLAYER);
    ai_msg.set_baselook(GetBaseLook());
    ai_msg.set_prof(GetProf());
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

    ZoneService()->SendMsgToAIService(ServerMSG::MsgID_ActorCreate, ai_msg);

    LOGLOGIN("CPlayer::OnEnterMapEnd: {} mapid: {}", GetID(), GetSceneID());

    SC_ENTERMAP msg;
    msg.set_x(GetPosX());
    msg.set_y(GetPosY());
    msg.set_face(GetFace());
    SendMsg(CMD_SC_ENTERMAP, msg);

    __LEAVE_FUNCTION
}

void CPlayer::OnLeaveMap(uint64_t idTargetScene)
{
    __ENTER_FUNCTION
    CHECK(GetCurrentScene());
    LOGLOGIN("Player OnLeaveMap:{} {}", GetID(), GetCurrentScene()->GetMapID());

    if(GetCurrentScene()->GetMap()->HasMapFlag(MAPFLAG_RECORD_DISABLE) == false)
    {
        m_pRecord->Field(TBLD_PLAYER::RECORD_SCENEID) = GetCurrentScene()->GetSceneID().data64;
        m_pRecord->Field(TBLD_PLAYER::RECORD_X)       = GetPosX();
        m_pRecord->Field(TBLD_PLAYER::RECORD_Y)       = GetPosY();
        m_pRecord->Field(TBLD_PLAYER::RECORD_FACE)    = GetFace();
    }

    m_SkillFSM.Stop();
    m_pStatus->Stop();
    m_pStatus->OnLeaveMap();
    m_pPetSet->CallBack();

    CActor::OnLeaveMap(idTargetScene);
    //从这里开始，Player就没有Scene指针了

    __LEAVE_FUNCTION
}

void CPlayer::_FlyMap(const SceneID& idScene, float fPosX, float fPosY, float fRange, float fFace)
{
    __ENTER_FUNCTION
    //查询是否有对应地图
    CHECK(GetCurrentScene());
    CScene* pScene = SceneManager()->QueryScene(idScene);
    if(pScene == nullptr)
    {
        // log error
        return;
    }

    if(m_pScene == pScene)
    {
        LOGLOGIN("Player FlySameMap:{} {} pos:{:.2f} {:.2f} {:.2f}s",
                 GetID(),
                 idScene.GetMapID(),
                 fPosX,
                 fPosY,
                 fRange);

        m_pScene->LeaveMap(this, idScene);
        m_pScene = nullptr;

        //判断所在位置是否是障碍
        auto findPos = pScene->FindPosNearby(Vector2(fPosX, fPosY), fRange);
        pScene->EnterMap(this, findPos.x, findPos.y, fFace);
    }
    else
    {
        LOGLOGIN("Player FlyMap:{} {} pos:{:.2f} {:.2f} {:.2f}s", GetID(), idScene.GetMapID(), fPosX, fPosY, fRange);

        //从老地图离开
        m_pScene->LeaveMap(this, idScene);
        m_pScene = nullptr;

        //进入新地图
        m_idLoadingScene = idScene;
        m_fLoadingPosX   = fPosX;
        m_fLoadingPosY   = fPosY;
        m_fLoadingFace   = fFace;

        SC_LOADMAP msg;
        msg.set_mapid(pScene->GetMapID());
        msg.set_posx(m_fLoadingPosX);
        msg.set_posy(m_fLoadingPosY);
        msg.set_face(m_fLoadingFace);

        SendMsg(CMD_SC_LOADMAP, msg);
    }

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

    ZoneService()->DelSocketMessagePool(GetSocket());
    //处理好数据
    LOGLOGIN("Player:{} Logout", GetName().c_str());
    ActorManager()->DelActor(this, false);
    m_pEventOnTimer.Clear();
    ZoneService()->GetLoadingThread()->AddClosePlayer(this, 0, 0.0f, 0.0f, 0.0f, 0.0f);

    __LEAVE_FUNCTION
}

bool CPlayer::Reborn(uint32_t nRebornType)
{
    __ENTER_FUNCTION

    if(IsDead() == false)
        return false;

    if(GetCurrentScene() == nullptr)
        return false;

    LOGLOGIN("Player Reborn:{} {}", GetID(), GetCurrentScene()->GetMapID());

    switch(nRebornType)
    {
        case REBORN_HOME: //回城复活
        {
            GetStatus()->DetachStatusByType(STATUSTYPE_DEAD);
            SetProperty(PROP_HP, GetHPMax() / 2, SYNC_ALL_DELAY);
            FlyMap(SceneID(GetHomeSceneID()).GetMapID(), GetHomePosX(), GetHomePosY(), 0.0f, GetHomeFace());
        }
        break;
        case REBORN_MAPPOS: //复活点复活
        {
            if(GetCurrentScene()->GetMap()->HasMapFlag(MAPFLAG_DISABLE_REBORN_MAPPOS) == true)
                return false;

            const auto& pRebornData = GetCurrentScene()->GetMap()->GetRebornDataByIdx(GetCampID());
            CHECKF_M(pRebornData,
                     fmt::format(FMT_STRING("can't find RebornData In Map {} WithCamp:{}"), GetMapID(), GetCampID())
                         .c_str());

            GetStatus()->DetachStatusByType(STATUSTYPE_DEAD);
            SetProperty(PROP_HP, MulDiv(GetHPMax(), 2, 3), SYNC_ALL_DELAY);
            FlyMap(pRebornData->reborn_map(),
                   pRebornData->reborn_x(),
                   pRebornData->reborn_y(),
                   pRebornData->reborn_range(),
                   pRebornData->reborn_face());
        }
        break;
        case REBORN_STANDPOS: //原地复活
        {
            if(GetCurrentScene()->GetMap()->HasMapFlag(MAPFLAG_DISABLE_REBORN_STANDPOS) == true)
                return false;
            //元宝检查

            GetStatus()->DetachStatusByType(STATUSTYPE_DEAD);
            SetProperty(PROP_HP, GetHPMax(), SYNC_ALL_DELAY);
            FlyMap(GetMapID(), GetPosX(), GetPosY(), 0.0f, GetFace());
        }
        break;
        default:
        {
            return false;
        }
        break;
    }

    return true;
    __LEAVE_FUNCTION
    return false;
}