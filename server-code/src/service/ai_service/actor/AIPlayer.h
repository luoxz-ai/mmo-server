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

public:
    MEMORYHEAP_DECLARATION(s_heap);

private:
};
#endif /* AIPLAYER_H */
