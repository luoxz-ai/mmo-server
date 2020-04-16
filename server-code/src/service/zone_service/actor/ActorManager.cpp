#include "ActorManager.h"

#include "Actor.h"
#include "Player.h"

constexpr OBJID ID_GEN_FACTOR = 100000000000000ull;
CActorManager::CActorManager()
{
}

CActorManager::~CActorManager()
{
    Destory();
}

bool CActorManager::Init()
{
    std::fill(m_ActorCount.begin(), m_ActorCount.end(), 0);
    m_ActorMap.reserve(GUESS_MAX_ACTOR_COUNT);
    m_PlayerRefMap.reserve(GUESS_MAX_PLAYER_COUNT);

    m_idPool[ACT_NPC].start(ACT_NPC * ID_GEN_FACTOR);
    m_idPool[ACT_MONSTER].start(ACT_MONSTER * ID_GEN_FACTOR);
    m_idPool[ACT_MAPITEM].start(ACT_MAPITEM * ID_GEN_FACTOR);
    m_idPool[ACT_BULLET].start(ACT_BULLET * ID_GEN_FACTOR);
    return true;
}

void CActorManager::Destory()
{
    for(auto it = m_PlayerRefMap.begin(); it != m_PlayerRefMap.end(); it++)
    {
        CPlayer* pPlayer = it->second;
        pPlayer->SaveInfo();
    }
    for(auto it = m_ActorMap.begin(); it != m_ActorMap.end(); it++)
    {
        CActor* pActor = it->second;
        SAFE_DELETE(pActor);
    }
    m_ActorMap.clear();
    m_PlayerRefMap.clear();
}

CActor* CActorManager::QueryActor(OBJID id) const
{
    auto itFind = m_ActorMap.find(id);
    if(itFind == m_ActorMap.end())
        return nullptr;

    if(itFind->second->IsDelThis())
        return nullptr;
    return itFind->second;
}

CPlayer* CActorManager::QueryPlayer(const VirtualSocket& vs) const
{
    auto it = m_PlayerRefMap.find(vs);
    if(it == m_PlayerRefMap.end())
        return nullptr;
    return it->second;
}

bool CActorManager::AddActor(CActor* pActor)
{
    auto itFind = m_ActorMap.find(pActor->GetID());
    if(itFind != m_ActorMap.end())
    {
        // log error

        CActor* pOldActor = itFind->second;
        if(pOldActor == pActor)
        {
            return true;
        }
        else
        {
            LOGERROR("Add Actor twice!!!!!!! {} {}", pOldActor->GetID(), pOldActor->GetName().c_str());
            if(pOldActor->GetActorType() == ACT_PLAYER)
            {

                // log error twice
                CPlayer* pOldPlayer = pOldActor->CastTo<CPlayer>();
                auto     it         = m_PlayerRefMap.find(pOldPlayer->GetSocket());
                if(it != m_PlayerRefMap.end())
                {
                    m_PlayerRefMap.erase(it);
                }
            }

            SAFE_DELETE(pOldActor);
        }
    }

    m_ActorMap[pActor->GetID()] = pActor;
    m_ActorCount[pActor->GetActorType()]++;
    if(pActor->IsPlayer())
    {
        CPlayer* pPlayer                     = pActor->CastTo<CPlayer>();
        m_PlayerRefMap[pPlayer->GetSocket()] = pPlayer;
    }

    return true;
}

bool CActorManager::DelActor(CActor* pActor, bool bDelete /* = true*/)
{
    return DelActorByID(pActor->GetID(), bDelete);
}

bool CActorManager::DelActorByID(OBJID id, bool bDelete /* = true*/)
{
    auto itFind = m_ActorMap.find(id);
    if(itFind == m_ActorMap.end())
        return false;

    CActor* pActor = itFind->second;
    m_ActorMap.erase(itFind);

    if(pActor->IsPlayer())
    {
        CPlayer* pPlayer      = pActor->CastTo<CPlayer>();
        auto     itFindPlayer = m_PlayerRefMap.find(pPlayer->GetSocket());
        if(itFindPlayer != m_PlayerRefMap.end())
        {
            m_PlayerRefMap.erase(itFindPlayer);
        }
    }
    else
    {
        m_idPool[pActor->GetActorType()].put(id);
    }
   
    m_ActorCount[pActor->GetActorType()]--;

    if(bDelete)
        SAFE_DELETE(pActor);

    return true;
}

OBJID CActorManager::GenNpcID()
{
    return m_idPool[ACT_NPC].get();
}

OBJID CActorManager::GenMonsterID()
{
    return m_idPool[ACT_MONSTER].get();
}

OBJID CActorManager::GenMapItemID()
{
    return m_idPool[ACT_MAPITEM].get();
}

OBJID CActorManager::GenBulletID()
{
    return m_idPool[ACT_BULLET].get();
}
