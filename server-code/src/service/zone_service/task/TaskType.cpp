#include "TaskType.h"

CTaskTypeSet::CTaskTypeSet() {}

CTaskTypeSet::~CTaskTypeSet()
{
   Clear();
}


void CTaskTypeSet::Clear()
{
    __ENTER_FUNCTION
    m_setData.clear();
    m_setDataByAcceptNPCID.clear();
    m_setDataBySubmitNPCID.clear();
    ClearRawData();
    __LEAVE_FUNCTION
}

void CTaskTypeSet::BuildIndex()
{
    __ENTER_FUNCTION
    for(const auto& ptr: m_vecData)
    {
        auto pData = ptr.get();
        m_setData[pData->GetID()] = pData;
        m_setDataByAcceptNPCID[pData->GetAcceptNpcID()].push_back(pData);
        m_setDataBySubmitNPCID[pData->GetSubmitNpcID()].push_back(pData);
    }
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
