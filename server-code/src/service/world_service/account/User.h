#ifndef USER_H
#define USER_H

#include "BaseCode.h"
#include "ObjectHeap.h"
#include "NetworkDefine.h"
#include "NetworkMessage.h"
class CAccount;
struct ST_ROLE_INFO;

class CUser: public NoncopyableT<CUser>
{   
    CUser();
    bool Init(CAccount* pAccount, ST_ROLE_INFO* pInfo);
public:
    CreateNewImpl(CUser);
public:
    ~CUser();
    

    const VirtualSocket& GetSocket() const;
    //进入游戏
    void EnterZone();
    //切换zone
    void OnChangeZone(uint16_t idZone);
    //退出游戏
    void Logout();
    bool SendMsg(const google::protobuf::Message& msg);
    bool SendMsg(uint16_t usCmd, const google::protobuf::Message& msg);

    uint32_t           GetLev() const;
    const std::string& GetName() const;
    uint64_t           GetTeamID() const;
    uint64_t           GetGuildID() const;

    bool ChangeName(const std::string& new_name);
    void OnLevChg(uint32_t v);
    void SetTeamID(uint64_t v);
    void SetGuildID(uint64_t v);
    void ClearMate();

public:
    OBJID    GetID() const;
    uint16_t GetZoneID() const;

public:
    OBJECTHEAP_DECLARATION(s_heap);

private:
    CAccount*     m_pAccount = nullptr;
    ST_ROLE_INFO* m_pInfo    = nullptr;
    //所在zone服务器
    uint16_t m_idZone = 0;
    uint32_t m_nLev;
};
#endif /* USER_H */
