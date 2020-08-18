#include "ZoneGuildManager.h"

#include "MysqlConnection.h"
#include "ZoneGuild.h"
#include "ZoneService.h"

CZoneGuildManager::CZoneGuildManager() {}
CZoneGuildManager::~CZoneGuildManager()
{
    Destory();
}

bool CZoneGuildManager::Init()
{
    // load from db
    auto pDB = ZoneService()->GetGameDB(ZoneService()->GetWorldID());

    auto result_ptr = pDB->QueryT<TBLD_GUILD, TBLD_GUILD::DEL_TIME>(0);

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