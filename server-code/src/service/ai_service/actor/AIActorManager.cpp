#include "AIActorManager.h"

#include "AIActor.h"
#include "AIPlayer.h"

CAIActorManager::CAIActorManager()
{

}

CAIActorManager::~CAIActorManager()
{
	Destroy();
}

bool CAIActorManager::Init()
{
	m_ActorMap.reserve(GUESS_MAX_ACTOR_COUNT);
	return true;
}

void CAIActorManager::Destroy()
{
	for(auto it = m_ActorMap.begin(); it != m_ActorMap.end(); it++)
	{
		CAIActor* pActor = it->second;
		SAFE_DELETE(pActor);
	}
	m_ActorMap.clear();
}

CAIActor* CAIActorManager::QueryActor(OBJID id) const
{
	auto itFind = m_ActorMap.find(id);
	if(itFind == m_ActorMap.end())
		return nullptr;

	return itFind->second;
}

bool CAIActorManager::AddActor(CAIActor* pActor)
{
	auto itFind = m_ActorMap.find(pActor->GetID());
	if(itFind != m_ActorMap.end())
	{
		// log error

		CAIActor* pOldActor = itFind->second;
		if(pOldActor == pActor)
		{
			return true;
		}
		else
		{
			LOGERROR("Add Actor twice!!!!!!! {} {}", pOldActor->GetID(), pOldActor->GetName().c_str());
			SAFE_DELETE(pOldActor);
		}
	}

	m_ActorMap[pActor->GetID()] = pActor;
	return true;
}

bool CAIActorManager::DelActor(CAIActor* pActor, bool bDelete /* = true*/)
{
	auto itFind = m_ActorMap.find(pActor->GetID());
	if(itFind == m_ActorMap.end())
		return false;

	m_ActorMap.erase(itFind);

	if(bDelete)
		SAFE_DELETE(pActor);
	return true;
}

bool CAIActorManager::DelActorByID(OBJID id, bool bDelete /* = true*/)
{
	auto itFind = m_ActorMap.find(id);
	if(itFind == m_ActorMap.end())
		return false;

	CAIActor* pActor = itFind->second;
	m_ActorMap.erase(itFind);
	if(bDelete)
		SAFE_DELETE(pActor);

	return true;
}

void CAIActorManager::OnTimer() {}
