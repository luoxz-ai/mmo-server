#ifndef TASKTYPE_H
#define TASKTYPE_H

#include "BaseCode.h"
#include "T_GameDataMap.h"
#include "config/Cfg_Task.pb.h"

export_lua enum TaskFlag {
    TASKFLAG_ACCEPT_ADDCOUNT = 0x0001, // 接受任务时增加任务计数
    TASKFLAG_SUBMIT_ADDCOUNT = 0x0002, // 提交任务时增加任务计数
    TASKFLAG_SUBMIT_DELITEM  = 0x0004, // 提交任务时需要扣除物品
    TASKFLAG_CAN_GIVEUP      = 0x0008, // 是否允许放弃

    TASKFLAG_REPEATABLE      = 0x0010, // 可重复执行
    TASKFLAG_NEXTDAY_EXPIRE  = 0x0020, // 当天23:59:59过期
    TASKFLAG_HIDE            = 0x0040, // 客户端不显示，不计已接任务数量限制
    TASKFLAG_MSGSUBMIT       = 0x0080, // 允许通过消息提交任务
    TASKFLAG_AUTO_ACITVE_NPC = 0x0100, // 接受任务/提交任务/快速完成处理完以后，重新激活NPC
};

class CTaskType
{
    CTaskType() {}

public:
    ~CTaskType() {}
    CREATE_NEW_IMPL(CTaskType);
    bool Init(const Cfg_Task_Row& row)
    {
        m_Data = row;
        return true;
    }

    using PB_T = Cfg_Task;
    static uint32_t getIDFromPBRow(const Cfg_Task_Row& row) { return row.id(); }

    uint32_t           GetID() const { return m_Data.id(); }
    uint32_t           GetType() const { return m_Data.type(); }
    const std::string& GetName() const { return m_Data.name(); }
    uint32_t           GetLevReq() const { return m_Data.level_req(); }
    uint32_t           GetProfReq() const { return m_Data.prof_req(); }
    decltype(auto)     GetTaskReqList() const { return m_Data.task_req(); }
    uint32_t           GetExpireSec() const { return m_Data.expire_sec(); }
    uint32_t           GetDayTimes() const { return m_Data.daytimes(); }
    uint32_t           GetAcceptNpcID() const { return m_Data.accept_npcid(); }
    uint32_t           GetSubmitNpcID() const { return m_Data.submit_npcid(); }
    uint32_t           GetFlag() const { return m_Data.task_flag(); }
    uint32_t           GetAwardExp() const { return m_Data.award_exp(); }
    decltype(auto)     GetAwardItemList() const { return m_Data.award_item_list(); }
    decltype(auto)     GetFinishDataList() const { return m_Data.finish_data_list(); }
    uint32_t           GetScriptID() const { return m_Data.scriptid(); }
    uint32_t           GetDayTimesMax() const { return m_Data.daytimes_max(); }
    uint32_t           GetDayTimePerCost() const { return m_Data.daytime_per_cost(); }
    uint32_t           GetSubmitMultipleMax() const { return m_Data.submit_multiple_max(); }
    uint32_t           GetSubmitMultipleCost() const { return m_Data.submit_multiple_cost(); }
    uint32_t           GetQuickSubmitCost() const { return m_Data.quick_submit_cost(); }
    const std::string& GetAcceptText() const { return m_Data.accept_txt(); }
    const std::string& GetDoingText() const { return m_Data.doing_txt(); }
    const std::string& GetSubmitText() const { return m_Data.submit_txt(); }
    uint32_t           GetNextAuotAcceptTaskID() const { return m_Data.next_auto_accept_task_id(); }

    bool HasFlag(uint32_t flag) const { return ::HasFlag(flag, GetFlag()); }

public:
    const Cfg_Task_Row& GetDataRef() const { return m_Data; }

private:
    Cfg_Task_Row m_Data;
};

//////////////////////////////////////////////////////////////////////
class CTaskTypeSet
{
    CTaskTypeSet();

public:
    virtual ~CTaskTypeSet();

public:
    CREATE_NEW_IMPL(CTaskTypeSet);
    bool Init(const char* szFileName);
    bool Reload(const char* szFileName);
    void Destroy();

    CTaskType*                     QueryObj(uint32_t idType) const;
    const std::vector<CTaskType*>* QueryTaskByAcceptNpcID(uint32_t idNpc) const;
    const std::vector<CTaskType*>* QueryTaskBySubmitNpcID(uint32_t idNpc) const;

protected:
    std::unordered_map<uint32_t, CTaskType*>              m_setData;
    std::unordered_map<uint32_t, std::vector<CTaskType*>> m_setDataByAcceptNPCID;
    std::unordered_map<uint32_t, std::vector<CTaskType*>> m_setDataBySubmitNPCID;
};

#endif /* TASKTYPE_H */
