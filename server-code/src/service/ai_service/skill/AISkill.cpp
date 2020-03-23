#include "AISkill.h"

#include "AIMonster.h"
#include "AIService.h"

CAISkillData::CAISkillData() {}

CAISkillData::~CAISkillData() {}

bool CAISkillData::Init(uint32_t idSkill)
{
    m_idSkill    = idSkill;
    m_pSkillType = SkillTypeSet()->QueryObj(idSkill);
    CHECKF_M(m_pSkillType, fmt::format(FMT_STRING("Skill {} Not Find"), idSkill).c_str());
    m_pSkillFAM = SkillFAMSet()->QueryObj(idSkill);

    return true;
}

void CAISkillData::StartCoolDown()
{
    m_tCoolDown = TimeGetMillisecond() + GetSkillType()->GetCDSec();
}

bool CAISkillData::IsCoolDown()
{
    return m_tCoolDown > (uint64_t)TimeGetMillisecond();
}

double CAISkillData::GetUtilityValue(double dist, double self_hp, double self_mp, double target_hp)
{
    if(IsCoolDown() == false && GetSkillFAM())
        return GetSkillFAM()->calculate(dist, self_hp, self_mp, target_hp);
    else
        return 0.0f;
}

CSkillType* CAISkillData::GetSkillType() const
{
    return m_pSkillType;
}

SkillFAM* CAISkillData::GetSkillFAM() const
{
    return m_pSkillFAM;
}

uint32_t CAISkillData::GetSkillTypeID() const
{
    return m_idSkill;
}

CAISkillSet::CAISkillSet() {}

CAISkillSet::~CAISkillSet()
{
    for(auto& pair_val: m_setSkill)
    {
        SAFE_DELETE(pair_val.second);
    }
    m_setSkill.clear();
}

void CAISkillSet::AddSkill(uint32_t idSkill)
{
    auto pSkilLData = CAISkillData::CreateNew(idSkill);
    if(pSkilLData)
        m_setSkill[idSkill] = pSkilLData;
}

bool CAISkillSet::CastSkill(CAISkillData* pSkill)
{
    if(pSkill == nullptr)
        return false;
    if(pSkill->IsCoolDown())
        return false;
    if(m_pMonster->GetMP() < pSkill->GetSkillType()->GetUseMP())
        return false;
    // send cast skill to ai
    pSkill->StartCoolDown();
    return true;
}

CAISkillData* CAISkillSet::ChooseSkill(double dist, double self_hp, double self_mp, double target_hp)
{
    //遍历所有技能
    double        cur_val  = 0.0;
    CAISkillData* pCurData = nullptr;
    for(const auto& pair_val: m_setSkill)
    {
        auto pData = pair_val.second;
        if(pData->IsCoolDown())
            continue;
        double util_value = pData->GetUtilityValue(dist, self_hp, self_mp, target_hp);
        if(pCurData == nullptr || cur_val < util_value)
        {
            cur_val  = util_value;
            pCurData = pData;
        }
    }

    return pCurData;
}

void CAISkillSet::OnCastSkill(uint32_t idSkill)
{
    auto it = m_setSkill.find(idSkill);
    if(it == m_setSkill.end())
        return;
    auto pData = it->second;
    pData->StartCoolDown();
}
