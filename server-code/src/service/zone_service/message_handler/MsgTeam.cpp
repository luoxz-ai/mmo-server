
#include "MsgZoneProcess.h"
#include "Phase.h"
#include "msg/ts_cmd.pb.h"
#include "msg/zone_service.pb.h"

ON_PLAYERMSG(CS_TEAMCREATE)
{
    __ENTER_FUNCTION
    if(pPlayer->GetCurrentScene() == nullptr)
        return;

    pPlayer->TeamCreate();

    __LEAVE_FUNCTION
}

ON_PLAYERMSG(CS_TEAMQUIT)
{
    __ENTER_FUNCTION
    if(pPlayer->GetCurrentScene() == nullptr)
        return;
    pPlayer->TeamQuit();

    __LEAVE_FUNCTION
}

ON_PLAYERMSG(CS_TEAMKICKMEMBER)
{
    __ENTER_FUNCTION
    if(pPlayer->GetCurrentScene() == nullptr)
        return;
    pPlayer->TeamKickMember(msg.member_id());

    __LEAVE_FUNCTION
}

ON_PLAYERMSG(CS_TEAMINVITEMEMBER)
{
    __ENTER_FUNCTION
    if(pPlayer->GetCurrentScene() == nullptr)
        return;

    pPlayer->TeamInviteMember(msg.invitee_id());

    __LEAVE_FUNCTION
}

ON_PLAYERMSG(CS_TEAMACCEPTINVITE)
{
    __ENTER_FUNCTION
    if(pPlayer->GetCurrentScene() == nullptr)
        return;

    pPlayer->TeamAcceptInvite(msg.team_id(), msg.inviter_id(), msg.result());

    __LEAVE_FUNCTION
}

ON_PLAYERMSG(CS_TEAMAPPLYMEMBER)
{
    __ENTER_FUNCTION
    if(pPlayer->GetCurrentScene() == nullptr)
        return;

    pPlayer->TeamApplyMember(msg.respondent_id());

    __LEAVE_FUNCTION
}

ON_PLAYERMSG(CS_TEAMACCEPTAPPLY)
{
    __ENTER_FUNCTION
    if(pPlayer->GetCurrentScene() == nullptr)
        return;

    pPlayer->TeamAcceptApply(msg.applicant_id(), msg.result());

    __LEAVE_FUNCTION
}

ON_PLAYERMSG(CS_TEAMNEWLEADER)
{
    __ENTER_FUNCTION
    if(pPlayer->GetCurrentScene() == nullptr)
        return;

    pPlayer->TeamChangeLeader(msg.new_leader_id());

    __LEAVE_FUNCTION
}
