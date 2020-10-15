#ifndef DATACOUNTLIMIT_H
#define DATACOUNTLIMIT_H

#include "BaseCode.h"
#include "T_GameDataMap.h"
#include "config/Cfg_DataCountLimit.pb.h"

class CDataCountLimit : public NoncopyableT<CDataCountLimit>
{
    CDataCountLimit() {}
    bool Init(const Cfg_DataCountLimit& row)
    {
        m_row = row;
        return true;
    }

public:
    CreateNewImpl(CDataCountLimit);

public:
    using PB_T = Cfg_DataCountLimit;
    virtual ~CDataCountLimit() {}

public:
    uint64_t GetID() const { return CDataCountLimit::MakeID(GetType(), GetKeyIdx()); }
    uint32_t GetType() const { return m_row.type(); }
    uint32_t GetKeyIdx() const { return m_row.keyidx(); }
    uint32_t GetMaxCount() const { return m_row.max_count(); }
    uint32_t GetResetType() const { return m_row.reset_type(); }
    uint32_t GetResetTime() const { return m_row.reset_time(); }

public:
    static uint64_t MakeID(uint32_t nType, uint32_t nKeyIdx) { return ((uint64_t)(nType) << 32) | (uint64_t)(nKeyIdx); }

protected:
    Cfg_DataCountLimit m_row;
};

DEFINE_GAMEMAPDATA(CDataCountLimitSet, CDataCountLimit);

#endif /* DATACOUNTLIMIT_H */
