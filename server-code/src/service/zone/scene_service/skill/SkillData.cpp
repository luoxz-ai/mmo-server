#include "SkillData.h"

#include "Actor.h"
#include "MysqlConnection.h"
#include "SceneService.h"
#include "gamedb.h"

CSkillData::CSkillData() {}

CSkillData::~CSkillData() {}

bool CSkillData::Init(CActor* pOwner, uint32_t idSkillSort, uint32_t nLev)
{
    __ENTER_FUNCTION
    auto pDB = SceneService()->GetGameDB(pOwner->GetWorldID());
    CHECKF(pDB);
    m_pData                               = pDB->MakeRecord(TBLD_SKILL::table_name());
    m_pData->Field(TBLD_SKILL::ID)        = SceneService()->CreateUID();
    m_pData->Field(TBLD_SKILL::USERID)    = pOwner->GetID();
    m_pData->Field(TBLD_SKILL::SKILLTYPE) = idSkillSort;
    m_pData->Field(TBLD_SKILL::LEV)       = nLev;
    CHECKF(m_pData->Update(true));
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CSkillData::Init(CActor* pOwner, CDBRecordPtr&& pRow)
{
    __ENTER_FUNCTION
    m_pData.reset(pRow.release());
    return true;
    __LEAVE_FUNCTION
    return false;
}
uint32_t CSkillData::GetSkillSort() const
{
    return m_pData->Field(TBLD_SKILL::SKILLTYPE);
}
uint32_t CSkillData::GetSkillLev() const
{
    return m_pData->Field(TBLD_SKILL::LEV);
}
void CSkillData::SetSkillLev(uint32_t nLev, bool bUpdate /*= true*/)
{
    __ENTER_FUNCTION
    m_pData->Field(TBLD_SKILL::LEV) = nLev;
    if(bUpdate)
        m_pData->Update();
    __LEAVE_FUNCTION
}

void CSkillData::Update()
{
    __ENTER_FUNCTION
    m_pData->Update();
    __LEAVE_FUNCTION
}

//////////////////////////////////////////////////////////////////////////