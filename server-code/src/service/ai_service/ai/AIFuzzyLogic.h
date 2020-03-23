#ifndef AIFUZZYLOGIC_H
#define AIFUZZYLOGIC_H

#include "BaseCode.h"
#include "Cfg_SkillFAM.pb.h"
#include "Cfg_TargetFAM.pb.h"
#include "T_GameDataMap.h"
#include "fam/rule_set.h"
#include "fam/trapezoid.h"
// Antecedent sets: enemy distance
enum distance_fuzzy_type
{
    disft_close,
    disft_medium,
    disft_far,
};
const fuzzy::Trapezoid distance_fuzzy[] = {{-15, -2.5, 2.5, 15}, {2.5, 15, 15, 30}, {15, 30, 50, 65}};

enum mp_fuzzy_type
{
    mpft_low,
    mpft_ok,
    mpft_lots,
};
const fuzzy::Trapezoid mp_fuzzy[] = {fuzzy::Trapezoid(0, 0, 0.10, 0.20),
                                     fuzzy::Trapezoid(0, 0.20, 0.40, 0.60),
                                     fuzzy::Trapezoid(0.20, 0.60, 1, 1)};

enum hp_fuzzy_type
{
    hpft_low,
    hpft_ok,
    hpft_lots,
};
const fuzzy::Trapezoid hp_fuzzy[] = {fuzzy::Trapezoid(0, 0, 0.10, 0.20),
                                     fuzzy::Trapezoid(0, 0.20, 0.40, 0.60),
                                     fuzzy::Trapezoid(0.20, 0.60, 1, 1)};

enum like_fuzzy_type
{
    likeft_unlike,
    likeft_like,
    likeft_verylike,
};
const fuzzy::Trapezoid like_fuzzy[] = {fuzzy::Trapezoid(0, 0, 20, 50),
                                       fuzzy::Trapezoid(30, 50, 70, 90),
                                       fuzzy::Trapezoid(50, 80, 100, 100)};

class SkillFAM
{
    SkillFAM()
        : m_rule_set("SkillFAM", fuzzy::Implication::MAMDANI)
    {
    }

public:
    ~SkillFAM() {}
    CREATE_NEW_IMPL(SkillFAM);
    using PB_T = Cfg_SkillFAM;

    bool Init(const Cfg_SkillFAM_Row& row)
    {
        m_ID = row.idskill();
        // Set up our rules.
        uint32_t           dis_idx       = row.dis();
        uint32_t           hp_idx        = row.self_hp();
        uint32_t           mp_idx        = row.self_mp();
        uint32_t           target_hp_idx = row.target_hp();
        uint32_t           like_idx      = row.like();
        fuzzy::Conjunction oper_type     = (fuzzy::Conjunction)(row.oper_type());
        m_rule_set.add(fuzzy::Rule{std::vector<fuzzy::Trapezoid>{distance_fuzzy[dis_idx],
                                                                 hp_fuzzy[hp_idx],
                                                                 mp_fuzzy[mp_idx],
                                                                 hp_fuzzy[target_hp_idx]},
                                   oper_type,
                                   like_fuzzy[like_idx]});

        return true;
    }
    static uint32_t GetIDFromPBRow(const Cfg_SkillFAM_Row& row) { return row.idskill(); }
    uint32_t        GetID() { return m_ID; }

    double calculate(double dist, double self_hp, double self_mp, double target_hp)
    {
        return m_rule_set.calculate(std::vector<double>{dist, self_hp, self_mp, target_hp});
    }

private:
    uint32_t m_ID;

    // The rules that will tie the logic together
    fuzzy::RuleSet m_rule_set;
};
typedef CGameDataMap<SkillFAM> CSkillFAMSet;

class TargetFAM
{
    TargetFAM()
        : m_rule_set("TargetFAM", fuzzy::Implication::MAMDANI)
    {
    }

public:
    ~TargetFAM() {}
    CREATE_NEW_IMPL(TargetFAM);
    using PB_T = Cfg_TargetFAM;

    bool Init(const Cfg_TargetFAM_Row& row)
    {
        m_ID = row.idmonster();
        // Set up our rules.
        uint32_t dis_idx       = row.dis();
        uint32_t hp_idx        = row.hp();
        uint32_t target_hp_idx = row.target_hp();
        uint32_t like_idx      = row.like();

        fuzzy::Conjunction oper_type = (fuzzy::Conjunction)(row.oper_type());
        m_rule_set.add(fuzzy::Rule{
            std::vector<fuzzy::Trapezoid>{distance_fuzzy[dis_idx], hp_fuzzy[hp_idx], hp_fuzzy[target_hp_idx]},
            oper_type,
            like_fuzzy[like_idx]});

        return true;
    }
    static uint32_t GetIDFromPBRow(const Cfg_TargetFAM_Row& row) { return row.idmonster(); }
    uint32_t        GetID() { return m_ID; }

    double calculate(double dist, double self_hp, double target_hp)
    {
        return m_rule_set.calculate(std::vector<double>{dist, self_hp, target_hp});
    }

private:
    uint32_t m_ID;
    // The rules that will tie the logic together
    fuzzy::RuleSet m_rule_set;
};

typedef CGameDataMap<TargetFAM> CTargetFAMSet;

#endif /* AIFUZZYLOGIC_H */
