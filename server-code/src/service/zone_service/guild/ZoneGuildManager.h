#ifndef ZONEGUILDMANAGER_H
#define ZONEGUILDMANAGER_H

#include "BaseCode.h"
#include <map>
class CZoneGuild;
class CZoneGuildManager: public NoncopyableT<CZoneGuildManager>
{
    CZoneGuildManager();

public:
    CreateNewImpl(CZoneGuildManager);

public:
    ~CZoneGuildManager();

    bool   Init();
    void   Destory();
    void   CreateGuild(const std::string& strGuildName, OBJID idLeader, const std::string& strLeaderName);
    
    CZoneGuild* QueryGuild(uint64_t idGuild);

public:
    void   OnCreateGuild();
    bool   OnDestoryGuild(uint64_t idGuild);
private:
    std::map<uint64_t, CZoneGuild*> m_setGuild;
};


#endif /* ZONEGUILDMANAGER_H */
