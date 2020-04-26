#include "ActorManager.h"
#include "GameEventDef.h"
#include "LoadingThread.h"
#include "MapManager.h"
#include "NetMSGProcess.h"
#include "Phase.h"
#include "Player.h"
#include "Scene.h"
#include "SceneManager.h"
#include "ZoneService.h"
#include "server_msg/server_side.pb.h"

bool CPlayer::FlyMap(uint16_t idMap, uint32_t idxPhase, float fPosX, float fPosY, float fRange, float fFace)
{
    __ENTER_FUNCTION

    CHECKF(GetCurrentScene());
    if(GetCurrentScene()->GetMap()->HasMapFlag(MAPFLAG_DISABLE_FLYMAP) == true)
        return false;

    //通过原始Mapid查找所在Zone
    const CGameMap* pMap = MapManager()->QueryMap(idMap);
    CHECKF_M(pMap, fmt::format(FMT_STRING("can't find map {}"), idMap).c_str())

    SceneID newSceneID(ZoneService()->GetZoneID(), idMap, idxPhase);

    CScene* pScene = SceneManager()->QueryScene(idMap);
    if(pScene || pMap->IsZoneMap(ZoneService()->GetZoneID()))
    {
        //场景在本地
        //延迟调用真正的FlyMap
        EventManager()->ScheduleEvent(EVENTID_FLYMAP,
                                      std::bind(&CPlayer::_FlyMap, this, newSceneID, fPosX, fPosY, fRange, fFace),
                                      0,
                                      false,
                                      GetEventMapRef());

        return true;
    }
    else
    {
        //切换zone
        EventManager()->ScheduleEvent(EVENTID_FLYMAP,
                                      std::bind(&CPlayer::_ChangeZone, this, newSceneID, fPosX, fPosY, fRange, fFace),
                                      0,
                                      false,
                                      GetEventMapRef());

        return true;
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

    ST_LOADINGTHREAD_PROCESS_DATA data;
    data.nPorcessType = LPT_SAVE;
    data.idPlayer     = GetID();
    data.bChangeZone  = true;
    data.socket       = GetSocket();
    data.idScene      = idScene;
    data.fPosX        = fPosX;
    data.fPosY        = fPosY;
    data.fRange       = 0.0f;
    data.fFace        = fFace;
    data.pPlayer      = this;
    ZoneService()->GetLoadingThread()->AddClosePlayer(std::move(data));
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

    ZoneService()->SendMsgToWorld(GetWorldID(), to_server_msgid(msg), msg);

    SendGameData(idScene);
    ZoneService()->DelSocketMessagePool(GetSocket());
    __LEAVE_FUNCTION
}

void CPlayer::OnLoadMapSucc()
{
    __ENTER_FUNCTION
    CHECK(m_idLoadingScene != 0);
    CHECK(GetCurrentScene() == nullptr);
    auto pPhase = SceneManager()->QueryPhase(m_idLoadingScene);
    CHECK(pPhase);
    LOGLOGIN("CPlayer::OnLoadMapSucc: {}", GetID());

    pPhase->EnterMap(this, m_fLoadingPosX, m_fLoadingPosY, m_fLoadingFace);
    __LEAVE_FUNCTION
}

void CPlayer::SendGameData(const SceneID& idScene)
{
    __ENTER_FUNCTION
    ServerMSG::PlayerChangeZone_Data msg;

    //通过World发送
    ZoneService()->SendMsgToWorld(GetWorldID(), to_server_msgid(msg), msg);
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
    ai_msg.set_phase_id(GetPhaseID());
    ai_msg.set_name(GetName());
    ai_msg.set_hp(GetHP());
    ai_msg.set_hpmax(GetHPMax());
    ai_msg.set_mp(GetMP());
    ai_msg.set_mpmax(GetMPMax());
    ai_msg.set_movespd(GetAttrib().get(ATTRIB_MOVESPD));
    ai_msg.set_posx(GetPosX());
    ai_msg.set_posy(GetPosY());

    ZoneService()->SendServerMsgToAIService(ai_msg);

    {
        ServerMSG::SyncTaskPhase send;
        send.set_player_id(GetID());
        for(const auto& [k, v]: m_TaskPhase)
        {
            send.add_task_phase_id(k);
        }
        ZoneService()->SendServerMsgToAIService(ai_msg);
    }

    LOGLOGIN("CPlayer::OnEnterMapEnd: {} mapid: {}", GetID(), GetSceneID());

    SC_ENTERMAP msg;
    msg.set_x(GetPosX());
    msg.set_y(GetPosY());
    msg.set_face(GetFace());
    SendMsg(msg);

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

    //只要地图一样就不需要reloading
    if(m_pScene->GetSceneID().GetMapID() == idScene.GetMapID())
    {
        LOGLOGIN("Player FlySamePhase:{} {} pos:{:.2f} {:.2f} {:.2f}s",
                 GetID(),
                 idScene.GetMapID(),
                 fPosX,
                 fPosY,
                 fRange);
        CPhase* pPhase = nullptr;
        if(idScene.IsPhaseIdxVaild())
        {
            pPhase = SceneManager()->QueryPhase(idScene);
        }
        else
        {
            pPhase = SceneManager()->CreatePhase(idScene.GetMapID(), GetID());
        }
        if(pPhase == nullptr)
        {
            // log error
            return;
        }

        m_pScene->LeaveMap(this, idScene);
        m_pScene = nullptr;

        //判断所在位置是否是障碍
        auto findPos = pPhase->FindPosNearby(Vector2(fPosX, fPosY), fRange);
        pPhase->EnterMap(this, findPos.x, findPos.y, fFace);
    }
    else
    {
        LOGLOGIN("Player FlyMap:{} {} pos:{:.2f} {:.2f} {:.2f}s", GetID(), idScene.GetMapID(), fPosX, fPosY, fRange);
        SceneID newSceneID = idScene;
        CPhase* pPhase     = nullptr;
        if(idScene.IsPhaseIdxVaild() == false)
        {
            //尝试自动创建地图
            pPhase = SceneManager()->CreatePhase(idScene.GetMapID(), GetID());
            CHECK(pPhase);

            newSceneID = pPhase->GetSceneID();
        }
        else
        {
            pPhase = SceneManager()->QueryPhase(idScene);
            CHECK(pPhase);
        }

        //从老地图离开
        m_pScene->LeaveMap(this, idScene);
        m_pScene = nullptr;

        //进入新地图
        m_idLoadingScene = idScene;
        m_fLoadingPosX   = fPosX;
        m_fLoadingPosY   = fPosY;
        m_fLoadingFace   = fFace;

        SC_LOADMAP msg;
        msg.set_mapid(pPhase->GetMapID());
        msg.set_posx(m_fLoadingPosX);
        msg.set_posy(m_fLoadingPosY);
        msg.set_face(m_fLoadingFace);

        SendMsg(msg);
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

    ST_LOADINGTHREAD_PROCESS_DATA data;
    data.nPorcessType = LPT_SAVE;
    data.idPlayer     = GetID();
    data.bChangeZone  = false;
    data.socket       = GetSocket();
    data.idScene      = 0;
    data.fPosX        = 0.0f;
    data.fPosY        = 0.0f;
    data.fRange       = 0.0f;
    data.fFace        = 0.0f;
    data.pPlayer      = this;
    ZoneService()->GetLoadingThread()->AddClosePlayer(std::move(data));

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
            FlyMap(SceneID(GetHomeSceneID()).GetMapID(), 0, GetHomePosX(), GetHomePosY(), 0.0f, GetHomeFace());
        }
        break;
        case REBORN_MAPPOS: //复活点复活
        {
            if(GetCurrentScene()->GetMap()->HasMapFlag(MAPFLAG_DISABLE_REBORN_MAPPOS) == true)
                return false;

            const auto& pRebornData = GetCurrentScene()->GetMap()->GetRebornDataByIdx(GetCampID());
            CHECKF_FMT(pRebornData, "can't find RebornData In Map {} WithCamp:{}", GetMapID(), GetCampID());

            GetStatus()->DetachStatusByType(STATUSTYPE_DEAD);
            SetProperty(PROP_HP, MulDiv(GetHPMax(), 2, 3), SYNC_ALL_DELAY);
            if(pRebornData->reborn_this_phase() == true)
            {
                FlyMap(pRebornData->reborn_map(),
                       static_cast<CPhase*>(m_pScene)->GetPhaseIdx(),
                       pRebornData->reborn_x(),
                       pRebornData->reborn_y(),
                       pRebornData->reborn_range(),
                       pRebornData->reborn_face());
            }
            else
            {
                FlyMap(pRebornData->reborn_map(),
                       0,
                       pRebornData->reborn_x(),
                       pRebornData->reborn_y(),
                       pRebornData->reborn_range(),
                       pRebornData->reborn_face());
            }
        }
        break;
        case REBORN_STANDPOS: //原地复活
        {
            if(GetCurrentScene()->GetMap()->HasMapFlag(MAPFLAG_DISABLE_REBORN_STANDPOS) == true)
                return false;
            //元宝检查

            GetStatus()->DetachStatusByType(STATUSTYPE_DEAD);
            SetProperty(PROP_HP, GetHPMax(), SYNC_ALL_DELAY);
            FlyMap(GetMapID(), static_cast<CPhase*>(m_pScene)->GetPhaseIdx(), GetPosX(), GetPosY(), 0.0f, GetFace());
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