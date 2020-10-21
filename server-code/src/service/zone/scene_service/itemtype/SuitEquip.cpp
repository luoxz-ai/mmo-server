#include "SuitEquip.h"

#include "ProtobuffUtil.h"
CSuitEquipSet::CSuitEquipSet() {}

CSuitEquipSet::~CSuitEquipSet()
{
    m_setSuitType.clear();
    ClearRawData();
}

void CSuitEquipSet::Clear()
{
    __ENTER_FUNCTION
    m_setSuitType.clear();
    ClearRawData();
    __LEAVE_FUNCTION
}

void CSuitEquipSet::BuildIndex()
{
    __ENTER_FUNCTION
    for(const auto& ptr: m_vecData)
    {
        auto& refList = m_setSuitType[ptr->GetID()];
        refList.push_back(ptr.get());
    }
    __LEAVE_FUNCTION
}

struct SUITTYPE_ORDER
{
    uint32_t m_nNum;
    SUITTYPE_ORDER(uint32_t nNum)
        : m_nNum(nNum)
    {
    }
    bool operator()(CSuitEquipData* lft, CSuitEquipData* rht) const
    {
        if(nullptr != lft && nullptr != rht)
            return lft->GetEquipNum() > rht->GetEquipNum();
        else if(nullptr != lft)
            return lft->GetEquipNum() > m_nNum;
        else if(nullptr != rht)
            return m_nNum > rht->GetEquipNum();
        return false;
    }
};

const CSuitEquipData* CSuitEquipSet::QuerySuitEquip(uint32_t idSuit, uint32_t nEquipNum)
{
    __ENTER_FUNCTION

    CHECKF(idSuit != ID_NONE && nEquipNum > 0);

    auto it = m_setSuitType.find(idSuit);
    if(it == m_setSuitType.end())
        return nullptr;

    const auto& typeSet = it->second;

    SUITTYPE_ORDER order(nEquipNum);
    auto           itFind = std::lower_bound(typeSet.begin(), typeSet.end(), nullptr, order);
    if(itFind != typeSet.end())
        return *itFind;

    __LEAVE_FUNCTION
    return nullptr;
}
