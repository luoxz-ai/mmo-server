#pragma once

#include "BaseCode.h"
#include "game_common_def.h"
#include "NetworkDefine.h"

class CActor;
class CMonster;
class CPlayer;
//max uint64_t = 1844 6744 0737 0955 1614
//PlayerID 
//9999 4 00 0000 0000 0000
//WorldID Type IDX

//PetID
//9999 3 00 0000 0000 0000
//WorldID Type ZoneID IDX

//MonsterID
//0000 1 00 0000 0000 0000
//

//NpcID
//0000 2 00 0000 0000 0000
//






class CActorManager
{
public:
	CActorManager();
	~CActorManager();

	void Destory();

	CActor* QueryActor(OBJID id) const;
	CPlayer* QueryPlayer(const VirtualSocket& vs) const;
	bool AddActor(CActor* pActor);

	bool DelActor(CActor* pActor, bool bDelete = true);
	bool DelActorByID(OBJID id, bool bDelete = true);

	size_t GetUserCount() const { return m_PlayerRefMap.size();}
	size_t GetMonsterCount() const { return m_ActorCount[ACT_MONSTER];}
	size_t GetNpcCount() const { return m_ActorCount[ACT_NPC];}
	size_t GetPetCount() const { return m_ActorCount[ACT_PET];}

	OBJID GenNpcID();
	OBJID GenMonsterID();
	OBJID GenMapItemID();
	OBJID GenBulletID();

	void foreach_player(std::function< void(const std::unordered_map<VirtualSocket, CPlayer*>::value_type&) >&& func)
	{
		std::for_each(m_PlayerRefMap.begin(), m_PlayerRefMap.end(), func);
	}

	void foreach_actor(std::function< void(const std::unordered_map<OBJID, CActor*>::value_type&) >&& func)
	{
		std::for_each(m_ActorMap.begin(), m_ActorMap.end(), func);
	}

protected:
private:
	std::array<size_t, ACT_MAX> m_ActorCount;
	std::unordered_map<OBJID, CActor*> m_ActorMap;
	std::unordered_map<VirtualSocket, CPlayer*> m_PlayerRefMap;


	std::deque<OBJID> m_idMonsterPool;
	std::deque<OBJID> m_idNpcPool;
	uint64_t m_nNpcIDGen;
	uint64_t m_nMonsterIDGen;

	std::deque<OBJID> m_idMapItemPool;
	std::deque<OBJID> m_idBulletPool;
	uint64_t m_nMapItemIDGen;
	uint64_t m_nBulletIDGen;
};