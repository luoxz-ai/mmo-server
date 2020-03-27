#ifndef SKILLTYPE_H
#define SKILLTYPE_H

#include "ActorAttrib.h"
#include "BaseCode.h"
#include "T_GameDataMap.h"
#include "config/Cfg_Skill.pb.h"
#include "config/Cfg_SkillAttachStatus.pb.h"
#include "config/Cfg_SkillDetachStatus.pb.h"

export_lua enum SkillType {
    SKILLTYPE_NONE    = 0,
    SKILLTYPE_ATTACK  = 1, // 普通技能
    SKILLTYPE_BULLET  = 2, // 飞弹类技能,每次施放将产生id=skill_data的飞弹, 飞弹根据自己的配表,自行行动与玩家脱钩
    SKILLTYPE_RUSH    = 3, // 冲锋(到目标点/目标), 距离不够无法释放
    SKILLTYPE_TRANS   = 4, // 传送(传送到目标点/目标方向), 最远传送range距离
    SKILLTYPE_CALLMON = 5, // 在目标点/目标周围range范围内 召唤id=skill_data的怪物
    SKILLTYPE_PASSIVE = 6, // 被动

};

//技能释放类型
export_lua enum SkillCastType {
    SCT_TARGET_SELF  = 0, //以自己作为目标
    SCT_TARGET_OTHER = 1, //以其他人作为目标
    SCT_TARGET_POS   = 2, //必须选择一个点
};

//技能伤害目标掩码
export_lua enum SkillDamageTargetFlag {
    STF_TARGET_FRIEND  = 0x01, //可以伤害友方
    STF_TARGET_MONSTER = 0x02, //可以伤害怪物
    STF_TARGET_PLAYER  = 0x04, //可以伤害玩家
    STF_TARGET_PET     = 0x08, //可以伤害宠物
    STF_TARGET_DEAD    = 0x10, //可以对死亡目标进行伤害
};

//伤害目标选取范围
export_lua enum SkillDamageShapeType {
    STF_TARGET_SINGLE = 0, //单体
    STF_TARGET_RECT = 1, //矩形 以目标/目标点为中心,range为长,width为宽, 垂直于地图坐标系的矩形
    STF_TARGET_RECT_DIR = 2, //矩形 已自己为中心，面向目标点,range为长,width为宽
    STF_TARGET_ROUND    = 3, //圆形 以目标/目标点为中心,range为半径的圆
    STF_TARGET_FAN      = 4, //根据目标点计算朝向, 以自身为中心, range为半径, width为(0-360)的扇形
    STF_TARGET_CUSTOM   = 5, //通过技能脚本自行计算
};

//技能掩码
export_lua enum SkillFlag {
    SKILLFLAG_NORMAL            = 0x0000, //
    SKILLFLAG_MOVE_BREAK_INTONE = 0x0001, // 吟唱时移动是否打断吟唱
    SKILLFLAG_CANCHANGE_DESTPOS = 0x0002, // 可以在过程中切换目标点的
    SKILLFLAG_INTONE_CANMOVE    = 0x0004, // 吟唱时是否可移动
    SKILLFLAG_LAUNCH_CANMOVE    = 0x0008, // 释放时是否可移动

    SKILLFLAG_DAMAGE_BY_DIST = 0x0010, // 根据距离调整伤害(当前距离/range)
    SKILLFLAG_SPENDITEM = 0x0020, // 消耗useitem字段指定的物品类型（不消耗则仅仅表示需要有这个道具）
    SKILLFLAG_DAMAGE_BEFORE_LAUNCH = 0x0040, // 该技能在launch开始的时候强行apply1次
    SKILLFLAG_DAMAGE_AFTER_LAUNCH  = 0x0080, // 该技能在launch结束的时候强行apply1次

    SKILLFLAG_AUTOLEARN = 0x0100, // 自动学习
    SKILLFLAG_AUTOUSE   = 0x0200, // 自动施放(前端标记)

    SKILLFLAG_BREAK_INTONE = 0x0400, // 该技能施展可以打断目标的吟唱
    SKILLFLAG_BREAK_LAUNCH = 0x0800, // 该技能施展可以打断目标的施法

    SKILLFLAG_IGNORE_HITRATE     = 0x1000, // 该技能忽略命中率计算
    SKILLFLAG_IGNORE_DEFENCE     = 0x2000, // 该技能忽略目标防御
    SKILLFLAG_IGNORE_DAMAGE      = 0x4000, // 该技能不产生伤害
    SKILLFLAG_NOT_UPDATEFIGHTING = 0x8000, // 该技能使用不会进入战斗状态

    SKILLFLAG_CANNOT_RESILIENCE  = 0x00010000, // 该技能不能反弹
    SKILLFLAG_CANNOT_REFLECT     = 0x00020000, // 该技能不能反射
    SKILLFLAG_INTONE_CANREINTONE = 0x00040000, // 技能吟唱时是否可以再次吟唱(重新开始新技能的吟唱)

    SKILLFLAG_INTONE_CANBREAK = 0x00100000, // 该技能吟唱时是否允许自行打断
    SKILLFLAG_LAUNCH_CANBREAK = 0x00200000, // 该技能释放时是否允许自行打断
};

class CSkillType : public Noncopyable<CSkillType>
{
    CSkillType() {}
    bool Init(const Cfg_Skill_Row& row)
    {
        m_Data = row;
        for(int32_t i = 0; i < row.attrib_change_list_size(); i++)
        {
            m_AttribChangeList.push_back(CActorAttribChange{row.attrib_change_list(i)});
        }
        return true;
    }
public:
    CreateNewImpl(CSkillType);
public:
    ~CSkillType() {}
    
    

    using PB_T = Cfg_Skill;
    static uint32_t    MakeID(uint16_t idSkillSort, uint8_t ucLevel) { return idSkillSort * 100 + ucLevel; }
    uint32_t           GetID() const { return GetSkillID(); }
    uint32_t           GetSkillID() const { return m_Data.id(); }
    uint32_t           GetSkillSort() const { return m_Data.sort(); }
    uint32_t           GetLevel() const { return m_Data.level(); }
    uint32_t           GetSkillType() const { return m_Data.type(); }
    const std::string& GetName() const { return m_Data.name(); }
    uint32_t           GetLearnLevel() const { return m_Data.learn_level(); }
    uint32_t           GetProfReq() const { return m_Data.prof_req(); }

    uint32_t GetUseItemType() const { return m_Data.use_itemtype(); }
    uint32_t GetUseMP() const { return m_Data.use_mp(); }
    uint32_t GetUseNeedStatus() const { return m_Data.use_needstatus(); }

    uint32_t GetPower() const { return m_Data.power(); }
    uint32_t GetMaxPower() const { return m_Data.max_power(); }
    uint32_t GetPowerAddition() const { return m_Data.power_addition(); }
    uint32_t GetData() const { return m_Data.data(); }

    uint32_t GetIntoneMS() const { return m_Data.intone_ms(); }
    uint32_t GetApplyTimes() const { return m_Data.apply_times(); }
    uint32_t GetApplyMS() const { return m_Data.apply_ms(); }
    uint32_t GetApplyAdjMS() const { return m_Data.apply_adj_ms(); }
    uint32_t GetStunMS() const { return m_Data.stun_ms(); }
    uint32_t GetCDSec() const { return m_Data.cd_sec(); }
    uint32_t GetCDType() const { return m_Data.cd_type(); }

    uint32_t GetHitType() const { return m_Data.hit_type(); }
    uint32_t GetDistance() const { return m_Data.distance(); }
    uint32_t GetMaxDistance() const { return m_Data.max_distance(); }
    uint32_t GetRange() const { return m_Data.range(); }
    uint32_t GetWidth() const { return m_Data.width(); }

    uint32_t GetCastType() const { return m_Data.cast_type(); }
    uint32_t GetDamageFlag() const { return m_Data.damage_flag(); }
    uint32_t GetDamageShape() const { return m_Data.damage_shape(); }
    uint32_t GetFlag() const { return m_Data.flag(); }
    OBJID    GetScirptID() const { return m_Data.scriptid(); }
    OBJID    GetBulletTypeID() const { return m_Data.bulletid(); }
    bool     IsDebug() const { return m_Data.isdebug(); }
    bool     CanAttackActor(class CActor* pAttacker, class CActor* pTarget) const;

public:
    const Cfg_Skill_Row&                   GetDataRef() const { return m_Data; }
    const std::vector<CActorAttribChange>& GetAttrib() const { return m_AttribChangeList; }

private:
    Cfg_Skill_Row                   m_Data;
    std::vector<CActorAttribChange> m_AttribChangeList;
};

typedef CGameDataMap<CSkillType> CSkillTypeSet;

class CSkillAttachStatusData : public Noncopyable<CSkillAttachStatusData>
{
    CSkillAttachStatusData() {}
    bool Init(const Cfg_SkillAttachStatus_Row& row)
    {
        m_Data = row;
        return true;
    }
public:
    CreateNewImpl(CSkillAttachStatusData);
public:
    ~CSkillAttachStatusData() {}
    
    

    using PB_T = Cfg_SkillAttachStatus;

    uint32_t GetID() const { return GetSkillID(); }
    uint32_t GetSkillID() const { return m_Data.idskill(); }
    uint32_t GetStatusID() const { return m_Data.status_typeid(); }
    uint32_t GetStatusLevel() const { return m_Data.status_lev(); }

    uint32_t GetPower() const { return m_Data.status_power(); }
    uint32_t GetSecs() const { return m_Data.status_secs(); }
    uint32_t GetTimes() const { return m_Data.status_times(); }

public:
    const Cfg_SkillAttachStatus_Row& GetDataRef() const { return m_Data; }

private:
    Cfg_SkillAttachStatus_Row m_Data;
};

typedef CGameMultiDataMap<CSkillAttachStatusData> CSkillAttachStatusDataSet;

class CSkillDetachStatusData : public Noncopyable<CSkillDetachStatusData>
{
    CSkillDetachStatusData() {}
    bool Init(const Cfg_SkillDetachStatus_Row& row)
    {
        m_Data = row;
        return true;
    }
public:
    CreateNewImpl(CSkillDetachStatusData);
public:
    ~CSkillDetachStatusData() {}
    
    

    using PB_T = Cfg_SkillDetachStatus;

    uint32_t GetID() const { return GetSkillID(); }
    uint32_t GetSkillID() const { return m_Data.idskill(); }
    uint32_t GetStatusID() const { return m_Data.status_id(); }
    uint32_t GetStatusType() const { return m_Data.status_type(); }
    uint32_t GetStatusFlag() const { return m_Data.status_flag(); }

public:
    const Cfg_SkillDetachStatus_Row& GetDataRef() const { return m_Data; }

private:
    Cfg_SkillDetachStatus_Row m_Data;
};

typedef CGameMultiDataMap<CSkillDetachStatusData> CSkillDetachStatusDataSet;

#endif /* SKILLTYPE_H */
