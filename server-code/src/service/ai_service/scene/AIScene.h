#pragma once

#include "SceneBase.h"
#include "NetworkMessage.h"
#include "MonsterGenerator.h"


class CAIScene : public CSceneBase
{
protected:
	CAIScene();
public:
	virtual ~CAIScene();
	CREATE_NEW_IMPL(CAIScene);
public:
	bool Init(const SceneID& idScene);
	CMonsterGenerator& GetMonsterGen() {return m_MonsterGen;}
private:
	CMonsterGenerator m_MonsterGen;
};


class CAISceneManager
{
public:
	CAISceneManager();
	~CAISceneManager();

	bool Init(uint32_t idZone);
	void Destory();
	void RegisterMessageHandler();

	CAIScene* CreateScene(CNetworkMessage* pMsg);
	void DestoryDynaScene(CNetworkMessage* pMsg);


	CAIScene* QueryScene(const SceneID& idScene);
	void OnTimer();
protected:
private:
	std::unordered_map<SceneID, CAIScene*> m_mapScene;
	size_t m_nStaticScene;
};