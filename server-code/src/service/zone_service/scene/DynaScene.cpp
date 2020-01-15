#include "DynaScene.h"

#include "Actor.h"
#include "ZoneService.h"
CDynaScene::CDynaScene() {}

CDynaScene::~CDynaScene() {}
// 5分钟后摧毁
static const uint32_t WAIT_DESTORY_MS = 5 * 60 * 1000;
//给玩家10分钟的loading时间
static const uint32_t WAIT_PLAYER_LOADING_MS = 10 * 60 * 1000;

bool CDynaScene::Init(const SceneID& idScene)
{
	CScene::Init(idScene);

	SetSceneState(SCENESTATE_WAIT_LOADING);
	EventManager()->ScheduleEvent(0, std::bind(&CDynaScene::_DestoryThisScene, this), WAIT_PLAYER_LOADING_MS, false, m_DelEvent);
	return false;
}

void CDynaScene::AddOwnerID(OBJID idOwner)
{
	m_OwnerIDSet[idOwner] = false;
}

void CDynaScene::SetOwnerID(const std::vector<OBJID>& setPlayerID)
{
	for(auto id: setPlayerID)
	{
		m_OwnerIDSet[id] = false;
	}
}

void CDynaScene::ClearOwner(OBJID idOwner)
{
	if(idOwner == 0)
		m_OwnerIDSet.clear();
	else
		m_OwnerIDSet.erase(idOwner);
}

bool CDynaScene::IsOwner(OBJID idOwner) const
{
	return m_OwnerIDSet.find(idOwner) != m_OwnerIDSet.end();
}

void CDynaScene::_DestoryThisScene()
{
	SceneID sceneID = GetSceneID();
	SceneManager()->DestoryDynaScene(sceneID);
}

void CDynaScene::EnterMap(CSceneObject* pActor, float fPosX, float fPosY, float fFace)
{
	CScene::EnterMap(pActor, fPosX, fPosY, fFace);
	SetSceneState(SCENESTATE_NORMAL);
	m_OwnerIDSet[pActor->GetID()] = true;
}

void CDynaScene::LeaveMap(CSceneObject* pActor, uint64_t idTargetScene /*= 0*/)
{
	CScene::LeaveMap(pActor, idTargetScene);
	// m_OwnerIDSet[pActor->GetID()] = false;
	if(GetPlayerCount() == 0)
	{
		if(idTargetScene == 0)
		{
			//等待玩家短线重连
			SetSceneState(SCENESTATE_WAIT_LOADING);
			EventManager()->ScheduleEvent(0, std::bind(&CDynaScene::_DestoryThisScene, this), WAIT_DESTORY_MS, false, m_DelEvent);
		}
		else
		{
			//真的需要摧毁了
			SetSceneState(SCENESTATE_WAIT_DESTORY);
			EventManager()->ScheduleEvent(0, std::bind(&CDynaScene::_DestoryThisScene, this), 0, false, m_DelEvent);
		}
	}
}
