#ifndef SKILLFSM_H
#define SKILLFSM_H

#include "EventManager.h"
#include "SkillType.h"

enum SkillState
{
    SKILLSTATE_IDLE,   //未处理
    SKILLSTATE_INTONE, //吟唱
    SKILLSTATE_LAUNCH, //施法引导
    SKILLSTATE_STUN,   //僵直
};

export_lua class CSkillFSM
{
public:
    CSkillFSM(CActor* pOwner);
    ~CSkillFSM();

    export_lua bool CastSkill(const uint32_t idSkill, OBJID idTarget, const Vector2& pos);

    export_lua bool CanIntone(const CSkillType* pSkillType, CActor* pTarget, const Vector2& target_pos);
    export_lua void DoIntone(const CSkillType* pSkillType);
    export_lua bool BreakIntone();
    export_lua bool _BreakIntone();

    void            DoLaunch();
    export_lua bool BreakLaunch();
    export_lua bool _BreakLaunch();

    export_lua void SetTargetPos(const Vector2& posTarget);
    void            SkillEffect();

    void DoApply();
    void ScheduleApply();

    void DoStun();
    void DoIdle();

    export_lua void StartSkillCoolDown(uint32_t cdType, uint32_t cd_sec);
    export_lua void ClearSkillCoolDown(uint32_t cdType);
    export_lua bool IsSkillCoolDown(uint32_t cdType) const;

    export_lua static void SkillEffectInRange(OBJID          idCaster,
                                              uint32_t       idSkillType,
                                              OBJID          idTarget,
                                              const Vector2& posTarget,
                                              uint32_t       nApplyTimes);

    export_lua static void FindTarget(CActor*               pOwner,
                                      const CSkillType*           pSkillType,
                                      OBJID                 idTarget,
                                      const Vector2&        posTarget,
                                      std::vector<CActor*>& vecTarget);

    export_lua static void DoMultiDamage(CActor*                     pOwner,
                                         const CSkillType*                 pSkillType,
                                         OBJID                       idTarget,
                                         const Vector2&              posTarget,
                                         const std::vector<CActor*>& vecTarget);

    export_lua static int32_t DoDamage(CActor*        pOwner,
                                       const CSkillType*    pSkillType,
                                       CActor*        pTarget,
                                       OBJID          idTarget,
                                       const Vector2& posTarget);

    export_lua static void AttachStatus(CActor* pOwner, const CSkillType* pSkillType, const std::vector<CActor*>& vecTarget);

    export_lua static void AddBullet(CActor*                     pOwner,
                                     uint32_t                    idBulletType,
                                     OBJID                       idTarget,
                                     const Vector2&              posTarget,
                                     const std::vector<CActor*>& vecTarget);
    void                   Stop();

private:
    CActor*        m_pOwner;
    CEventEntryPtr m_pEvent;
    const CSkillType*    m_pCurSkillType = nullptr;
    OBJID          m_idTarget;
    Vector2        m_posTarget;

    uint32_t                     m_nApplyTimes = 0;
    SkillState                   m_curState    = SKILLSTATE_IDLE;
    std::map<uint32_t, uint32_t> m_SkillCD;
};
#endif /* SKILLFSM_H */
