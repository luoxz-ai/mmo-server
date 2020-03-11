#ifndef WorldService_h__
#define WorldService_h__

#include "AccountManager.h"
#include "BornPos.h"
#include "GMManager.h"
#include "IService.h"
#include "MapManager.h"
#include "MyTimer.h"
#include "NetMSGProcess.h"
#include "NetSocket.h"
#include "ServiceComm.h"
#include "SystemVars.h"
#include "TeamManager.h"
#include "UIDFactory.h"
#include "UserAttr.h"
#include "UserManager.h"
#include "globaldb.h"
#include "server_msg/server_side.pb.h"
#include "msg/ts_cmd.pb.h"
#include "msg/world_service.pb.h"

struct event;
class CNetMSGProcess;

class CWorldService : public IService, public CServiceCommon
{
public:
	CWorldService(const ServerPort& nServerPort);
	virtual ~CWorldService();
	void Release() override { delete this; }
	bool Create();

public:
	virtual void OnLogicThreadProc() override;
	virtual void OnLogicThreadCreate() override;
	virtual void OnLogicThreadExit() override;

	virtual void OnProcessMessage(CNetworkMessage*) override;

public:
	uint64_t CreatePlayerID();
	void	 RecyclePlayerID(OBJID idPlayer);
	void	 SetServiceReady(uint16_t idService);

	bool CheckProgVer(const std::string& prog_ver);
	bool BroadcastToZone(uint16_t nCmd, const google::protobuf::Message& msg);
	bool BroadcastToAllPlayer(uint16_t nCmd, const google::protobuf::Message& msg);
	//发送广播包给玩家
	using VSMap_t = std::unordered_map<ServerPort, std::vector<VirtualSocket>>;
	void _ID2VS(OBJID id, VSMap_t& VSMap);
	template<class T>
	bool BroadcastMessageToPlayer(const T& idList, uint16_t nCmd, const google::protobuf::Message& msg, OBJID idExtInclude = 0)
	{
		VSMap_t setSocketMap;
		if constexpr(std::is_same<typename T::value_type, OBJID>::value)
		{
			for(OBJID id: idList)
			{
				_ID2VS(id, setSocketMap);
			}
		}
		else if constexpr(std::is_function<T>::value)
		{
			idList([this, &setSocketMap](OBJID id) { _ID2VS(id, setSocketMap); });
		}
		else
		{
			for(const auto& [id, v]: idList)
			{
				_ID2VS(id, setSocketMap);
			}
		}

		if(idExtInclude != 0)
			_ID2VS(idExtInclude, setSocketMap);

		CNetworkMessage _msg(nCmd, msg, GetServerVirtualSocket());
		for(auto& [nServerPort, socket_list]: setSocketMap)
		{
			if(socket_list.size() == 1)
			{
				_msg.SetTo(socket_list.front());
				SendMsg(_msg);
			}
			else
			{
				SendPortMultiMsg(nServerPort, socket_list, _msg);
			}
		}
		return true;
	}

public:
	CAccountManager*  GetAccountManager() const { return m_pAccountManager.get(); }
	CUserManager*	  GetUserManager() const { return m_pUserManager.get(); }
	CUserAttrSet*	  GetUserAttrSet() const { return m_pUserAttrSet.get(); }
	CBornPosSet*	  GetBornPosSet() const { return m_pBornPosSet.get(); }
	CMapManager*	  GetMapManager() const { return m_pMapManager.get(); }
	CMysqlConnection* GetGlobalDB() const { return m_pGlobalDB.get(); }
	CMysqlConnection* GetGameDB() const { return m_pGameDB.get(); }
	CSystemVarSet*	  GetSystemVarSet() const { return m_pSystemVarSet.get(); }
	CGMManager*		  GetGMManager() const { return m_pGMManager.get(); }
	CTeamManager*	  GetTeamManager() const { return m_pTeamManager.get(); }

private:
	uint64_t					 m_nCurPlayerMaxID;
	std::deque<OBJID>			 m_setPlayerIDPool;
	std::unordered_set<uint16_t> m_setServiceNeedReady;

	std::unique_ptr<CMysqlConnection> m_pGlobalDB;
	std::unique_ptr<CMysqlConnection> m_pGameDB;

	std::unique_ptr<CUserManager>	 m_pUserManager;
	std::unique_ptr<CAccountManager> m_pAccountManager;
	std::unique_ptr<CGMManager>		 m_pGMManager;

	std::unique_ptr<CUserAttrSet>  m_pUserAttrSet;
	std::unique_ptr<CBornPosSet>   m_pBornPosSet;
	std::unique_ptr<CMapManager>   m_pMapManager;
	std::unique_ptr<CSystemVarSet> m_pSystemVarSet;
	std::unique_ptr<CTeamManager>  m_pTeamManager;

	CMyTimer m_tLastDisplayTime;
};

CWorldService* WorldService();
void		   SetWorldServicePtr(CWorldService* ptr);

inline auto GMManager()
{
	return WorldService()->GetGMManager();
}
inline auto EventManager()
{
	return WorldService()->GetEventManager();
}
inline auto NetMsgProcess()
{
	return WorldService()->GetNetMsgProcess();
}
inline auto AccountManager()
{
	return WorldService()->GetAccountManager();
}
inline auto UserManager()
{
	return WorldService()->GetUserManager();
}
inline auto UserAttrSet()
{
	return WorldService()->GetUserAttrSet();
}
inline auto BornPosSet()
{
	return WorldService()->GetBornPosSet();
}
inline auto MapManager()
{
	return WorldService()->GetMapManager();
}
inline auto SystemVarSet()
{
	return WorldService()->GetSystemVarSet();
}
inline auto TeamManager()
{
	return WorldService()->GetTeamManager();
}

#endif // WorldService_h__
