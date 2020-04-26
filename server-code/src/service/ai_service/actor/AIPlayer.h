#ifndef AIPLAYER_H
#define AIPLAYER_H
#include "AIActor.h"
#include "server_msg/server_side.pb.h"

class CAIPlayer : public CAIActor
{
    CAIPlayer();
    bool Init(const ServerMSG::ActorCreate& msg);
public:
    CreateNewImpl(CAIPlayer);
public:
    
    ~CAIPlayer();
    
    

    virtual ActorType GetActorType() const override { return ActorType::ACT_PLAYER; }
    static ActorType  GetActorTypeStatic() { return ActorType::ACT_PLAYER; }

    void ClearTaskPhase();
    bool CheckTaskPhase(uint64_t idPhase);
    void AddTaskPhase(uint64_t idPhase);
    void RemoveTaskPhase(uint64_t idPhase);
public:
    OBJECTHEAP_DECLARATION(s_heap);

private:
    std::unordered_map<uint64_t, uint32_t> m_TaskPhase;
};
#endif /* AIPLAYER_H */
