#include "Phase.h"

#include "Actor.h"
#include "ActorManager.h"
#include "EventManager.h"
#include "GameEventDef.h"
#include "LoadingThread.h"
#include "Monster.h"
#include "Npc.h"
#include "Player.h"
#include "Scene.h"
#include "SceneManager.h"
#include "ScriptManager.h"
#include "ZoneService.h"
#include "msg/zone_service.pb.h"
#include "server_msg/server_side.pb.h"

OBJECTHEAP_IMPLEMENTATION(CPhase, s_heap);

constexpr int WAIT_PLAYER_LOADING_MS = 5 * 60 * 1000;
constexpr int WAIT_DESTORY_MS        = 1 * 60 * 1000;

CPhase::CPhase() {}

CPhase::~CPhase()
{
    __ENTER_FUNCTION
    LOGDEBUG("PhaseDestory:{} {} idPhase:{}", GetSceneID().GetMapID(), GetSceneID().GetPhaseIdx(), m_idPhase);
    while(m_setActor.empty() == false)
    {
        CActor* pActor = static_cast<CActor*>(m_setActor.begin()->second);
        LeaveMap(pActor);
        ActorManager()->DelActor(pActor);
    }

    __LEAVE_FUNCTION
}

bool CPhase::Init(CScene* pScene, const SceneID& idScene, uint64_t idPhase, const PhaseData* pPhaseData)
{
    __ENTER_FUNCTION
    m_pScene = pScene;
    m_pMapValSet.reset(CMapValSet::CreateNew(this));
    CSceneBase::Init(idScene, MapManager());
    if(pPhaseData)
    {
        uint64_t idPhaseLink = pPhaseData->link_phase();
        auto     pPhase      = pScene->QueryPhase(idPhaseLink);
        if(pPhase)
        {
            LinkSceneTree(pPhase);
        }
        else
        {
            CPos2D vBasePos{pPhaseData->left(), pPhaseData->top()};
            float  fWidth  = pPhaseData->right() - pPhaseData->left();
            float  fHeight = pPhaseData->bottom() - pPhaseData->top();
            InitSceneTree(vBasePos, fWidth, fHeight, pPhaseData->viewgrid_width());
        }
    }
    else
    {
        InitSceneTree({0.0f, 0.0f}, 0.0f, 0.0f, 0);
    }

    //通知AI服务器,创建场景
    // Monster会由AIServer来创建
    ServerMSG::PhaseCreate msg;
    msg.set_scene_id(idScene);
    msg.set_phase_id(idPhase);
    ZoneService()->SendServerMsgToAIService(msg);

    TryExecScript<void>(SCB_MAP_ONCREATE, this);

    //刷新所有的NPC
    auto pVec = NpcTypeSet()->QueryObjByMapID(idScene.GetMapID(), idPhase);
    if(pVec)
    {
        for(auto pNpcType: *pVec)
        {
            CreateNpc(pNpcType->GetID(), CPos2D(pNpcType->GetPosX(), pNpcType->GetPosY()), pNpcType->GetFace());
        }
    }

    if(IsStatic())
    {
        SetSceneState(SCENESTATE_NORMAL);
    }
    else
    {
        SetSceneState(SCENESTATE_WAIT_LOADING);
        ScheduleDelPhase(WAIT_PLAYER_LOADING_MS);
    }

    LOGDEBUG("Phase {} Created, Map:{} Idx:{}", idPhase, idScene.GetMapID(), idScene.GetPhaseIdx());

    return true;
    __LEAVE_FUNCTION
    return false;
}

export_lua void CPhase::AddDynaRegion(uint32_t nRegionType, const FloatRect& rect)
{
    // todo: send to ai_server
    CSceneBase::AddDynaRegion(nRegionType, rect);
}

export_lua void CPhase::ClearDynaRegion(uint32_t nRegionType)
{
    // todo: send to ai_server
    CSceneBase::ClearDynaRegion(nRegionType);
}

bool CPhase::SendSceneMessage(const google::protobuf::Message& msg) const
{
    return SendSceneMessage(to_sc_cmd(msg), msg);
}

bool CPhase::SendSceneMessage(uint16_t cmd, const google::protobuf::Message& msg) const
{
    __ENTER_FUNCTION
    auto setSocketMap = ZoneService()->IDList2VSMap(m_setPlayer, 0);
    return ZoneService()->SendMsgTo(setSocketMap, cmd, msg);
    __LEAVE_FUNCTION
    return false;
}

void CPhase::AddTimedCallback(uint32_t tIntervalMS, const std::string& func_name)
{
    __ENTER_FUNCTION
    if(m_pMap->GetScriptID() == 0)
        return;
    
    CEventEntryCreateParam param;
    param.evType = 0;
    param.cb     =  [pThis = this, _func_name = func_name]() {
            ScriptManager()->ExecScript<void>(pThis->m_pMap->GetScriptID(), _func_name.c_str(), pThis);
        };
    param.tWaitTime = tIntervalMS;
    param.bPersist  = false;
    EventManager()->ScheduleEvent(param, m_StatusEventList);
    __LEAVE_FUNCTION
}

void CPhase::ClearAllCllback()
{
    m_StatusEventList.Clear();
}

CNpc* CPhase::CreateNpc(uint32_t idNpcType, const CPos2D& pos, float face)
{
    __ENTER_FUNCTION
    CNpc* pNpc = CNpc::CreateNew(idNpcType);
    if(pNpc)
    {
        EnterMap(pNpc, pos.x, pos.y, face);
        ActorManager()->AddActor(pNpc);
    }

    return pNpc;
    __LEAVE_FUNCTION
    return nullptr;
}

CMonster* CPhase::CreateMonster(const CreateMonsterParam& param)
{
    __ENTER_FUNCTION
    CMonster* pMonster =
        CMonster::CreateNew(param.idMonsterType, param.idOwner, param.idGen, param.idPhase, param.idCamp);
    if(pMonster)
    {
        // notify ai

        EnterMap(pMonster, param.pos.x, param.pos.y, param.face);
        ActorManager()->AddActor(pMonster);
    }

    return pMonster;
    __LEAVE_FUNCTION
    return nullptr;
}

bool CPhase::CreateMultiMonster(const CreateMonsterParam& param, uint32_t nNum, float range)
{
    __ENTER_FUNCTION
    for(size_t i = 0; i < nNum; i++)
    {
        CPos2D             newPos   = param.pos + CPos2D::UNIT_X.randomDeviant(1.0f) * random_float(0.0f, range);
        CreateMonsterParam newParam = param;
        newParam.pos                = newPos;
        newParam.face               = random_float();
        CreateMonster(newParam);
    }

    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CPhase::CanDestory()
{
    return m_setPlayer.empty() == true;
}

void CPhase::KickAllPlayer(const char* pszReason)
{
    while(m_setPlayer.empty() == false)
    {
        auto     it      = m_setPlayer.begin();
        CPlayer* pPlayer = it->second->CastTo<CPlayer>();
        m_setPlayer.erase(it);
        if(pPlayer)
        {
            _KickPlayer(pszReason, pPlayer);
        }
    }
}

void CPhase::_KickPlayer(const char* pszReason, CPlayer* pPlayer)
{
    CHECK(pPlayer);

    if(pszReason)
        pPlayer->SendTalkMsg(CHANNEL_SYSTEM, pszReason);

    // 找到不是本地图的复活点
    const auto& refRobronDataSet = m_pMap->GetRebornData();
    for(const auto& [k, refRebornData]: refRobronDataSet)
    {
        uint16_t idNewMap = refRebornData.reborn_map();
        if(idNewMap != 0 && idNewMap != GetID())
        {
            pPlayer->FlyMap(idNewMap,
                            0,
                            refRebornData.reborn_x(),
                            refRebornData.reborn_y(),
                            refRebornData.reborn_range(),
                            refRebornData.reborn_face());
            return;
        }
    }

    if(pPlayer->GetHomeSceneID() != GetSceneID())
    {
        //如果Home记录点不是本地图
        pPlayer->FlyMap(SceneID(pPlayer->GetHomeSceneID()).GetMapID(),
                        0,
                        pPlayer->GetHomePosX(),
                        pPlayer->GetHomePosY(),
                        0.0f,
                        pPlayer->GetHomeFace());
        return;
    }
    else if(pPlayer->GetRecordSceneID() != GetSceneID())
    {
        //如果Record记录点不是本地图
        pPlayer->FlyMap(SceneID(pPlayer->GetRecordSceneID()).GetMapID(),
                        0,
                        pPlayer->GetRecordPosX(),
                        pPlayer->GetRecordPosY(),
                        0.0f,
                        pPlayer->GetRecordFace());
        return;
    }
    else
    {
        LOGWARNING("Error User:{} HomeMap/RebornMap is ThisMap{}. In CPhase::KickAllPlayer",
                   pPlayer->GetID(),
                   GetMapID());
        pPlayer->OnLogout();
    }
}

bool CPhase::NeedDestory() const
{
    return m_bDelThis;
}

void CPhase::AddOwnerID(OBJID idOwner)
{
    m_OwnerIDSet[idOwner] = false;
}

void CPhase::SetOwnerID(const std::vector<OBJID>& setPlayerID)
{
    for(auto id: setPlayerID)
    {
        m_OwnerIDSet[id] = false;
    }
}

void CPhase::ClearOwner(OBJID idOwner)
{
    if(idOwner == 0)
        m_OwnerIDSet.clear();
    else
        m_OwnerIDSet.erase(idOwner);
}

bool CPhase::IsOwner(OBJID idOwner) const
{
    return m_OwnerIDSet.find(idOwner) != m_OwnerIDSet.end();
}

bool CPhase::EnterMap(CSceneObject* pActor, float fPosX, float fPosY, float fFace)
{
    if(CSceneBase::EnterMap(pActor, fPosX, fPosY, fFace) == false)
    {
        return false;
    }
    pActor->_SetPhaseID(m_idPhase);

    if(IsStatic() == false)
    {
        SetSceneState(SCENESTATE_NORMAL);
        m_OwnerIDSet[pActor->GetID()] = true;
    }

    return true;
}

void CPhase::ScheduleDelPhase(uint32_t wait_ms)
{
    auto del_func = [sceneID = GetSceneID(), idPhaseID = m_idPhase]() {
        auto pScene = SceneManager()->_QueryScene(sceneID);
        if(pScene)
        {
            pScene->DestoryPhase(idPhaseID);
        }
    };

    CEventEntryCreateParam param;
    param.evType = 0;
    param.cb     =  std::move(del_func);
    param.tWaitTime = wait_ms;
    param.bPersist  = false;

    EventManager()->ScheduleEvent(param, m_DelEvent);
}

void CPhase::LeaveMap(CSceneObject* pActor, uint64_t idTargetScene /*= 0*/)
{
    CSceneBase::LeaveMap(pActor, idTargetScene);
    pActor->_SetPhaseID(0);
    if(IsStatic())
        return;

    if(GetPlayerCount() == 0)
    {
        if(idTargetScene == 0)
        {
            //等待玩家短线重连
            SetSceneState(SCENESTATE_WAIT_LOADING);
            ScheduleDelPhase(WAIT_DESTORY_MS);
        }
        else
        {
            //真的需要摧毁了
            SetSceneState(SCENESTATE_WAIT_DESTORY);
            ScheduleDelPhase(0);
        }
    }
}
