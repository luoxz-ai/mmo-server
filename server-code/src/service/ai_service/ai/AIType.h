#ifndef AITYPE_H
#define AITYPE_H

#include "AIFuzzyLogic.h"
#include "Cfg_AIType.pb.h"

enum AIType
{
    AITYPE_NONE    = 0, //木桩
    AITYPE_PASSIVE = 1, //被动
    AITYPE_ACTIVE  = 2, //主动
};
class CAIType: public Noncopyable<CAIType>
{
    CAIType() {}
    bool Init(const Cfg_AIType_Row& row);
public:
    CreateNewImpl(CAIType);
public:
    ~CAIType() {}
    

    using PB_T = Cfg_AIType;
    uint32_t GetID() const { return m_Data.idmonster(); }

    const TargetFAM* GetTargetFAM()const { return m_pTargetFAM; }

public:
    const Cfg_AIType_Row& GetDataRef() const { return m_Data; }

private:
    Cfg_AIType_Row m_Data;

    const TargetFAM* m_pTargetFAM;
};

typedef CGameDataMap<CAIType> CAITypeSet;

#endif /* AITYPE_H */
