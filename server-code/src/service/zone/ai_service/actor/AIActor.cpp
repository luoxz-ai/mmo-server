#include "AIActor.h"

#include "AIMonster.h"
#include "AIPlayer.h"
#include "AIScene.h"
#include "AIService.h"

CAIActor::CAIActor() {}

CAIActor::~CAIActor() {}

void CAIActor::OnCastSkill(uint32_t idSkill)
{
    m_SkillSet.OnCastSkill(idSkill);
}

void CAIActor::SetProperty(uint32_t nType, uint32_t nVal)
{
    __ENTER_FUNCTION
    switch(nType)
    {
        case PROP_HP:
        {
            SetHP(nVal);
        }
        break;
        case PROP_MP:
        {
            SetMP(nVal);
        }
        break;
        case PROP_FP:
        {
        }
        break;
        case PROP_NP:
        {
        }
        break;
        case PROP_CAMP:
        {
            SetCampID(nVal);
        }
        break;
        default:
            break;
    }
    __LEAVE_FUNCTION
}

void CAIActor::CastSkill(uint32_t idSkill, OBJID idTarget)
{
    __ENTER_FUNCTION
    ServerMSG::ActorCastSkill msg;
    msg.set_actor_id(GetID());
    msg.set_target_id(idTarget);
    msg.set_skill_id(idSkill);

    AIService()->SendProtoMsgToScene(msg);
    __LEAVE_FUNCTION
}

float CAIActor::GetMoveSpeed() const
{
    __ENTER_FUNCTION
    return float(m_nMoveSPD) / static_cast<float>(DEFAULT_MOVE_RADIO);
    __LEAVE_FUNCTION
    return 0.0f;
}

void CAIActor::FlyTo(const Vector2& posTarget)
{
    __ENTER_FUNCTION

    ServerMSG::ActorFlyTo msg;
    msg.set_actor_id(GetID());
    msg.set_x(posTarget.x);
    msg.set_y(posTarget.y);
    AIService()->SendProtoMsgToScene(msg);
    LOGAIDEBUG(true, GetID(), "Actor: {} From {} {} FlyTo {} {}", GetID(), m_Pos.x, m_Pos.y, msg.x(), msg.y());

    __LEAVE_FUNCTION
}

void CAIActor::MoveToTarget(const Vector2& posTarget)
{
    __ENTER_FUNCTION
    Vector2 dir = (posTarget - m_Pos);
    float   dis = dir.normalise();
    if(dis < GetMoveSpeed())
    {
        ServerMSG::ActorMove msg;
        msg.set_actor_id(GetID());
        msg.set_x(posTarget.x);
        msg.set_y(posTarget.y);
        AIService()->SendProtoMsgToScene(msg);
        LOGAIDEBUG(true, GetID(), "Actor: {} From {} {} MoveToTargetE {} {}", GetID(), m_Pos.x, m_Pos.y, posTarget.x, posTarget.y);
        SetLastMoveTime(TimeGetMonotonic());
    }
    else
    {
        dir = dir * GetMoveSpeed() * GetMovePassedTime();
        ServerMSG::ActorMove msg;
        msg.set_actor_id(GetID());
        msg.set_x(GetPos().x + dir.x);
        msg.set_y(GetPos().y + dir.y);
        AIService()->SendProtoMsgToScene(msg);
        LOGAIDEBUG(true, GetID(), "Actor: {} From {} {} MoveToTarget {} {}", GetID(), m_Pos.x, m_Pos.y, msg.x(), msg.y());
        SetLastMoveTime(TimeGetMonotonic());
    }
    __LEAVE_FUNCTION
}

float CAIActor::GetMovePassedTime() const
{
    __ENTER_FUNCTION
    float pass_time = (TimeGetMonotonic() - m_LastMoveTime) / 1000;
    if(pass_time > 1.0f)
        return 1.0f;
    else
        return pass_time;

    __LEAVE_FUNCTION
    return 1.0f;
}
