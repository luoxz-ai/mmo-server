#include "Item.h"
#include "MsgZoneProcess.h"
#include "Player.h"
#include "ZoneService.h"
#include "msg/ts_cmd.pb.h"
#include "msg/zone_service.pb.h"

void OnMsg_CS_ITEM_SWAP(CPlayer* pPlayer, const CS_ITEM_SWAP& msg, CNetworkMessage* pMsg)
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

void OnMsg_CS_ITEM_SPLIT(CPlayer* pPlayer, const CS_ITEM_SPLIT& msg, CNetworkMessage* pMsg)
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

void OnMsg_CS_ITEM_COMBINE(CPlayer* pPlayer, const CS_ITEM_COMBINE& msg, CNetworkMessage* pMsg)
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

void OnMsg_CS_ITEM_TIDY(CPlayer* pPlayer, const CS_ITEM_TIDY& msg, CNetworkMessage* pMsg)
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

void OnMsg_CS_ITEM_USE(CPlayer* pPlayer, const CS_ITEM_USE& msg, CNetworkMessage* pMsg)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;

	if(pPlayer->IsDead() == false)
		return;

	pPlayer->UseItem(msg.grid(), msg.count());
	__LEAVE_FUNCTION
}

void OnMsg_CS_ITEM_STORAGE_OPEN(CPlayer* pPlayer, const CS_ITEM_STORAGE_OPEN& msg, CNetworkMessage* pMsg)
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

void OnMsg_CS_ITEM_STORAGE_CHECKIN(CPlayer* pPlayer, const CS_ITEM_STORAGE_CHECKIN& msg, CNetworkMessage* pMsg)
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

void OnMsg_CS_ITEM_STORAGE_CHECKOUT(CPlayer* pPlayer, const CS_ITEM_STORAGE_CHECKOUT& msg, CNetworkMessage* pMsg)
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

void OnMsg_CS_EQUIP(CPlayer* pPlayer, const CS_EQUIP& msg, CNetworkMessage* pMsg)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;

	if(pPlayer->IsDead() == false)
		return;

	pPlayer->GetEquipmentSet()->EquipItem(msg.grid_in_package(), msg.grid_in_equip(), SYNC_TRUE);
	__LEAVE_FUNCTION
}

void OnMsg_CS_UNEQUIP(CPlayer* pPlayer, const CS_UNEQUIP& msg, CNetworkMessage* pMsg)
{
	__ENTER_FUNCTION
	if(pPlayer->GetCurrentScene() == nullptr)
		return;

	if(pPlayer->IsDead() == false)
		return;
	pPlayer->GetEquipmentSet()->UnequipItem(msg.grid_in_equip(), SYNC_TRUE, true, true);
	__LEAVE_FUNCTION
}





////////////////////////////////////////////////////////////////////////////////////////////////


void ZoneItemMessageHandlerRegister()
{
	__ENTER_FUNCTION

	auto pNetMsgProcess = ZoneService()->GetNetMsgProcess();
#define REGISTER_MSG(MsgT) pNetMsgProcess->Register(CMD_##MsgT, std::bind(&ProcPlayerMsg<MsgT, decltype(OnMsg_##MsgT)>, std::placeholders::_1, &OnMsg_##MsgT) );


	REGISTER_MSG(CS_ITEM_SWAP);
	REGISTER_MSG(CS_ITEM_SPLIT);
	REGISTER_MSG(CS_ITEM_COMBINE);
	REGISTER_MSG(CS_ITEM_TIDY);
	REGISTER_MSG(CS_ITEM_USE);
	REGISTER_MSG(CS_ITEM_STORAGE_OPEN);
	REGISTER_MSG(CS_ITEM_STORAGE_CHECKIN);
	REGISTER_MSG(CS_ITEM_STORAGE_CHECKOUT);
	REGISTER_MSG(CS_EQUIP);
	REGISTER_MSG(CS_UNEQUIP);





#undef REGISTER_MSG
	__LEAVE_FUNCTION
}
