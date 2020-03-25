#include "TaskType.h"

CTaskTypeSet::CTaskTypeSet() {}

CTaskTypeSet::~CTaskTypeSet()
{
    for(auto& [k, v]: m_setData)
    {
        SAFE_DELETE(v);
    }
}

bool CTaskTypeSet::Init(const char* szFileName)
{
    __ENTER_FUNCTION
    Cfg_Task cfg;
    if(pb_util::LoadFromBinaryFile(szFileName, cfg) == false)
    {
        LOGERROR("InitFromFile {} Fail.", szFileName);
        return false;
    }
    for(const auto& iter: cfg.rows())
    {
        auto pData = CTaskType::CreateNew(iter);
        CHECKF(pData);
        m_setData[pData->GetID()] = pData;
        m_setDataByAcceptNPCID[pData->GetAcceptNpcID()].push_back(pData);
        m_setDataBySubmitNPCID[pData->GetSubmitNpcID()].push_back(pData);
    }
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CTaskTypeSet::Reload(const char* szFileName)
{
    __ENTER_FUNCTION
    Destroy();
    return Init(szFileName);
    __LEAVE_FUNCTION
    return false;
}

void CTaskTypeSet::Destroy()
{
    __ENTER_FUNCTION
    for(auto& [k, v]: m_setData)
    {
        SAFE_DELETE(v);
    }
    m_setData.clear();
    m_setDataByAcceptNPCID.clear();
    m_setDataBySubmitNPCID.clear();
    __LEAVE_FUNCTION
}

const CTaskType* CTaskTypeSet::QueryObj(uint32_t idType) const
{
    __ENTER_FUNCTION
    auto it = m_setData.find(idType);
    if(it != m_setData.end())
    {
        return it->second;
    }

    __LEAVE_FUNCTION
    return nullptr;
}

const std::vector<CTaskType*>* CTaskTypeSet::QueryTaskByAcceptNpcID(uint32_t idNpc) const
{
    __ENTER_FUNCTION
    auto it = m_setDataByAcceptNPCID.find(idNpc);
    if(it != m_setDataByAcceptNPCID.end())
    {
        return &(it->second);
    }
    __LEAVE_FUNCTION
    return nullptr;
}

const std::vector<CTaskType*>* CTaskTypeSet::QueryTaskBySubmitNpcID(uint32_t idNpc) const
{
    __ENTER_FUNCTION
    auto it = m_setDataBySubmitNPCID.find(idNpc);
    if(it != m_setDataBySubmitNPCID.end())
    {
        return &(it->second);
    }
    __LEAVE_FUNCTION
    return nullptr;
}
