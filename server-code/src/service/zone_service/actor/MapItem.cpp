#include "MapItem.h"

#include "Scene.h"
MEMORYHEAP_IMPLEMENTATION(CMapItem, s_heap);
CMapItem::CMapItem()
{
	SetCampID(CAMP_PLAYER);
}

CMapItem::~CMapItem()
{
	if(GetCurrentScene() != nullptr)
		GetCurrentScene()->LeaveMap(this);
}

bool CMapItem::Init(uint32_t idType)
{
	m_idType = idType;
	SetID(ActorManager()->GenMapItemID());
	CHECKF(CActor::Init());
	return true;
}

void CMapItem::MakeShowData(SC_AOI_NEW& msg)
{
	msg.set_mapid(GetMapID());

	msg.set_actor_id(GetID());
	msg.set_actortype(ACT_MAPITEM);
	msg.set_prof(m_idType);
	msg.set_lev(GetLev());

	msg.set_campid(GetCampID());
	msg.set_name(GetName());
	msg.set_hp(GetHP());
	msg.set_hpmax(GetHPMax());
}

void CMapItem::OnEnterMap(CSceneBase* pScene)
{
	CActor::OnEnterMap(pScene);

	ServerMSG::ActorCreate ai_msg;
	ai_msg.set_actor_id(GetID());
	ai_msg.set_scene_id(GetSceneID());
	ai_msg.set_actortype(ACT_MAPITEM);
	ai_msg.set_prof(GetTypeID());
	ai_msg.set_lev(GetLev());
	ai_msg.set_campid(GetCampID());
	ai_msg.set_name(GetName());
	ai_msg.set_hp(GetHP());
	ai_msg.set_hpmax(GetHPMax());
	ai_msg.set_mp(GetMP());
	ai_msg.set_mpmax(GetMPMax());
	ai_msg.set_movespd(GetAttrib().get(ATTRIB_MOVESPD));
	ai_msg.set_posx(GetPosX());
	ai_msg.set_posy(GetPosY());
	ai_msg.set_ownerid(GetOwnerID());

	ZoneService()->SendMsgToAIService(ServerMSG::MsgID_ActorCreate, ai_msg);
}

void CMapItem::OnLeaveMap(uint64_t idTargetScene)
{
	CActor::OnLeaveMap(idTargetScene);
}
