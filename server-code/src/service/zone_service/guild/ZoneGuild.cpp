#include "ZoneGuild.h"
#include "ZoneService.h"
#include "MysqlConnection.h"

CZoneGuild::CZoneGuild()
{}

bool CZoneGuild::Init(CDBRecordPtr&& pDBRecord)
{
    m_pDBRecord = std::move(pDBRecord);
    //query member info
    auto pDB        = ZoneService()->GetGameDB(ZoneService()->GetWorldID());
    auto result_ptr = pDB->QueryT<TBLD_PLAYER_GUILDINFO,TBLD_PLAYER_GUILDINFO::GUILDID>(GetGuildID());

    if(result_ptr)
    {
        for(size_t i = 0; i < result_ptr->get_num_row(); i++)
        {
            auto db_record_ptr = result_ptr->fetch_row(false);

            OBJID idMember = db_record_ptr->Field(TBLD_PLAYER_GUILDINFO::GUILDID);
            m_setMember.emplace(idMember);
        }
    }

    return true;
}

CZoneGuild::~CZoneGuild()
{

}

bool CZoneGuild::IsLeader(OBJID idActor)const
{
    __ENTER_FUNCTION
    return GetLeaderID() == idActor;
    __LEAVE_FUNCTION
    return false;
}

bool CZoneGuild::IsMember(OBJID idActor)const
{
    __ENTER_FUNCTION
    return m_setMember.find(idActor) != m_setMember.end();
    __LEAVE_FUNCTION
    return false;
}

bool CZoneGuild::IsFull() const
{
        __ENTER_FUNCTION
    static constexpr size_t _MAX_GUILD_MEMBER = 100;
    return m_setMember.size() > _MAX_GUILD_MEMBER; 
    __LEAVE_FUNCTION
    return false;
}
