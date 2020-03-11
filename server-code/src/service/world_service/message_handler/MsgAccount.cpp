#include "AccountManager.h"
#include "WorldService.h"
#include "MsgWorldProcess.h"
#include "msg/World_service.pb.h"

DEFINE_RAWMSG_PROCESS(CS_LOGIN)
{
    AccountManager()->Auth(pMsg);
}

DEFINE_RAWMSG_PROCESS(CS_CREATEACTOR)
{
    AccountManager()->OnMsgCreateActor(pMsg);
}

DEFINE_RAWMSG_PROCESS(CS_SELECTACTOR)
{
    AccountManager()->OnMsgSelectActor(pMsg);
}