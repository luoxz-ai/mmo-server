#ifndef PLAYER_GUILDATTR_H
#define PLAYER_GUILDATTR_H

#include "BaseCode.h"
#include "DBRecord.h"
#include "db_proto/gamedb.pb.h"

class CPlayer;
class CPlayerGuildAttr : public NoncopyableT<CPlayerGuildAttr>
{
    CPlayerGuildAttr();
    bool Init(CPlayer* pPlayer);

public:
    CreateNewImpl(CPlayerGuildAttr);

public:
    ~CPlayerGuildAttr();

    uint64_t get_guild_id() const { return (m_pDBRecord) ? m_pDBRecord->guildid() : 0; }
    uint32_t get_guild_rank() const { return (m_pDBRecord) ? m_pDBRecord->rank() : 0; }
    uint32_t get_guild_score() const { return (m_pDBRecord) ? m_pDBRecord->score() : 0; }
    uint32_t get_guild_total_score() const { return (m_pDBRecord) ? m_pDBRecord->total_score() : 0; }
    uint32_t get_guild_jointime() const { return (m_pDBRecord) ? m_pDBRecord->join_time() : 0; }

private:
    CPlayer*                                   m_pPlayer = nullptr;
    std::unique_ptr<db::tbld_player_guildinfo> m_pDBRecord;
};
#endif /* PLAYER_GUILDATTR_H */
