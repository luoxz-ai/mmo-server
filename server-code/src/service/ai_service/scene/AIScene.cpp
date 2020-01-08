#include "AIService.h"
#include "AIScene.h"
#include "AIActor.h"
#include "NetMSGProcess.h"
CAIScene::CAIScene()
{

}

CAIScene::~CAIScene()
{
	while(m_setActor.empty() == false)
	{
		CAIActor* pActor = static_cast<CAIActor*>(m_setActor.begin()->second);
		LeaveMap(pActor);
		AIActorManager()->DelActor(pActor);
	}
}

bool CAIScene::Init(const SceneID& idScene)
{
	CSceneBase::_Init(idScene, MapManager());
	m_MonsterGen.Init(this);
	return true;
}

CAISceneManager::CAISceneManager()
{

}

CAISceneManager::~CAISceneManager()
{
	Destory();
}

bool CAISceneManager::Init(uint32_t idZone)
{
	return true;
}

void CAISceneManager::Destory()
{
	for (auto& pair_val : m_mapScene)
	{
		LOGDEBUG("AIScene {} Destroy", pair_val.first.GetMapID());
		SAFE_DELETE(pair_val.second);
	}
	m_mapScene.clear();
}

CAIScene* CAISceneManager::CreateScene(CNetworkMessage* pMsg)
{
	ServerMSG::SceneCreate msg;
	if (msg.ParseFromArray(pMsg->GetMsgBody(), pMsg->GetBodySize()) == false)
	{
		return nullptr;
	}
	const SceneID& idScene = msg.scene_id();
	CGameMap* pMap = MapManager()->QueryMap(idScene.GetMapID());
	CHECKF(pMap);
	
	CAIScene* pScene = CAIScene::CreateNew(idScene);
	CHECKF(pScene);

	m_mapScene[idScene] = pScene;

	LOGDEBUG("AIScene {} Created", idScene.GetMapID());
	m_nStaticScene++;
	return pScene;
}

void CAISceneManager::DestoryDynaScene(CNetworkMessage* pMsg)
{
	ServerMSG::SceneDestory msg;
	if (msg.ParseFromArray(pMsg->GetMsgBody(), pMsg->GetBodySize()) == false)
	{
		return;
	}
	const SceneID& idScene = msg.scene_id();

	auto itFind = m_mapScene.find(idScene);
	if (itFind == m_mapScene.end())
		return;

	CAIScene* pScene = itFind->second;
	
	LOGDEBUG("AIScene {} Destroy", itFind->first.GetMapID());

	m_mapScene.erase(itFind);
	SAFE_DELETE(pScene);
}

CAIScene* CAISceneManager::QueryScene(const SceneID& idScene)
{
	auto itFind = m_mapScene.find(idScene);
	if (itFind == m_mapScene.end())
		return nullptr;
	
	CAIScene* pScene = itFind->second;
	return pScene;
}

void CAISceneManager::OnTimer()
{
}
