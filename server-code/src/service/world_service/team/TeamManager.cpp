#include "TeamManager.h"

#include "Team.h"
#include "User.h"
#include "UserManager.h"
#include "WorldService.h"
#include "msg/zone_service.pb.h"
#include "MsgWorldProcess.h"

CTeamManager::CTeamManager()
{
}

CTeamManager::~CTeamManager() {}

bool CTeamManager::Init()
{
    return true;
}

void CTeamManager::Destory()
{
    for(auto& [k, v]: m_setTeam)
    {
        SAFE_DELETE(v);
    }
    m_setTeam.clear();
}

CTeam* CTeamManager::CreateTeam(uint64_t idTeam, OBJID idLeader)
{
    CUser* pUser = UserManager()->QueryUser(idLeader);
    CHECKF(pUser);
    CHECKF(pUser->GetTeamID() == 0);

    CHECKF(QueryTeam(idTeam) == nullptr);
    CTeam* pTeam = CTeam::CreateNew(idTeam, idLeader);
    CHECKF(pTeam);
    m_setTeam[idTeam] = pTeam;

    // send msg to all zone
    ServerMSG::TeamCreate msg;
    msg.set_team_id(idTeam);
    msg.set_leader_id(idLeader);
    WorldService()->BroadcastToZone(ServerMSG::MsgID_TeamCreate, msg);

    pTeam->_AddMember(pUser);
    return pTeam;
}

CTeam* CTeamManager::QueryTeam(uint64_t idTeam)
{
    auto it = m_setTeam.find(idTeam);
    if(it != m_setTeam.end())
        return it->second;
    return nullptr;
}

bool CTeamManager::DestoryTeam(uint64_t idTeam)
{
    auto it = m_setTeam.find(idTeam);
    if(it == m_setTeam.end())
        return false;

    ServerMSG::TeamCreate msg;
    msg.set_team_id(idTeam);
    WorldService()->BroadcastToZone(ServerMSG::MsgID_TeamDestory, msg);
    CTeam* pTeam = it->second;
    pTeam->SendTeamAction(SC_TEAMMEMBER_ACTION::TEAM_DESTORY, 0, 0);

    SAFE_DELETE(pTeam);
    m_setTeam.erase(it);
    return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
ON_SERVERMSG(TeamCreate)
{
    TeamManager()->CreateTeam(msg.team_id(), msg.leader_id());
}

ON_SERVERMSG(TeamQuit)
{
    CTeam* pTeam = TeamManager()->QueryTeam(msg.team_id());
    CHECK(pTeam);
    pTeam->QuitTeam(msg.operator_id());
}

ON_SERVERMSG(TeamKickMember)
{
    CTeam* pTeam = TeamManager()->QueryTeam(msg.team_id());
    CHECK(pTeam);
    pTeam->KickMember(msg.operator_id(), msg.kick_id());
}

ON_SERVERMSG(TeamInviteMember)
{
    CTeam* pTeam = TeamManager()->QueryTeam(msg.team_id());
    CHECK(pTeam);
    pTeam->InviteMember(msg.operator_id(), msg.invitee_id());
}

ON_SERVERMSG(TeamAcceptInvite)
{

    CTeam* pTeam = TeamManager()->QueryTeam(msg.team_id());
    pTeam->AcceptInvite(msg.inviter_id(), msg.invitee_id(), msg.result());
}

ON_SERVERMSG(TeamApplyMember)
{
    CUser* pApplicant = UserManager()->QueryUser(msg.applicant_id());
    if(pApplicant == nullptr)
    {
        return;
    }
    CUser* pUser = UserManager()->QueryUser(msg.respondent_id());
    if(pUser == nullptr)
    {
        // send err msg to sender
        return;
    }

    if(pUser->GetTeamID() != 0)
    {
        CTeam* pTeam = TeamManager()->QueryTeam(pUser->GetTeamID());
        CHECK(pTeam);
        pTeam->ApplyMember(msg.applicant_id());
    }
    else
    {
        //发送申请给队长
        SC_TEAMAPPLYMEMBER msg;
        msg.set_applicant_id(msg.applicant_id());
        msg.set_applicant_name(pApplicant->GetName());
        pUser->SendMsg(CMD_SC_TEAMAPPLYMEMBER, msg);
    }
}

ON_SERVERMSG(TeamAcceptApply)
{

    CUser* pUser = UserManager()->QueryUser(msg.respondent_id());
    if(pUser == nullptr)
    {
        return;
    }
    CUser* pApplicant = UserManager()->QueryUser(msg.applicant_id());
    if(pApplicant == nullptr)
    {
        // send err msg to sender
        return;
    }
    if(pApplicant->GetTeamID() != 0)
    {
        // send err msg to sender
        return;
    }

    if(pUser->GetTeamID() != 0)
    {
        CTeam* pTeam = TeamManager()->QueryTeam(pUser->GetTeamID());
        CHECK(pTeam);
        pTeam->AcceptApply(msg.applicant_id(), msg.respondent_id(), msg.result());
    }
    else
    {
        if(msg.result() == false)
        {
            // send err msg to sender
            return;
        }
        CTeam* pTeam = TeamManager()->CreateTeam(WorldService()->CreateUID(), pUser->GetID());
        CHECK(pTeam);
        pTeam->AddMember(msg.applicant_id());
    }
}

ON_SERVERMSG(TeamNewLeader)
{
    CTeam* pTeam = TeamManager()->QueryTeam(msg.team_id());
    CHECK(pTeam);
    pTeam->SetLeader(msg.operator_id(), msg.new_leader_id());
}

