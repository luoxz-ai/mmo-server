#include "ItemAddition.h"

#include "ProtobuffUtil.h"
CItemAdditionSet::CItemAdditionSet() {}

CItemAdditionSet::~CItemAdditionSet()
{
    m_setAddition.clear();
    ClearRawData();
}

void CItemAdditionSet::Clear()
{
    __ENTER_FUNCTION
    m_setAddition.clear();
    ClearRawData();
    __LEAVE_FUNCTION
}

void CItemAdditionSet::BuildIndex()
{
    __ENTER_FUNCTION
    for(const auto& ptr: m_vecData)
    {
        auto& refList = m_setAddition[ptr->GetID()];
        refList.push_back(ptr.get());
    }
    __LEAVE_FUNCTION
}

const CItemAdditionData* CItemAdditionSet::QueryItemAddition(uint32_t idType, int32_t nLevel)
{
    __ENTER_FUNCTION
    auto it_map = m_setAddition.find(idType);
    if(it_map == m_setAddition.end())
        return nullptr;
    const auto& refList = it_map->second;
    if(nLevel > (int32_t)refList.size())
        return nullptr;
    return refList[nLevel - 1];
    __LEAVE_FUNCTION
    return nullptr;
}
