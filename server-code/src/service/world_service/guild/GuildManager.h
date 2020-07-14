#ifndef GUILDMANAGER_H
#define GUILDMANAGER_H

#include "BaseCode.h"
#include "game_common_def.h"
#include "msg/zone_service.pb.h"
#include "server_msg/server_side.pb.h"

class CGuild;
class CUser;
class CNetworkMessage;
class CGuildManager : public NoncopyableT<CGuildManager>
{
    CGuildManager();

public:
    CreateNewImpl(CGuildManager);

public:
    ~CGuildManager();

    bool   Init();
    void   Destory();
    CGuild* CreateGuild(uint64_t idGuild, const std::string& strGuildName, OBJID idLeader, const std::string& strLeaderName);
    CGuild* QueryGuild(uint64_t idGuild);
    bool   DestoryGuild(uint64_t idGuild);

private:
    std::map<uint64_t, CGuild*> m_setGuild;
};

#endif /* GUILDMANAGER_H */
