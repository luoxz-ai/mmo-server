#include "AIService.h"

bool CAIType::Init(const Cfg_AIType_Row& row)
{
    m_Data       = row;
    m_pTargetFAM = TargetFAMSet()->QueryObj(row.targetfam_id());

    return true;
}
