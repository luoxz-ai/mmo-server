
#include "ActorAI.h"

#include "AIActor.h"
#include "AIPathFinder.h"
#include "AIService.h"

constexpr int32_t MOVE_PER_WAIT_MS = 500; //每500ms向zone发送一次移动消息

CActorAI::CActorAI() {}

CActorAI::~CActorAI()
{
    SAFE_DELETE(m_pAIPathFinder);
}

bool CActorAI::Init(CAIActor* pActor, CAIType* pAIType)
{
    m_pActor        = pActor;
    m_pAIType       = pAIType;
    m_pAIPathFinder = new CAIPathFinder(pActor);
    if(GetAIData().follow_path() != 0)
    {
        m_pPathData = GetActor()->GetCurrentScene()->GetMap()->GetPatrolDataByIdx(GetAIData().follow_path());
    }

    SetMainTarget(0);
    SetAutoSearchEnemy();
    m_posBorn = GetActor()->GetPos();
    ToIdle();
    return true;
}

void CActorAI::OnUnderAttack(OBJID idTarget, int32_t nDamage)
{
    if(TryExecScript(SCB_AI_ONUNDERATTACK, this, idTarget, nDamage) == true)
        return;
    AddHate(idTarget, nDamage);
}

void CActorAI::OnDead() {}

const STATE_DATA& CActorAI::GetStateData(int32_t nState)
{
    CHECKFSR_FMT(nState >= 0 && nState < ATT_MAX, STATE_DATA, "nState={}", nState);
    static STATE_DATA STATE_DATA_ARRAY[] = {
        {"ATT_IDLE", &CActorAI::ProcessIdle},
        {"ATT_ATTACK", &CActorAI::ProcessAttack},
        {"ATT_APPROACH", &CActorAI::ProcessApproach},
        {"ATT_SKILL", &CActorAI::ProcessSkill},
        {"ATT_SKILLWAIT", &CActorAI::ProcessSkillWait},
        {"ATT_ESCAPE", &CActorAI::ProcessEscape},
        {"ATT_GOBACK", &CActorAI::ProcessPatrol},
        {"ATT_PRATOL", &CActorAI::ProcessGoback},
        {"ATT_PRATOLWAIT", &CActorAI::ProcessPatrolWait},
        {"ATT_RANDMOVE", &CActorAI::ProcessRandMove},
    };
    return STATE_DATA_ARRAY[nState];
}

void CActorAI::Process()
{
    std::invoke(GetStateData(m_nState).func, this);
}

uint32_t CActorAI::GetState() const
{
    return m_nState;
}

void CActorAI::ChangeState(uint32_t val)
{
    m_nState = val;
    LOGAIDEBUG(GetAIData().ai_debug(), "AI: {} ChangeState: {}", GetActor()->GetID(), GetStateData(val).name);
}

void CActorAI::SetAISleep(bool v)
{
    if(m_bSleep == v)
        return;

    if(m_nState == ATT_PRATOL || m_nState == ATT_PRATOLWAIT)
        return;

    m_bSleep = v;
    LOGAIDEBUG(GetAIData().ai_debug(), "AI: {} SetAISleep: {}", GetActor()->GetID(), v ? "true" : "false");

    if(m_bSleep == true)
    {
        // to sleeep
        m_SearchEnemyEvent.Cancel();
        m_Event.Cancel();
    }
    else
    {
        // unsleep
        SetAutoSearchEnemy();
        ToIdle();
    }
}

bool CActorAI::ToRandMove()
{
    ChangeState(ATT_RANDMOVE);

    float dis = GameMath::distance(m_posBorn, GetActor()->GetPos());
    if(dis < GetAIData().idle_randomwalk_range())
    {
        m_posTarget = GetActor()->GetPos() + GameMath::random_vector2(GetAIData().idle_randomwalk_step_min(),
                                                                      GetAIData().idle_randomwalk_step_max());
    }
    else
    {
        m_posTarget = m_posBorn + GameMath::random_vector2(GetAIData().idle_randomwalk_step_min(),
                                                           GetAIData().idle_randomwalk_step_max());
    }

    AddNextCall(MOVE_PER_WAIT_MS);
    return true;
}

bool CActorAI::ToPratol()
{
    if(m_pPathData == nullptr)
        return false;

    if(m_pPathData->patrol_type() == PARTOL_ONCE && m_nCurPathNode > m_pPathData->data_size())
    {
        return false;
    }
    else
    {
        const auto& data = GetCurPratolData();
        m_posTarget      = Vector2(data.x(), data.y());
        AddNextCall(MOVE_PER_WAIT_MS);
        ChangeState(ATT_PRATOL);
        return true;
    }
}

bool CActorAI::ToIdle()
{
    if(TryExecScript(SCB_AI_TO_IDLE, this) == true)
        return true;

    if(GetAIData().follow_path())
    {
        // 如果是巡逻怪物，则开始巡逻
        if(ToPratol())
            return true;
    }

    ChangeState(ATT_IDLE);

    if(GetActor()->GetCurrentViewActorCount() == 0)
    {
        SetAISleep(true);
        return false;
    }

    if(GetAIData().idle_randomwalk_ms_max() > 0 && GetActor()->GetMoveSpeed() != 0)
    {
        AddNextCall(random_uint32_range(GetAIData().idle_randomwalk_ms_min(), GetAIData().idle_randomwalk_ms_max()));
    }
    return true;
}

bool CActorAI::ToAttack()
{
    ChangeState(ATT_ATTACK);
    AddNextCall(GetAIData().attack_wait_ms());
    return true;
}

bool CActorAI::ToApproach()
{
    if(GetActor()->GetMoveSpeed() == 0 || GetAIData().can_approach() == false)
    {
        return ToAttack();
    }

    ChangeState(ATT_APPROACH); // 转换到approach状态
    AddNextCall(200);
    return true;
}

bool CActorAI::ToSkill()
{
    CAIActor* pTarget = AIActorManager()->QueryActor(GetMainTarget());
    if(pTarget == nullptr || pTarget->IsDead() == true)
    { // 目标不存在或已死亡
        SetMainTarget(0);
        return ToAttack();
    }
    CSkillType* pCurSkillType = SkillTypeSet()->QueryObj(GetCurSkillTypeID());
    if(pCurSkillType == nullptr)
    {
        SetMainTarget(0);
        return ToAttack();
    }

    float dis = GameMath::distance(GetActor()->GetPos(), pTarget->GetPos());
    if(dis < pCurSkillType->GetDistance())
    {
        //距离不正确,找到正确的距离
        m_fTargetDis = random_float(pCurSkillType->GetDistance(), pCurSkillType->GetMaxDistance());
        ToApproach();
        return true;
    }
    else if(dis > pCurSkillType->GetMaxDistance())
    {
        m_fTargetDis = random_float(pCurSkillType->GetDistance(), pCurSkillType->GetMaxDistance());
        return ToApproach();
    }

    LOGAIDEBUG(GetAIData().ai_debug(),
               "AI: {} castSkill: {} Target:{}",
               GetActor()->GetID(),
               pCurSkillType->GetSkillID(),
               m_idTarget);

    //可以释放
    GetActor()->CastSkill(pCurSkillType->GetSkillID(), m_idTarget);
    ChangeState(ATT_SKILL); // 转换到skill状态
    return true;
}

bool CActorAI::ToEscape(OBJID idTarget)
{
    CAIActor* pTarget = AIActorManager()->QueryActor(idTarget);
    if(pTarget == nullptr || pTarget->IsDead() == true)
    { // 目标不存在或已死亡
        return ToAttack();
    }

    auto vDir   = GetActor()->GetPos() - pTarget->GetPos();
    m_posTarget = vDir * random_float(GetAIData().escape_range_min(), GetAIData().escape_range_max());

    ChangeState(ATT_ESCAPE); // 转换到ESCAPE状态
    AddNextCall(MOVE_PER_WAIT_MS);
    return true;
}

bool CActorAI::ToGoBack()
{
    if(TryExecScript(SCB_AI_TO_GOBACK, this) == true)
        return true;

    m_posTarget = m_posRecord;
    ChangeState(ATT_GOBACK); // 转换到ESCAPE状态
    AddNextCall(MOVE_PER_WAIT_MS);
    return true;
}

bool CActorAI::ToPatrolWait(uint32_t wait_min, uint32_t wait_max)
{
    ChangeState(ATT_PRATOLWAIT); // 转换到ESCAPE状态
    AddNextCall(random_uint32_range(wait_min, wait_max));
    return true;
}

bool CActorAI::ToSkillFinish(uint32_t stun_ms)
{
    ChangeState(ATT_SKILLWAIT); // 转换到ESCAPE状态
    AddNextCall(stun_ms);
    return true;
}

void CActorAI::ProcessAttack()
{
    if(GetMainTarget() == 0)
    {
        if(FindNextEnemy() == false)
        {
            ToGoBack();
            return;
        }
    }

    CAIActor* pTarget = AIActorManager()->QueryActor(m_idTarget);
    if(pTarget == nullptr || pTarget->IsDead())
    {
        if(FindNextEnemy() == false)
        {
            ToGoBack();
            return;
        }
        else
        {
            ToAttack();
            return;
        }
    }

    if(TryExecScript(SCB_AI_PROCESS_ATTACK, this, pTarget) == true)
        return;

    float dis = GameMath::distance(GetActor()->GetPos(), pTarget->GetPos());
    if(dis > GetAIData().attack_target_range())
    {
        SetMainTarget(0);
        ToAttack();
        return;
    }

    double self_hp    = double(GetActor()->GetHP()) / double(GetActor()->GetHPMax());
    double self_mp    = double(GetActor()->GetMP()) / double(GetActor()->GetMPMax());
    double target_hp  = double(pTarget->GetHP()) / double(pTarget->GetHPMax());
    auto   pSkillData = GetActor()->GetSkillSet().ChooseSkill(dis, self_hp, self_mp, target_hp);
    if(pSkillData == nullptr)
    {
        SetMainTarget(0);
        ToAttack();
        return;
    }

    LOGAIDEBUG(GetAIData().ai_debug(),
               "AI: {} SelectSkill: {}",
               GetActor()->GetID(),
               pSkillData->GetSkillType()->GetID());
    SetCurSkillTypeID(pSkillData->GetSkillType()->GetID());
    ToSkill();
}

void CActorAI::ProcessRandMove()
{
    if(GetActor()->GetPos() == m_posTarget)
    {
        ToIdle();
        return;
    }
    auto posStep = PathFind()->SearchStep(m_posTarget, GetActor()->GetMoveSpeed());
    if(posStep == GetActor()->GetPos())
    {
        //没有找到路径
    }
    GetActor()->MoveToTarget(posStep);
    AddNextCall(MOVE_PER_WAIT_MS);
}

void CActorAI::ProcessIdle()
{
    ToRandMove();
}

void CActorAI::ProcessSkillWait()
{
    ToAttack();
}

void CActorAI::ProcessApproach()
{
    CAIActor* pTarget = AIActorManager()->QueryActor(GetMainTarget());
    if(pTarget == nullptr || pTarget->IsDead() == true)
    {
        ToAttack();
        return;
    }

    float dis_born = GameMath::distance(GetActor()->GetPos(), m_posRecord);
    if(dis_born > GetAIData().attack_pursue_selfrange())
    {
        ToGoBack();
        return;
    }

    //从对方的位置，计算反向的距离
    auto  vDir  = GetActor()->GetPos() - pTarget->GetPos();
    float fDis  = vDir.normalise();
    m_posTarget = pTarget->GetPos() + vDir * m_fTargetDis;

    if(GetActor()->GetPos() != m_posTarget)
    {
        auto posStep = PathFind()->SearchStep(m_posTarget, GetActor()->GetMoveSpeed());
        if(posStep == GetActor()->GetPos())
        {
            //没有找到路径
        }
        GetActor()->MoveToTarget(posStep);
        AddNextCall(MOVE_PER_WAIT_MS);
    }
    else
    {
        ToSkill();
    }
}

void CActorAI::ProcessEscape()
{
    if(GetActor()->GetPos() != m_posTarget)
    {
        auto posStep = PathFind()->SearchStep(m_posTarget, GetActor()->GetMoveSpeed());
        if(posStep == GetActor()->GetPos())
        {
            //没有找到路径
        }
        GetActor()->MoveToTarget(posStep);
        AddNextCall(MOVE_PER_WAIT_MS);
    }
    else
    {
        ToAttack();
    }
}

void CActorAI::ProcessGoback()
{
    if(GetActor()->GetPos() != m_posTarget)
    {
        auto posStep = PathFind()->SearchStep(m_posTarget, GetActor()->GetMoveSpeed());
        if(posStep == GetActor()->GetPos())
        {
            //没有找到路径
        }

        GetActor()->MoveToTarget(posStep);
        AddNextCall(MOVE_PER_WAIT_MS);
    }
    else
    {
        SetAutoSearchEnemy();
        ToIdle();
    }
}

void CActorAI::ProcessSkill()
{
    //等待技能释放完成的消息收到
}

void CActorAI::ProcessPatrol()
{
    if(GetActor()->GetPos() != m_posTarget)
    {
        auto posStep = PathFind()->SearchStep(m_posTarget, GetActor()->GetMoveSpeed());
        if(posStep == GetActor()->GetPos())
        {
            //没有找到路径
        }
        GetActor()->MoveToTarget(posStep);
        AddNextCall(MOVE_PER_WAIT_MS);
    }
    else
    {
        const auto& data = GetCurPratolData();
        if(data.wait_ms_min() == 0 && data.wait_ms_max() == 0)
            ToPatrolWait(GetAIData().patrol_wait_ms_min(), GetAIData().patrol_wait_ms_max());
        else
            ToPatrolWait(data.wait_ms_min(), data.wait_ms_max());
    }
}

void CActorAI::ProcessPatrolWait()
{
    ProcessPatrol();
}

const ::Cfg_Scene_Patrol_Row_patrol_data& CActorAI::GetCurPratolData()
{
    uint32_t nIdxPath = m_nCurPathNode;
    switch(m_pPathData->patrol_type())
    {
        case PARTOL_ONCE:
        {
        }
        break;
        case PARTOL_RING:
        {
            nIdxPath = m_nCurPathNode % m_pPathData->data_size();
        }
        break;
        case PARTOL_BACK:
        {
            uint32_t nRound = nIdxPath / m_pPathData->data_size();
            if((nRound & 1) == 0)
            {
                //正向
                nIdxPath = m_nCurPathNode % m_pPathData->data_size();
            }
            else
            {
                //反向
                nIdxPath = m_pPathData->data_size() - (m_nCurPathNode % m_pPathData->data_size());
            }
        }
        break;
    }
    return m_pPathData->data(nIdxPath);
}

void CActorAI::OrderAttack(OBJID idTarget)
{
    //判断ai类型
    if(m_pAIType->GetDataRef().ai_type() == AITYPE_NONE)
        return;
    if(m_nState == ATT_GOBACK || m_nState == ATT_ESCAPE)
        return;
    if(m_nState == ATT_PRATOL || m_nState == ATT_PRATOLWAIT || m_nState == ATT_RANDMOVE || m_nState == ATT_IDLE)
        m_posRecord = GetActor()->GetPos();

    SetMainTarget(idTarget);
    ToAttack();
    m_SearchEnemyEvent.Cancel();
}

void CActorAI::ClearHateList()
{
    m_HateList.ClearHateList();
}

void CActorAI::AddHate(OBJID idTarget, int32_t nHate)
{
    float fHate = m_HateList.AddHate(idTarget, nHate);
    if(m_idTarget != 0)
    {
        auto pMainHateData = m_HateList.GetHate(m_idTarget);
        CHECK(pMainHateData);

        float fMainTargetHate = pMainHateData->fHate;
        if(fHate > fMainTargetHate * 1.5f)
        {
            // change enemy
            OrderAttack(idTarget);
        }
    }
    else
    {
        OrderAttack(idTarget);
    }
}

bool CActorAI::FindEnemyInHateList()
{
    m_HateList.FindIF([pThis = this](ST_HATE_DATA* pHateData) {
        CAIActor* pActor = AIActorManager()->QueryActor(pHateData->idTarget);
        if(pActor == nullptr)
            return false;
        if(pActor->IsDead())
            return false;
        if(GameMath::distance(pActor->GetPos(), pThis->GetActor()->GetPos()) >=
           pThis->GetAIData().attack_target_range())
            return false;

        //设置目标
        pThis->SetMainTarget(pActor->GetID());
        return true;
    });
    return GetMainTarget() != 0;
}

bool CActorAI::ForEachInHateList(std::function<bool(ST_HATE_DATA*)> func)
{
    m_HateList.FindIF([pThis = this](ST_HATE_DATA* pHateData) {
        CAIActor* pActor = AIActorManager()->QueryActor(pHateData->idTarget);
        if(pActor == nullptr)
            return false;
        if(GameMath::distance(pActor->GetPos(), pThis->GetActor()->GetPos()) >=
           pThis->GetAIData().attack_target_range())
            return false;

        //设置目标
        pThis->SetMainTarget(pActor->GetID());
        return true;
    });

    return true;
}

bool CActorAI::FindNextEnemy()
{
    SetMainTarget(0);
    if(TryExecScript(SCB_AI_FINDNEXTENEMY, this) == true)
        return true;

    if(FindEnemyInHateList() == true)
        return true;

    //遍历周围的敌人
    if(m_pAIType->GetDataRef().ai_type() == AITYPE_ACTIVE)
    {
        if(m_pAIType == nullptr || m_pAIType->GetTargetFAM() == nullptr)
        {
            OBJID idTarget = SearchEnemy();
            if(idTarget != ID_NONE)
            {
                SetMainTarget(ID_NONE);
                return GetMainTarget() != ID_NONE;
            }
        }
        else
        {
            double self_hp     = (double)m_pActor->GetHP() / (double)m_pActor->GetHPMax();
            double max_fDom    = -1.0f;
            OBJID  idNewTarget = ID_NONE;
            for(OBJID idActor: m_pActor->_GetViewList())
            {
                //找到第一个在范围内的敌人
                CAIActor* pActor = AIActorManager()->QueryActor(idActor);
                if(pActor == nullptr)
                    continue;
                if(pActor->IsDead())
                    continue;
                if(GetActor()->IsEnemy(pActor) == false)
                    continue;

                float dis = GameMath::distance(pActor->GetPos(), GetActor()->GetPos());
                if(dis >= m_pAIType->GetDataRef().search_enemy_range())
                    continue;
                double target_hp = (double)pActor->GetHP() / (double)pActor->GetHPMax();

                double fDom = m_pAIType->GetTargetFAM()->calculate(dis, self_hp, target_hp);
                if(fDom > max_fDom)
                {
                    max_fDom    = fDom;
                    idNewTarget = idActor;
                }
            }
            SetMainTarget(idNewTarget);
            return GetMainTarget() != ID_NONE;
        }
    }

    return false;
}

OBJID CActorAI::GetMainTarget() const
{
    return m_idTarget;
}

void CActorAI::SetMainTarget(OBJID val)
{
    if(m_idTarget == val)
        return;

    LOGAIDEBUG(GetAIData().ai_debug(), "AI: {} SetMainTarget: {} To {}", GetActor()->GetID(), m_idTarget, val);
    m_idTarget = val;
}

CAIActor* CActorAI::GetActor() const
{
    return m_pActor;
}

CAIType* CActorAI::GetAIType() const
{
    return m_pAIType;
}

const Cfg_AIType_Row& CActorAI::GetAIData() const
{
    return GetAIType()->GetDataRef();
}

void CActorAI::AddNextCall(uint32_t ms)
{
    m_Event.Cancel();
    EventManager()->ScheduleEvent(0, std::bind(&CActorAI::Process, this), ms, false, m_Event);
}

void CActorAI::SetAutoSearchEnemy()
{
    if(GetAIType()->GetDataRef().ai_type() != AITYPE_ACTIVE)
        return;

    m_SearchEnemyEvent.Cancel();
    uint32_t ms = random_uint32_range(GetAIType()->GetDataRef().search_enemy_ms_min(),
                                      GetAIType()->GetDataRef().search_enemy_ms_max());
    EventManager()->ScheduleEvent(0, std::bind(&CActorAI::_SearchEnemy_CallBack, this), ms, false, m_SearchEnemyEvent);
}

void CActorAI::_SearchEnemy_CallBack()
{
    OBJID idActor = SearchEnemy();
    if(idActor == ID_NONE)
    {
        SetAutoSearchEnemy();
    }
    else
    {
        OrderAttack(idActor);
    }
}

OBJID CActorAI::SearchEnemy()
{
    if(GetAIData().script_id() != 0)
    {
        OBJID idTarget = ScriptManager()->TryExecScript<OBJID>(GetAIData().script_id(), SCB_AI_SEARCHENEMY, this);
        if(idTarget != 0)
            return idTarget;
    }

    for(OBJID idActor: m_pActor->_GetViewList())
    {
        //找到第一个在范围内的敌人
        CAIActor* pActor = AIActorManager()->QueryActor(idActor);
        if(pActor == nullptr)
            continue;
        if(pActor->IsDead())
            continue;
        if(GetActor()->IsEnemy(pActor) == false)
            continue;
        if(GameMath::distance(pActor->GetPos(), GetActor()->GetPos()) >= m_pAIType->GetDataRef().search_enemy_range())
            continue;

        //设置目标

        return idActor;
    }
    return ID_NONE;
}
