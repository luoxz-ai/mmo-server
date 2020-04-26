#ifndef ITEMDATA_H
#define ITEMDATA_H

#include "DBRecord.h"
#include "ItemType.h"
#include "gamedb.h"

//////////////////////////////////////////////////////////////////////
export_lua struct ST_ITEMINFO
{
    export_lua OBJID    idItem    = 0; // id
    export_lua OBJID    idOwner   = 0; // 拥有者ID
    export_lua uint32_t idType    = 0; // itemtype
    export_lua uint32_t nPosition = 0; //
    export_lua uint32_t nGrid     = 0; //
    export_lua uint32_t nNum      = 0; //
    export_lua uint32_t nAddition = 0; // 追加等级
    export_lua uint32_t dwFlag    = 0; // 掩码
};

class CItemAdditionData;
export_lua class CItemData : public NoncopyableT<CItemData>
{
protected:
    CItemData();
    bool Init(CDBRecordPtr&& pRes);
    bool Init(CMysqlConnection* pDB, OBJID idItem);
    bool Init(CMysqlConnection* pDB, ST_ITEMINFO& info);
    bool Init(CMysqlConnection* pDB,
              uint64_t          idPlayer,
              uint32_t          idItemType,
              uint32_t          nNum,
              uint32_t          dwFlag,
              uint32_t          nPosition);
public:
    CreateNewImpl(CItemData);
public:
    virtual ~CItemData();
    
    

public:
    export_lua bool DelRecord();
    static bool     DelRecordStatic(CMysqlConnection* pDB, OBJID idItem);

public: // get attr
    export_lua OBJID    GetID() const { return m_pRecord->Field(TBLD_ITEM::ID); }
    export_lua uint32_t GetType() const { return m_pRecord->Field(TBLD_ITEM::ITEMTYPE); }
    export_lua OBJID    GetOwnerID() const { return m_pRecord->Field(TBLD_ITEM::OWNER_ID); }
    export_lua uint32_t GetPosition() const { return m_pRecord->Field(TBLD_ITEM::POSITION); }
    export_lua uint32_t GetGrid() const { return m_pRecord->Field(TBLD_ITEM::GRID); }
    export_lua uint32_t GetPileNum() const { return m_pRecord->Field(TBLD_ITEM::PILENUM); }
    export_lua uint32_t GetDura() const { return m_pRecord->Field(TBLD_ITEM::DURA); }
    export_lua uint32_t GetDuraLimit() const { return m_pRecord->Field(TBLD_ITEM::DURA_LIMIT); }
    export_lua uint32_t GetExpireTime() const { return m_pRecord->Field(TBLD_ITEM::EXPIRE_TIME); }
    export_lua uint32_t GetAddition() const { return m_pRecord->Field(TBLD_ITEM::ADDITION_LEV); }
    export_lua uint32_t _GetFlag() const { return m_pRecord->Field(TBLD_ITEM::FLAG); }
    export_lua uint32_t GetFlag() const { return _GetFlag() | ItemTypePtr()->GetFlag(); }
    export_lua bool     HasFlag(uint32_t dwFlag) const { return ::HasFlag(GetFlag(), dwFlag); }
    export_lua uint32_t GetExtra(uint32_t nIdx) const;

    export_lua const std::string& GetName() const { return m_pType->GetName(); }
    export_lua const CItemType* ItemTypePtr() const { return m_pType; }
    export_lua const CItemAdditionData* AdditionType() { return m_pAddition; }
    export_lua uint32_t                 GetMainType() const { return GetMainTypeByID(GetType()); }
    export_lua static uint32_t          GetMainTypeByID(uint32_t idType) { return idType / ITEM_MAINTYPE_MASK; }
    export_lua uint32_t                 GetSubType() const { return GetSubTypeByID(GetType()); }
    export_lua static uint32_t          GetSubTypeByID(uint32_t idType)
    {
        return (idType % ITEM_MAINTYPE_MASK) / ITEM_SUBTYPE_MASK;
    }

public: // set attr
    export_lua void SetOwnerID(OBJID idOwner, bool bUpdate = true);
    export_lua void SetTypeID(OBJID idType, bool bUpdate = true);
    export_lua void SetPosition(uint8_t ucPosition, bool bUpdate = true);
    export_lua void SetGrid(uint32_t nData, bool bUpdate /*=true*/);
    export_lua void SetPileNum(uint32_t nNum, bool bUpdate = false);
    export_lua void AddPileNum(uint32_t nNum, bool bUpdate = false);
    export_lua void DecPileNum(uint32_t nNum, bool bUpdate = false);
    export_lua void SetDura(uint32_t usDura, bool bUpdate = true);
    export_lua void SetDuraLimit(uint32_t usDuraLimit, bool bUpdate = true);
    export_lua void SetExpireTime(uint32_t nExpireTime, bool bUpdate = false);
    export_lua void SetAddition(uint32_t nAddition, bool bUpdate = false);
    export_lua void SetFlag(uint32_t dwFlag, bool bUpdate = false);

    export_lua void SetExtra(int32_t nIdx, uint32_t nExtra, bool bUpdate = false);

    export_lua void SaveInfo() { m_pRecord->Update(); }

public:
    OBJECTHEAP_DECLARATION(s_heap);

protected:
    CDBRecordPtr             m_pRecord; // 物品数据
    ItemExtraData            m_ExtraData;
    const CItemType*               m_pType     = nullptr; // 物品类型数据
    const CItemAdditionData* m_pAddition = nullptr; // 强化数据
};
#endif /* ITEMDATA_H */
