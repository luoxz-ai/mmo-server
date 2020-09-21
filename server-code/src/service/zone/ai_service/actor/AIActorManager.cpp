#include "AIActorManager.h"

#include "AIActor.h"
#include "AIPhase.h"
#include "AIPlayer.h"

CAIActorManager::CAIActorManager() {}

CAIActorManager::~CAIActorManager()
{
    Destroy();
}

bool CAIActorManager::Init()
{
    __ENTER_FUNCTION
    m_ActorMap.reserve(GUESS_MAX_ACTOR_COUNT);
    return true;

    __LEAVE_FUNCTION
    return false;
}

void CAIActorManager::Destroy()
{
    __ENTER_FUNCTION
    for(auto it = m_ActorMap.begin(); it != m_ActorMap.end(); it++)
    {
        CAIActor* pActor = it->second;
        SAFE_DELETE(pActor);
    }
    m_ActorMap.clear();

    __LEAVE_FUNCTION
}

CAIActor* CAIActorManager::QueryActor(OBJID id) const
{
    __ENTER_FUNCTION
    auto itFind = m_ActorMap.find(id);
    if(itFind == m_ActorMap.end())
        return nullptr;

    return itFind->second;

    __LEAVE_FUNCTION
    return nullptr;
}

bool CAIActorManager::AddActor(CAIActor* pActor)
{
    __ENTER_FUNCTION
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
            if(pOldActor->GetCurrentScene())
            {
                pOldActor->GetCurrentScene()->LeaveMap(pOldActor);
            }
            SAFE_DELETE(pOldActor);
        }
    }

    m_ActorMap[pActor->GetID()] = pActor;
    return true;

    __LEAVE_FUNCTION
    return false;
}

bool CAIActorManager::DelActor(CAIActor* pActor, bool bDelete /* = true*/)
{
    __ENTER_FUNCTION
    auto itFind = m_ActorMap.find(pActor->GetID());
    if(itFind == m_ActorMap.end())
        return false;

    m_ActorMap.erase(itFind);

    if(bDelete)
        SAFE_DELETE(pActor);
    return true;

    __LEAVE_FUNCTION
    return false;
}

bool CAIActorManager::DelActorByID(OBJID id, bool bDelete /* = true*/)
{
    __ENTER_FUNCTION
    auto itFind = m_ActorMap.find(id);
    if(itFind == m_ActorMap.end())
        return false;

    CAIActor* pActor = itFind->second;
    m_ActorMap.erase(itFind);
    if(bDelete)
        SAFE_DELETE(pActor);

    return true;

    __LEAVE_FUNCTION
    return false;
}

void CAIActorManager::OnTimer() {}
