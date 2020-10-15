#ifndef USER_H
#define USER_H

#include "BaseCode.h"
#include "NetworkDefine.h"
#include "ObjectHeap.h"
class CAccount;
struct ST_ROLE_INFO;

class CUser : public NoncopyableT<CUser>
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
    bool SendMsg(const proto_msg_t& msg);

    uint32_t           GetLev() const;
    const std::string& GetName() const;
    uint64_t           GetTeamID() const;
    uint64_t           GetGuildID() const;
    void               SetTeamID(uint64_t v) { m_idTeam = v; }
    void               SetGuildID(uint64_t v) { m_idGuild = v; }

    bool ChangeName(const std::string& new_name);
    void OnLevChg(uint32_t v);
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

    uint64_t m_idGuild = 0;
    uint64_t m_idTeam  = 0;
};
#endif /* USER_H */
