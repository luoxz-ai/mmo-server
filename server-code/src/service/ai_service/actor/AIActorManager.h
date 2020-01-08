#pragma once

#include "BaseCode.h"
#include "game_common_def.h"
#include "NetworkDefine.h"

class CAIActor;
class CAIMonster;
class CAIPlayer;

class CAIActorManager
{
public:
	CAIActorManager();
	~CAIActorManager();

	void Destroy();

	CAIActor* QueryActor(OBJID id) const;
	bool AddActor(CAIActor* pActor);

	bool DelActor(CAIActor* pActor, bool bDelete = true);
	bool DelActorByID(OBJID id, bool bDelete = true);

	void OnTimer();

protected:
private:
	std::unordered_map<OBJID, CAIActor*> m_ActorMap;
};