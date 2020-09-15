#include "ActorManager.h"
#include "GameEventDef.h"
#include "Npc.h"
#include "Player.h"

bool CPlayer::DialogBegin(const std::string& title)
{
    __ENTER_FUNCTION
    m_dialog_msg.Clear();
    m_dialog_msg.set_dialog_id(SceneService()->CreateUID());
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CPlayer::DialogAddText(const std::string& txt)
{
    __ENTER_FUNCTION
    m_dialog_msg.add_dialog_txt(txt);
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CPlayer::DialogAddLink(uint32_t           nLinkType,
                            const std::string& link_txt,
                            uint32_t           idFuncType,
                            uint64_t           idData,
                            const std::string& callback_func,
                            uint64_t           idNpc)
{
    __ENTER_FUNCTION
    m_dialog_callback[m_dialog_msg.dialog_id()].push_back({idFuncType, idData, callback_func, idNpc});
    auto pLinkButton = m_dialog_msg.add_dialog_link_list();
    pLinkButton->set_style(nLinkType);
    pLinkButton->set_txt(link_txt);
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CPlayer::DialogSend(uint32_t nDialogType /*=0*/)
{
    __ENTER_FUNCTION
    m_dialog_msg.set_dialog_type(nDialogType);
    SendMsg(m_dialog_msg);
    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CPlayer::OnDialogClick(uint64_t idDialog, uint32_t nIdx)
{
    __ENTER_FUNCTION
    auto it = m_dialog_callback.find(idDialog);
    if(it == m_dialog_callback.end())
    {
        return false;
    }

    const auto& refList = it->second;
    auto        v       = refList[nIdx % refList.size()];
    m_dialog_callback.erase(it);
    switch(v.idFuncType)
    {
        case DIALOG_FUNC_ACCEPTTASK:
        {
            return GetTaskSet()->AcceptTask(v.idData, true, false);
        }
        break;
        case DIALOG_FUNC_SUBMITTASK:
        {
            if(GetTaskSet()->SubmitTask(GetLowFromU64(v.idData), GetHighFromU64(v.idData)) == true)
            {
                if(v.idNpc != 0)
                {
                    return ActiveNpc(v.idNpc);
                }
            }
            return true;
        }
        break;
        case DIALOG_FUNC_QUICKFINISHTASK:
        {
            if(GetTaskSet()->QuickFinish(GetLowFromU64(v.idData)))
            {
                if(v.idNpc != 0)
                {
                    return ActiveNpc(v.idNpc);
                }
            }
            return true;
        }
        break;
        case DIALOG_FUNC_SHOWTASK:
        {
            return GetTaskSet()->ShowTaskDialog(GetLowFromU64(v.idData), v.idNpc);
        }
        break;
        case DIALOG_FUNC_OPENSHOP:
        {
            DialogBegin("");
            DialogSend(GetLowFromU64(v.idData));
            return true;
        }
        break;
        case DIALOG_FUNC_SCRIPT:
        {
            ScriptManager()->ExecScript<void>(v.idData, v.callback_func.c_str(), this);
            return true;
        }
        break;
        default:
            break;
    }

    return false;
    __LEAVE_FUNCTION
    return false;
}

bool CPlayer::ActiveNpc(OBJID idNpc)
{
    __ENTER_FUNCTION
    CActor* pActor = ActorManager()->QueryActor(idNpc);
    if(pActor->IsNpc() == false)
        return false;

    if(GameMath::distance(pActor->GetPos(), GetPos()) < MIN_INTERACT_DIS)
        return false;

    CNpc* pNpc = pActor->CastTo<CNpc>();
    pNpc->ActiveNpc(this);
    return true;
    __LEAVE_FUNCTION
    return false;
}