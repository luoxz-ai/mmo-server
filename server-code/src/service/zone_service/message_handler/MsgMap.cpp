#include "MsgZoneProcess.h"
#include "Player.h"
#include "ZoneService.h"
#include "msg/ts_cmd.pb.h"
#include "msg/zone_service.pb.h"

void OnMsg_CS_LOADMAP_SUCC(CPlayer* pPlayer, const CS_LOADMAP_SUCC& msg, CNetworkMessage* pMsg)
{
    __ENTER_FUNCTION
    if(pPlayer->GetCurrentScene() != nullptr)
        return;

    pPlayer->OnLoadMapSucc();
    __LEAVE_FUNCTION
}

void OnMsg_CS_CHANGEMAP(CPlayer* pPlayer, const CS_CHANGEMAP& msg, CNetworkMessage* pMsg)
{
    __ENTER_FUNCTION

    if(pPlayer->GetCurrentScene() == nullptr)
        return;

    if(pPlayer->GetCurrentScene()->GetMapID() != msg.mapid())
        return;
    pPlayer->TryChangeMap(msg.leavepoint_idx());
    __LEAVE_FUNCTION
}

void OnMsg_CS_MOVE(CPlayer* pPlayer, const CS_MOVE& msg, CNetworkMessage* pMsg)
{
    __ENTER_FUNCTION
    if(pPlayer->GetCurrentScene() == nullptr)
        return;

    if(pPlayer->GetCurrentScene()->GetMapID() != msg.mapid())
        return;

    if(pPlayer->MoveTo(Vector2(msg.x(), msg.y())) == false)
    {
        // send kick back to client
        return;
    }
    pPlayer->SetFace(msg.face());

    __LEAVE_FUNCTION
}

void OnMsg_CS_REBORN(CPlayer* pPlayer, const CS_REBORN& msg, CNetworkMessage* pMsg)
{
    __ENTER_FUNCTION
    if(pPlayer->GetCurrentScene() == nullptr)
        return;

    if(pPlayer->IsDead() == false)
        return;

    pPlayer->Reborn(msg.reborn_type());
    __LEAVE_FUNCTION
}

/////////////////////////////////////////////////////////////////////////

void ZoneMapMessageHandlerRegister()
{
    __ENTER_FUNCTION

    auto pNetMsgProcess = ZoneService()->GetNetMsgProcess();
#define REGISTER_MSG(MsgT)    \
    pNetMsgProcess->Register( \
        CMD_##MsgT,           \
        std::bind(&ProcPlayerMsg<MsgT, decltype(OnMsg_##MsgT)>, std::placeholders::_1, &OnMsg_##MsgT));

    REGISTER_MSG(CS_LOADMAP_SUCC);
    REGISTER_MSG(CS_CHANGEMAP);
    REGISTER_MSG(CS_MOVE);
    REGISTER_MSG(CS_REBORN);

#undef REGISTER_MSG
    __LEAVE_FUNCTION
}
