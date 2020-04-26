#include "AchievementType.h"

CAchievementTypeSet::CAchievementTypeSet() {}

CAchievementTypeSet::~CAchievementTypeSet()
{
    Clear();
}

void CAchievementTypeSet::Clear()
{
    __ENTER_FUNCTION
    m_setData.clear();
    m_setDataByGroupID.clear();
    m_setDataByCheckType.clear();
    ClearRawData();
    __LEAVE_FUNCTION
}

void CAchievementTypeSet::BuildIndex()
{
    __ENTER_FUNCTION
    for(const auto& ptr: m_vecData)
    {
        auto pData                = ptr.get();
        m_setData[pData->GetID()] = pData;
        m_setDataByCheckType[pData->GetCheckData().check_type()].push_back(pData);
        m_setDataByGroupID[pData->GetGroupID()].push_back(pData);
    }

    __LEAVE_FUNCTION
}

const CAchievementTypeSet::DATA_TYPE& CAchievementTypeSet::GetData() const
{
    return m_setData;
}

const CAchievementType* CAchievementTypeSet::GetData(uint32_t idType) const
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

const CAchievementTypeSet::SUB_DATA_LIST* CAchievementTypeSet::QueryAchiemenetByGroupID(uint32_t idGroup) const
{
    __ENTER_FUNCTION
    auto it = m_setDataByGroupID.find(idGroup);
    if(it != m_setDataByGroupID.end())
    {
        return &(it->second);
    }
    __LEAVE_FUNCTION
    return nullptr;
}

const CAchievementTypeSet::SUB_DATA_LIST* CAchievementTypeSet::QueryAchiemenetByCheckType(uint32_t idType) const
{
    __ENTER_FUNCTION
    auto it = m_setDataByCheckType.find(idType);
    if(it != m_setDataByCheckType.end())
    {
        return &(it->second);
    }
    __LEAVE_FUNCTION
    return nullptr;
}
