#include "Item.h"

#include "Actor.h"
#include "ItemAddition.h"
#include "ItemType.h"
#include "SceneService.h"
#include "msg/zone_service.pb.h"
CItem::CItem() {}

CItem::~CItem() {}

void CItem::FillItemInfo(SC_ITEM_INFO* pMsg)
{
    CHECK(pMsg);
    auto& msg = *pMsg;
    msg.set_actor_id(GetOwnerID());
    msg.set_item_id(GetID());
    msg.set_position(GetPosition());
    msg.set_grid(GetGrid());
    msg.set_pile_num(GetPileNum());
    msg.set_dura(GetDura());
    msg.set_dura_limit(GetDuraLimit());
    msg.set_expire_time(GetExpireTime());
    msg.set_addition_lev(GetAddition());
    msg.set_flag(GetFlag());
}

void CItem::SendItemInfo(CActor* pActor)
{
    __ENTER_FUNCTION
    CHECK(pActor);
    SC_ITEM_INFO msg;
    FillItemInfo(&msg);
    pActor->SendMsg(msg);
    __LEAVE_FUNCTION
}

void CItem::SyncExpireData(CActor* pActor)
{
    __ENTER_FUNCTION
    CHECK(pActor);
    SC_ITEM_CHANGE msg;
    msg.set_item_id(GetID());
    msg.set_position(GetPosition());
    auto pData = msg.add_change_list();
    pData->set_change_type(SC_ITEM_CHANGE::ChangeExpireData);
    pData->set_value(GetExpireTime());
    pActor->SendMsg(msg);
    __LEAVE_FUNCTION
}

void CItem::SyncDuraData(CActor* pActor)
{
    __ENTER_FUNCTION
    CHECK(pActor);
    SC_ITEM_CHANGE msg;
    msg.set_item_id(GetID());
    msg.set_position(GetPosition());
    auto pData = msg.add_change_list();
    pData->set_change_type(SC_ITEM_CHANGE::ChangeDura);
    pData->set_value(GetDura());
    pActor->SendMsg(msg);
    __LEAVE_FUNCTION
}

void CItem::SyncPileNum(CActor* pActor)
{
    __ENTER_FUNCTION
    CHECK(pActor);
    SC_ITEM_CHANGE msg;
    msg.set_item_id(GetID());
    msg.set_position(GetPosition());
    auto pData = msg.add_change_list();
    pData->set_change_type(SC_ITEM_CHANGE::ChangePileNum);
    pData->set_value(GetPileNum());
    pActor->SendMsg(msg);
    __LEAVE_FUNCTION
}

void CItem::SyncGridData(CActor* pActor)
{
    __ENTER_FUNCTION
    CHECK(pActor);
    SC_ITEM_CHANGE msg;
    msg.set_item_id(GetID());
    msg.set_position(GetPosition());
    auto pData = msg.add_change_list();
    pData->set_change_type(SC_ITEM_CHANGE::ChangeGrid);
    pData->set_value(GetGrid());
    pActor->SendMsg(msg);
    __LEAVE_FUNCTION
}

void CItem::SendDeleteMsg(CActor* pActor)
{
    __ENTER_FUNCTION
    CHECK(pActor);
    SC_ITEM_DELETE msg;
    msg.set_item_id(GetID());
    msg.set_position(GetPosition());
    msg.set_grid(GetGrid());
    pActor->SendMsg(msg);
    __LEAVE_FUNCTION
}
// 是否可叠加的
bool CItem::IsPileEnable()
{
    return ItemTypePtr()->IsPileEnable() && GetPileNum() < ItemTypePtr()->GetPileLimit();
}
// 是否可交易
bool CItem::IsExchangeEnable()
{
    return HasFlag(ITEMFLAG_EXCHANGE_DISABLE) == false;
}
// 是否可存仓库
bool CItem::IsStorageEnable()
{
    return HasFlag(ITEMFLAG_STORAGE_DISABLE) == false;
}
// 是否可出售
bool CItem::IsSellEnable()
{
    return HasFlag(ITEMFLAG_SELL_DISABLE) == false;
}
// 是否可丢弃
bool CItem::IsDropEnable()
{
    return HasFlag(ITEMFLAG_DROP_DISABLE) == false;
}
// 是否客户端不可销毁
bool CItem::IsDelEnable()
{
    return HasFlag(ITEMFLAG_DEL_DISABLE) == false;
}
// 是否可升级品质
bool CItem::IsForgingEnable()
{
    return HasFlag(ITEMFLAG_FORGING_DISABLE) == false;
}
// 是否可修理
bool CItem::IsRepairEnable()
{
    return (IsEquipment() && (HasFlag(ITEMFLAG_REPAIR_DISABLE)) == false);
}
// 配方合成、拾取怪物掉落的时候是否广播传闻
bool CItem::IsPickRumor()
{
    return HasFlag(ITEMFLAG_PICK_RUMOR);
}
// 商城购买、帮派商店购买 的时候是否广播传闻
bool CItem::IsBuyRumor()
{
    return HasFlag(ITEMFLAG_BUY_RUMOR);
}
// 赌博产出 的时候是否广播传闻
bool CItem::IsGamblingRumor()
{
    return HasFlag(ITEMFLAG_GAMBLING_RUMOR);
}
// 是否任务追踪物品
bool CItem::IsTraceItem()
{
    return HasFlag(ITEMFLAG_TRACE_ITEM);
}
bool CItem::IsEquipment()
{
    return IsEquipmentStatic(GetType());
}

bool CItem::IsExpire()
{
    __ENTER_FUNCTION
    return GetExpireTime() != 0 && GetExpireTime() > TimeGetSecond();
    __LEAVE_FUNCTION
    return true;
}

bool CItem::IsEquipmentStatic(uint32_t idType)
{
    return CItemType::IsEquipment(idType);
}

bool CItem::IsSuit()
{
    __ENTER_FUNCTION
    return ItemTypePtr()->IsSuit();
    __LEAVE_FUNCTION
    return false;
}

bool CItem::IsCombineEnable(OBJID idItemType, uint32_t dwFlag)
{
    __ENTER_FUNCTION
    if(GetType() != idItemType)
        return false;
    if(!IsPileEnable())
        return false;

    if(_GetFlag() == dwFlag)
        return false;

    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CItem::IsCombineEnable(CItem* pItem)
{
    __ENTER_FUNCTION
    CHECKF(pItem);
    if(!IsCombineEnable(pItem->GetType(), pItem->_GetFlag()))
        return false;

    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CItem::ChangeType(uint32_t idType, bool bUpdate /*=true*/)
{
    __ENTER_FUNCTION
    // change type
    const CItemType* pType = ItemTypeSet()->QueryObj(idType);
    if(!pType)
    {
        LOGERROR("Change item type [{}] error!", idType);
        return false;
    }

    m_pType = pType;
    SetTypeID(idType, UPDATE_FALSE);
    SetDuraLimit(m_pType->GetDuraLimit(), UPDATE_FALSE);

    if(bUpdate)
        SaveInfo();

    return true;
    __LEAVE_FUNCTION
    return false;
}

bool CItem::ChangeAddition(uint32_t nLevel, bool bUpdate /*=true*/)
{
    __ENTER_FUNCTION
    const CItemAdditionData* pAddition = NULL;
    if(nLevel > 0)
    {
        pAddition = ItemAdditionSet()->QueryItemAddition(GetType(), nLevel);
    }
    m_pAddition = pAddition;
    SetAddition(nLevel, bUpdate);

    return true;
    __LEAVE_FUNCTION
    return false;
}

uint32_t CItem::GetEquipPosition()
{
    __ENTER_FUNCTION
    if(!IsEquipment())
        return EQUIPPOSITION_NONE;

    switch(GetSubType())
    {
        case ITEM_SUBTYPE_EQUIP_WEAPON: // 武器
            return EQUIPPOSITION_WEAPON;
            break;
        case ITEM_SUBTYPE_EQUIP_FASHION_WEAPON:
            return EQUIPPOSITION_FASHION_WEAPON;
        case ITEM_SUBTYPE_EQUIP_HELMET: // 帽子
            return EQUIPPOSITION_HELMET;
        case ITEM_SUBTYPE_EQUIP_ARMOR: // 衣服
            return EQUIPPOSITION_ARMOR;
        case ITEM_SUBTYPE_EQUIP_GIRDLE: // 腰带
            return EQUIPPOSITION_GIRDLE;
        case ITEM_SUBTYPE_EQUIP_SHOES: // 鞋子
            return EQUIPPOSITION_SHOES;
        case ITEM_SUBTYPE_EQUIP_HAND: // 手套
            return EQUIPPOSITION_HAND_L;
        case ITEM_SUBTYPE_EQUIP_FASHION_DRESS:
            return EQUIPPOSITION_FASHION_DRESS;
            break;
        case ITEM_SUBTYPE_EQUIP_NECKLACE: // 项链
            return EQUIPPOSITION_NECKLACE;
        case ITEM_SUBTYPE_EQUIP_RING: // 戒指
            return EQUIPPOSITION_RING_L;
        case ITEM_SUBTYPE_EQUIP_WING: // 翅膀
            return EQUIPPOSITION_WING;
        case ITEM_SUBTYPE_EQUIP_HORSE: // 灵石
            return EQUIPPOSITION_RUNE;
        default:
            return EQUIPPOSITION_NONE;
    }
    __LEAVE_FUNCTION
    return EQUIPPOSITION_NONE;
}

bool CItem::ChkEquipPosition(uint32_t nPosition)
{
    __ENTER_FUNCTION
    if(!IsEquipment())
        return false;

    switch(GetSubType())
    {
        case ITEM_SUBTYPE_EQUIP_WEAPON: // 武器
            return EQUIPPOSITION_WEAPON == nPosition;
            break;
        case ITEM_SUBTYPE_EQUIP_FASHION_WEAPON:
            return EQUIPPOSITION_FASHION_WEAPON == nPosition;
        case ITEM_SUBTYPE_EQUIP_HELMET: // 帽子
            return EQUIPPOSITION_HELMET == nPosition;
        case ITEM_SUBTYPE_EQUIP_ARMOR: // 衣服
            return EQUIPPOSITION_ARMOR == nPosition;
        case ITEM_SUBTYPE_EQUIP_GIRDLE: // 腰带
            return EQUIPPOSITION_GIRDLE == nPosition;
        case ITEM_SUBTYPE_EQUIP_SHOES: // 鞋子
            return EQUIPPOSITION_SHOES == nPosition;
        case ITEM_SUBTYPE_EQUIP_HAND: // 手套
            return EQUIPPOSITION_HAND_L == nPosition || EQUIPPOSITION_HAND_R == nPosition;
        case ITEM_SUBTYPE_EQUIP_FASHION_DRESS:
            return EQUIPPOSITION_FASHION_DRESS == nPosition;
            break;
        case ITEM_SUBTYPE_EQUIP_NECKLACE: // 项链
            return EQUIPPOSITION_NECKLACE == nPosition;
        case ITEM_SUBTYPE_EQUIP_RING: // 戒指
            return EQUIPPOSITION_RING_L == nPosition || EQUIPPOSITION_RING_R == nPosition;
        case ITEM_SUBTYPE_EQUIP_WING: // 翅膀
            return EQUIPPOSITION_WING == nPosition;
        case ITEM_SUBTYPE_EQUIP_HORSE: // 灵石
            return EQUIPPOSITION_RUNE == nPosition;
        default:
            return false;
    }
    __LEAVE_FUNCTION
    return false;
}

uint32_t CItem::GetCanPileNum()
{
    __ENTER_FUNCTION
    return ItemTypePtr()->GetPileLimit() - GetPileNum();
    __LEAVE_FUNCTION
    return 0;
}
