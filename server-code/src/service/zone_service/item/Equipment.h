#ifndef EQUIPMENT_H
#define EQUIPMENT_H

#include <map>

#include "ActorAttrib.h"
#include "BaseCode.h"

class CItem;
class CItemType;
class CPlayer;
export_lua class CEquipment : public Noncopyable<CEquipment>
{
protected:
    CEquipment();
    bool Init(CPlayer* pPlayer);
public:
    CreateNewImpl(CEquipment);
public:
    virtual ~CEquipment();
    
    

public:
public:
    export_lua CItem* QueryEquipment(uint32_t nGrid) const;
    export_lua CItem* QueryEquipmentById(OBJID idItem) const;

    export_lua bool EquipItem(uint32_t nGridInPackage, uint32_t nGrid, bool bSync = true);
    export_lua CItem* UnequipItem(uint32_t nGrid,
                                  bool     bSync             = true,
                                  bool     bRecalcAbility    = true,
                                  bool     bRemoveItemExpire = true);

    export_lua uint32_t GetWeaponTypeID() const;
    export_lua uint32_t GetArmorTypeID() const;
    export_lua void     SaveAll();
    export_lua void     SendInfo();

    export_lua bool DecEquipmentDurability(bool bBeAttack, bool bMagic, int32_t nDecValue = 1);
    export_lua void AddEquipmentDurability(uint32_t nPosition, int32_t nAddValue);

    export_lua void DeleteAll(bool bSynchro = true, bool bTraceTaskItem = true);

    export_lua bool CanEquipByItemType(const CItemType* pItemType) const ;
    export_lua bool CanEquip(CItem* pItem, uint32_t& nGrid)const ;

    export_lua virtual int32_t GetSuitTypeAmount() const { return (int32_t)m_setSuitNum.size(); }

    void CheckItemExpire(uint32_t dwTimeNow);

protected:
    void RemoveItemExpireCallBack(uint32_t nGrid, uint32_t dwExpireData);
    void AddItemExpireCallBack(uint32_t nGrid, uint32_t dwExpireData);
    void OnItemEquiped(CItem* pItem, bool bRepair);
    void OnItemUnEquiped(CItem* pItem, bool bBroked);
    void AddSuitNum(const CItemType* pItemType);
    void DecSuitNum(const CItemType* pItemType);

protected:
    bool CheckEquipPosition(CItem* pItem, uint32_t nGrid) const;
    bool SetEquipment(uint32_t nPosition, CItem* pItem);
    void NotifyEquip(uint16_t usAction, CItem* pItem, uint32_t nGrid);

protected:
    CPlayer*                   m_pOwner = nullptr;
    std::map<uint32_t, CItem*> m_setItem; // 装备物品指针

    typedef std::map<OBJID, uint32_t> SUITNUM_SET;
    SUITNUM_SET                       m_setSuitNum;

    typedef std::multimap<uint32_t, uint8_t> EXPIRE_ITEM_SET;
    EXPIRE_ITEM_SET                          m_setExpireItem;
};
#endif /* EQUIPMENT_H */
