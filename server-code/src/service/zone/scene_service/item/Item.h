#ifndef ITEM_H
#define ITEM_H

#include "ItemData.h"

class CActor;
class SC_ITEM_INFO;
export_lua class CItem : public CItemData
{
private:
    CItem();

public:
    CreateNewImpl(CItem);

public:
    virtual ~CItem();

public:
    //! 直接发送物品信息给pRole
    export_lua void FillItemInfo(SC_ITEM_INFO* pMsg);
    export_lua void SendItemInfo(CActor* pActor);
    //! 发送装备的叠加数据到pRole，默认参数ucAction=ITEMACT_EXPIREDATA
    export_lua void SyncExpireData(CActor* pActor);
    //! 发送装备的耐久度到pRole
    export_lua void SyncDuraData(CActor* pActor);
    export_lua void SyncPileNum(CActor* pActor);
    export_lua void SyncGridData(CActor* pActor);
    export_lua void SendDeleteMsg(CActor* pActor);

public:
    // 是否可叠加的
    export_lua bool IsPileEnable();
    // 是否过期
    export_lua bool IsExpire();
    // 是否可交易
    export_lua bool IsExchangeEnable();
    // 是否可存仓库
    export_lua bool IsStorageEnable();
    // 是否可出售
    export_lua bool IsSellEnable();
    // 是否可丢弃
    export_lua bool IsDropEnable();
    // 是否客户端不可销毁
    export_lua bool IsDelEnable();
    // 是否可升级品质
    export_lua bool IsForgingEnable();
    // 是否可修理
    export_lua bool IsRepairEnable();
    // 配方合成、拾取怪物掉落的时候是否广播传闻
    export_lua bool IsPickRumor();
    // 商城购买、帮派商店购买 的时候是否广播传闻
    export_lua bool IsBuyRumor();
    // 赌博产出 的时候是否广播传闻
    export_lua bool IsGamblingRumor();
    // 是否任务追踪物品
    export_lua bool        IsTraceItem();
    export_lua bool        IsEquipment();
    export_lua static bool IsEquipmentStatic(uint32_t idType);
    export_lua bool        IsSuit();
    export_lua bool        IsCombineEnable(OBJID idItemType, uint32_t dwFlag);
    export_lua bool        IsCombineEnable(CItem* pItem);

public:
    //! 改变物品类型——主要用于装备品质变化
    export_lua bool ChangeType(uint32_t idType, bool bUpdate = true);
    //! 改变物品的追加等级
    export_lua bool     ChangeAddition(uint32_t nLevel, bool bUpdate = true);
    export_lua uint32_t GetEquipPosition();
    export_lua bool     ChkEquipPosition(uint32_t nPosition);
    export_lua uint32_t GetCanPileNum();
};
#endif /* ITEM_H */
