#include "ActorManager.h"
#include "Equipment.h"
#include "Item.h"
#include "MsgSceneProcess.h"
#include "Package.h"
#include "Player.h"
#include "SceneService.h"
#include "StoragePackage.h"
#include "msg/ts_cmd.pb.h"
#include "msg/zone_service.pb.h"

ON_PLAYERMSG(CS_ITEM_SWAP)
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

ON_PLAYERMSG(CS_ITEM_SPLIT)
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

ON_PLAYERMSG(CS_ITEM_COMBINE)
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

ON_PLAYERMSG(CS_ITEM_TIDY)
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

ON_PLAYERMSG(CS_ITEM_USE)
{
    __ENTER_FUNCTION
    if(pPlayer->GetCurrentScene() == nullptr)
        return;

    if(pPlayer->IsDead() == false)
        return;

    pPlayer->UseItem(msg.grid(), msg.count());
    __LEAVE_FUNCTION
}

ON_PLAYERMSG(CS_ITEM_STORAGE_OPEN)
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

ON_PLAYERMSG(CS_ITEM_STORAGE_CHECKIN)
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

ON_PLAYERMSG(CS_ITEM_STORAGE_CHECKOUT)
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

ON_PLAYERMSG(CS_EQUIP)
{
    __ENTER_FUNCTION
    if(pPlayer->GetCurrentScene() == nullptr)
        return;

    if(pPlayer->IsDead() == false)
        return;

    pPlayer->GetEquipmentSet()->EquipItem(msg.grid_in_package(), msg.grid_in_equip(), SYNC_TRUE);
    __LEAVE_FUNCTION
}

ON_PLAYERMSG(CS_UNEQUIP)
{
    __ENTER_FUNCTION
    if(pPlayer->GetCurrentScene() == nullptr)
        return;

    if(pPlayer->IsDead() == false)
        return;
    pPlayer->GetEquipmentSet()->UnequipItem(msg.grid_in_equip(), SYNC_TRUE, true, true);
    __LEAVE_FUNCTION
}
