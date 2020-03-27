#ifndef PLAYER_H
#define PLAYER_H

#include "Actor.h"
#include "CommonData.h"
#include "DBRecord.h"
#include "DataCount.h"
#include "DynaScene.h"
#include "Equipment.h"
#include "NetworkDefine.h"
#include "Package.h"
#include "PetSet.h"
#include "PlayerAchievement.h"
#include "PlayerTask.h"
#include "Skill.h"
#include "StoragePackage.h"
#include "gamedb.h"

export_lua enum {
    DIALOG_FUNC_ACCEPTTASK      = 1,
    DIALOG_FUNC_SUBMITTASK      = 2,
    DIALOG_FUNC_QUICKFINISHTASK = 3,
    DIALOG_FUNC_SHOWTASK        = 4,
    DIALOG_FUNC_OPENSHOP        = 5,
    DIALOG_FUNC_SCRIPT          = 6,
};
export_lua enum {
    DIALOGTYPE_NORMAL          = 0,
    DIALOGTYPE_ACCEPT_TASK     = 1,
    DIALOGTYPE_WAITFINISH_TASK = 2,
    DIALOGTYPE_SUBMIT_TASK     = 3,
};

export_lua enum {
    DIALOGLINK_TYPE_BUTTON = 0, //底部按钮
    DIALOGLINK_TYPE_LIST   = 1, //中间的列表式按钮
};

class CNetworkMessage;

export_lua class CPlayer : public CActor
{
protected:
    CPlayer();
    bool Init(OBJID idPlayer, const VirtualSocket& socket);
public:
    CreateNewImpl(CPlayer);
public:
    virtual ~CPlayer();

    
    

    export_lua bool EnterDynaScene(CDynaScene* pScene, float fPosX, float fPosY, float fRange, float fFace);
    export_lua bool FlyMap(uint32_t idMap, float fPosX, float fPosY, float fRange, float fFace);

public:
    export_lua virtual bool SendMsg(uint16_t cmd, const google::protobuf::Message& msg) const override;

public:
    export_lua virtual ActorType GetActorType() const override { return ActorType::ACT_PLAYER; }
    export_lua static ActorType  GetActorTypeStatic() { return ActorType::ACT_PLAYER; }

    export_lua virtual float GetHeight() const { return 1.0f; }
    export_lua virtual float GetVolume() const { return 0.25f; }

    export_lua virtual uint16_t GetWorldID() const override { return m_pRecord->Field(TBLD_PLAYER::WORLDID); }
    export_lua const std::string&         GetOpenID() const { return m_pRecord->Field(TBLD_PLAYER::OPENID); }
    export_lua virtual const std::string& GetName() const override { return m_pRecord->Field(TBLD_PLAYER::NAME); }
    export_lua uint32_t                   GetProf() const { return m_pRecord->Field(TBLD_PLAYER::PROF); }
    export_lua uint32_t                   GetBaseLook() const { return m_pRecord->Field(TBLD_PLAYER::BASELOOK); }
    export_lua uint32_t                   GetVipLev() const { return m_pRecord->Field(TBLD_PLAYER::VIPLEV); }
    export_lua uint32_t                   GetPKVal() const { return m_pRecord->Field(TBLD_PLAYER::PKVAL); }
    export_lua uint32_t                   GetHonor() const { return m_pRecord->Field(TBLD_PLAYER::HONOR); }
    export_lua uint64_t         GetRecordSceneID() const { return m_pRecord->Field(TBLD_PLAYER::RECORD_SCENEID); }
    export_lua uint64_t         GetHomeSceneID() const { return m_pRecord->Field(TBLD_PLAYER::HOME_SCENEID); }
    export_lua virtual uint32_t GetLev() const override { return m_pRecord->Field(TBLD_PLAYER::LEV); }
    export_lua uint32_t         GetExp() const { return m_pRecord->Field(TBLD_PLAYER::EXP); }
    export_lua uint32_t         GetMoney() const { return m_pRecord->Field(TBLD_PLAYER::MONEY); }
    export_lua uint32_t         GetMoneyBind() const { return m_pRecord->Field(TBLD_PLAYER::MONEY_BIND); }
    export_lua uint32_t         GetGold() const { return m_pRecord->Field(TBLD_PLAYER::GOLD); }
    export_lua uint32_t         GetGoldBind() const { return m_pRecord->Field(TBLD_PLAYER::GOLD_BIND); }
    export_lua uint32_t         GetBagMaxSize() const;
    export_lua uint32_t         GetStrogeMaxSize() const;
    export_lua uint32_t         GetAchiPoint() const { return m_pRecord->Field(TBLD_PLAYER::ACHIPOINT); }
    export_lua virtual uint32_t GetHP() const override { return m_pRecord->Field(TBLD_PLAYER::HP); }
    export_lua virtual uint32_t GetMP() const override { return m_pRecord->Field(TBLD_PLAYER::MP); }
    export_lua virtual uint32_t GetFP() const override { return m_pRecord->Field(TBLD_PLAYER::FP); }
    export_lua virtual uint32_t GetNP() const override { return m_pRecord->Field(TBLD_PLAYER::NP); }
    export_lua virtual uint32_t GetHPMax() const override { return m_ActorAttrib[ATTRIB_HP_MAX]; }
    export_lua virtual uint32_t GetMPMax() const override { return m_ActorAttrib[ATTRIB_MP_MAX]; }
    export_lua virtual uint32_t GetFPMax() const override { return m_ActorAttrib[ATTRIB_FP_MAX]; }
    export_lua virtual uint32_t GetNPMax() const override { return m_ActorAttrib[ATTRIB_NP_MAX]; }
    export_lua virtual void     _SetHP(uint32_t v) override { m_pRecord->Field(TBLD_PLAYER::HP) = v; };
    export_lua virtual void     _SetMP(uint32_t v) override { m_pRecord->Field(TBLD_PLAYER::MP) = v; };
    export_lua virtual void     _SetFP(uint32_t v) override { m_pRecord->Field(TBLD_PLAYER::FP) = v; };
    export_lua virtual void     _SetNP(uint32_t v) override { m_pRecord->Field(TBLD_PLAYER::NP) = v; };

    export_lua float GetRecordPosX() const { return m_pRecord->Field(TBLD_PLAYER::RECORD_X); }
    export_lua float GetRecordPosY() const { return m_pRecord->Field(TBLD_PLAYER::RECORD_Y); }
    export_lua float GetRecordFace() const { return m_pRecord->Field(TBLD_PLAYER::RECORD_FACE); }
    export_lua float GetHomePosX() const { return m_pRecord->Field(TBLD_PLAYER::HOME_X); }
    export_lua float GetHomePosY() const { return m_pRecord->Field(TBLD_PLAYER::HOME_Y); }
    export_lua float GetHomeFace() const { return m_pRecord->Field(TBLD_PLAYER::HOME_FACE); }

    export_lua uint32_t GetPKMode() const { return m_nPKMode; }
    export_lua void     SetPKMode(uint32_t val);
    export_lua OBJID    GetTeamID() const { return m_pRecord->Field(TBLD_PLAYER::TEAMID); }
    export_lua void     SetTeamID(OBJID val)
    {
        m_pRecord->Update();
        m_pRecord->Field(TBLD_PLAYER::TEAMID) = val;
        m_pRecord->ClearDirty();
    }
    export_lua bool  HasTeam() const { return GetTeamID() != 0; }
    export_lua OBJID GetGuildID() const { return m_pRecord->Field(TBLD_PLAYER::GUILDID); }
    export_lua void  SetGuildID(OBJID val)
    {
        m_pRecord->Update();
        m_pRecord->Field(TBLD_PLAYER::GUILDID) = val;
        m_pRecord->ClearDirty();
    }

    export_lua uint32_t GetTeamMemberCount() const;
    export_lua uint32_t GetGuildLev() const;

    export_lua void BrodacastShow();
    export_lua bool IsTalkEnable(uint32_t nTalkChannel);

public:
    //属性
    export_lua virtual void     _SetProperty(uint32_t nType, uint32_t nVal, uint32_t nSync = SYNC_TRUE) override;
    export_lua virtual uint32_t GetPropertyMax(uint32_t nType) const override;
    export_lua virtual uint32_t GetProperty(uint32_t nType) const override;
    export_lua virtual bool     CanDamage(CActor* pTarget) const override;
    export_lua virtual void     BeKillBy(CActor* pAttacker) override;
    export_lua virtual bool     IsEnemy(CSceneObject* pTarget) const override;
    virtual void                MakeShowData(SC_AOI_NEW& msg) override;

public:
    // part
    export_lua CCommonDataSet* GetCommonDataSet() const { return m_pCommonDataSet.get(); }
    export_lua CDataCountSet* GetDataCountSet() const { return m_pDataCountSet.get(); }
    export_lua CPetSet* GetPetSet() const { return m_pPetSet.get(); }
    export_lua CUserSkillManager* GetSkillManager() const { return m_pUserSkillManager.get(); }
    export_lua CPackage* QueryPackage(uint32_t nPosition);
    export_lua CPackage* GetBag() { return m_pBag.get(); }
    export_lua CStoragePackage* GetStroagePackage();
    export_lua CEquipment* GetEquipmentSet() const { return m_pEquipmentSet.get(); }
    export_lua CPlayerTask* GetTaskSet() const { return m_pTaskSet.get(); }
    export_lua CPlayerAchievement* GetAchievement() const { return m_pAchievement.get(); }

public:
    //货币
    export_lua bool CheckMoney(uint32_t nMoneyType, uint32_t nVal);
    export_lua bool SpendMoney(uint32_t nMoneyType, uint32_t nVal);
    export_lua bool AwardMeony(uint32_t nMoneyType, uint32_t nVal);

public:
    //给以经验
    export_lua void AwardExp(uint32_t nExp);
    export_lua void AwardBattleExp(uint32_t nExp, bool bKillBySelf);

public:
    export_lua bool CheckItem(uint32_t idItemType, uint32_t nCount = 1, uint32_t dwFlag = 0);
    export_lua bool SpendItem(uint32_t nItemLogType,
                              uint32_t idItemType,
                              uint32_t nCount        = 1,
                              bool     bUseBindFirst = true);
    export_lua bool AwardItem(uint32_t nItemLogType, uint32_t idItemType, uint32_t nCount, uint32_t dwFlag);
    export_lua bool UseItem(uint32_t nGridInBag, uint32_t nCount = 1);

public:
    void TeamCreate();
    void TeamQuit();
    void TeamKickMember(OBJID idMember);
    void TeamInviteMember(OBJID idTarget);
    void TeamAcceptInvite(uint64_t idTeam, OBJID idInviter, bool bResult);
    void TeamApplyMember(OBJID idTarget);
    void TeamAcceptApply(OBJID idApplicant, bool bResult);
    void TeamChangeLeader(OBJID idMember);

public:
    export_lua void SendTalkMsg(uint32_t nTalkChannel, const std::string& txt);

    //回写数据库
    void SaveInfo();
    //处理网络消息
    void             ProcessMsg();
    export_lua const VirtualSocket& GetSocket() const { return m_Socket; }
    export_lua virtual void         RecalcAttrib(bool bClearCache = false) override;

public:
    void OnRecvGameData(CNetworkMessage* pMsg);

    virtual void OnEnterMap(CSceneBase* pScene) override;
    virtual void OnLeaveMap(uint64_t idTargetScene) override;

public:
    void OnTimer();
    void OnLogin(bool bLogin, const SceneID& idScene, float fPosX, float fPosY, float fRange, float fFace);
    void OnLogout();
    void OnChangeZoneSaveFinish(const SceneID& idScene, float fPosX, float fPosY, float fRange, float fFace);
    void OnLoadMapSucc();

public:
    export_lua bool Reborn(uint32_t nRebornType);
    bool            TryChangeMap(uint32_t nLeavePointIdx);

public:
    export_lua bool DialogBegin(const std::string& title);
    export_lua bool DialogAddText(const std::string& txt);
    export_lua bool DialogAddLink(uint32_t           nLinkType,
                                  const std::string& link_txt,
                                  uint32_t           idFuncType,
                                  uint64_t           idData,
                                  const std::string& callback_func,
                                  uint64_t           idNpc = 0);
    export_lua bool DialogSend(uint32_t nDialogType = 0);
    bool            OnDialogClick(uint64_t idDialog, uint32_t nIdx);

    export_lua bool ActiveNpc(OBJID idNpc);

private:
    void _ChangeZone(const SceneID& idScene, float fPosX, float fPosY, float fRange, float fFace);
    void _FlyMap(const SceneID& idScene, float fPosX, float fPosY, float fRange, float fFace);
    void SendGameData(const SceneID& idScene);

    void SendPlayerInfoToClient();
    void SendAttribToClient();
    void OnLevUp(uint32_t nLev);

public:
    MEMORYHEAP_DECLARATION(s_heap);

private:
    CDBRecordPtr                       m_pRecord;
    VirtualSocket                      m_Socket;
    CEventEntryPtr                     m_pEventOnTimer;
    std::deque<CNetworkMessage*>       m_MessageList;
    std::unique_ptr<CCommonDataSet>    m_pCommonDataSet;
    std::unique_ptr<CDataCountSet>     m_pDataCountSet;
    std::unique_ptr<CPetSet>           m_pPetSet;
    std::unique_ptr<CUserSkillManager> m_pUserSkillManager;

    uint64_t m_idLoadingScene = 0;
    float    m_fLoadingPosX   = 0.0f;
    float    m_fLoadingPosY   = 0.0f;
    float    m_fLoadingFace   = 0.0f;

    uint32_t m_nPKMode = 0;
    OBJID    m_idTeam  = 0;
    OBJID    m_idGuild = 0;

    struct ST_CALLBACK_INFO
    {
        uint32_t    idFuncType;
        uint64_t    idData;
        std::string callback_func;
        uint64_t    idNpc;
    };
    std::map<uint64_t, std::vector<ST_CALLBACK_INFO>> m_dialog_callback;

    SC_DIALOG m_dialog_msg;

    std::unique_ptr<CPackage>           m_pBag;
    std::unique_ptr<CStoragePackage>    m_pStoragePackage;
    std::unique_ptr<CEquipment>         m_pEquipmentSet;
    std::unique_ptr<CPlayerTask>        m_pTaskSet;
    std::unique_ptr<CPlayerAchievement> m_pAchievement;
};
#endif /* PLAYER_H */
