#include "AIPlayer.h"

MEMORYHEAP_IMPLEMENTATION(CAIPlayer, s_heap);
CAIPlayer::CAIPlayer() {}

CAIPlayer::~CAIPlayer() {}

bool CAIPlayer::Init(const ServerMSG::ActorCreate& msg)
{
	SetID(msg.actor_id());
	SetLev(msg.lev());
	SetMoveSPD(msg.movespd());
	SetName(msg.name());
	SetCampID(msg.campid());
	SetHP(msg.hp());
	SetHPMax(msg.hpmax());
	SetMP(msg.mp());
	SetMPMax(msg.mpmax());
	SetPos(Vector2(msg.posx(), msg.posy()));

	return true;
}
