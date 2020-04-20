
#include "PlayerTaskData.h"

CPlayerTaskData::CPlayerTaskData() {}

CPlayerTaskData::~CPlayerTaskData() {}

bool CPlayerTaskData::Init(CDBRecordPtr&& pRow)
{
    __ENTER_FUNCTION
    m_pData.reset(pRow.release());
    CHECKF(m_pData.get());
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CPlayerTaskData::Init(CPlayer* pPlayer, uint32_t idTask)
{
    __ENTER_FUNCTION
    CHECKF(pPlayer);
    auto pDB = ZoneService()->GetGameDB(pPlayer->GetWorldID());
    CHECKF(pDB);
    auto row = pDB->MakeRecord(TBLD_TASK::table_name);
    m_pData.reset(row.release());
    CHECKF(m_pData.get());
    m_pData->Field(TBLD_TASK::ID)     = ZoneService()->CreateUID();
    m_pData->Field(TBLD_TASK::TASKID) = idTask;
    m_pData->Field(TBLD_TASK::USERID) = pPlayer->GetID();

    CHECKF(m_pData->Update());
    return true;
    __LEAVE_FUNCTION
    return false;
}

void CPlayerTaskData::DelRecord()
{
    __ENTER_FUNCTION
    m_pData->DeleteRecord();
    m_pData.reset();
    __LEAVE_FUNCTION
}

bool CPlayerTaskData::SetNum(uint32_t nIdx, uint32_t v, bool bUpdate /*= true*/)
{
    __ENTER_FUNCTION
    CHECKF(nIdx < MAX_TASKDATA_NUM);
    m_pData->Field(TBLD_TASK::NUM0 + nIdx) = v;
    if(bUpdate)
        m_pData->Update();
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CPlayerTaskData::SetAcceptUserLev(uint32_t v, bool bUpdate /*= true*/)
{
    __ENTER_FUNCTION
    m_pData->Field(TBLD_TASK::ACCEPT_USERLEV) = v;
    if(bUpdate)
        m_pData->Update();
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CPlayerTaskData::SetAcceptTime(uint32_t v, bool bUpdate /*= true*/)
{
    __ENTER_FUNCTION
    m_pData->Field(TBLD_TASK::ACCEPT_TIME) = v;
    if(bUpdate)
        m_pData->Update();
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CPlayerTaskData::SetFinishTime(uint32_t v, bool bUpdate /*= true*/)
{
    __ENTER_FUNCTION
    m_pData->Field(TBLD_TASK::FINISH_TIME) = v;
    if(bUpdate)
        m_pData->Update();
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CPlayerTaskData::SetExpireTime(uint32_t v, bool bUpdate /*= true*/)
{
    __ENTER_FUNCTION
    m_pData->Field(TBLD_TASK::EXPIRE_TIME) = v;
    if(bUpdate)
        m_pData->Update();
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CPlayerTaskData::SetDayCount(uint32_t v, bool bUpdate /*= true*/)
{
    __ENTER_FUNCTION
    m_pData->Field(TBLD_TASK::DAYCOUNT) = v;
    if(bUpdate)
        m_pData->Update();
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CPlayerTaskData::SetDayCountMax(uint32_t v, bool bUpdate /*= true*/)
{
    __ENTER_FUNCTION
    m_pData->Field(TBLD_TASK::DAYCOUNT_MAX) = v;
    if(bUpdate)
        m_pData->Update();
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CPlayerTaskData::SetState(uint32_t v, bool bUpdate /*= true*/)
{
    __ENTER_FUNCTION
    m_pData->Field(TBLD_TASK::STATE) = v;
    if(bUpdate)
        m_pData->Update();
    return true;
    __LEAVE_FUNCTION
    return false;
}

void CPlayerTaskData::SaveInfo()
{
    __ENTER_FUNCTION
    m_pData->Update();
    __LEAVE_FUNCTION
}

//////////////////////////////////////////////////////////////////////////