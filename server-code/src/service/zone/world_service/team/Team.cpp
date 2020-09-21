#include "Team.h"

#include "TeamManager.h"
#include "User.h"
#include "UserManager.h"
#include "WorldService.h"
#include "msg/zone_service.pb.h"

OBJECTHEAP_IMPLEMENTATION(CTeam, s_heap);

CTeam::CTeam() {}

CTeam::~CTeam() {}

bool CTeam::Init(uint64_t idTeam, OBJID idLeader)
{
    m_idTeam   = idTeam;
    m_idLeader = idLeader;
    return true;
}

void CTeam::SendTeamAction(uint32_t nAction, OBJID idOperator, OBJID idMember)
{
    SC_TEAMMEMBER_ACTION msg;
    msg.set_action(SC_TEAMMEMBER_ACTION_Action(nAction));
    msg.set_operator_id(idOperator);
    msg.set_member_id(idMember);
    for(const auto& v: m_setMember)
    {
        CUser* pUser = UserManager()->QueryUser(v.member_id());
        if(pUser)
        {
            pUser->SendMsg(msg);
        }
    }
}

void CTeam::SendTeamMemberInfo(const TeamMemberInfo& info)
{
    SC_TEAMMEMBER_INFO msg;
    auto               pInfo = msg.add_member_list();
    pInfo->CopyFrom(info);

    for(const auto& v: m_setMember)
    {
        CUser* pUser = UserManager()->QueryUser(v.member_id());
        if(pUser)
        {
            pUser->SendMsg(msg);
        }
    }
}

void CTeam::SendAllTeamMemberInfo(CUser* pUser)
{
    SC_TEAMMEMBER_INFO msg;

    for(const auto& v: m_setMember)
    {
        auto pInfo = msg.add_member_list();
        pInfo->CopyFrom(v);
    }

    pUser->SendMsg(msg);
}

void CTeam::SetLeader(OBJID idOperator, OBJID idLeader)
{
    if(IsLeader(idOperator) == false)
        return;

    if(IsMember(idLeader) == false)
        return;

    m_idLeader = idLeader;
    ServerMSG::TeamNewLeader msg;
    msg.set_team_id(m_idTeam);
    msg.set_operator_id(idOperator);
    msg.set_new_leader_id(m_idLeader);
    WorldService()->SendProtoMsgToAllScene(msg);

    SendTeamAction(SC_TEAMMEMBER_ACTION::TEAM_CHANGE_LEADER, idOperator, idLeader);
}

void CTeam::AddMember(OBJID idMember)
{
    CUser* pUser = UserManager()->QueryUser(idMember);
    CHECK(pUser);

    _AddMember(pUser);
}

void CTeam::_AddMember(CUser* pUser)
{
    CHECK(pUser);
    TeamMemberInfo info;
    info.set_member_id(pUser->GetID());
    info.set_member_lev(pUser->GetLev());
    info.set_member_name(pUser->GetName());
    // send msg to all zone
    ServerMSG::TeamAddMember msg;
    msg.set_team_id(m_idTeam);
    msg.set_team_id(pUser->GetID());
    WorldService()->SendProtoMsgToAllScene(msg);
    // send to all member,exclude pUser;
    SendTeamMemberInfo(info);
    // send all member info to pUser
    SendAllTeamMemberInfo(pUser);
    m_setMember.push_back(std::move(info));
}

void CTeam::KickMember(OBJID idOperator, OBJID idMember)
{
    if(IsLeader(idOperator) == false)
        return;

    if(idOperator == idMember || IsMember(idMember) == false)
        return;

    for(auto it = m_setMember.begin(); it != m_setMember.end(); it++)
    {
        if(it->member_id() == idMember)
        {
            {
                ServerMSG::TeamKickMember msg;
                msg.set_team_id(m_idTeam);
                msg.set_operator_id(idOperator);
                msg.set_kick_id(idMember);
                WorldService()->SendProtoMsgToAllScene(msg);
            }

            CUser* pUser = UserManager()->QueryUser(idMember);
            if(pUser)
            {
                pUser->SetTeamID(0);
            }

            SendTeamAction(SC_TEAMMEMBER_ACTION::TEAM_KICKMEMBER, idOperator, idMember);

            m_setMember.erase(it);
            return;
        }
    }
}

void CTeam::QuitTeam(OBJID idOperator)
{
    if(IsMember(idOperator) == false)
        return;

    //如果是队长
    if(m_setMember.size() == 1)
    {
        //解散
        TeamManager()->DestoryTeam(m_idTeam);
    }
    else if(IsLeader(idOperator))
    {
        for(auto it = m_setMember.begin(); it != m_setMember.end(); it++)
        {
            if(it->member_id() == idOperator)
            {
                ServerMSG::TeamQuit msg;
                msg.set_team_id(m_idTeam);
                msg.set_operator_id(idOperator);
                WorldService()->SendProtoMsgToAllScene(msg);

                SendTeamAction(SC_TEAMMEMBER_ACTION::TEAM_QUIT, idOperator, idOperator);
                m_setMember.erase(it);
                break;
            }
        }
        //那么要制定队长
        auto& ref = m_setMember.front();
        SetLeader(idOperator, ref.member_id());
    }
    else
    {
        for(auto it = m_setMember.begin(); it != m_setMember.end(); it++)
        {
            if(it->member_id() == idOperator)
            {
                ServerMSG::TeamQuit msg;
                msg.set_team_id(m_idTeam);
                msg.set_operator_id(idOperator);
                WorldService()->SendProtoMsgToAllScene(msg);
                SendTeamAction(SC_TEAMMEMBER_ACTION::TEAM_QUIT, idOperator, idOperator);
                m_setMember.erase(it);
                return;
            }
        }
    }
}

void CTeam::InviteMember(OBJID idInviter, OBJID idInvitee)
{
    if(IsMember(idInviter) == false)
        return;
    if(IsFull())
        return;

    CUser* pInviter = UserManager()->QueryUser(idInviter);
    CUser* pInvitee = UserManager()->QueryUser(idInvitee);
    if(pInviter == nullptr)
        return;
    if(pInvitee == nullptr)
    {
        //通知前端没有找到对应玩家
        return;
    }

    if(pInvitee->GetTeamID() != 0)
    {
        //通知前端对方已经有队伍了
        return;
    }

    if(m_setInvite.find(idInvitee) != m_setInvite.end())
    {
        //已经邀请过了
        return;
    }

    //记录邀请
    m_setInvite.insert(idInvitee);
    //发送邀请给目标
    SC_TEAMINVITEMEMBER msg;
    msg.set_team_id(m_idTeam);
    msg.set_inviter_id(idInviter);
    msg.set_inviter_name(pInviter->GetName());
    pInvitee->SendMsg(msg);
}

void CTeam::AcceptInvite(OBJID idInviter, OBJID idInvitee, bool bResult)
{
    auto it = m_setInvite.find(idInvitee);
    if(it == m_setInvite.end())
        return;

    m_setInvite.erase(it);
    if(bResult == false)
    {
        //发送拒绝邀请给原邀请者
        return;
    }
    CUser* pInvitee = UserManager()->QueryUser(idInvitee);
    if(pInvitee == nullptr)
    {
        //发送拒绝邀请给原邀请者
        return;
    }
    if(pInvitee->GetTeamID() != 0)
    {
        //通知原邀请者,对方已经有队伍了
        return;
    }

    if(IsFull())
    {
        //通知被邀请者,对方队伍已满
        return;
    }

    AddMember(idInvitee);
}

void CTeam::ApplyMember(OBJID idApplicant)
{
    if(IsFull())
    {
        //通知申请者,对方队伍已满
        return;
    }

    CUser* pApplicant = UserManager()->QueryUser(idApplicant);
    if(pApplicant == nullptr)
    {
        return;
    }
    if(pApplicant->GetTeamID() != 0)
    {
        return;
    }

    CUser* pLeader = UserManager()->QueryUser(m_idLeader);
    if(pLeader == nullptr)
    {
        //通知申请人，对方队长不在线
        return;
    }

    if(m_setApply.find(idApplicant) != m_setApply.end())
    {
        //已经申请过了
        return;
    }

    //记录申请
    m_setApply.insert(idApplicant);
    //发送申请给队长
    SC_TEAMAPPLYMEMBER msg;
    msg.set_applicant_id(idApplicant);
    msg.set_applicant_name(pApplicant->GetName());
    pLeader->SendMsg(msg);
}

void CTeam::AcceptApply(OBJID idApplicant, OBJID idRespondent, bool bResult)
{
    m_setApply.erase(idApplicant);

    if(bResult == false)
    {
        //发送拒绝邀请给原邀请者
        return;
    }
    CUser* pApplicant = UserManager()->QueryUser(idApplicant);
    if(pApplicant == nullptr)
    {
        //发送拒绝邀请给原邀请者
        return;
    }
    if(pApplicant->GetTeamID() != 0)
    {
        //通知原邀请者,对方已经有队伍了
        return;
    }

    if(IsFull())
    {
        //通知申请者,对方队伍已满
        return;
    }

    AddMember(idApplicant);
}

bool CTeam::IsLeader(OBJID idActor)
{
    return m_idLeader == idActor;
}

bool CTeam::IsMember(OBJID idActor)
{
    for(auto it = m_setMember.begin(); it != m_setMember.end(); it++)
    {
        if(it->member_id() == idActor)
            return true;
    }

    return false;
}

void CTeam::OnUserOnline(OBJID idActor, bool bOnline) {}

void CTeam::OnUserLevChg(OBJID idActor, uint32_t nLev) {}

void CTeam::OnUserNameChg(OBJID idActor, const std::string& name) {}
