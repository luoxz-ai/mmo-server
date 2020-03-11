#ifndef PACKAGE_H
#define PACKAGE_H

#include "BaseCode.h"

//////////////////////////////////////////////////////////////////////
const int32_t _MAX_ITEMPACKAGE_SIZE = 30;  // 普通物品背包最大容量
const int32_t _MAX_SYSSTORAGE_SIZE	= 300; // 系统物品仓库最大容量

//////////////////////////////////////////////////////////////////////
class CItem;
class CPlayer;
export_lua class CPackage
{
protected:
	CPackage();

public:
	virtual ~CPackage();
	CREATE_NEW_IMPL(CPackage);

public:
	virtual bool Init(CPlayer* pOwner, uint32_t nPackageType, uint32_t nMaxSize);

public:
	export_lua virtual uint32_t GetPackageType() { return m_nPosition; }
	export_lua virtual uint32_t GetMaxSize() { return m_nMaxSize; }
	export_lua virtual bool		ChangeMaxSize(uint32_t nMaxSize)
	{
		m_nMaxSize = nMaxSize;
		return true;
	}
	export_lua virtual uint32_t GetAmount() { return m_setItem.size(); }

	export_lua virtual bool		IsFull(uint32_t nAmount = 1) { return (m_nMaxSize > 0 && m_setItem.size() + nAmount > m_nMaxSize); }
	export_lua virtual bool		IsFull(uint32_t idType, uint32_t nAmount, uint32_t dwFlag = 0);
	export_lua virtual bool		IsFull(CItem* pItem);
	export_lua virtual uint32_t GetSpareSpace(uint32_t idType = ID_NONE, uint32_t nAmount = 0, uint32_t nFillSpace = 0, uint32_t dwFlag = 0);

	export_lua virtual bool AwardItem(uint32_t idItemType, uint32_t nAmount, uint32_t dwFlag = 0);

	export_lua virtual CItem*	AddItem(CItem* pItem, bool bSync = false, bool bCheckFull = true, bool bTrackTaskItem = true);
	export_lua virtual bool		CombineAddItem(CItem* pItem, bool bSync = true);
	export_lua virtual uint32_t CombineAddItem(uint32_t idItemType, uint32_t nNum, uint32_t dwFlag, bool bSync /*=true*/);
	export_lua virtual bool		DelItem(OBJID idItem, bool bSync = false, bool bTraceTaskItem = true);
	export_lua virtual bool		DelItemByGrid(uint32_t nGrid, bool bSync = false, bool bTraceTaskItem = true);
	export_lua virtual CItem*	PopItem(OBJID idItem, bool bSync = false);
	export_lua virtual CItem*	PopItemByGrid(uint32_t nGrid, bool bSync /*=false*/);
	export_lua virtual bool		DelAll(bool bSync = false, bool bTraceTaskItem = true);

	export_lua virtual bool SwapItem(uint32_t nGrid1, uint32_t nGrid2);
	export_lua virtual bool SplitItem(uint32_t nGrid1, uint32_t nGrid2, uint32_t nSplitNum);
	export_lua virtual bool CombineItem(uint32_t nGrid1, uint32_t nGrid2);
	export_lua virtual bool TidyItem();

	export_lua virtual CItem* QueryItem(OBJID idItem);
	export_lua virtual CItem* QueryItemByType(uint32_t idType, uint32_t dwFlag = 0);
	export_lua virtual CItem* QueryItemByGrid(uint32_t nGrid);

	export_lua virtual CItem* FindCombineItem(uint32_t idType, uint32_t dwFlag, uint32_t nAmount);

	export_lua virtual void SaveAll();
	export_lua virtual void SendAllItemInfo();

public:
	export_lua CItem* ForEach(std::function<bool(CItem*)> func);
	void			  CheckItemExpire(uint32_t dwTimeNow);

public:
	export_lua virtual bool		HaveSoManyItem(uint32_t idType, uint32_t nNum, uint32_t dwFlag = 0);
	export_lua virtual uint32_t DelItemByType(uint32_t idType, uint32_t nNum, uint32_t dwFlag = 0, bool bTraceTaskItem = true);
	export_lua virtual uint32_t GetItemTypeAmount(uint32_t idType, uint32_t dwFlag = 0);

protected:
	void RemoveItemExpireCallBack(OBJID idItem, uint32_t dwExpireData);
	void AddItemExpireCallBack(OBJID idItem, uint32_t dwExpireData);

protected:
	CPlayer* m_pOwner	 = nullptr;
	uint32_t m_nMaxSize	 = 0;
	uint32_t m_nPosition = 0;

	typedef std::map<uint32_t, CItem*> ITEM_SET;
	ITEM_SET						   m_setItem;

	typedef std::multimap<uint32_t, OBJID> EXPIRE_ITEM_SET;
	EXPIRE_ITEM_SET						   m_setExpireItem;

	MEMORYHEAP_DECLARATION(s_heap)
	uint32_t FindFirstEmptyGrid();
};

#endif /* PACKAGE_H */
