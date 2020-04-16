#include "Npc.h"

#include "Player.h"
#include "Phase.h"
#include "ActorManager.h"
#include "ZoneService.h"

MEMORYHEAP_IMPLEMENTATION(CNpc, s_heap);
CNpc::CNpc()
{
    SetCampID(CAMP_PLAYER);
}

CNpc::~CNpc()
{
    if(GetCurrentScene() != nullptr)
        GetCurrentScene()->LeaveMap(this);
}

bool CNpc::Init(uint32_t idType)
{
    m_idType = idType;
    m_pType  = NpcTypeSet()->QueryObj(idType);
    CHECKF(m_pType);

    SetID(ActorManager()->GenNpcID());
    SetCampID(m_pType->GetCampID());
    _SetPhaseID(m_pType->GetPhaseID());
    m_ActorAttrib = m_pType->GetAbility();
    CHECKF(CActor::Init());

    RecalcAttrib(true);
    _SetHP(GetHPMax());
    _SetMP(GetMPMax());

    return true;
}

void CNpc::MakeShowData(SC_AOI_NEW& msg)
{
    msg.set_mapid(GetMapID());

    msg.set_actor_id(GetID());
    msg.set_actortype(ACT_NPC);
    msg.set_prof(m_idType);
    msg.set_lev(GetLev());
    msg.set_movespd(GetAttrib().get(ATTRIB_MOVESPD));
    msg.set_campid(GetCampID());
    msg.set_name(GetName());
    msg.set_hp(GetHP());
    msg.set_hpmax(GetHPMax());
}

void CNpc::OnEnterMap(CSceneBase* pScene)
{
    CActor::OnEnterMap(pScene);
    if(m_pType->GetScriptID() != 0)
    {
        ScriptManager()->TryExecScript<void>(m_pType->GetScriptID(), SCB_NPC_ONBORN, this);
    }
    //AI暂时不需要Npc
    // ServerMSG::ActorCreate ai_msg;
    // ai_msg.set_actor_id(GetID());
    // ai_msg.set_scene_id(GetSceneID());
    // ai_msg.set_actortype(ACT_NPC);
    // ai_msg.set_prof(GetTypeID());
    // ai_msg.set_lev(GetLev());
    // ai_msg.set_phase_id(GetPhaseID());
    // ai_msg.set_campid(GetCampID());
    // ai_msg.set_name(GetName());
    // ai_msg.set_hp(GetHP());
    // ai_msg.set_hpmax(GetHPMax());
    // ai_msg.set_mp(GetMP());
    // ai_msg.set_mpmax(GetMPMax());
    // ai_msg.set_movespd(GetAttrib().get(ATTRIB_MOVESPD));
    // ai_msg.set_posx(GetPosX());
    // ai_msg.set_posy(GetPosY());
    // ai_msg.set_ownerid(GetOwnerID());

    // ZoneService()->SendMsgToAIService(ServerMSG::MsgID_ActorCreate, ai_msg);
}

void CNpc::OnLeaveMap(uint64_t idTargetScene)
{
    CActor::OnLeaveMap(idTargetScene);
}

void CNpc::_ActiveNpc(CPlayer* pPlayer)
{
    __ENTER_FUNCTION

    //检查挂在改NPC身上有多少个任务
    auto pVecAccept = TaskTypeSet()->QueryTaskByAcceptNpcID(GetTypeID());
    auto pVecSubmit = TaskTypeSet()->QueryTaskBySubmitNpcID(GetTypeID());
    //检查这些任务有哪些是可以接的
    std::vector<CTaskType*> setShowTask;
    if(pVecSubmit)
    {
        for(auto pTaskType: *pVecSubmit)
        {
            if(pPlayer->GetTaskSet()->CanSubmit(pTaskType) == true)
            {
                setShowTask.push_back(pTaskType);
            }
        }
    }
    if(pVecAccept)
    {
        for(auto pTaskType: *pVecAccept)
        {
            if(pPlayer->GetTaskSet()->CanAccept(pTaskType) == true)
            {
                setShowTask.push_back(pTaskType);
            }
        }
    }

    if(setShowTask.empty() && HasFlag(m_pType->GetTypeFlag(), NPC_TYPE_FLAG_SHOP))
    {
        //直接打开商店界面
        pPlayer->DialogBegin("");
        pPlayer->DialogSend(m_pType->GetShopID());
    }
    if(setShowTask.size() == 1 && HasFlag(m_pType->GetTypeFlag(), NPC_TYPE_FLAG_SHOP) == false)
    {
        //如果只有一个任务，直接跳等待接任务的界面
        auto pTaskType = setShowTask.front();
        pPlayer->GetTaskSet()->ShowTaskDialog(pTaskType->GetID(), GetID());
    }
    else
    {
        pPlayer->DialogBegin(m_pType->GetName());
        pPlayer->DialogAddText(m_pType->GetDialogText());
        if(HasFlag(m_pType->GetTypeFlag(), NPC_TYPE_FLAG_SHOP))
        {
            pPlayer->DialogAddLink(DIALOGLINK_TYPE_LIST,
                                m_pType->GetShopLinkName(),
                                DIALOG_FUNC_OPENSHOP,
                                m_pType->GetShopID(),
                                "",
                                GetID());
        }

        for(auto pTaskType: setShowTask)
        {
            pPlayer->DialogAddLink(DIALOGLINK_TYPE_LIST,
                                   pTaskType->GetName(),
                                   DIALOG_FUNC_SHOWTASK,
                                   pTaskType->GetScriptID(),
                                   "",
                                   GetID());
        }

        pPlayer->DialogSend(DIALOGTYPE_NORMAL);
    }
    __LEAVE_FUNCTION
}

void CNpc::ActiveNpc(CPlayer* pPlayer)
{
    __ENTER_FUNCTION
    if(m_pType->GetScriptID() != 0)
    {
        const std::string& funcName = ScriptManager()->QueryFunc(m_pType->GetScriptID(), SCB_NPC_ONACTIVE);
        if(funcName.empty() == false)
        {
            ScriptManager()->_ExecScript<void>(funcName.c_str(), this, pPlayer);
            return;
        }
    }

    _ActiveNpc(pPlayer);
    __LEAVE_FUNCTION
}
