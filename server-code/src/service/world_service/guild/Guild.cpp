#include "Guild.h"

#include "GuildManager.h"
#include "User.h"
#include "UserManager.h"
#include "WorldService.h"
#include "msg/zone_service.pb.h"

OBJECTHEAP_IMPLEMENTATION(CGuild, s_heap);
OBJECTHEAP_IMPLEMENTATION(CGuildMemberInfo, s_heap);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CGuildMemberInfo::CGuildMemberInfo() {}

CGuildMemberInfo::~CGuildMemberInfo() {}

bool CGuildMemberInfo::Init(CDBRecordPtr&& pDBRecord)
{
    __ENTER_FUNCTION
    m_pDBRecord = std::move(pDBRecord);

    return true;
    __LEAVE_FUNCTION
    return false;
}

GuildMemberInfo CGuildMemberInfo::to_pb() const
{
    GuildMemberInfo result;

    result.set_member_id(get_member_id());
    result.set_member_lev(get_member_lev());
    result.set_member_online(get_member_online());
    result.set_member_name(get_member_name());
    result.set_member_rank(get_member_rank());
    result.set_member_score(get_member_score());
    result.set_member_jointime(get_member_jointime());
    result.set_member_last_logout(get_member_last_logout());

    return result;
}

void CGuildMemberInfo::to_pb(GuildMemberInfo& info) const
{
    info.set_member_id(get_member_id());
    info.set_member_lev(get_member_lev());
    info.set_member_online(get_member_online());
    info.set_member_name(get_member_name());
    info.set_member_rank(get_member_rank());
    info.set_member_score(get_member_score());
    info.set_member_jointime(get_member_jointime());
    info.set_member_last_logout(get_member_last_logout());
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CGuild::CGuild() {}

CGuild::~CGuild() {}

bool CGuild::Init(CDBRecordPtr&& pDBRecord, CUser* pLeader)
{
    __ENTER_FUNCTION
    CHECKF(pDBRecord);
    m_pDBRecord = std::move(pDBRecord);

    AddMember(pLeader->GetID(), GUILD_RANK_LEADER);
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CGuild::Init(CDBRecordPtr&& pDBRecord)
{
    __ENTER_FUNCTION
    CHECKF(pDBRecord);
    m_pDBRecord = std::move(pDBRecord);
    // query member info
    auto pDB = WorldService()->GetGameDB();
    CHECKF(pDB);
    auto result_ptr = pDB->QueryKey<TBLD_PLAYER_GUILDINFO, TBLD_PLAYER_GUILDINFO::GUILDID>(GetGuildID());

    if(result_ptr)
    {
        for(size_t i = 0; i < result_ptr->get_num_row(); i++)
        {
            auto db_record_ptr = result_ptr->fetch_row(true);

            CGuildMemberInfo* pMemberInfo = CGuildMemberInfo::CreateNew(std::move(db_record_ptr));
            if(pMemberInfo)
            {
                m_setMemberInfo.emplace(pMemberInfo->get_member_id(), pMemberInfo);
            }
        }
    }
    return true;
    __LEAVE_FUNCTION
    return false;
}

void CGuild::SendGuildAction(uint32_t nAction, OBJID idOperator, OBJID idMember) const
{
    __ENTER_FUNCTION
    SC_GUILDMEMBER_ACTION msg;
    msg.set_action(SC_GUILDMEMBER_ACTION_Action(nAction));
    msg.set_operator_id(idOperator);
    msg.set_member_id(idMember);

    auto setSocketMap = WorldService()->IDList2VSMap(m_setMemberInfo, 0);
    WorldService()->SendMsgTo(setSocketMap, msg);

    __LEAVE_FUNCTION
}

void CGuild::SendGuildMemberInfoToAll(CGuildMemberInfo* pMemberInfo) const
{
    __ENTER_FUNCTION
    SC_GUILDMEMBER_INFO msg;

    auto pInfo = msg.add_member_list();
    pMemberInfo->to_pb(*pInfo);

    auto setSocketMap = WorldService()->IDList2VSMap(m_setMemberInfo, 0);
    WorldService()->SendMsgTo(setSocketMap, msg);
    __LEAVE_FUNCTION
}

void CGuild::SendAllGuildMemberInfoTo(CUser* pUser) const
{
    __ENTER_FUNCTION
    CHECK(pUser);

    SC_GUILDMEMBER_INFO msg;

    for(const auto& [k, v]: m_setMemberInfo)
    {
        auto pInfo = msg.add_member_list();
        v->to_pb(*pInfo);
    }

    pUser->SendMsg(msg);
    __LEAVE_FUNCTION
}

void CGuild::SetLeader(OBJID idOperator, OBJID idLeader, const std::string& strLeaderName)
{
    __ENTER_FUNCTION
    if(IsLeader(idOperator) == false)
        return;

    if(IsMember(idLeader) == false)
        return;

    m_pDBRecord->Field(TBLD_GUILD::LEADER_ID)   = idLeader;
    m_pDBRecord->Field(TBLD_GUILD::LEADER_NAME) = strLeaderName;
    m_pDBRecord->Update();

    auto pOldLeader = QueryMember(idOperator);
    auto pNewLeader = QueryMember(idLeader);
    pOldLeader->set_member_rank(GUILD_RANK_MEMBER);
    pNewLeader->set_member_rank(GUILD_RANK_LEADER);

    ServerMSG::GuildNewLeader msg;
    msg.set_guild_id(GetGuildID());
    msg.set_operator_id(idOperator);
    msg.set_new_leader_id(GetLeaderID());
    WorldService()->BroadcastToZone(msg);

    SendGuildAction(SC_GUILDMEMBER_ACTION::GUILD_CHANGE_LEADER, idOperator, idLeader);
    __LEAVE_FUNCTION
}

bool CGuild::IsFull() const
{
    __ENTER_FUNCTION
    static constexpr size_t _MAX_GUILD_MEMBER = 100;
    return m_setMemberInfo.size() > _MAX_GUILD_MEMBER;
    __LEAVE_FUNCTION
    return false;
}

void CGuild::AddMember(OBJID idMember, uint32_t nRank)
{
    __ENTER_FUNCTION
    auto pMemberInfo = CreateMemberInfo(idMember, nRank);
    CHECK(pMemberInfo);

    auto pUser = UserManager()->QueryUser(idMember);
    CHECK(pUser);
    pMemberInfo->set_member_lev(pUser->GetLev());
    pMemberInfo->set_member_name(pUser->GetName());
    pMemberInfo->set_member_last_logout(0);
    pMemberInfo->set_member_online(true);
    // send operator msg to all zone
    ServerMSG::GuildAddMember msg;
    msg.set_guild_id(GetGuildID());
    msg.set_member_id(idMember);
    WorldService()->BroadcastToZone(msg);

    // send to all member,exclude pUser;
    SendGuildMemberInfoToAll(pMemberInfo);

    // send all member info to pUser
    SendAllGuildMemberInfoTo(pUser);
    m_setMemberInfo.emplace(pMemberInfo->get_member_id(), pMemberInfo);
    __LEAVE_FUNCTION
}

CGuildMemberInfo* CGuild::CreateMemberInfo(OBJID idMember, uint32_t nRank)
{
    __ENTER_FUNCTION
    auto pDB = WorldService()->GetGameDB();
    CHECKF(pDB);
    auto db_record_ptr = pDB->MakeRecord(TBLD_PLAYER_GUILDINFO::table_name());
    CHECKF(db_record_ptr);
    db_record_ptr->Field(TBLD_PLAYER_GUILDINFO::ID)          = idMember;
    db_record_ptr->Field(TBLD_PLAYER_GUILDINFO::GUILDID)     = GetGuildID();
    db_record_ptr->Field(TBLD_PLAYER_GUILDINFO::RANK)        = nRank;
    db_record_ptr->Field(TBLD_PLAYER_GUILDINFO::SCORE)       = 0;
    db_record_ptr->Field(TBLD_PLAYER_GUILDINFO::TOTAL_SCORE) = 0;

    db_record_ptr->Update();

    CGuildMemberInfo* pMemberInfo = CGuildMemberInfo::CreateNew(std::move(db_record_ptr));
    return pMemberInfo;

    __LEAVE_FUNCTION
    return nullptr;
}

bool CGuild::AddMemberOffline(OBJID idMember, uint32_t nRank)
{
    __ENTER_FUNCTION
    // check have guild
    auto pDB = WorldService()->GetGameDB();
    CHECKF(pDB);
    CGuildMemberInfo* pMemberInfo = nullptr;

    auto db_query_ptr = pDB->QueryKeyLimit<TBLD_PLAYER_GUILDINFO, TBLD_PLAYER_GUILDINFO::ID>(idMember, 1);
    if(db_query_ptr)
    {
        auto row = db_query_ptr->fetch_row();

        uint64_t idGuild = row->Field(TBLD_PLAYER_GUILDINFO::GUILDID);
        if(idGuild != 0)
        {
            //已经加入其他guild了
            return false;
        }
        //修改
        row->Field(TBLD_PLAYER_GUILDINFO::GUILDID)     = GetGuildID();
        row->Field(TBLD_PLAYER_GUILDINFO::RANK)        = nRank;
        row->Field(TBLD_PLAYER_GUILDINFO::SCORE)       = 0;
        row->Field(TBLD_PLAYER_GUILDINFO::TOTAL_SCORE) = 0;
        row->Field(TBLD_PLAYER_GUILDINFO::JOIN_TIME)   = now();

        row->Update();

        pMemberInfo = CGuildMemberInfo::CreateNew(std::move(row));
    }
    else
    {
        //创建
        auto row = pDB->MakeRecord(TBLD_PLAYER_GUILDINFO::table_name());

        row->Field(TBLD_PLAYER_GUILDINFO::ID)          = idMember;
        row->Field(TBLD_PLAYER_GUILDINFO::GUILDID)     = GetGuildID();
        row->Field(TBLD_PLAYER_GUILDINFO::RANK)        = nRank;
        row->Field(TBLD_PLAYER_GUILDINFO::SCORE)       = 0;
        row->Field(TBLD_PLAYER_GUILDINFO::TOTAL_SCORE) = 0;
        row->Field(TBLD_PLAYER_GUILDINFO::JOIN_TIME)   = now();
        row->Update();
        pMemberInfo = CGuildMemberInfo::CreateNew(std::move(row));
    }

    CHECKF(pMemberInfo);
    m_setMemberInfo.emplace(pMemberInfo->get_member_id(), pMemberInfo);

    // send operator msg to all zone
    ServerMSG::GuildAddMember msg;
    msg.set_guild_id(GetGuildID());
    msg.set_member_id(idMember);
    WorldService()->BroadcastToZone(msg);

    // send to all member,exclude pUser;
    SendGuildMemberInfoToAll(pMemberInfo);
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CGuild::CheckMemberRank(uint32_t nRank, uint32_t nAction)
{
    return true;
}

bool CGuild::KickMember(OBJID idOperator, OBJID idMember)
{
    __ENTER_FUNCTION
    if(idOperator == idMember || IsMember(idMember) == false)
        return false;
    auto pOperator = QueryMember(idOperator);
    CHECKF(pOperator);
    if(CheckMemberRank(pOperator->get_member_rank(), SC_GUILDMEMBER_ACTION::GUILD_KICKMEMBER) == false)
        return false;

    {
        ServerMSG::GuildKickMember msg;
        msg.set_guild_id(GetGuildID());
        msg.set_operator_id(idOperator);
        msg.set_kick_id(idMember);
        WorldService()->BroadcastToZone(msg);
    }

    CUser* pUser = UserManager()->QueryUser(idMember);
    if(pUser)
    {
        pUser->SetGuildID(0);
    }

    SendGuildAction(SC_GUILDMEMBER_ACTION::GUILD_KICKMEMBER, idOperator, idMember);

    m_setMemberInfo.erase(idMember);
    return true;
    __LEAVE_FUNCTION

    return false;
}

const CGuildMemberInfo* CGuild::QueryMember(OBJID idMember) const
{
    __ENTER_FUNCTION
    auto it = m_setMemberInfo.find(idMember);
    if(it != m_setMemberInfo.end())
    {
        return it->second.get();
    }
    __LEAVE_FUNCTION

    return nullptr;
}

CGuildMemberInfo* CGuild::QueryMember(OBJID idMember)
{
    __ENTER_FUNCTION
    auto it = m_setMemberInfo.find(idMember);
    if(it != m_setMemberInfo.end())
    {
        return it->second.get();
    }
    __LEAVE_FUNCTION

    return nullptr;
}

void CGuild::Dismiss(OBJID idOperator)
{
    __ENTER_FUNCTION
    if(IsLeader(idOperator) == false)
        return;
    if(m_setMemberInfo.size() == 1)
        return;
    GuildManager()->DestoryGuild(GetGuildID());

    __LEAVE_FUNCTION
}

bool CGuild::SetMemberRank(OBJID idOperator, OBJID idMember, uint32_t nRank)
{
    __ENTER_FUNCTION
    auto pOperator = QueryMember(idOperator);
    auto pMember   = QueryMember(idMember);
    CHECKF(pOperator);
    CHECKF(pMember);

    if(pOperator->get_member_rank() <= pMember->get_member_rank())
    {
        return false;
    }

    if(pOperator->get_member_rank() <= nRank)
    {
        return false;
    }

    if(GetRankCount(nRank) < GetRankLimit(nRank))
    {
        return false;
    }
    pMember->set_member_rank(nRank);
    pMember->Save();
    SendGuildAction(SC_GUILDMEMBER_ACTION::GUILD_MEMBER_RANK_CHG, idMember, idMember);

    return true;
    __LEAVE_FUNCTION
    return false;
}

size_t CGuild::GetRankCount(uint32_t nRank)
{
    __ENTER_FUNCTION

    size_t nCount = 0;
    for(const auto& [k, v]: m_setMemberInfo)
    {
        if(v->get_member_rank() == nRank)
        {
            nCount++;
        }
    }
    return nCount;
    __LEAVE_FUNCTION
    return 0;
}

size_t CGuild::GetRankLimit(uint32_t nRank)
{
    __ENTER_FUNCTION
    size_t nCount = 0;
    //找到当前等级对应的config
    //找到对应rank的limit
    return nCount;
    __LEAVE_FUNCTION
    return 0;
}

void CGuild::SetMemberScore(OBJID idMember, uint32_t nScore)
{
    __ENTER_FUNCTION
    auto pMember = QueryMember(idMember);
    CHECK(pMember);

    pMember->set_member_score(nScore);
    pMember->ClearDirty();

    SendGuildAction(SC_GUILDMEMBER_ACTION::GUILD_MEMBER_SCORE_CHG, idMember, idMember);
    __LEAVE_FUNCTION
}

void CGuild::QuitGuild(OBJID idOperator)
{
    __ENTER_FUNCTION
    if(IsMember(idOperator) == false)
        return;

    if(IsLeader(idOperator))
    {
        //必须先退位
        return;
    }

    //先发消息
    ServerMSG::GuildQuit msg;
    msg.set_guild_id(GetGuildID());
    msg.set_operator_id(idOperator);
    WorldService()->BroadcastToZone(msg);

    SendGuildAction(SC_GUILDMEMBER_ACTION::GUILD_QUIT, idOperator, idOperator);

    //再删除
    m_setMemberInfo.erase(idOperator);
    __LEAVE_FUNCTION
}

void CGuild::InviteMember(OBJID idInviter, OBJID idInvitee)
{
    __ENTER_FUNCTION
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

    if(pInvitee->GetGuildID() != 0)
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
    SC_GUILDINVITEMEMBER msg;
    msg.set_guild_id(GetGuildID());
    msg.set_inviter_id(idInviter);
    msg.set_inviter_name(pInviter->GetName());
    pInvitee->SendMsg(msg);
    __LEAVE_FUNCTION
}

void CGuild::AcceptInvite(OBJID idInviter, OBJID idInvitee, bool bResult)
{
    __ENTER_FUNCTION
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
    if(pInvitee->GetGuildID() != 0)
    {
        //通知原邀请者,对方已经有队伍了
        return;
    }

    if(IsFull())
    {
        //通知被邀请者,对方队伍已满
        return;
    }

    AddMember(idInvitee, GUILD_RANK_NEWIBLE);
    __LEAVE_FUNCTION
}

void CGuild::ApplyMember(OBJID idApplicant)
{
    __ENTER_FUNCTION
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
    if(pApplicant->GetGuildID() != 0)
    {
        return;
    }

    CUser* pLeader = UserManager()->QueryUser(GetLeaderID());
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
    SC_GUILDAPPLYMEMBER msg;
    msg.set_applicant_id(idApplicant);
    msg.set_applicant_name(pApplicant->GetName());
    pLeader->SendMsg(msg);
    __LEAVE_FUNCTION
}

bool CGuild::AcceptApply(OBJID idApplicant, OBJID idRespondent, bool bResult)
{
    __ENTER_FUNCTION
    auto pRespondent = QueryMember(idRespondent);
    CHECKF(pRespondent);

    if(CheckMemberRank(pRespondent->get_member_rank(), SC_GUILDMEMBER_ACTION::GUILD_ACCEPTAPPLY) == false)
    {
        return false;
    }

    m_setApply.erase(idApplicant);

    if(bResult == false)
    {
        //发送拒绝邀请给原邀请者
        return false;
    }
    if(IsFull())
    {
        //通知申请者,对方队伍已满
        return false;
    }

    CUser* pApplicant = UserManager()->QueryUser(idApplicant);
    if(pApplicant == nullptr)
    {
        AddMemberOffline(idApplicant, GUILD_RANK_NEWIBLE);
        return true;
    }
    else if(pApplicant->GetGuildID() != 0)
    {
        //发送错误答复给邀请给idRespondent
        return false;
    }

    AddMember(idApplicant, GUILD_RANK_NEWIBLE);
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CGuild::IsLeader(OBJID idActor) const
{
    __ENTER_FUNCTION
    return GetLeaderID() == idActor;
    __LEAVE_FUNCTION
    return false;
}

bool CGuild::IsMember(OBJID idActor) const
{
    __ENTER_FUNCTION
    return m_setMemberInfo.find(idActor) != m_setMemberInfo.end();
    __LEAVE_FUNCTION
    return false;
}

void CGuild::OnUserOnline(OBJID idActor, bool bOnline) {}

void CGuild::OnUserLevChg(OBJID idActor, uint32_t nLev) {}

void CGuild::OnUserNameChg(OBJID idActor, const std::string& name) {}
