#include "Player.h"
#include "TeamInfoManager.h"
#include "ZoneService.h"
#include "server_msg/server_side.pb.h"

void CPlayer::TeamCreate()
{
    if(HasTeam() == true)
        return;

    ServerMSG::TeamCreate msg;
    msg.set_team_id(ZoneService()->CreateUID());
    msg.set_leader_id(GetID());
    ZoneService()->SendMsgToWorld(GetWorldID(), to_server_msgid(msg), msg);
}

void CPlayer::TeamInviteMember(OBJID idTarget)
{
    if(HasTeam() == false)
        return;

    ServerMSG::TeamInviteMember msg;
    msg.set_team_id(GetTeamID());
    msg.set_operator_id(GetID());
    msg.set_invitee_id(idTarget);
    ZoneService()->SendPortMsg(GetWorldID(), msg);
}

void CPlayer::TeamAcceptInvite(uint64_t idTeam, OBJID idInviter, bool bResult)
{
    if(HasTeam() == true)
        return;

    ServerMSG::TeamAcceptInvite msg;
    msg.set_team_id(idTeam);
    msg.set_inviter_id(idInviter);
    msg.set_invitee_id(GetID());
    msg.set_result(bResult);
    ZoneService()->SendPortMsg(GetWorldID(), msg);
}

void CPlayer::TeamApplyMember(OBJID idTarget)
{
    if(HasTeam() == false)
        return;

    ServerMSG::TeamApplyMember msg;
    msg.set_applicant_id(GetID());
    msg.set_respondent_id(idTarget);
    ZoneService()->SendPortMsg(GetWorldID(), msg);
}

void CPlayer::TeamAcceptApply(OBJID idApplicant, bool bResult)
{
    ServerMSG::TeamAcceptApply msg;
    msg.set_applicant_id(idApplicant);
    msg.set_respondent_id(GetID());
    msg.set_result(bResult);
    ZoneService()->SendPortMsg(GetWorldID(), msg);
}

void CPlayer::TeamQuit()
{
    if(HasTeam() == false)
        return;

    ServerMSG::TeamQuit msg;
    msg.set_team_id(GetTeamID());
    msg.set_operator_id(GetID());
    ZoneService()->SendPortMsg(GetWorldID(), msg);
}

void CPlayer::TeamKickMember(OBJID idMember)
{
    if(HasTeam() == false)
        return;
    auto pTeam = TeamManager()->QueryTeam(GetTeamID());
    if(pTeam == nullptr)
        return;
    if(pTeam->IsTeamMember(idMember) == false)
        return;
    if(pTeam->IsTeamLeader(GetID()) == false)
        return;

    ServerMSG::TeamKickMember msg;
    msg.set_team_id(GetTeamID());
    msg.set_operator_id(GetID());
    msg.set_kick_id(idMember);
    ZoneService()->SendPortMsg(GetWorldID(), msg);
}

void CPlayer::TeamChangeLeader(OBJID idMember)
{
    if(HasTeam() == false)
        return;
    auto pTeam = TeamManager()->QueryTeam(GetTeamID());
    CHECK(pTeam);
    if(pTeam->IsTeamLeader(GetID()) == false)
        return;

    if(pTeam->IsTeamMember(idMember) == false)
        return;

    ServerMSG::TeamNewLeader msg;
    msg.set_team_id(GetTeamID());
    msg.set_operator_id(GetID());
    msg.set_new_leader_id(idMember);
    ZoneService()->SendPortMsg(GetWorldID(), msg);
}