#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <vector>

#include "BaseCode.h"
#include "DBRecord.h"
#include "NetworkDefine.h"
#include "game_common_def.h"
#include "gamedb.h"

struct ST_ROLE_INFO
{
    OBJID              GetID() const { return m_pRecord->Field(TBLD_PLAYER::ID); }
    uint16_t           GetWorldID() const { return m_pRecord->Field(TBLD_PLAYER::WORLDID); }
    uint16_t           GetOriWorldID() const { return m_pRecord->Field(TBLD_PLAYER::WORLDID); }
    const std::string& GetOpenID() const { return m_pRecord->Field(TBLD_PLAYER::OPENID); }
    const std::string& GetName() const { return m_pRecord->Field(TBLD_PLAYER::NAME); }
    uint32_t           GetProf() const { return m_pRecord->Field(TBLD_PLAYER::PROF); }
    uint32_t           GetBaseLook() const { return m_pRecord->Field(TBLD_PLAYER::BASELOOK); }
    uint32_t           GetLev() const { return m_pRecord->Field(TBLD_PLAYER::LEV); }
    uint32_t           GetVipLev() const { return m_pRecord->Field(TBLD_PLAYER::VIPLEV); }
    uint64_t           GetRecordSceneID() const { return m_pRecord->Field(TBLD_PLAYER::RECORD_SCENEID); }
    uint64_t           GetHomeSceneID() const { return m_pRecord->Field(TBLD_PLAYER::HOME_SCENEID); }
    float              GetRecordPosX() const { return m_pRecord->Field(TBLD_PLAYER::RECORD_X); }
    float              GetRecordPosY() const { return m_pRecord->Field(TBLD_PLAYER::RECORD_Y); }
    float              GetRecordFace() const { return m_pRecord->Field(TBLD_PLAYER::RECORD_FACE); }
    float              GetHomePosX() const { return m_pRecord->Field(TBLD_PLAYER::HOME_X); }
    float              GetHomePosY() const { return m_pRecord->Field(TBLD_PLAYER::HOME_Y); }
    float              GetHomeFace() const { return m_pRecord->Field(TBLD_PLAYER::HOME_FACE); }
    uint64_t           GetTeamID() const { return m_pRecord->Field(TBLD_PLAYER::TEAMID); }
    uint64_t           GetGuildID() const { return m_pRecord->Field(TBLD_PLAYER::GUILDID); }
    uint64_t           GetMateID() const { return m_pRecord->Field(TBLD_PLAYER::MATE_ID); }

    void SetLastLoginTime(uint32_t v)
    {
        __ENTER_FUNCTION
        m_pRecord->Field(TBLD_PLAYER::LAST_LOGINTIME) = v;
        m_pRecord->Update();
        __LEAVE_FUNCTION
    }
    void SetLastLogoutTime(uint32_t v)
    {
        __ENTER_FUNCTION
        m_pRecord->Field(TBLD_PLAYER::LAST_LOGOUTTIME) = v;
        m_pRecord->Update();
        __LEAVE_FUNCTION
    }
    void SetTeamID(uint64_t v)
    {
        __ENTER_FUNCTION
        m_pRecord->Field(TBLD_PLAYER::TEAMID) = v;
        m_pRecord->Update();
        __LEAVE_FUNCTION
    }
    void SetGuildID(uint64_t v)
    {
        __ENTER_FUNCTION
        m_pRecord->Field(TBLD_PLAYER::GUILDID) = v;
        m_pRecord->Update();
        __LEAVE_FUNCTION
    }
    void SetDelTime(uint32_t v)
    {
        __ENTER_FUNCTION
        m_pRecord->Field(TBLD_PLAYER::DEL_TIME) = v;
        m_pRecord->Update();
        __LEAVE_FUNCTION
    }
    void ClearMate()
    {
        __ENTER_FUNCTION
        m_pRecord->Field(TBLD_PLAYER::MATE_ID)   = 0;
        m_pRecord->Field(TBLD_PLAYER::MATE_NAME) = "";
        m_pRecord->Update();
        __LEAVE_FUNCTION
    }

    ST_ROLE_INFO(CDBRecordPtr&& pRecord)
    :m_pRecord(std::move(pRecord))
    {}
    
    CDBRecordPtr m_pRecord;
};
using ST_ROLE_INFO_PTR = std::unique_ptr<ST_ROLE_INFO>;

class CUser;
class CAccount : public NoncopyableT<CAccount>
{
   
private:
    CAccount();
    bool Init(const std::string& open_id, const VirtualSocket& from);
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
    VirtualSocket              m_Socket;
    char                       m_openid[2048];
    std::vector<ST_ROLE_INFO_PTR> m_setActorInfo;
    CUser*                     m_pUser = nullptr;
    bool                       m_bWait = false;
};
#endif /* ACCOUNT_H */
