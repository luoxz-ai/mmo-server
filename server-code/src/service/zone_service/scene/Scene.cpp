#include "Scene.h"

#include "Actor.h"
#include "Monster.h"
#include "Npc.h"
#include "Player.h"
#include "Phase.h"
#include "ZoneService.h"
#include "ActorManager.h"
#include "MapManager.h"

#include "msg/zone_service.pb.h"
#include "server_msg/server_side.pb.h"

CScene::CScene() {}

CScene::~CScene()
{
    __ENTER_FUNCTION
    m_setPhase.clear();
    m_setPhaseByIdx.clear();

    __LEAVE_FUNCTION
}

bool CScene::Init(const SceneID& idScene, uint64_t idMainPhase)
{
    __ENTER_FUNCTION
    auto pMap = MapManager()->QueryMap(idScene.GetMapID());
    CHECKF(pMap);
    m_idSceneID = idScene;
    //通知AI服务器,创建场景
    ServerMSG::SceneCreate msg;
    msg.set_scene_id(idScene);
    ZoneService()->SendMsgToAIService(ServerMSG::MsgID_SceneCreate, msg);

    
    //创建静态位面
    const auto& phaseDataSet = pMap->GetPhaseData();
    for(const auto& [idPhase,v]: phaseDataSet)
    {
        CreatePhase(idPhase);
    }
    
    

    //创建主位面,可能已经创建过了
    CreatePhase(idMainPhase);

    return true;
    __LEAVE_FUNCTION
    return false;
}

CPhase* CScene::CreatePhase(uint64_t idPhase)
{
    auto pMap = MapManager()->QueryMap(m_idSceneID.GetMapID());
    auto pPhaseData = pMap->GetPhaseDataById(idPhase);
    auto pPhase = CreatePhase(idPhase, pPhaseData);
    return pPhase;
}

CPhase* CScene::CreatePhase(uint64_t idPhase, const PhaseData* pPhaseData)
{
    CPhase* pPhase = QueryPhase(idPhase);
    if(pPhase != nullptr)
    {
        return pPhase;
    }
    auto idxPhase = m_DynaIDPool.get();
    SceneID newSceneID(m_idSceneID.GetZoneID(), m_idSceneID.GetMapID(), idxPhase);
    pPhase = CPhase::CreateNew(this, newSceneID, idPhase, pPhaseData);
    CHECKF(pPhase);
    m_setPhase[idPhase].reset(pPhase);
    m_setPhaseByIdx[idxPhase] = pPhase;
    return pPhase;
}

void CScene::ForEach(std::function<void(const CPhase*)> func) const
{
    for(const auto& [k,v]: m_setPhaseByIdx)
    {
        std::invoke(func,v);
    }
}

CPhase* CScene::QueryPhase(uint64_t idPhase) const
{
    auto it = m_setPhase.find(idPhase);
    if(it != m_setPhase.end())
    {
        auto pPhase = it->second.get();
        CHECKF(pPhase);
        if(pPhase->GetSceneState() == SCENESTATE_WAIT_DESTORY)
        {
            return nullptr;
        }
        return pPhase;
    }
    return nullptr;
}

CPhase* CScene::QueryPhaseByIdx(uint32_t idxPhase) const
{
    auto it = m_setPhaseByIdx.find(idxPhase);
    if(it != m_setPhaseByIdx.end())
    {
        auto pPhase = it->second;
        CHECKF(pPhase);
        if(pPhase->GetSceneState() == SCENESTATE_WAIT_DESTORY)
        {
            return nullptr;
        }
        return pPhase;
    }
    return nullptr;
}

bool CScene::DestoryPhase(uint64_t idPhase)
{
    CPhase* pPhase =QueryPhase(idPhase);
    CHECKF(pPhase);
    CHECKF(pPhase->IsStatic() == false);

    pPhase->KickAllPlayer();
    auto idSceneID = pPhase->GetSceneID();
    auto idxPhase = pPhase->GetSceneID().GetPhaseIdx();
    CHECKF(pPhase->CanDestory() == true);
    m_setPhaseByIdx.erase(idxPhase);
    m_setPhase.erase(idPhase);
    
    // send msg to AI
    ServerMSG::PhaseDestory msg;
    msg.set_scene_id(idSceneID);
    msg.set_phase_id(idPhase);
    ZoneService()->SendMsgToAIService(ServerMSG::MsgID_PhaseDestory, msg);
}
