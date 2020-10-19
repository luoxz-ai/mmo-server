#include "ActorAttrib.h"
#include "ActorManager.h"
#include "ActorStatusSet.h"
#include "GameEventDef.h"
#include "GameMap.h"
#include "GameMapDef.h"
#include "LoadingThread.h"
#include "MapManager.h"
#include "NetMSGProcess.h"
#include "PetSet.h"
#include "Phase.h"
#include "Player.h"
#include "Scene.h"
#include "SceneManager.h"
#include "SceneService.h"
#include "SkillFSM.h"
#include "config/Cfg_Scene_EnterPoint.pb.h"
#include "config/Cfg_Scene_LeavePoint.pb.h"
#include "config/Cfg_Scene_Reborn.pb.h"
#include "gamedb.h"
#include "msg/zone_service.pb.h"
#include "server_msg/server_side.pb.h"

bool CPlayer::TryChangeMap(uint32_t nLeavePointIdx)
{
    __ENTER_FUNCTION
    auto pLeaveData = GetCurrentScene()->GetMap()->GetLeavePointByIdx(nLeavePointIdx);
    CHECKF_FMT(pLeaveData, "Can't Find LeaveMap {} On Map {}", GetMapID(), nLeavePointIdx);

    CHECKF(GetPos().distance(Vector2(pLeaveData->x(), pLeaveData->y())) > pLeaveData->range());

    auto pGameMap = MapManager()->QueryMap(pLeaveData->dest_map_id());
    CHECKF_FMT(pGameMap, "Can't Find Map {} When LeaveMap {} On Map {}", pLeaveData->dest_map_id(), GetMapID(), nLeavePointIdx);

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

    return FlyMap(pEnterData->idmap(), pEnterData->idphase(), pEnterData->x(), pEnterData->y(), pEnterData->range(), pEnterData->face());
    __LEAVE_FUNCTION
    return false;
}

bool CPlayer::FlyMap(uint16_t idMap, int32_t idPhase, float fPosX, float fPosY, float fRange, float fFace)
{
    __ENTER_FUNCTION

    CHECKF(GetCurrentScene());
    if(GetCurrentScene()->GetMap()->HasMapFlag(MAPFLAG_DISABLE_FLYMAP) == true)
        return false;

    //通过原始Mapid查找所在Zone
    const CGameMap* pMap = MapManager()->QueryMap(idMap);
    CHECKF_M(pMap, fmt::format(FMT_STRING("can't find map {}"), idMap).c_str())

    TargetSceneID idTargetScene(pMap->GetZoneID(), idMap, idPhase);

    CScene* pScene = SceneManager()->QueryScene(idMap);
    if(pScene || pMap->IsZoneMap(SceneService()->GetZoneID()))
    {
        //场景在本地
        //延迟调用真正的FlyMap
        CEventEntryCreateParam param;
        param.evType    = EVENTID_FLY_MAP;
        param.cb        = std::bind(&CPlayer::_FlyMap, this, idTargetScene, fPosX, fPosY, fRange, fFace);
        param.tWaitTime = 0;
        param.bPersist  = false;
        EventManager()->ScheduleEvent(param, GetEventMapRef());

        return true;
    }
    else
    {
        //切换zone
        CEventEntryCreateParam param;
        param.evType    = EVENTID_FLY_MAP;
        param.cb        = std::bind(&CPlayer::_ChangeZone, this, idTargetScene, fPosX, fPosY, fRange, fFace);
        param.tWaitTime = 0;
        param.bPersist  = false;
        EventManager()->ScheduleEvent(param, GetEventMapRef());

        return true;
    }

    __LEAVE_FUNCTION
    return false;
}

void CPlayer::_ChangeZone(TargetSceneID idTargetScene, float fPosX, float fPosY, float fRange, float fFace)
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
        m_pScene->LeaveMap(this, idTargetScene.GetMapID());
    }

    //删除
    ActorManager()->DelActor(this, false);
    m_pEventOnTimer.Clear();

    ST_LOADINGTHREAD_PROCESS_DATA data;
    data.nPorcessType  = LPT_SAVE;
    data.idPlayer      = GetID();
    data.bChangeZone   = true;
    data.socket        = GetSocket();
    data.idTargetScene = idTargetScene;
    data.fPosX         = fPosX;
    data.fPosY         = fPosY;
    data.fRange        = 0.0f;
    data.fFace         = fFace;
    data.pPlayer       = this;
    SceneService()->GetLoadingThread()->AddClosePlayer(std::move(data));
    __LEAVE_FUNCTION
}

void CPlayer::OnChangeZoneSaveFinish(const TargetSceneID& idTargetScene, float fPosX, float fPosY, float fRange, float fFace)
{
    __ENTER_FUNCTION
    //已经Save结束
    //发送ChangeZone消息给World， 由World转发该消息给对应的Zone
    ServerMSG::PlayerChangeZone msg;
    msg.set_socket(GetSocket());
    msg.set_idplayer(GetID());
    msg.set_target_scene(idTargetScene);
    msg.set_posx(fPosX);
    msg.set_posy(fPosY);
    msg.set_range(fRange);
    msg.set_face(fFace);

    SceneService()->SendProtoMsgToWorld(GetWorldID(), msg);

    SendGameData(idTargetScene);
    // maybe 需要把当前未处理的消息转发到新zone上
    SceneService()->DelSocketMessagePool(GetSocket());
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

void CPlayer::SendGameData(const TargetSceneID& idTargetScene)
{
    __ENTER_FUNCTION
    ServerMSG::PlayerChangeZone_Data msg;

    //通过World发送
    SceneService()->SendProtoMsgToWorld(GetWorldID(), msg);
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
        m_pRecord->Field(TBLD_PLAYER::RECORD_SCENEID) = pScene->GetSceneIdx().data64;
        m_pRecord->Field(TBLD_PLAYER::RECORD_X)       = GetPosX();
        m_pRecord->Field(TBLD_PLAYER::RECORD_Y)       = GetPosY();
        m_pRecord->Field(TBLD_PLAYER::RECORD_FACE)    = GetFace();
    }
    //进入地图了， 保存一下当前的一些修改
    m_pRecord->Update(true);

    {
        ServerMSG::ActorCreate ai_msg;
        ai_msg.set_actor_id(GetID());
        ai_msg.set_scene_id(GetSceneIdx());
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

        SceneService()->SendProtoMsgToAIService(ai_msg);
    }

    {
        ServerMSG::SyncTaskPhase send;
        send.set_player_id(GetID());
        for(const auto& [k, v]: m_TaskPhase)
        {
            send.add_task_phase_id(k);
        }
        SceneService()->SendProtoMsgToAIService(send);
    }

    LOGLOGIN("CPlayer::OnEnterMapEnd: {} mapid: {}", GetID(), GetSceneIdx());

    {
        SC_ENTERMAP msg;
        msg.set_x(GetPosX());
        msg.set_y(GetPosY());
        msg.set_face(GetFace());
        SendMsg(msg);
    }

    __LEAVE_FUNCTION
}

void CPlayer::OnLeaveMap(uint16_t idTargetMap)
{
    __ENTER_FUNCTION
    CHECK(GetCurrentScene());
    LOGLOGIN("Player OnLeaveMap:{} {}", GetID(), GetCurrentScene()->GetMapID());

    if(GetCurrentScene()->GetMap()->HasMapFlag(MAPFLAG_RECORD_DISABLE) == false)
    {
        m_pRecord->Field(TBLD_PLAYER::RECORD_SCENEID) = GetCurrentScene()->GetSceneIdx().data64;
        m_pRecord->Field(TBLD_PLAYER::RECORD_X)       = GetPosX();
        m_pRecord->Field(TBLD_PLAYER::RECORD_Y)       = GetPosY();
        m_pRecord->Field(TBLD_PLAYER::RECORD_FACE)    = GetFace();
    }

    m_SkillFSM->Stop();
    m_pStatusSet->Stop();
    m_pStatusSet->OnLeaveMap();
    m_pPetSet->CallBack();

    CActor::OnLeaveMap(idTargetMap);
    //从这里开始，Player就没有Scene指针了

    __LEAVE_FUNCTION
}

void CPlayer::_FlyMap(TargetSceneID idTargetScene, float fPosX, float fPosY, float fRange, float fFace)
{
    __ENTER_FUNCTION
    //查询是否有对应地图
    CHECK(GetCurrentScene());
    CPhase* pOldPhase = static_cast<CPhase*>(GetCurrentScene());
    LOGLOGIN("Player FlyMap:{} {} pos:{:.2f} {:.2f} {:.2f}", GetID(), idTargetScene.GetMapID(), fPosX, fPosY, fRange);
    uint64_t idPhase = idTargetScene.GetPhaseID();
    if(idTargetScene.IsSelfPhaseID())
    {
        idPhase = GetID();
    }

    CPhase* pPhase = SceneManager()->CreatePhase(idTargetScene.GetMapID(), idPhase);
    if(pPhase == nullptr)
    {
        // log error
        return;
    }

    pOldPhase->LeaveMap(this, idTargetScene.GetMapID());
    m_pScene = nullptr;

    //只要地图一样就不需要reloading
    if(pOldPhase->GetSceneIdx().GetMapID() == idTargetScene.GetMapID())
    {
        //判断所在位置是否是障碍
        auto findPos = pPhase->FindPosNearby(Vector2(fPosX, fPosY), fRange);
        pPhase->EnterMap(this, findPos.x, findPos.y, fFace);
    }
    else
    {
        //进入新地图
        m_idLoadingScene = pPhase->GetSceneIdx();
        m_fLoadingPosX   = fPosX;
        m_fLoadingPosY   = fPosY;
        m_fLoadingFace   = fFace;

        SC_LOADMAP msg;
        msg.set_scene_idx(pPhase->GetSceneIdx());
        msg.set_mapid(pPhase->GetMapID());
        msg.set_posx(m_fLoadingPosX);
        msg.set_posy(m_fLoadingPosY);
        msg.set_face(m_fLoadingFace);

        SendMsg(msg);
    }

    __LEAVE_FUNCTION
}

bool CPlayer::Reborn(uint32_t nRebornType)
{
    __ENTER_FUNCTION

    if(IsDead() == false)
        return false;

    if(GetCurrentScene() == nullptr)
        return false;

    switch(nRebornType)
    {
        case REBORN_HOME: //回城复活
        {
            LOGLOGIN("Player Reborn HOME:{} {}", GetID(), GetCurrentScene()->GetMapID());
            GetStatus()->DetachStatusByType(STATUSTYPE_DEAD);
            SetProperty(PROP_HP, GetHPMax() / 2, SYNC_ALL_DELAY);
            FlyMap(GetHomeSceneIdx().GetMapID(), 0, GetHomePosX(), GetHomePosY(), 0.0f, GetHomeFace());
        }
        break;
        case REBORN_MAPPOS: //复活点复活
        {
            if(GetCurrentScene()->GetMap()->HasMapFlag(MAPFLAG_DISABLE_REBORN_MAPPOS) == true)
                return false;

            const auto& pRebornData = GetCurrentScene()->GetMap()->GetRebornDataByIdx(GetCampID());
            CHECKF_FMT(pRebornData, "can't find RebornData In Map {} WithCamp:{}", GetMapID(), GetCampID());

            LOGLOGIN("Player Reborn MAPPOS:{} {}", GetID(), GetCurrentScene()->GetMapID());
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
            LOGLOGIN("Player Reborn STANDPOS:{} {}", GetID(), GetCurrentScene()->GetMapID());
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