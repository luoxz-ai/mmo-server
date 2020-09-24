#ifndef MessageRoute_h__
#define MessageRoute_h__

#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <unordered_set>

#include "MysqlConnection.h"
#include "NetworkDefine.h"
#include "SettingMap.h"

class CMessagePort;
class CNetworkService;
class CServiceControl;
class CEventManager;

//每个Service会有一个自己的MessagePort, 当Service开启时会开始监听socket
// Service与Service之间的通讯, 如果在一个Exe内, 那么可以直接找到MessagePort并放入队列,
//如果MessagePort使用的是Remote端口,那么就会连接到远端并使用Socket发送到另外的Exe上
//也就说, Service直接的连接通道是双通道的

class CMessageRoute : NoncopyableT<CMessageRoute>
{
public:
    CMessageRoute();
    virtual ~CMessageRoute();

    uint16_t GetWorldID() const { return m_nWorldID; }
    void     SetWorldID(uint16_t val) { m_nWorldID = val; }
    void     Destory();

public:
    //从配置文件读取数据库地址,并重读配置
    bool LoadServiceSetting(const std::string& filename, uint16_t nWorldID);
    //创建所有本地端口
    bool CreateAllMessagePort(uint16_t nWorldID, const std::set<ServiceID>& create_service_set);
    //从数据库读取Service配置
    void ReloadServiceInfo(uint32_t update_time, uint16_t nNewWorldID);
    //查询服务地址
    const ServerAddrInfo* QueryServiceInfo(const ServerPort& nServerPort);
    //遍历服务
    using ForeachServiceInfoFunc = std::function<bool(const ServerAddrInfo*)>;
    using ForeachServerPortFunc  = std::function<bool(const ServerPort&)>;
    void           ForeachServiceInfoByWorldID(uint16_t idWorld, bool bIncludeShare, ForeachServiceInfoFunc&& func);
    ServerPortList GetServerPortListByWorldID(uint16_t idWorld, bool bIncludeShare);
    ServerPortList GetServerPortListByWorldIDAndServiceType(uint16_t idWorld, uint8_t idServiceType, bool bIncludeShare);
    ServerPortList GetServerPortListByWorldIDExcept(uint16_t idWorld, uint8_t idExceptServiceType, bool bIncludeShare);
    ServerPortList GetServerPortListByServiceType(uint8_t idServiceType);

    //判断是否连接
    bool IsConnected(const ServerPort& nServerPort);
    //返回服务器被合并到哪里了
    uint16_t GetMergeTo(uint16_t idWorld);
    //查询端口,如果不存在,是否自动连接远端
    CMessagePort* QueryMessagePort(const ServerPort& nServerPort, bool bAutoConnectRemote = true);
    //连接远端,一般无需手动调用
    CMessagePort* ConnectRemoteServer(const ServerPort& nServerPort);
    //关闭端口
    void CloseMessagePort(CMessagePort*& pMessagePort);
    //遍历所有端口
    void ForEach(const std::function<void(CMessagePort*)>& func);
    //通过worldid连接所有远端端口
    void ConnectAllRemoteServerWithWorldID(uint16_t nWorldID);
    //关闭对应端口
    void _CloseRemoteServerByWorldID(uint16_t idWorld);
    void _CloseRemoteServer(const ServerPort& nServerPort);

public:
    bool           IsWorldReady(uint16_t idWorld);
    void           SetWorldReady(uint16_t idWorld, bool bReady);
    decltype(auto) GetWorldReadyList()
    {
        std::unique_lock<std::mutex> locker(m_mutex);
        return m_WorldReadyList;
    }

public:
    CSettingMap&       GetSettingMap() { return m_setDataMap; }
    const CSettingMap& GetSettingMap() const { return m_setDataMap; }
    CEventManager*     GetEventManager() const { return m_pEventManager.get(); }

protected:
    bool ConnectServerInfoDB(const std::string& host, const std::string& user, const std::string& password, const std::string& db, uint32_t port);
    CMessagePort* _ConnectRemoteServer(const ServerPort& nServerPort, const ServerAddrInfo& info);
    //监听本地,一般无需手动调用, CreateAllMessagePort时已经调用了
    CMessagePort*         _ListenMessagePort(const ServerPort& nServerPort, const ServerAddrInfo& info);
    const ServerAddrInfo* _QueryServiceInfo(const ServerPort& nServerPort);

    void _ReadMergeList();
    void _ReadServerIPList(uint16_t nNewWorldID);
    void OnServerAddrInfoChange(const ServerPort& serverport, const ServerAddrInfo& new_info);

protected:
    CSettingMap m_setDataMap;
    uint16_t    m_nWorldID;

    std::mutex                                    m_mutex;
    std::unordered_map<ServerPort, CMessagePort*> m_setMessagePort;

    std::unique_ptr<CNetworkService>  m_pNetworkService;
    std::unique_ptr<CEventManager>    m_pEventManager;
    std::unique_ptr<CMysqlConnection> m_pServerInfoDB;

    uint32_t m_lastUpdateTime = 0;

    std::unordered_map<ServerPort, ServerAddrInfo>                                            m_setServerInfo;
    std::unordered_map<uint16_t, std::map<ServerPort, ServerAddrInfo, std::less<ServerPort>>> m_setServerInfoByWorldID;

    std::unordered_map<uint16_t, uint16_t> m_MergeList;
    std::unordered_map<uint16_t, time_t>   m_WorldReadyList;
};

CMessageRoute* GetMessageRoute();
void           CreateMessageRoute();
void           ReleaseMessageRoute();

#endif // MessageRoute_h__
