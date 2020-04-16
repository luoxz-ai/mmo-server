#include "AIPlayer.h"

MEMORYHEAP_IMPLEMENTATION(CAIPlayer, s_heap);
CAIPlayer::CAIPlayer() {}

CAIPlayer::~CAIPlayer() {}

bool CAIPlayer::Init(const ServerMSG::ActorCreate& msg)
{
    SetID(msg.actor_id());
    SetLev(msg.lev());
    SetMoveSPD(msg.movespd());
    SetName(msg.name());
    SetCampID(msg.campid());
    _SetPhaseID(msg.phase_id());
    SetHP(msg.hp());
    SetHPMax(msg.hpmax());
    SetMP(msg.mp());
    SetMPMax(msg.mpmax());
    SetPos(Vector2(msg.posx(), msg.posy()));

    return true;
}


void CAIPlayer::ClearTaskPhase()
{
    m_TaskPhase.clear();
}

bool CAIPlayer::CheckTaskPhase(uint64_t idPhase)
{
    auto it = m_TaskPhase.find(idPhase);
    if(it != m_TaskPhase.end())
    {
        return it->second == 0;
    }
    return false;
}

void CAIPlayer::AddTaskPhase(uint64_t idPhase)
{
    auto& refData = m_TaskPhase[idPhase];
    refData++;
}


void CAIPlayer::RemoveTaskPhase(uint64_t idPhase)
{
    auto& refData = m_TaskPhase[idPhase];
    refData--;
    if(refData == 0)
    {
        m_TaskPhase.erase(idPhase);
    }
}