#include "Account.h"
#include "AccountManager.h"
#include "GMManager.h"
#include "MD5.h"
#include "MsgWorldProcess.h"
#include "WorldService.h"
#include "msg/world_service.pb.h"
#include "msg/zone_service.pb.h"
#include "server_msg/server_side.pb.h"

ON_MSG(CWorldService, CS_CREATEACTOR)
{
    __ENTER_FUNCTION

    CAccount* pAccount = AccountManager()->QueryAccountBySocket(pMsg->GetFrom());
    CHECK(pAccount);
    CHECK(pAccount->IsWait() == false);
    pAccount->CreateActor(msg.name(), msg.prof(), msg.baselook());

    __LEAVE_FUNCTION
}

ON_MSG(CWorldService, CS_SELECTACTOR)
{
    __ENTER_FUNCTION
    CAccount* pAccount = AccountManager()->QueryAccountBySocket(pMsg->GetFrom());
    CHECK(pAccount);
    CHECK(pAccount->IsWait() == false);
    pAccount->SelectActor(msg.actor_idx());
    __LEAVE_FUNCTION
}

ON_SERVERMSG(CWorldService, SocketLogin)
{
    AccountManager()->Login(msg.vs(), msg.open_id());
}