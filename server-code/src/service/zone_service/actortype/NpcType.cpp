#include "NpcType.h"

#include "ProtobuffUtil.h"
CNpcTypeSet::CNpcTypeSet() {}

CNpcTypeSet::~CNpcTypeSet()
{
    __ENTER_FUNCTION
    for(auto& [k, v]: m_setData)
    {
        SAFE_DELETE(v);
    }
    m_setData.clear();
    m_setDataByMapID.clear();
    __LEAVE_FUNCTION
}


void CNpcTypeSet::Clear()
{
    __ENTER_FUNCTION
    m_setData.clear();
    m_setDataByMapID.clear();
    ClearRawData();
    __LEAVE_FUNCTION
}

void CNpcTypeSet::BuildIndex()
{
    __ENTER_FUNCTION
    for(const auto& ptr: m_vecData)
    {
        auto pData = ptr.get();
        m_setData[pData->GetID()] = pData;
        m_setDataByMapID[pData->GetMapID()][pData->GetPhaseID()].push_back(pData);
    }
    __LEAVE_FUNCTION
}

const CNpcType* CNpcTypeSet::QueryObj(uint32_t idType) const
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

const CNpcTypeSet::NpcTypeList* CNpcTypeSet::QueryObjByMapID(uint32_t idMap, uint64_t idPhase) const
{
    __ENTER_FUNCTION
    auto it = m_setDataByMapID.find(idMap);
    if(it != m_setDataByMapID.end())
    {
        const auto& refPhase = it->second;
        auto it_p = refPhase.find(idPhase);
        if(it_p != refPhase.end())
        {
            return &(it_p->second);
        }
    }
    __LEAVE_FUNCTION
    return nullptr;
}
