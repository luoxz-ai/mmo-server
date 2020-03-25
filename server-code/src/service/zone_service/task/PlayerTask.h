#ifndef PLAYERTASK_H
#define PLAYERTASK_H

#include "BaseCode.h"
#include "DBRecord.h"
#include "TaskType.h"
#include "gamedb.h"
#include "msg/zone_service.pb.h"

const uint32_t MAX_TASKDATA_NUM = 4;

export_lua constexpr const char* STR_TASK_ACCEPT   = "接受任务";
export_lua constexpr const char* STR_TASK_SUBMIT[] = {"领取奖励", "一倍奖励", "两倍奖励", "三倍奖励"};
export_lua constexpr const char* STR_TASK_QUICK_FINISH = "快速完成";

//每一条未完成的任务占据一条数据
class CPlayer;
export_lua class CPlayerTaskData : public Noncopyable<CPlayerTaskData>
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

export_lua const uint32_t MAX_ACCEPT_TASK = 20;

export_lua class CPlayerTask : public Noncopyable<CPlayerTask>
{
    CPlayerTask();
public:
    CreateNewImpl(CPlayerTask);
public:
    ~CPlayerTask();
    
    bool Init(CPlayer* pPlayer);

    export_lua CPlayerTaskData* QueryTaskData(uint32_t idTask);

    export_lua bool AcceptTask(uint32_t idTask, bool bChkCondition = true, bool bIgnoreChkNum = false);
    export_lua bool SubmitTaskByMessage(uint32_t idTask, uint32_t nSubmitMultiple);
    export_lua bool SubmitTask(uint32_t idTask, uint32_t nSubmitMultiple);
    export_lua bool QuickFinish(uint32_t idTask);
    export_lua bool GiveupTask(uint32_t idTask);

    export_lua bool    CanAccept(uint32_t idTask);
    export_lua bool    CanAccept(const CTaskType* pType);
    export_lua bool    CanSubmit(uint32_t idTask);
    export_lua bool    CanSubmit(const CTaskType* pTaskType);
    export_lua int32_t GetLeftTimes(uint32_t idTask);
    export_lua bool    IsFinished(uint32_t idTask);

    export_lua void            SaveInfo();
    export_lua void            SendTaskInfo();
    export_lua export_lua void SendTaskInfo(CPlayerTaskData* pTaskData);
    export_lua void            SendTaskDataChange(CPlayerTaskData* pTaskData, uint32_t i);
    export_lua bool            ShowTaskDialog(uint32_t idTask, OBJID idNpc);

    void OnAwardTaskItem(uint32_t idItemType, uint32_t nNum);
    void OnKillMonster(uint32_t idMonster, bool bKillBySelf);
    void OnDelTaskItem(uint32_t idItemType, uint32_t nNum);
    void OnFinishAchi(uint32_t idAchi);

private:
    CPlayer*                             m_pOwner = nullptr;
    std::set<uint32_t>                   m_setFinishedTaskID;
    std::map<uint32_t, CPlayerTaskData*> m_setTask;
    uint32_t                             m_nCurAcceptNum = 0;
};
#endif /* PLAYERTASK_H */
