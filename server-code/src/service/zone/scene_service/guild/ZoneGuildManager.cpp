#include "ZoneGuildManager.h"

#include "MysqlConnection.h"
#include "ZoneGuild.h"
#include "SceneService.h"

CZoneGuildManager::CZoneGuildManager() {}
CZoneGuildManager::~CZoneGuildManager()
{
    Destory();
}

bool CZoneGuildManager::Init()
{
    // load from db
    auto pDB = SceneService()->GetGameDB(SceneService()->GetWorldID());

    auto result_ptr = pDB->QueryKey<TBLD_GUILD, TBLD_GUILD::DEL_TIME>(0);

    if(result_ptr)
    {
        for(size_t i = 0; i < result_ptr->get_num_row(); i++)
        {
            auto db_record_ptr = result_ptr->fetch_row(true);

            CZoneGuild* pGuild = CZoneGuild::CreateNew(std::move(db_record_ptr));
            if(pGuild)
            {
                m_setGuild.emplace(pGuild->GetGuildID(), pGuild);
            }
        }
    }
    return true;
}

void CZoneGuildManager::Destory()
{
    for(auto& [k, v]: m_setGuild)
    {
        SAFE_DELETE(v);
    }
    m_setGuild.clear();
}

void CZoneGuildManager::CreateGuild(const std::string& strGuildName, OBJID idLeader, const std::string& strLeaderName)
{
    // send msg to world
    ServerMSG::GuildCreate msg;
    msg.set_guild_name(strGuildName);
    msg.set_leader_id(idLeader);
    msg.set_leader_name(strLeaderName);
    SceneService()->SendMsgToWorld(SceneService()->GetWorldID(), to_server_msgid(msg), msg);
}

CZoneGuild* CZoneGuildManager::QueryGuild(uint64_t idGuild)
{
    auto it = m_setGuild.find(idGuild);
    if(it != m_setGuild.end())
    {
        return it->second;
    }
    return nullptr;
}

void CZoneGuildManager::OnCreateGuild() {}
bool CZoneGuildManager::OnDestoryGuild(uint64_t idGuild)
{
    return true;
}