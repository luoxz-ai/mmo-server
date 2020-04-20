#ifndef PLAYERTASKDATA_H
#define PLAYERTASKDATA_H

#include "BaseCode.h"
#include "DBRecord.h"
#include "gamedb.h"

constexpr uint32_t MAX_TASKDATA_NUM = 4;
//每一条未完成的任务占据一条数据
class CPlayer;
export_lua class CPlayerTaskData : public NoncopyableT<CPlayerTaskData>
{
    CPlayerTaskData();
public:
    CreateNewImpl(CPlayerTaskData);
public:
    
    ~CPlayerTaskData();

public:
    
    bool                Init(CDBRecordPtr&& pRow);
    bool                Init(CPlayer* pPlayer, uint32_t idTask);
    void                DelRecord();
    export_lua uint32_t GetTaskID() const { return m_pData->Field(TBLD_TASK::TASKID); }
    export_lua uint32_t GetNum(uint32_t nIdx) const
    {
        CHECKF(nIdx < MAX_TASKDATA_NUM) return m_pData->Field(TBLD_TASK::NUM0 + nIdx);
    }
    export_lua uint32_t GetAcceptUserLev() const { return m_pData->Field(TBLD_TASK::ACCEPT_USERLEV); }
    export_lua uint32_t GetAcceptTime() const { return m_pData->Field(TBLD_TASK::ACCEPT_TIME); }
    export_lua uint32_t GetFinishTime() const { return m_pData->Field(TBLD_TASK::FINISH_TIME); }
    export_lua uint32_t GetExpireTime() const { return m_pData->Field(TBLD_TASK::EXPIRE_TIME); }
    export_lua uint32_t GetDayCount() const { return m_pData->Field(TBLD_TASK::DAYCOUNT); }
    export_lua uint32_t GetDayCountMax() const { return m_pData->Field(TBLD_TASK::DAYCOUNT_MAX); }
    export_lua uint32_t GetState() const { return m_pData->Field(TBLD_TASK::STATE); }

    export_lua bool SetNum(uint32_t nIdx, uint32_t v, bool bUpdate = true);
    export_lua bool SetAcceptUserLev(uint32_t v, bool bUpdate = true);
    export_lua bool SetAcceptTime(uint32_t v, bool bUpdate = true);
    export_lua bool SetFinishTime(uint32_t v, bool bUpdate = true);
    export_lua bool SetExpireTime(uint32_t v, bool bUpdate = true);
    export_lua bool SetDayCount(uint32_t v, bool bUpdate = true);
    export_lua bool SetDayCountMax(uint32_t v, bool bUpdate = true);
    export_lua bool SetState(uint32_t v, bool bUpdate = true);

    export_lua void SaveInfo();

    export_lua bool IsTaskDoing() const { return GetState() == TASKSTATE_ACCEPTED; }
    export_lua bool IsExpire() const { return GetExpireTime() != 0 && GetExpireTime() < TimeGetSecond(); }

private:
    CDBRecordPtr m_pData;
};


#endif /* PLAYERTASKDATA_H */
