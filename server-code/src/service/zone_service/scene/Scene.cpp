#include "Scene.h"

#include "Actor.h"
#include "Monster.h"
#include "Npc.h"
#include "Player.h"
#include "ZoneService.h"
CScene::CScene() {}

CScene::~CScene()
{
    __ENTER_FUNCTION
    while(m_setActor.empty() == false)
    {
        CActor* pActor = static_cast<CActor*>(m_setActor.begin()->second);
        LeaveMap(pActor);
        ActorManager()->DelActor(pActor);
    }

    __LEAVE_FUNCTION
}

bool CScene::Init(const SceneID& idScene)
{
    __ENTER_FUNCTION
    CSceneBase::_Init(idScene, MapManager());
    //通知AI服务器,创建场景
    ServerMSG::SceneCreate msg;
    msg.set_scene_id(idScene);
    ZoneService()->SendMsgToAIService(ServerMSG::MsgID_SceneCreate, msg);

    TryExecScript<void>(SCB_MAP_ONCREATE, this);

    //刷新所有的NPC
    auto pVec = NpcTypeSet()->QueryObjByMapID(idScene.GetMapID());
    if(pVec)
    {
        for(auto pNpcType: *pVec)
        {
            CreateNpc(pNpcType->GetID(), CPos2D(pNpcType->GetPosX(), pNpcType->GetPosY()), pNpcType->GetFace());
        }
    }

   

    return true;
    __LEAVE_FUNCTION
    return false;
}

void CScene::Destory()
{
    __ENTER_FUNCTION
    m_bDelThis = true;
    KickAllPlayer();
    __LEAVE_FUNCTION
}

export_lua void CScene::AddDynaRegion(uint32_t nRegionType, const FloatRect& rect)
{
    // todo: send to ai_server
    CSceneBase::AddDynaRegion(nRegionType, rect);
}

export_lua void CScene::ClearDynaRegion(uint32_t nRegionType)
{
    // todo: send to ai_server
    CSceneBase::ClearDynaRegion(nRegionType);
}

bool CScene::SendSceneMessage(uint16_t cmd, const google::protobuf::Message& msg)
{
    __ENTER_FUNCTION
    return ZoneService()->BroadcastMessageToPlayer(m_setPlayer, cmd, msg);
    __LEAVE_FUNCTION
    return false;
}

void CScene::SendAllMapValToClient(CActor* pPlayer)
{
    __ENTER_FUNCTION
    // send map val

    //根据playerid 发送MapUserVal

    __LEAVE_FUNCTION
}

void CScene::ClearAllMapVal()
{
    __ENTER_FUNCTION
    ClearMapVal();
    ClearAllMapUserVal();
    __LEAVE_FUNCTION
}

void CScene::SetMapVal(uint32_t nIdx, int64_t nVal, bool bSync /*= false*/)
{
    __ENTER_FUNCTION
    m_MapVal[nIdx] = nVal;
    if(bSync)
    {
        // send msg
    }
    __LEAVE_FUNCTION
}

int64_t CScene::GetMapVal(uint32_t nIdx)
{
    return m_MapVal[nIdx];
}

int64_t CScene::AddMapVal(uint32_t nIdx, int64_t nVal, bool bSync /*= false*/)
{
    __ENTER_FUNCTION
    auto& refData = m_MapVal[nIdx];
    refData += nVal;
    if(bSync)
    {
        // send msg
    }

    return refData;
    __LEAVE_FUNCTION
    return 0;
}

void CScene::ClearMapVal()
{
    m_MapVal.clear();
}

void CScene::SyncAllMapVal()
{
    __ENTER_FUNCTION

    __LEAVE_FUNCTION
}

void CScene::SetMapUserVal(uint64_t idUser, uint32_t nIdx, int64_t nVal, bool bSync /*= false*/)
{
    __ENTER_FUNCTION
    m_MapUserVal[idUser][nIdx] = nVal;

    if(bSync)
    {
        // send msg
    }
    __LEAVE_FUNCTION
}

int64_t CScene::GetMapUserVal(uint64_t idUser, uint32_t nIdx)
{
    return m_MapUserVal[idUser][nIdx];
}

int64_t CScene::AddMapUserVal(uint64_t idUser, uint32_t nIdx, int64_t nVal, bool bSync /*= false*/)
{
    __ENTER_FUNCTION
    auto& refData = m_MapUserVal[idUser][nIdx];
    refData += nVal;
    if(bSync)
    {
        // send msg
    }

    return refData;
    __LEAVE_FUNCTION
    return 0;
}

void CScene::ClearMapUserVal(uint64_t idUser)
{
    __ENTER_FUNCTION
    auto itFind = m_MapUserVal.find(idUser);
    if(itFind != m_MapUserVal.end())
        m_MapUserVal.erase(itFind);
    __LEAVE_FUNCTION
}

void CScene::ClearAllMapUserVal()
{
    m_MapUserVal.clear();
}

void CScene::AddTimedCallback(uint32_t tIntervalMS, const std::string& func_name)
{
    __ENTER_FUNCTION
    if(m_pMap->GetScriptID() == 0)
        return;

    EventManager()->ScheduleEvent(
        0,
        [pThis = this, _func_name = func_name]() {
            ScriptManager()->ExecScript<void>(pThis->m_pMap->GetScriptID(), _func_name.c_str(), pThis);
        },
        tIntervalMS,
        false,
        m_StatusEventList);
    __LEAVE_FUNCTION
}

void CScene::ClearAllCllback()
{
    m_StatusEventList.Clear();
}

CNpc* CScene::CreateNpc(uint32_t idNpcType, const CPos2D& pos, float face)
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

CMonster* CScene::OnMsgCreateMonster(uint32_t      idMonsterType,
                                uint32_t      idGen,
                                uint32_t      idCamp,
                                OBJID         idOwner,
                                const CPos2D& pos,
                                float         face)
{
    __ENTER_FUNCTION
    CMonster* pMonster = CMonster::CreateNew(idMonsterType, idOwner, idGen, idCamp);
    if(pMonster)
    {
        EnterMap(pMonster, pos.x, pos.y, face);
        ActorManager()->AddActor(pMonster);
    }

    return pMonster;
    __LEAVE_FUNCTION
    return nullptr;
}

CMonster* CScene::CreateMonster(uint32_t      idMonsterType,
                                uint32_t      idGen,
                                uint32_t      idCamp,
                                OBJID         idOwner,
                                const CPos2D& pos,
                                float         face)
{
    __ENTER_FUNCTION
    CMonster* pMonster = CMonster::CreateNew(idMonsterType, idOwner, idGen, idCamp);
    if(pMonster)
    {
        //notify ai

        EnterMap(pMonster, pos.x, pos.y, face);
        ActorManager()->AddActor(pMonster);
    }

    return pMonster;
    __LEAVE_FUNCTION
    return nullptr;
}


bool CScene::CreateMultiMonster(uint32_t      idMonsterType,
                                uint32_t      nNum,
                                uint32_t      idCamp,
                                OBJID         idOwner,
                                const CPos2D& pos,
                                float         range)
{
    __ENTER_FUNCTION
    for(size_t i = 0; i < nNum; i++)
    {
        CPos2D newPos = pos + CPos2D::UNIT_X.randomDeviant(1.0f) * random_float(0.0f, range);
        CreateMonster(idMonsterType, 0, idCamp, idOwner, newPos, random_float());
    }

    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CScene::OnMsgCreateMultiMonster(uint32_t      idMonsterType,
                                uint32_t      nNum,
                                uint32_t      idCamp,
                                OBJID         idOwner,
                                const CPos2D& pos,
                                float         range)
{
    __ENTER_FUNCTION
    for(size_t i = 0; i < nNum; i++)
    {
        CPos2D newPos = pos + CPos2D::UNIT_X.randomDeviant(1.0f) * random_float(0.0f, range);
        CreateMonster(idMonsterType, 0, idCamp, idOwner, newPos, random_float());
    }

    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CScene::CanDestory()
{
    return m_setPlayer.empty() == true;
}

void CScene::KickAllPlayer(const char* pszReason)
{
    while(m_setPlayer.empty() == false)
    {
        auto     it      = m_setPlayer.begin();
        CPlayer* pPlayer = it->second->ConvertToDerived<CPlayer>();
        m_setPlayer.erase(it);
        if(pPlayer)
        {
            _KickPlayer(pszReason, pPlayer);
        }
    }
}

void CScene::_KickPlayer(const char* pszReason, CPlayer* pPlayer)
{
    CHECK(pPlayer);

    if(pszReason)
        pPlayer->SendTalkMsg(CHANNEL_SYSTEM, pszReason);

    // 检查地图复活点是否是本地图
    const auto& refRobronDataSet = m_pMap->GetRebornData();
    for(const auto& [k, refRebornData]: refRobronDataSet)
    {
        uint16_t idNewMap = refRebornData.reborn_map();
        if(idNewMap != 0 && idNewMap != GetID())
        {
            pPlayer->FlyMap(idNewMap,
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
                        pPlayer->GetHomePosX(),
                        pPlayer->GetHomePosY(),
                        0.0f,
                        pPlayer->GetHomeFace());
        return;
    }
    else if(pPlayer->GetRecordSceneID() != GetSceneID())
    {
        //如果Home记录点不是本地图
        pPlayer->FlyMap(SceneID(pPlayer->GetRecordSceneID()).GetMapID(),
                        pPlayer->GetRecordPosX(),
                        pPlayer->GetRecordPosY(),
                        0.0f,
                        pPlayer->GetRecordFace());
        return;
    }
    else
    {
        LOGWARNING("Error User:{} HomeMap/RebornMap is ThisMap{}. In CScene::KickAllPlayer",
                   pPlayer->GetID(),
                   GetMapID());
        pPlayer->OnLogout();
    }
}

bool CScene::NeedDestory() const
{
    return m_bDelThis;
}
