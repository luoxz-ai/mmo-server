#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <vector>

#include "BaseCode.h"
#include "NetworkDefine.h"

struct ST_ROLE_INFO;

using ST_ROLE_INFO_PTR = std::unique_ptr<ST_ROLE_INFO>;
class CUser;
class CAccount : public NoncopyableT<CAccount>
{

private:
    CAccount();
    bool                Init(const std::string& open_id, const VirtualSocket& from);
    size_t              GetRoleAmount() const;
    const ST_ROLE_INFO* QueryRoleByIndex(size_t nIdx);

public:
    ~CAccount();
    CreateNewImpl(CAccount);

    void ReloadActorInfo();
    //创建角色
    bool CreateActor(const std::string& name, uint32_t dwProf, uint32_t dwLook);
    //选择角色进入游戏
    void SelectActor(size_t nIdx);
    void DelActor(size_t nIdx);
    //返回角色选择界面
    void ExitZone(bool bReload = false);
    // Kickout
    void KickOut();

    //发送角色列表
    void SendActorInfo();
    void SendWaitInfo();

public:
    void SetWait(bool bWait);
    bool IsWait();

    const std::string GetOpenID() const { return m_openid; }

    const VirtualSocket& GetSocket() const { return m_Socket; }
    void                 SetSocket(const VirtualSocket& val) { m_Socket = val; }

public:
    OBJECTHEAP_DECLARATION(s_heap);

private:
    VirtualSocket                 m_Socket;
    char                          m_openid[2048];
    std::vector<ST_ROLE_INFO_PTR> m_setActorInfo;
    CUser*                        m_pUser = nullptr;
    bool                          m_bWait = false;
};
#endif /* ACCOUNT_H */
