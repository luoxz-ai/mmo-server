#include <regex>

#include "Item.h"
#include "Npc.h"
#include "Player.h"
#include "ZoneService.h"
#include "msg/ts_cmd.pb.h"
#include "msg/zone_service.pb.h"

void OnMsg_CS_LOADMAP_SUCC(CPlayer* pPlayer, const CS_LOADMAP_SUCC& msg)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() != nullptr)
		return;

	pPlayer->OnLoadMapSucc();
	__LEAVE_FUNCTION
}

void OnMsg_CS_MOVE(CPlayer* pPlayer, const CS_MOVE& msg)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;

	if(pPlayer->GetCurrentScene()->GetMapID() != msg.mapid())
		return;

	if(pPlayer->MoveTo(Vector2(msg.x(), msg.y())) == false)
	{
		// send kick back to client
		return;
	}
	pPlayer->SetFace(msg.face());

	__LEAVE_FUNCTION
}

void OnMsg_CS_CHANGEMAP(CPlayer* pPlayer, const CS_CHANGEMAP& msg)
{
	__ENTER_FUNCTION

	if(pPlayer->GetCurrentScene() == nullptr)
		return;

	if(pPlayer->GetCurrentScene()->GetMapID() != msg.mapid())
		return;
	pPlayer->TryChangeMap(msg.leavepoint_idx());
	__LEAVE_FUNCTION
}

void OnMsg_CS_CASTSKILL(CPlayer* pPlayer, const CS_CASTSKILL& msg)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;

	if(pPlayer->GetCurrentScene()->GetMapID() != msg.mapid())
		return;

	if(pPlayer->GetCurrentScene()->GetMap()->HasMapFlag(MAPFLAG_DISABLE_ATTACK) == false)
		return;

	pPlayer->GetSkillManager()->CastSkill(msg.target_id(), msg.skill_id(), Vector2(msg.x(), msg.y()));

	__LEAVE_FUNCTION
}

void OnMsg_CS_SKILL_BREAK(CPlayer* pPlayer, const CS_SKILL_BREAK& msg)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;
	if(pPlayer->GetCurrentScene()->GetMapID() != msg.mapid())
		return;

	if(pPlayer->GetCurrentScene()->GetMap()->HasMapFlag(MAPFLAG_DISABLE_ATTACK) == false)
		return;

	pPlayer->GetSkillFSM().BreakIntone();
	pPlayer->GetSkillFSM().BreakLaunch();

	__LEAVE_FUNCTION
}

void OnMsg_CS_CHANGE_PKMODE(CPlayer* pPlayer, const CS_CHANGE_PKMODE& msg)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;

	if(pPlayer->GetCurrentScene()->GetMap()->HasMapFlag(MAPFLAG_DISABLE_CHANGEPK) == false)
		return;

	pPlayer->SetPKMode(msg.pkmode());

	__LEAVE_FUNCTION
}

void OnMsg_CS_REBORN(CPlayer* pPlayer, const CS_REBORN& msg)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;

	if(pPlayer->IsDead() == false)
		return;

	pPlayer->Reborn(msg.reborn_type());
	__LEAVE_FUNCTION
}

void OnMsg_CS_ITEM_SWAP(CPlayer* pPlayer, const CS_ITEM_SWAP& msg)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;

	if(pPlayer->IsDead() == false)
		return;

	CPackage* pPackage = pPlayer->QueryPackage(msg.position());
	CHECK(pPackage);
	pPackage->SwapItem(msg.grid1(), msg.grid2());
	__LEAVE_FUNCTION
}

void OnMsg_CS_ITEM_SPLIT(CPlayer* pPlayer, const CS_ITEM_SPLIT& msg)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;

	if(pPlayer->IsDead() == false)
		return;

	CPackage* pPackage = pPlayer->QueryPackage(msg.position());
	CHECK(pPackage);
	pPackage->SplitItem(msg.grid1(), msg.grid2(), msg.split_num());
	__LEAVE_FUNCTION
}

void OnMsg_CS_ITEM_COMBINE(CPlayer* pPlayer, const CS_ITEM_COMBINE& msg)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;

	if(pPlayer->IsDead() == false)
		return;

	CPackage* pPackage = pPlayer->QueryPackage(msg.position());
	CHECK(pPackage);
	pPackage->CombineItem(msg.grid1(), msg.grid2());
	__LEAVE_FUNCTION
}

void OnMsg_CS_ITEM_TIDY(CPlayer* pPlayer, const CS_ITEM_TIDY& msg)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;

	if(pPlayer->IsDead() == false)
		return;

	CPackage* pPackage = pPlayer->QueryPackage(msg.position());
	CHECK(pPackage);
	pPackage->TidyItem();
	__LEAVE_FUNCTION
}

void OnMsg_CS_ITEM_USE(CPlayer* pPlayer, const CS_ITEM_USE& msg)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;

	if(pPlayer->IsDead() == false)
		return;

	pPlayer->UseItem(msg.grid(), msg.count());
	__LEAVE_FUNCTION
}

void OnMsg_CS_ITEM_STORAGE_OPEN(CPlayer* pPlayer, const CS_ITEM_STORAGE_OPEN& msg)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;

	if(pPlayer->IsDead() == false)
		return;

	CStoragePackage* pPackage = pPlayer->GetStroagePackage();
	CHECK(pPackage);
	pPackage->SendAllItemInfo();
	__LEAVE_FUNCTION
}

void OnMsg_CS_ITEM_STORAGE_CHECKIN(CPlayer* pPlayer, const CS_ITEM_STORAGE_CHECKIN& msg)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;

	if(pPlayer->IsDead() == false)
		return;

	CStoragePackage* pPackage = pPlayer->GetStroagePackage();
	CHECK(pPackage);
	pPackage->CheckIn(msg.grid_in_package());
	__LEAVE_FUNCTION
}

void OnMsg_CS_ITEM_STORAGE_CHECKOUT(CPlayer* pPlayer, const CS_ITEM_STORAGE_CHECKOUT& msg)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;

	if(pPlayer->IsDead() == false)
		return;

	CStoragePackage* pPackage = pPlayer->GetStroagePackage();
	CHECK(pPackage);
	pPackage->CheckIn(msg.grid_in_storage());
	__LEAVE_FUNCTION
}

void OnMsg_CS_EQUIP(CPlayer* pPlayer, const CS_EQUIP& msg)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;

	if(pPlayer->IsDead() == false)
		return;

	pPlayer->GetEquipmentSet()->EquipItem(msg.grid_in_package(), msg.grid_in_equip(), SYNC_TRUE);
	__LEAVE_FUNCTION
}

void OnMsg_CS_UNEQUIP(CPlayer* pPlayer, const CS_UNEQUIP& msg)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;

	if(pPlayer->IsDead() == false)
		return;
	pPlayer->GetEquipmentSet()->UnequipItem(msg.grid_in_equip(), SYNC_TRUE, true, true);
	__LEAVE_FUNCTION
}

void OnMsg_CS_ACHI_TAKE(CPlayer* pPlayer, const CS_ACHI_TAKE& msg)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;

	if(pPlayer->IsDead() == false)
		return;

	pPlayer->GetAchievement()->TakeReward(msg.achi_id());
	__LEAVE_FUNCTION
}

void OnMsg_CS_TALK(CPlayer* pPlayer, const CS_TALK& msg)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;

	if(msg.words().empty())
		return;

	if(pPlayer->IsTalkEnable(msg.channel()) == false)
	{
		return;
	}

	uint32_t nGMLev = GMManager()->GetGMLevel(pPlayer->GetOpenID());
	if(nGMLev > 0)
	{
		// gm发言全部记录
		LOGGM("{}", msg.words().c_str());
		if(msg.words()[0] == GMCMD_PREFIX)
		{
			GMManager()->ProcessGMCmd(pPlayer, msg.words());
			return;
		}
	}
	else
	{
		if(msg.words()[0] == GMCMD_PREFIX)
		{
			//非GM则屏蔽所有以GMCMD_PREFIX打头的文字
			return;
		}
	}

	std::string send_txt = msg.words();

	if(nGMLev == 0)
	{
		// 屏蔽字过滤
		ReplaceIllegaWords(send_txt);
	}
	SC_TALK send_msg;
	send_msg.set_words(std::move(send_txt));
	send_msg.set_channel(msg.channel());
	send_msg.set_sender_id(pPlayer->GetID());
	send_msg.set_sender_name(pPlayer->GetName());
	send_msg.set_sender_viplev(pPlayer->GetVipLev());
	send_msg.set_sender_sign(0); //发送者标志（是否新手辅导员，或者其他的特殊标志)
	send_msg.set_send_time(TimeGetSecond());

	// 物品展示信息替换
	// Extraction of a sub-match
	static std::regex base_regex("\\[#i ([0-9]+)\\]", std::regex::optimize);
	std::smatch		  base_match;

	std::vector<OBJID> vecIDItem;
	try
	{
		std::string txt = send_txt;
		while(std::regex_search(txt, base_match, base_regex) == true)
		{
			vecIDItem.push_back(std::stoul(base_match[1].str()));
			txt = base_match.suffix();
		}
	}
	catch(...)
	{
	}

	for(auto idItem: vecIDItem)
	{
		CItem* pItem = pPlayer->GetBag()->QueryItem(idItem);
		if(pItem == nullptr)
			pItem = pPlayer->GetEquipmentSet()->QueryEquipmentById(idItem);

		if(pItem)
		{
			SC_ITEM_INFO* pData = send_msg.add_item_list();
			pItem->FillItemInfo(pData);
		}
	}

	switch(msg.channel())
	{
		case CHANNEL_NORMAL: //周围聊天
		{
			pPlayer->SendRoomMessage(CMD_SC_TALK, send_msg);
		}
		break;
		case CHANNEL_SCENE: //场景内聊天
		{
			static_cast<CScene*>(pPlayer->GetCurrentScene())->SendSceneMessage(CMD_SC_TALK, send_msg);
		}
		break;
		case CHANNEL_PRIVATE: //私聊
		{
			//发送给reciver所在的world， 由world转发
			ZoneService()->SendPortMsg(ServerPort(GetWorldIDFromPlayerID(msg.reciver_id()), WORLD_SERVICE_ID), CMD_SC_TALK, send_msg);
		}
		break;
		case CHANNEL_TEAM: //组队
		{
			//发送给自己当前的World
			pPlayer->SendWorldMessage(CMD_SC_TALK, send_msg);
		}
		break;
		case CHANNEL_GUILD: //公会
		{
			//发送给自己当前的World
			pPlayer->SendWorldMessage(CMD_SC_TALK, send_msg);
		}
		break;
		case CHANNEL_WORLD: //世界
		{
			//扣除消费道具

			//发送给自己当前的World
			pPlayer->SendWorldMessage(CMD_SC_TALK, send_msg);
		}
		break;
		case CHANNEL_TRUMPET: //小喇叭
		{
			//扣除消费道具

			//发送给自己当前的World
			pPlayer->SendWorldMessage(CMD_SC_TALK, send_msg);
		}
		break;
		case CHANNEL_GLOBAL: //全游戏
		case CHANNEL_SYSTEM: //系统
		case CHANNEL_RUMOR:	 //广播
		{
			return;
		}
		break;
		default:
			break;
	}

	__LEAVE_FUNCTION
}

void OnMsg_CS_TASK_SUBMIT(CPlayer* pPlayer, const CS_TASK_SUBMIT& msg)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;

	if(pPlayer->IsDead() == false)
		return;

	pPlayer->GetTaskSet()->SubmitTaskByMessage(msg.task_id(), msg.submit_multiple());
	__LEAVE_FUNCTION
}

void OnMsg_CS_ACTIVE_NPC(CPlayer* pPlayer, const CS_ACTIVE_NPC& msg)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;

	if(pPlayer->IsDead() == false)
		return;

	if(pPlayer->IsInViewActorByID(msg.npc_id()) == false)
		return;

	CActor* pActor = ActorManager()->QueryActor(msg.npc_id());
	if(pActor->IsNpc() == false)
		return;

	if(GameMath::distance(pActor->GetPos(), pPlayer->GetPos()) < MIN_INTERACT_DIS)
		return;

	CNpc* pNpc = pActor->ConvertToDerived<CNpc>();
	pNpc->ActiveNpc(pPlayer);
	__LEAVE_FUNCTION
}

void OnMsg_CS_DIALOG_CLICK(CPlayer* pPlayer, const CS_DIALOG_CLICK& msg)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;

	if(pPlayer->IsDead() == false)
		return;

	pPlayer->OnDialogClick(msg.dialog_id(), msg.dialog_button_idx());
	__LEAVE_FUNCTION
}

void OnMsg_CS_TEAMCREATE(CPlayer* pPlayer, const CS_TEAMCREATE& msg)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;

	pPlayer->TeamCreate();

	__LEAVE_FUNCTION
}

void OnMsg_CS_TEAMQUIT(CPlayer* pPlayer, const CS_TEAMQUIT& msg)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;
	pPlayer->TeamQuit();

	__LEAVE_FUNCTION
}

void OnMsg_CS_TEAMKICKMEMBER(CPlayer* pPlayer, const CS_TEAMKICKMEMBER& msg)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;
	pPlayer->TeamKickMember(msg.member_id());

	__LEAVE_FUNCTION
}

void OnMsg_CS_TEAMINVITEMEMBER(CPlayer* pPlayer, const CS_TEAMINVITEMEMBER& msg)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;

	pPlayer->TeamInviteMember(msg.invitee_id());

	__LEAVE_FUNCTION
}

void OnMsg_CS_TEAMACCEPTINVITE(CPlayer* pPlayer, const CS_TEAMACCEPTINVITE& msg)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;

	pPlayer->TeamAcceptInvite(msg.team_id(), msg.inviter_id(), msg.result());

	__LEAVE_FUNCTION
}

void OnMsg_CS_TEAMAPPLYMEMBER(CPlayer* pPlayer, const CS_TEAMAPPLYMEMBER& msg)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;

	pPlayer->TeamApplyMember(msg.respondent_id());

	__LEAVE_FUNCTION
}

void OnMsg_CS_TEAMACCEPTAPPLY(CPlayer* pPlayer, const CS_TEAMACCEPTAPPLY& msg)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;

	pPlayer->TeamAcceptApply(msg.applicant_id(), msg.result());

	__LEAVE_FUNCTION
}
void OnMsg_CS_TEAMNEWLEADER(CPlayer* pPlayer, const CS_TEAMNEWLEADER& msg)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;

	pPlayer->TeamChangeLeader(msg.new_leader_id());

	__LEAVE_FUNCTION
}

//////////////////////////////////////////////////////////////////////////

template<class MsgType>
void OnPlayerMsgProcess(CNetworkMessage* pMsg, const std::function<void(CPlayer* pPlayer, const MsgType& msg)>& func)
{
	__ENTER_FUNCTION
	CPlayer* pPlayer = ZoneService()->GetActorManager()->QueryPlayer(pMsg->GetFrom());
	CHECK(pPlayer);

	MsgType msg;
	if(msg.ParseFromArray(pMsg->GetMsgBody(), pMsg->GetBodySize()) == false)
	{
		return;
	}
	func(pPlayer, msg);
	__LEAVE_FUNCTION
}

void PlayerMessageHandlerRegister()
{
	auto pNetMsgProcess = ZoneService()->GetNetMsgProcess();
	using namespace std::placeholders;
#define REGISTER_MSG_PROCESS(msg) pNetMsgProcess->Register(CMD_##msg, std::bind(&OnPlayerMsgProcess<msg>, _1, &OnMsg_##msg));
	REGISTER_MSG_PROCESS(CS_LOADMAP_SUCC);
	REGISTER_MSG_PROCESS(CS_MOVE);
	REGISTER_MSG_PROCESS(CS_CHANGEMAP);
	REGISTER_MSG_PROCESS(CS_CASTSKILL);
	REGISTER_MSG_PROCESS(CS_SKILL_BREAK);
	REGISTER_MSG_PROCESS(CS_CHANGE_PKMODE);
	REGISTER_MSG_PROCESS(CS_REBORN);
	REGISTER_MSG_PROCESS(CS_ITEM_SWAP);
	REGISTER_MSG_PROCESS(CS_ITEM_SPLIT);
	REGISTER_MSG_PROCESS(CS_ITEM_COMBINE);
	REGISTER_MSG_PROCESS(CS_ITEM_TIDY);
	REGISTER_MSG_PROCESS(CS_ITEM_USE);
	REGISTER_MSG_PROCESS(CS_ITEM_STORAGE_OPEN);
	REGISTER_MSG_PROCESS(CS_ITEM_STORAGE_CHECKIN);
	REGISTER_MSG_PROCESS(CS_ITEM_STORAGE_CHECKOUT);
	REGISTER_MSG_PROCESS(CS_EQUIP);
	REGISTER_MSG_PROCESS(CS_UNEQUIP);
	REGISTER_MSG_PROCESS(CS_TALK);
	REGISTER_MSG_PROCESS(CS_ACHI_TAKE);
	REGISTER_MSG_PROCESS(CS_TASK_SUBMIT);
	REGISTER_MSG_PROCESS(CS_ACTIVE_NPC);
	REGISTER_MSG_PROCESS(CS_TEAMCREATE);
	REGISTER_MSG_PROCESS(CS_TEAMQUIT);
	REGISTER_MSG_PROCESS(CS_TEAMKICKMEMBER);
	REGISTER_MSG_PROCESS(CS_TEAMINVITEMEMBER);
	REGISTER_MSG_PROCESS(CS_TEAMACCEPTINVITE);
	REGISTER_MSG_PROCESS(CS_TEAMAPPLYMEMBER);
	REGISTER_MSG_PROCESS(CS_TEAMACCEPTAPPLY);
	REGISTER_MSG_PROCESS(CS_TEAMNEWLEADER);

#undef REGISTER_MSG_PROCESS
}
