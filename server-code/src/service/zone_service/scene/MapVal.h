#ifndef MAPVAL_H
#define MAPVAL_H

#include "BaseCode.h"

class CActor;
class CSceneBase;
export_lua class CMapValSet : NoncopyableT<CMapValSet>
{
protected:
    CMapValSet();
public:
    CreateNewImpl(CMapValSet);
    virtual ~CMapValSet();
    bool Init(CSceneBase* pScene){m_pScene = pScene; return true;}
public:
    //发送所有mapval以及和该玩家id一样的mapuserval 给玩家
    export_lua virtual void SendAllMapValToClient(CActor* pPlayer);
    //清理所有mapval
    export_lua void    ClearAllMapVal();

    //设置mapval,并广播
    export_lua void    SetMapVal(uint32_t nIdx, int64_t nVal, bool bSync = false);
    export_lua int64_t GetMapVal(uint32_t nIdx);
    export_lua int64_t AddMapVal(uint32_t nIdx, int64_t nVal, bool bSync = false);
    export_lua void    ClearMapVal();
    //广播所有mapval
    export_lua void     SyncAllMapVal();

    //根据玩家id设置mapval,并广播
    export_lua void    SetMapUserVal(uint64_t idUser, uint32_t nIdx, int64_t nVal, bool bSync = false);
    export_lua int64_t GetMapUserVal(uint64_t idUser, uint32_t nIdx);
    export_lua int64_t AddMapUserVal(uint64_t idUser, uint32_t nIdx, int64_t nVal, bool bSync = false);
    export_lua void    ClearMapUserVal(uint64_t idUser);
    export_lua void    ClearAllMapUserVal();

protected:
    CSceneBase* m_pScene;
    std::map<uint32_t, int64_t>                     m_MapVal;
    std::map<uint64_t, std::map<uint32_t, int64_t>> m_MapUserVal;
};

#endif /* MAPVAL_H */
