#ifndef ZoneService_h__
#define ZoneService_h__

#include "AchievementType.h"
#include "ActorManager.h"
#include "BulletType.h"
#include "DataCount.h"
#include "GMManager.h"
#include "GameEventDef.h"
#include "IService.h"
#include "IStatus.h"
#include "ItemAddition.h"
#include "ItemFormula.h"
#include "ItemType.h"
#include "ItemUpgrade.h"
#include "LoadingThread.h"
#include "MapManager.h"
#include "MonsterType.h"
#include "MyTimer.h"
#include "NetSocket.h"
#include "NpcType.h"
#include "PetType.h"
#include "SceneManager.h"
#include "ScriptManager.h"
#include "ServiceComm.h"
#include "SkillType.h"
#include "SuitEquip.h"
#include "SystemVars.h"
#include "TaskType.h"
#include "TeamInfoManager.h"
#include "UserAttr.h"
#include "game_common_def.h"
#include "gamedb.h"
#include "globaldb.h"
#include "MonitorMgr.h"
#include "server_msg/server_side.pb.h"
#include "msg/ts_cmd.pb.h"
#include "msg/zone_service.pb.h"

struct event;
class CNetMSGProcess;
export_lua class CZoneService : public IService, public CServiceCommon
{
public:
	CZoneService(const ServerPort& nServerPort);
	virtual ~CZoneService();
	void Release() override { delete this; }
	bool Create();

public:
	virtual void OnLogicThreadProc() override;
	virtual void OnLogicThreadCreate() override;
	virtual void OnLogicThreadExit() override;

	virtual void OnProcessMessage(CNetworkMessage*) override;

public:
	void						  CreateSocketMessagePool(const VirtualSocket& vs);
	void						  DelSocketMessagePool(const VirtualSocket& vs);
	void						  PushMsgToMessagePool(const VirtualSocket& vs, CNetworkMessage* pMsg);
	std::deque<CNetworkMessage*>& GetMessagePoolRef(const VirtualSocket& vs);
	bool						  PopMsgFromMessagePool(const VirtualSocket& vs, CNetworkMessage*& pMsg);
	//发送消息给World
	export_lua bool SendMsgToWorld(uint16_t idWorld, uint16_t nCmd, const google::protobuf::Message& msg);
	//通过World转发消息
	export_lua bool TransmiteMsgFromWorldToOther(uint16_t idWorld, uint16_t idService, uint16_t nCmd, const google::protobuf::Message& msg);
	//转发消息给其他的zone
	export_lua bool BroadcastToZone(uint16_t nCmd, const google::protobuf::Message& msg);
	//广播消息给所有的玩家
	export_lua bool BroadcastToAllPlayer(uint16_t nCmd, const google::protobuf::Message& msg);
	//发送消息给玩家
	export_lua bool SendMsgToPlayer(const VirtualSocket& vs, uint16_t nCmd, const google::protobuf::Message& msg);
	//发送消息给AIService
	export_lua bool SendMsgToAIService(uint16_t nCmd, const google::protobuf::Message& msg);

	//发送广播包给玩家
	using VSMap_t = std::unordered_map<ServerPort, std::vector<VirtualSocket>>;
	void _ID2VS(OBJID id, VSMap_t& VSMap);
	template<class T>
	bool BroadcastMessageToPlayer(T&& idList, uint16_t nCmd, const google::protobuf::Message& msg, OBJID idExtInclude = 0)
	{
		VSMap_t setSocketMap;
		if constexpr(is_container<base_type<T>>::value)
		{
			if constexpr(is_associative_container<base_type<T>>::value)
			{
				for(const auto& [id, v]: idList)
				{
					_ID2VS(id, setSocketMap);
				}
			}
			else
			{
				for(OBJID id: idList)
				{
					_ID2VS(id, setSocketMap);
				}
			}
		}
		else if constexpr(std::is_invocable<base_type<T>>::value)
		{
			idList([this, &setSocketMap](OBJID id) { _ID2VS(id, setSocketMap); });
		}
		else
		{
			static_assert(!std::is_same<T, T>::value, "T can't convert idList");
			return false;
		}

		if(idExtInclude != 0)
			_ID2VS(idExtInclude, setSocketMap);

		CNetworkMessage _msg(nCmd, msg, GetServerVirtualSocket());
		for(auto& [nServerPort, socket_list]: setSocketMap)
		{
			if(socket_list.size() == 1)
			{
				_msg.SetTo(socket_list.front());
				m_MonitorMgr.AddSendInfo(nCmd, _msg.GetSize());
				SendMsg(_msg);
			}
			else
			{
				m_MonitorMgr.AddSendInfo_some(nCmd, _msg.GetSize(), socket_list.size());
				SendPortMultiMsg(nServerPort, socket_list, _msg);
			}
		}
		return true;
	}

public:
	export_lua CMysqlConnection* GetGlobalDB() const { return m_pGlobalDB.get(); }
	export_lua CMysqlConnection* GetGameDB(uint16_t nWorldID);
	void						 ReleaseGameDB(uint16_t nWorldID);

	export_lua CLUAScriptManager* GetScriptManager() const { return m_pScriptManager.get(); }
	export_lua CMapManager* GetMapManager() const { return m_pMapManager.get(); }
	export_lua CSystemVarSet* GetSystemVarSet() const { return m_pSystemVarSet.get(); }

	export_lua CActorManager* GetActorManager() { return &m_ActorManager; }
	export_lua CSceneManager* GetSceneManager() { return &m_SceneManager; }
	export_lua CLoadingThread* GetLoadingThread() { return m_pLoadingThread; }
	export_lua CMonitorMgr* GetMonitorMgr() { return &m_MonitorMgr; }
	export_lua CGMManager* GetGMManager() { return m_pGMManager.get(); }
	export_lua CTeamInfoManager* GetTeamInfoManager() { return &m_TeamInfoManager; }

public:
	void OnMsgPlayerEnterZone(CNetworkMessage* pMsg);
	void OnMsgPlayerChangeZone(CNetworkMessage* pMsg);

	void OnMsgPlayerChangeZone_Data(CNetworkMessage* pMsg);
	void OnMsgMonsterCreate(CNetworkMessage* pMsg);
	void OnMsgPlayerLogout(CNetworkMessage* pMsg);

private:
	void ProcessPortMessage();

private:
	std::unique_ptr<CMysqlConnection>								m_pGlobalDB = nullptr;
	std::unordered_map<uint16_t, std::unique_ptr<CMysqlConnection>> m_GameDBMap;

	CMyTimer m_tLastDisplayTime;
	size_t	 m_nMessageProcess = 0;

	CActorManager	m_ActorManager;
	CSceneManager	m_SceneManager;
	CLoadingThread* m_pLoadingThread;

	std::unordered_map<uint64_t, std::deque<CNetworkMessage*>> m_MessagePoolBySocket;

	std::unique_ptr<CGMManager>		   m_pGMManager;
	std::unique_ptr<CLUAScriptManager> m_pScriptManager;
	std::unique_ptr<CMapManager>	   m_pMapManager;
	std::unique_ptr<CSystemVarSet>	   m_pSystemVarSet;
	CMonitorMgr						   m_MonitorMgr;
	CTeamInfoManager				   m_TeamInfoManager;

public:
	//配置文件

#define DEFINE_CONFIG_SET(T)                   \
public:                                        \
	T* Get##T() const { return m_p##T.get(); } \
private:                                       \
	std::unique_ptr<T> m_p##T;

	DEFINE_CONFIG_SET(CStatusTypeSet);
	DEFINE_CONFIG_SET(CUserAttrSet);
	DEFINE_CONFIG_SET(CDataCountLimitSet);
	DEFINE_CONFIG_SET(CSkillTypeSet);
	DEFINE_CONFIG_SET(CSkillAttachStatusDataSet);
	DEFINE_CONFIG_SET(CSkillDetachStatusDataSet);
	DEFINE_CONFIG_SET(CMonsterTypeSet);
	DEFINE_CONFIG_SET(CBulletTypeSet);
	DEFINE_CONFIG_SET(CItemTypeSet);
	DEFINE_CONFIG_SET(CItemAdditionSet);
	DEFINE_CONFIG_SET(CItemFormulaDataSet);
	DEFINE_CONFIG_SET(CItemUpgradeDataSet);
	DEFINE_CONFIG_SET(CSuitEquipSet);
	DEFINE_CONFIG_SET(CTaskTypeSet);
	DEFINE_CONFIG_SET(CAchievementTypeSet);
	DEFINE_CONFIG_SET(CNpcTypeSet);
	DEFINE_CONFIG_SET(CPetTypeSet);
#undef DEFINE_CONFIG_SET
};

CZoneService* ZoneService();
void		  SetZoneServicePtr(CZoneService*);

inline auto EventManager()
{
	return ZoneService()->GetEventManager();
}
inline auto NetMsgProcess()
{
	return ZoneService()->GetNetMsgProcess();
}
inline auto ScriptManager()
{
	return ZoneService()->GetScriptManager();
}
inline auto ActorManager()
{
	return ZoneService()->GetActorManager();
}
inline auto SceneManager()
{
	return ZoneService()->GetSceneManager();
}
inline auto MapManager()
{
	return ZoneService()->GetMapManager();
}
inline auto SystemVarSet()
{
	return ZoneService()->GetSystemVarSet();
}
inline auto MonitorMgr()
{
	return ZoneService()->GetMonitorMgr();
}
inline auto GMManager()
{
	return ZoneService()->GetGMManager();
}
inline auto TeamManager()
{
	return ZoneService()->GetTeamInfoManager();
}

inline auto StatusTypeSet()
{
	return ZoneService()->GetCStatusTypeSet();
}
inline auto UserAttrSet()
{
	return ZoneService()->GetCUserAttrSet();
}
inline auto DataCountLimitSet()
{
	return ZoneService()->GetCDataCountLimitSet();
}
inline auto SkillTypeSet()
{
	return ZoneService()->GetCSkillTypeSet();
}
inline auto SkillAttachStatusDataSet()
{
	return ZoneService()->GetCSkillAttachStatusDataSet();
}
inline auto SkillDetachStatusDataSet()
{
	return ZoneService()->GetCSkillDetachStatusDataSet();
}
inline auto MonsterTypeSet()
{
	return ZoneService()->GetCMonsterTypeSet();
}
inline auto BulletTypeSet()
{
	return ZoneService()->GetCBulletTypeSet();
}
inline auto ItemTypeSet()
{
	return ZoneService()->GetCItemTypeSet();
}
inline auto ItemAdditionSet()
{
	return ZoneService()->GetCItemAdditionSet();
}
inline auto ItemFormulaDataSet()
{
	return ZoneService()->GetCItemFormulaDataSet();
}
inline auto ItemUpgradeDataSet()
{
	return ZoneService()->GetCItemUpgradeDataSet();
}
inline auto SuitEquipSet()
{
	return ZoneService()->GetCSuitEquipSet();
}
inline auto TaskTypeSet()
{
	return ZoneService()->GetCTaskTypeSet();
}
inline auto AchievementTypeSet()
{
	return ZoneService()->GetCAchievementTypeSet();
}
inline auto NpcTypeSet()
{
	return ZoneService()->GetCNpcTypeSet();
}
inline auto PetTypeSet()
{
	return ZoneService()->GetCPetTypeSet();
}

#endif // ZoneService_h__
