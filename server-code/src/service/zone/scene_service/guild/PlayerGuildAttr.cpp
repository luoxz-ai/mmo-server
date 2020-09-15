#include "PlayerGuildAttr.h"

#include "DB2PB.h"
#include "MysqlConnection.h"
#include "Player.h"
#include "SceneService.h"

CPlayerGuildAttr::CPlayerGuildAttr() {}

bool CPlayerGuildAttr::Init(CPlayer* pPlayer)
{
    __ENTER_FUNCTION
    CHECKF(pPlayer);
    m_pPlayer = pPlayer;

    auto pDB = SceneService()->GetGameDB(m_pPlayer->GetWorldID());
    CHECKF(pDB);
    m_pDBRecord = DB2PB::QueryOneConst<TBLD_PLAYER_GUILDINFO, db::tbld_player_guildinfo, TBLD_PLAYER_GUILDINFO::ID>(pDB, m_pPlayer->GetID());
    return true;
    __LEAVE_FUNCTION
    return false;
}