#ifndef SKILLMANAGER_H
#define SKILLMANAGER_H

#include "SkillData.h"
#include "SkillType.h"

//玩家的技能表
class CPlayer;
export_lua class CPlayerSkillManager : public NoncopyableT<CPlayerSkillManager>
{
    CPlayerSkillManager();
    bool Init(CPlayer* pOwner);

public:
    CreateNewImpl(CPlayerSkillManager);

public:
    ~CPlayerSkillManager();

public:
    export_lua bool LearnSkill(uint32_t idSkillSort);
    export_lua bool CastSkill(uint32_t idSkillSort, OBJID idTarget, const Vector2& pos);
    export_lua bool UpgradeSkill(uint32_t idSkillSort);
    export_lua CSkillData* _QuerySkill(uint32_t idSkillSort) const;

private:
    bool CheckSkillReq(const CSkillType* pSkillType);

private:
    CPlayer*                                        m_pOwner = nullptr;
    std::map<uint32_t, std::unique_ptr<CSkillData>> m_setSkillData;
};
#endif /* SKILLMANAGER_H */
