#ifndef SCENE_H
#define SCENE_H

#include "EventEntry.h"
#include "SceneBase.h"
#include "ZoneService.h"

export_lua enum SceneState {
    SCENESTATE_NORMAL       = 0,
    SCENESTATE_WAIT_LOADING = 1,
    SCENESTATE_WAIT_DESTORY = 2,

};

class CActor;
class CMonster;
class CNpc;
export_lua class CScene : public CSceneBase 
{
protected:
    CScene();
public:
    CreateNewImpl(CScene);
public:
    virtual ~CScene();
    

public:
    bool            Init(const SceneID& idScene);
    void            Destory();
    export_lua bool NeedDestory() const;
    export_lua bool CanDestory();

    export_lua void KickAllPlayer(const char* pszReason = "");

    export_lua void _KickPlayer(const char* pszReason, CPlayer* pPlayer);

    export_lua uint32_t     GetSceneState() const { return m_curSceneState; }
    export_lua void         SetSceneState(uint32_t val) { m_curSceneState = val; }
    export_lua virtual void AddDynaRegion(uint32_t nRegionType, const FloatRect& rect) override;
    export_lua virtual void ClearDynaRegion(uint32_t nRegionType) override;

public:
    export_lua bool SendSceneMessage(uint16_t cmd, const google::protobuf::Message& msg);

    //发送所有mapval以及和该玩家id一样的mapuserval 给玩家
    export_lua void SendAllMapValToClient(CActor* pPlayer);
    //清理所有mapval
    export_lua void ClearAllMapVal();

    //设置mapval,并广播
    export_lua void    SetMapVal(uint32_t nIdx, int64_t nVal, bool bSync = false);
    export_lua int64_t GetMapVal(uint32_t nIdx);
    export_lua int64_t AddMapVal(uint32_t nIdx, int64_t nVal, bool bSync = false);
    export_lua void    ClearMapVal();
    //广播所有mapval
    export_lua void SyncAllMapVal();

    //根据玩家id设置mapval,并广播
    export_lua void    SetMapUserVal(uint64_t idUser, uint32_t nIdx, int64_t nVal, bool bSync = false);
    export_lua int64_t GetMapUserVal(uint64_t idUser, uint32_t nIdx);
    export_lua int64_t AddMapUserVal(uint64_t idUser, uint32_t nIdx, int64_t nVal, bool bSync = false);
    export_lua void    ClearMapUserVal(uint64_t idUser);
    export_lua void    ClearAllMapUserVal();

    //增加一个延时回调脚本
    export_lua void AddTimedCallback(uint32_t tIntervalMS, const std::string& func_name);
    export_lua void ClearAllCllback();

    template<typename RVal, typename... Args>
    RVal TryExecScript(uint32_t idxCallBackType, Args&&... args)
    {
        __ENTER_FUNCTION
        if(m_pMap->GetScriptID() == 0)
            return RVal();
        else
            return ScriptManager()->TryExecScript<RVal>(m_pMap->GetScriptID(),
                                                        idxCallBackType,
                                                        std::forward<Args>(args)...);
        __LEAVE_FUNCTION
        return RVal();
    }

    
    export_lua CNpc* CreateNpc(uint32_t idNpcType, const CPos2D& pos, float face);

    export_lua CMonster* CreateMonster(uint32_t      idMonsterType,
                                       uint32_t      idGen,
                                       uint32_t      idCamp,
                                       OBJID         idOwner,
                                       const CPos2D& pos,
                                       float         face);
    export_lua CMonster* OnMsgCreateMonster(uint32_t      idMonsterType,
                                            uint32_t      idGen,
                                            uint32_t      idCamp,
                                            OBJID         idOwner,
                                            const CPos2D& pos,
                                            float         face);

    export_lua bool OnMsgCreateMultiMonster(uint32_t      idMonsterType,
                                            uint32_t      nNum,
                                            uint32_t      idCamp,
                                            OBJID         idOwner,
                                            const CPos2D& pos,
                                            float         range);
    export_lua bool CreateMultiMonster(uint32_t      idMonsterType,
                                       uint32_t      nNum,
                                       uint32_t      idCamp,
                                       OBJID         idOwner,
                                       const CPos2D& pos,
                                       float         range);

private:
    uint32_t m_curSceneState;
    bool     m_bDelThis = false;

    CEventEntryQueue m_StatusEventList;

    std::map<uint32_t, int64_t>                     m_MapVal;
    std::map<uint64_t, std::map<uint32_t, int64_t>> m_MapUserVal;
};
#endif /* SCENE_H */
