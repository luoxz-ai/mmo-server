#include <regex>

#include "MsgZoneProcess.h"
#include "Npc.h"
#include "Player.h"
#include "ZoneService.h"
#include "msg/ts_cmd.pb.h"
#include "msg/zone_service.pb.h"

void OnMsg_CS_TASK_SUBMIT(CPlayer* pPlayer, const CS_TASK_SUBMIT& msg, CNetworkMessage* pMsg)
{
    __ENTER_FUNCTION
    if(pPlayer->GetCurrentScene() == nullptr)
        return;

    if(pPlayer->IsDead() == false)
        return;

    pPlayer->GetTaskSet()->SubmitTaskByMessage(msg.task_id(), msg.submit_multiple());
    __LEAVE_FUNCTION
}

void OnMsg_CS_ACTIVE_NPC(CPlayer* pPlayer, const CS_ACTIVE_NPC& msg, CNetworkMessage* pMsg)
{
    __ENTER_FUNCTION
    if(pPlayer->GetCurrentScene() == nullptr)
        return;

    if(pPlayer->IsDead() == false)
        return;

    if(pPlayer->IsInViewActorByID(msg.npc_id()) == false)
        return;

    CActor* pActor = ActorManager()->QueryActor(msg.npc_id());
    if(pActor->IsNpc() == false)
        return;

    if(GameMath::distance(pActor->GetPos(), pPlayer->GetPos()) < MIN_INTERACT_DIS)
        return;

    CNpc* pNpc = pActor->ConvertToDerived<CNpc>();
    pNpc->ActiveNpc(pPlayer);
    __LEAVE_FUNCTION
}

void OnMsg_CS_DIALOG_CLICK(CPlayer* pPlayer, const CS_DIALOG_CLICK& msg, CNetworkMessage* pMsg)
{
    __ENTER_FUNCTION
    if(pPlayer->GetCurrentScene() == nullptr)
        return;

    if(pPlayer->IsDead() == false)
        return;

    pPlayer->OnDialogClick(msg.dialog_id(), msg.dialog_button_idx());
    __LEAVE_FUNCTION
}

/////////////////////////////////////////////////////////////////////////

void ZoneTaskMessageHandlerRegister()
{
    __ENTER_FUNCTION

    auto pNetMsgProcess = ZoneService()->GetNetMsgProcess();
#define REGISTER_MSG(MsgT)    \
    pNetMsgProcess->Register( \
        CMD_##MsgT,           \
        std::bind(&ProcPlayerMsg<MsgT, decltype(OnMsg_##MsgT)>, std::placeholders::_1, &OnMsg_##MsgT));

    REGISTER_MSG(CS_DIALOG_CLICK);
    REGISTER_MSG(CS_ACTIVE_NPC);
    REGISTER_MSG(CS_TASK_SUBMIT);

#undef REGISTER_MSG
    __LEAVE_FUNCTION
}
