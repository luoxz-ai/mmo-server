#ifndef TEAMMANAGER_H
#define TEAMMANAGER_H

#include "BaseCode.h"
#include "game_common_def.h"
#include "msg/zone_service.pb.h"
#include "server_msg/server_side.pb.h"

class CUser;
class CTeam;

class CNetworkMessage;
class CTeamManager
{
	CTeamManager();
public:
	~CTeamManager();
	CREATE_NEW_IMPL(CTeamManager);

	bool   Init();
	void   Destory();
	CTeam* CreateTeam(uint64_t idTeam, OBJID idLeader);
	CTeam* QueryTeam(uint64_t idTeam);
	bool   DestoryTeam(uint64_t idTeam);

	void RegisterMessageHandler();

private:
	std::map<uint64_t, CTeam*> m_setTeam;
};
#endif /* TEAMMANAGER_H */
