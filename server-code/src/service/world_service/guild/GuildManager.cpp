#include "GuildManager.h"

#include "Guild.h"
#include "MsgWorldProcess.h"
#include "User.h"
#include "UserManager.h"
#include "WorldService.h"
#include "msg/zone_service.pb.h"

CGuildManager::CGuildManager() {}

CGuildManager::~CGuildManager()
{
    Destory();
}

bool CGuildManager::Init()
{
    __ENTER_FUNCTION
    auto pDB        = WorldService()->GetGameDB();
    auto result_ptr = pDB->QueryKey<TBLD_GUILD, TBLD_GUILD::DEL_TIME>(0);

    if(result_ptr)
    {
        for(size_t i = 0; i < result_ptr->get_num_row(); i++)
        {
            auto db_record_ptr = result_ptr->fetch_row(true);

            CGuild* pGuild = CGuild::CreateNew(std::move(db_record_ptr));
            if(pGuild)
            {
                m_setGuild.emplace(pGuild->GetGuildID(), pGuild);
            }
        }
    }
    return true;
    __LEAVE_FUNCTION
    return false;
}

void CGuildManager::Destory()
{
    __ENTER_FUNCTION
    for(auto& [k, v]: m_setGuild)
    {
        SAFE_DELETE(v);
    }
    m_setGuild.clear();
    __LEAVE_FUNCTION
}

CGuild* CGuildManager::CreateGuild(uint64_t idGuild, const std::string& strGuildNamenNeedEscape, OBJID idLeader, const std::string& strLeaderName)
{
    __ENTER_FUNCTION
    CUser* pUser = UserManager()->QueryUser(idLeader);
    CHECKF(pUser);
    CHECKF(pUser->GetGuildID() == 0);

    CHECKF(QueryGuild(idGuild) == nullptr);
    
    auto pDB       = WorldService()->GetGameDB();
    std::string strGuildName;
    CHECKF(pDB->EscapeString(strGuildName, strGuildNamenNeedEscape));
    

    auto pDBRecord = pDB->MakeRecord(TBLD_GUILD::table_name());
    auto now       = TimeGetSecond();

    pDBRecord->Field(TBLD_GUILD::ID)           = idGuild;
    pDBRecord->Field(TBLD_GUILD::LEV)          = 1;
    pDBRecord->Field(TBLD_GUILD::NAME)         = strGuildName;
    pDBRecord->Field(TBLD_GUILD::LEADER_ID)    = idLeader;
    pDBRecord->Field(TBLD_GUILD::LEADER_NAME)  = strLeaderName;
    pDBRecord->Field(TBLD_GUILD::CREATOR_ID)   = idLeader;
    pDBRecord->Field(TBLD_GUILD::CREATOR_NAME) = strLeaderName;
    pDBRecord->Field(TBLD_GUILD::CREATE_TIME)  = now;
    pDBRecord->Field(TBLD_GUILD::DEL_TIME)     = 0;
    if(pDBRecord->Update(true) == false)
    {
        LOGDEBUG("CGuildManager: CreateGuild Fail. guild_id:{}, guildname:{}", idGuild, strGuildName);
        return nullptr;
    }

    LOGINFO("CGuildManager: CreateGuild Succ. guild_id:{}, guildname:{}", idGuild, strGuildName);

    CGuild* pGuild = CGuild::CreateNew(std::move(pDBRecord));
    CHECKF(pGuild);
    m_setGuild.emplace(pGuild->GetGuildID(), pGuild);

    // send msg to all zone
    ServerMSG::GuildCreate msg;
    msg.set_guild_id(idGuild);
    msg.set_guild_name(strGuildName);
    msg.set_leader_id(idLeader);
    msg.set_leader_name(strLeaderName);
    msg.set_create_time(now);
    WorldService()->BroadcastToZone(msg);

    pGuild->AddMember(pUser->GetID(), GUILD_RANK_LEADER);
    return pGuild;
    __LEAVE_FUNCTION
    return nullptr;
}

CGuild* CGuildManager::QueryGuild(uint64_t idGuild)
{
    __ENTER_FUNCTION
    auto it = m_setGuild.find(idGuild);
    if(it != m_setGuild.end())
        return it->second;

    __LEAVE_FUNCTION
    return nullptr;
}

bool CGuildManager::DestoryGuild(uint64_t idGuild)
{
    __ENTER_FUNCTION
    auto it = m_setGuild.find(idGuild);
    if(it == m_setGuild.end())
        return false;

    ServerMSG::GuildCreate msg;
    msg.set_guild_id(idGuild);
    WorldService()->BroadcastToZone(msg);
    CGuild* pGuild = it->second;
    pGuild->SendGuildAction(SC_GUILDMEMBER_ACTION::GUILD_DESTORY, 0, 0);

    SAFE_DELETE(pGuild);
    m_setGuild.erase(it);
    return true;

    __LEAVE_FUNCTION
    return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
ON_SERVERMSG(CWorldService, GuildCreate)
{
    GuildManager()->CreateGuild(WorldService()->CreateUID(), msg.guild_name(), msg.leader_id(), msg.leader_name());
}

ON_SERVERMSG(CWorldService, GuildQuit)
{
    CGuild* pGuild = GuildManager()->QueryGuild(msg.guild_id());
    CHECK(pGuild);
    pGuild->QuitGuild(msg.operator_id());
}

ON_SERVERMSG(CWorldService, GuildKickMember)
{
    CGuild* pGuild = GuildManager()->QueryGuild(msg.guild_id());
    CHECK(pGuild);
    pGuild->KickMember(msg.operator_id(), msg.kick_id());
}

ON_SERVERMSG(CWorldService, GuildInviteMember)
{
    CGuild* pGuild = GuildManager()->QueryGuild(msg.guild_id());
    CHECK(pGuild);
    pGuild->InviteMember(msg.operator_id(), msg.invitee_id());
}

ON_SERVERMSG(CWorldService, GuildAcceptInvite)
{

    CGuild* pGuild = GuildManager()->QueryGuild(msg.guild_id());
    pGuild->AcceptInvite(msg.inviter_id(), msg.invitee_id(), msg.result());
}

ON_SERVERMSG(CWorldService, GuildApplyMember)
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

    if(pUser->GetGuildID() != 0)
    {
        CGuild* pGuild = GuildManager()->QueryGuild(pUser->GetGuildID());
        CHECK(pGuild);
        pGuild->ApplyMember(msg.applicant_id());
    }
    else
    {
        //发送申请给队长
        SC_GUILDAPPLYMEMBER msg;
        msg.set_applicant_id(msg.applicant_id());
        msg.set_applicant_name(pApplicant->GetName());
        pUser->SendMsg(msg);
    }
}

ON_SERVERMSG(CWorldService, GuildAcceptApply)
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
    if(pApplicant->GetGuildID() != 0)
    {
        // send err msg to sender
        return;
    }

    if(msg.result() == false)
    {
        // send err msg to sender
        return;
    }

    if(pUser->GetGuildID() != 0)
    {
        CGuild* pGuild = GuildManager()->QueryGuild(pUser->GetGuildID());
        CHECK(pGuild);
        pGuild->AcceptApply(msg.applicant_id(), msg.respondent_id(), msg.result());
    }
    else
    {
        // send err msg to sender
    }
}

ON_SERVERMSG(CWorldService, GuildNewLeader)
{
    CGuild* pGuild = GuildManager()->QueryGuild(msg.guild_id());
    CHECK(pGuild);
    pGuild->SetLeader(msg.operator_id(), msg.new_leader_id(), msg.new_leader_name());
}
