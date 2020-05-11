#include "Pet.h"

#include "ActorManager.h"
#include "PetType.h"
#include "Phase.h"
#include "Player.h"
#include "ZoneService.h"
#include "server_msg/server_side.pb.h"

OBJECTHEAP_IMPLEMENTATION(CPet, s_heap);

CPet::CPet()
{
    SetCampID(CAMP_PLAYER);
}

CPet::~CPet()
{
    if(GetCurrentScene() != nullptr)
        GetCurrentScene()->LeaveMap(this);
}

bool CPet::Init(CPetSet* pPetSet, CDBRecordPtr&& pRecord)
{
    __ENTER_FUNCTION
    m_pRecord.reset(pRecord.release());
    m_pPetSet = pPetSet;
    SetID(m_pRecord->Field(TBLD_PET::ID));
    CHECKF(CActor::Init());

    m_pType = PetTypeSet()->QueryObj(GetPetTypeID());
    m_ActorAttrib.load_from(m_pType->getData());

    m_pCDSet.reset(CCoolDownSet::CreateNew());
    CHECKF(m_pCDSet.get());

    RecalcAttrib(true);

    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CPet::CanDamage(CActor* pTarget) const
{
    __ENTER_FUNCTION
    if(this == pTarget)
        return false;
    if(GetOwnerID() != 0)
    {
        CPlayer* pOwner = ActorManager()->QueryPlayer(GetOwnerID());
        if(pOwner)
            return pOwner->CanDamage(pTarget);
    }
    __LEAVE_FUNCTION
    return false;
}

void CPet::BeKillBy(CActor* pAttacker)
{
    CActor::BeKillBy(pAttacker);
}

bool CPet::IsEnemy(CSceneObject* pTarget) const
{
    __ENTER_FUNCTION
    if(this == pTarget)
        return false;
    if(GetOwnerID() != 0)
    {
        CPlayer* pOwner = ActorManager()->QueryPlayer(GetOwnerID());
        if(pOwner)
            return pOwner->IsEnemy(pTarget);
    }

    __LEAVE_FUNCTION
    return false;
}

void CPet::MakeShowData(SC_AOI_NEW& msg)
{
    __ENTER_FUNCTION
    msg.set_mapid(GetMapID());

    msg.set_actor_id(GetID());
    msg.set_actortype(ACT_PET);
    msg.set_campid(GetCampID());
    msg.set_ownerid(GetOwnerID());
    msg.set_lev(GetLev());

    msg.set_prof(GetPetTypeID());
    msg.set_name(GetName());
    msg.set_hp(GetHP());
    msg.set_hpmax(GetHPMax());
    __LEAVE_FUNCTION
}

void CPet::OnEnterMap(CSceneBase* pScene)
{
    __ENTER_FUNCTION

    CActor::OnEnterMap(pScene);

    ServerMSG::ActorCreate ai_msg;
    ai_msg.set_actor_id(GetID());
    ai_msg.set_scene_id(GetSceneIdx());
    ai_msg.set_actortype(ACT_PET);
    ai_msg.set_prof(GetPetTypeID());
    ai_msg.set_lev(GetLev());
    ai_msg.set_campid(GetCampID());
    ai_msg.set_phase_id(GetPhaseID());
    ai_msg.set_name(GetName());
    ai_msg.set_hp(GetHP());
    ai_msg.set_hpmax(GetHPMax());
    ai_msg.set_mp(GetMP());
    ai_msg.set_mpmax(GetMPMax());
    ai_msg.set_movespd(GetAttrib().get(ATTRIB_MOVESPD));
    ai_msg.set_posx(GetPosX());
    ai_msg.set_posy(GetPosY());
    ai_msg.set_ownerid(GetOwnerID());
    ZoneService()->SendServerMsgToAIService(ai_msg);
    __LEAVE_FUNCTION
}

void CPet::OnLeaveMap(uint16_t idTargetMap)
{
    __ENTER_FUNCTION

    CActor::OnLeaveMap(idTargetMap);
    __LEAVE_FUNCTION
}

void CPet::Save() {}

void CPet::CallOut()
{
    __ENTER_FUNCTION
    if(GetCurrentScene() != nullptr)
        return;

    CPlayer* pOwner = ActorManager()->QueryPlayer(GetOwnerID());
    CHECK(pOwner);
    CHECK(pOwner->GetCurrentScene());

    _SetPhaseID(pOwner->GetPhaseID());
    auto findPos = pOwner->GetCurrentScene()->FindPosNearby(Vector2(pOwner->GetPosX(), pOwner->GetPosY()), 2.0f);
    pOwner->GetCurrentScene()->EnterMap(this, findPos.x, findPos.y, 0.0f);

    __LEAVE_FUNCTION
}

void CPet::CallBack()
{
    __ENTER_FUNCTION
    if(GetCurrentScene() == nullptr)
        return;

    CPlayer* pOwner = ActorManager()->QueryPlayer(GetOwnerID());
    CHECK(pOwner);
    CHECK(pOwner->GetCurrentScene());

    pOwner->GetCurrentScene()->LeaveMap(this);
    __LEAVE_FUNCTION
}
