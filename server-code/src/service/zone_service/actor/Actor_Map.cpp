#include "Actor.h"
#include "ActorManager.h"
#include "Monster.h"
#include "Player.h"
#include "Scene.h"
#include "SceneTree.h"
#include "ZoneService.h"

void CActor::OnEnterMap(CSceneBase* pScene)
{
	CHECK(pScene);
	CSceneObject::OnEnterMap(pScene);

	static_cast<CScene*>(pScene)->TryExecScript<void>(SCB_MAP_ONENTERMAP, this);
}

void CActor::OnLeaveMap(uint64_t idTargetScene)
{
	SendDelayAttribChage();

	ServerMSG::ActorDestory ai_msg;
	ai_msg.set_actor_id(GetID());
	ZoneService()->SendMsgToAIService(ServerMSG::MsgID_ActorDestory, ai_msg);

	if(m_pScene)
		static_cast<CScene*>(m_pScene)->TryExecScript<void>(SCB_MAP_ONLEAVEMAP, this, idTargetScene);

	CSceneObject::OnLeaveMap(idTargetScene);

	m_EventMap.Clear();
	m_EventQueue.Clear();
}

void CActor::_AddToAOIRemoveMessage(SC_AOI_REMOVE& removeMsg, OBJID id)
{
	if(GetActorType() == ACT_PLAYER)
	{
		removeMsg.add_idlist(id);
		//为了加速前端处理,以及避免单个包过大,每个包中允许缓冲64个aoi_info
		static const int32_t MAX_AOI_SIZE_IN_ONE_PACKET = 64;
		if(removeMsg.idlist_size() > MAX_AOI_SIZE_IN_ONE_PACKET)
		{
			SendMessage(CMD_SC_AOI_REMOVE, removeMsg);
			removeMsg.clear_idlist();
		}
	}
}

void CActor::_TrySendAOIRemoveMessage(const SC_AOI_REMOVE& removeMsg)
{
	if(removeMsg.idlist_size() > 0)
	{
		SendMessage(CMD_SC_AOI_REMOVE, removeMsg);
	}
}

//////////////////////////////////////////////////////////////////////
void CActor::RemoveFromViewList(CSceneObject* pActor, OBJID idActor, bool bErase)
{
	// 通知自己对方消失
	if(pActor)
	{
		if(pActor->GetActorType() == ACT_PLAYER)
		{
			RemoveDelaySendShowTo(pActor->GetID());
		}
	}
	else
	{
		//延迟发送队列里的也可以不要了
		RemoveDelaySendShowTo(idActor);
	}
}

//////////////////////////////////////////////////////////////////////
void CActor::AddToViewList(CSceneObject* pActor, bool bChkDuplicate, bool bSendShow)
{
	CSceneObject::AddToViewList(pActor, bChkDuplicate, bSendShow);

	//如果自己是怪物
	if(GetActorType() == ACT_MONSTER)
	{
		ConvertToDerived<CMonster>()->SetIsAISleep(false);
	}

	if(GetActorType() == ACT_PLAYER && bSendShow)
	{
		//发送对方信息给自己
		static_cast<CActor*>(pActor)->AddDelaySendShowTo(GetID());
	}
}

//////////////////////////////////////////////////////////////////////
void CActor::ClearViewList(bool bSendMsgToSelf)
{
	SC_AOI_REMOVE hold_info;
	for(uint64_t id: m_ViewActors)
	{
		// 通知对方自己消失
		CActor* pActor = ActorManager()->QueryActor(id);
		if(pActor)
		{
			pActor->RemoveFromViewList(this, GetID(), true);
			if(bSendMsgToSelf)
				_AddToAOIRemoveMessage(hold_info, pActor->GetID());
		}
	}
	//发送删除包

	SC_AOI_REMOVE ntc_aoiInfo;
	ntc_aoiInfo.set_mapid(GetMapID());
	ntc_aoiInfo.add_idlist(GetID());
	SendRoomMessage(CMD_SC_AOI_REMOVE, ntc_aoiInfo, false);

	if(bSendMsgToSelf)
		_TrySendAOIRemoveMessage(hold_info);

	m_ViewActorsByType.clear();
	m_ViewActors.clear();
	if(m_setDealySendShow.empty() == false)
	{
		GetEventMapRef().Cancel(EVENTID_BROCAST_SHOW);
	}
	m_setDealySendShow.clear();
}

void CActor::AddDelaySendShowTo(OBJID id)
{
	if(m_setDealySendShow.empty() == true)
	{
		auto pEntry = GetEventMapRef().Query(EVENTID_BROCAST_SHOW);
		if(pEntry == nullptr || pEntry->IsCanceled() || pEntry->IsRunning() == false)
		{
			EventManager()->ScheduleEvent(EVENTID_BROCAST_SHOW, std::bind(&CActor::SendShowToDealyList, this), 0, false, GetEventMapRef());
		}
	}
	m_setDealySendShow.insert(id);
}

void CActor::AddDelaySendShowToAllViewPlayer()
{
	if(m_setDealySendShow.empty() == true)
	{
		auto pEntry = GetEventMapRef().Query(EVENTID_BROCAST_SHOW);
		if(pEntry == nullptr || pEntry->IsCanceled() || pEntry->IsRunning() == false)
		{
			EventManager()->ScheduleEvent(EVENTID_BROCAST_SHOW, std::bind(&CActor::SendShowToDealyList, this), 0, false, GetEventMapRef());
		}
	}
	m_setDealySendShow.insert(m_ViewActorsByType[ACT_PLAYER].begin(), m_ViewActorsByType[ACT_PLAYER].end());
}

void CActor::RemoveDelaySendShowTo(OBJID id)
{
	if(m_setDealySendShow.empty())
		return;

	m_setDealySendShow.erase(id);
	if(m_setDealySendShow.empty())
	{
		GetEventMapRef().Cancel(EVENTID_BROCAST_SHOW);
	}
}

void CActor::SendShowToDealyList()
{
	if(m_setDealySendShow.empty())
		return;

	BrodacastShowTo(m_setDealySendShow);
	m_setDealySendShow.clear();
}

void CActor::SendShowTo(CPlayer* pPlayer)
{
	if(pPlayer == nullptr)
		return;

	SC_AOI_NEW msg;
	MakeShowData(msg);
	pPlayer->SendMessage(CMD_SC_AOI_NEW, msg);
}

class NEED_ADD_TO_BROADCASTSET_T
{
public:
	NEED_ADD_TO_BROADCASTSET_T()
	{
		m_DataMap[ACT_PLAYER][ACT_PLAYER] = [](CSceneObject*, CSceneObject*) {
			return true;
		};
		m_DataMap[ACT_PLAYER][ACT_MONSTER] = [](CSceneObject*, CSceneObject*) {
			return true;
		};
		m_DataMap[ACT_PLAYER][ACT_PET] = [](CSceneObject*, CSceneObject*) {
			return true;
		};
		m_DataMap[ACT_PLAYER][ACT_NPC] = [](CSceneObject*, CSceneObject*) {
			return true;
		};
		m_DataMap[ACT_PLAYER][ACT_BULLET] = [](CSceneObject*, CSceneObject*) {
			return true;
		};
		m_DataMap[ACT_PLAYER][ACT_MAPITEM] = [](CSceneObject*, CSceneObject*) {
			return true;
		};

		m_DataMap[ACT_MONSTER][ACT_PLAYER] = [](CSceneObject*, CSceneObject*) {
			return true;
		};
		m_DataMap[ACT_MONSTER][ACT_MONSTER] = [](auto self, auto target) {
			return self->IsEnemy(target);
		};
		m_DataMap[ACT_MONSTER][ACT_PET] = [](CSceneObject*, CSceneObject*) {
			return true;
		};

		m_DataMap[ACT_PET][ACT_PLAYER] = [](CSceneObject*, CSceneObject*) {
			return true;
		};
		m_DataMap[ACT_PET][ACT_MONSTER] = [](CSceneObject*, CSceneObject*) {
			return true;
		};
		m_DataMap[ACT_PET][ACT_PET] = [](CSceneObject*, CSceneObject*) {
			return true;
		};

		m_DataMap[ACT_BULLET][ACT_PLAYER] = [](CSceneObject*, CSceneObject*) {
			return true;
		};
		m_DataMap[ACT_BULLET][ACT_MONSTER] = [](CSceneObject*, CSceneObject*) {
			return true;
		};
		m_DataMap[ACT_BULLET][ACT_PET] = [](CSceneObject*, CSceneObject*) {
			return true;
		};
	}
	bool test(CSceneObject* pSelfActor, CSceneObject* pTargetActor) const
	{
		const auto& func = m_DataMap[pSelfActor->GetActorType()][pTargetActor->GetActorType()];
		if(func)
			return func(pSelfActor, pTargetActor);
		else
			return false;
	}

private:
	std::function<bool(CSceneObject*, CSceneObject*)> m_DataMap[ACT_MAX][ACT_MAX];
} const NEED_ADD_TO_BROADCASTSET;

bool CActor::IsNeedAddToBroadCastSet(CSceneObject* pActor)
{

	return NEED_ADD_TO_BROADCASTSET.test(this, pActor);
}

bool CActor::IsMustAddToBroadCastSet(CSceneObject* pActor)
{
	//必须加入视野的, BOSS怪, 组队成员
	if(pActor->GetActorType() == ACT_MONSTER)
	{
		CMonster* pMonster = pActor->ConvertToDerived<CMonster>();
		if(pMonster == nullptr)
			return false;

		//除了普通怪,其他的怪优先放入视野
		// return pMonster->GetViewPriority() != 0;
		return true;
	}
	else
	{
		CPlayer* pPlayer = pActor->ConvertToDerived<CPlayer>();
		if(pPlayer == nullptr)
			return false;

		// return (GetTeamID() != 0 && pPlayer->GetTeamID() == GetTeamID());
		return true;
	}
}

uint32_t CActor::GetMapID() const
{
	if(m_pScene)
		return m_pScene->GetMapID();
	return 0;
}

uint64_t CActor::GetSceneID() const
{
	if(m_pScene)
		return m_pScene->GetSceneID();
	return 0;
}

void CActor::AOIProcessActorRemoveFromAOI(const BROADCAST_SET& setBCActorDel, BROADCAST_SET& setBCActor, int32_t nCanReserveDelCount, uint32_t view_range_out_square)
{
	SC_AOI_REMOVE hold_info;
	hold_info.set_mapid(GetMapID());

	//////////////////////////////////////////////////////////////////////////
	// step4: 需要离开视野的角色Remove
	{
		BROADCAST_SET setBCActorDelPlayer;
		for(auto it = setBCActorDel.begin(); it != setBCActorDel.end(); it++)
		{
			// 如果对方还在脱离视野范围内，则不删除
			uint64_t id		= *it;
			CActor*	 pActor = ActorManager()->QueryActor(id);
			if(pActor == nullptr)
			{
				//如果对方已经消失,则移除
				RemoveFromViewList(nullptr, id, false);
				_AddToAOIRemoveMessage(hold_info, id);
				continue;
			}
			if(nCanReserveDelCount > 0)
			{
				if(view_range_out_square > 0)
				{
					uint32_t distance_square = GameMath::simpleDistance(GetPos(), pActor->GetPos());
					if(distance_square < view_range_out_square) // 在脱离视野半径内的，不需要离开广播区域)
					{
						nCanReserveDelCount--;
						setBCActor.insert(std::lower_bound(setBCActor.begin(), setBCActor.end(), id), id);
						continue;
					}
				}
			}

			// 对方已脱离视野

			// 通知自己对方消失
			//不需要从自己的m_ViewActors移除,因为等下会一次性移除,
			//为了减少发送次数,发送给自己的移除消息一次性发送
			RemoveFromViewList(pActor, id, false);
			_AddToAOIRemoveMessage(hold_info, pActor->GetID());
			// 通知对方自己消失,
			pActor->RemoveFromViewList(this, this->GetID(), true);
			//不发送移除消息,等下一次性广播
			if(pActor->GetActorType() == ACT_PLAYER)
			{
				setBCActorDelPlayer.push_back(id);
			}
		}

		//通知Del列表删除自己
		if(setBCActorDelPlayer.empty() == false)
		{

			SC_AOI_REMOVE ntc_aoiInfo;
			ntc_aoiInfo.set_mapid(GetMapID());
			ntc_aoiInfo.add_idlist(GetID());
			BroadcastMessageTo(setBCActorDelPlayer, CMD_SC_AOI_REMOVE, ntc_aoiInfo, 0);
		}
	}

	_TrySendAOIRemoveMessage(hold_info);
}

void CActor::AOIProcessPosUpdate()
//////////////////////////////////////////////////////////////////////////
//
{

	//发送移动同步
	SC_AOI_UPDATE ntc;
	ntc.set_mapid(GetMapID());
	ntc.set_actor_id(GetID());
	ntc.set_posx(GetPosX());
	ntc.set_posy(GetPosY());

	SendRoomMessage(CMD_SC_AOI_UPDATE, ntc);
}

void CActor::AOIProcessActorAddToAOI(BROADCAST_SET& setBCActorAdd, const ACTOR_MAP& mapAllViewActor)
{
	for(auto it = setBCActorAdd.begin(); it != setBCActorAdd.end();)
	{
		auto itr = mapAllViewActor.find(*it);
		if(itr != mapAllViewActor.end())
		{
			CActor* pActor = static_cast<CActor*>(itr->second);

			// 通知自己,需要发送对方的信息给自己
			AddToViewList(pActor, false, true);

			// 通知目标,不要发送自己的信息给对方,后面会统一广播
			pActor->AddToViewList(this, true, false);

			//如果目标不是Player,那么后面就不需要广播给他了
			if(pActor->GetActorType() != ACT_PLAYER)
			{
				it = setBCActorAdd.erase(it);
			}
			else
			{
				it++;
			}
		}
		else
		{
			it = setBCActorAdd.erase(it);
		}
	}

	//合并延迟发送Show的队列一起发送
	if(setBCActorAdd.size() > 0 || m_setDealySendShow.size() > 0)
	{
		if(m_setDealySendShow.empty())
		{
			BrodacastShowTo(setBCActorAdd);
		}
		else if(setBCActorAdd.empty())
		{
			BrodacastShowTo(m_setDealySendShow);
			m_setDealySendShow.clear();
			GetEventMapRef().Cancel(EVENTID_BROCAST_SHOW);
		}
		else
		{
			setBCActorAdd.insert(setBCActorAdd.end(), m_setDealySendShow.begin(), m_setDealySendShow.end());
			BrodacastShowTo(setBCActorAdd);
			m_setDealySendShow.clear();
			GetEventMapRef().Cancel(EVENTID_BROCAST_SHOW);
		}
	}
}

void CActor::SendRoomMessage(uint16_t cmd, const google::protobuf::Message& msg, bool bIncludeSelf /*= true*/)
{
	SendShowToDealyList();

	if(bIncludeSelf)
		ZoneService()->BroadcastMessageToPlayer(m_ViewActorsByType[ACT_PLAYER], cmd, msg, GetID());
	else
		ZoneService()->BroadcastMessageToPlayer(m_ViewActorsByType[ACT_PLAYER], cmd, msg, 0);

	// send message to ai_service
	if(cmd == CMD_SC_AOI_UPDATE || cmd == CMD_SC_CASTSKILL || cmd == CMD_SC_ATTRIB_CHANGE)
		ZoneService()->SendMsgToAIService(cmd, msg);
}

void CActor::SendWorldMessage(uint16_t cmd, const google::protobuf::Message& msg)
{
	if(GetWorldID() != 0)
		ZoneService()->SendPortMsg(ServerPort(GetWorldID(), WORLD_SERVICE_ID), cmd, msg);
}
