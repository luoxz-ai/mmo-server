#include "ItemData.h"
#include "ZoneService.h"
#include "Player.h"
MEMORYHEAP_IMPLEMENTATION(CItemData,s_heap);

CItemData::CItemData()
{

}

CItemData::~CItemData()
{

}

bool CItemData::Init(CDBRecordPtr&& pRes)
{
__ENTER_FUNCTION
	CHECKF(pRes != nullptr);
	m_pRecord.reset(pRes.release());
	m_pType	= ItemTypeSet()->QueryObj(GetType());
	if (!m_pType)
	{
		LOGERROR("UNKNOW itemtype:{}", GetType());
		return false;
	}

	uint32_t nAddition = GetAddition();
	if (nAddition > 0)
	{
		m_pAddition = ItemAdditionSet()->QueryItemAddition(GetType(),nAddition);
	}
	return true;
__LEAVE_FUNCTION
	return false;
}

bool CItemData::Init(CMysqlConnection* pDB, OBJID idItem)
{
__ENTER_FUNCTION
	CHECKF(pDB);
	CHECKF(idItem);
	
	
	auto pResult = pDB->Query(fmt::format("SELECT * FROM {} WHERE id={} LIMIT 1", TBLD_ITEM::table_name, idItem));
	if(pResult == nullptr)
		return false;

	m_pRecord = pResult->fetch_row();
	CHECKF(m_pRecord != nullptr);

	m_pType	= ItemTypeSet()->QueryObj(GetType());
	if (!m_pType)
	{
		LOGERROR("UNKNOW itemtype:{}", GetType());
		return false;
	}

	uint32_t nAddition = GetAddition();
	if (nAddition > 0)
	{
		m_pAddition = ItemAdditionSet()->QueryItemAddition(GetType(),nAddition);
	}
	return true;
__LEAVE_FUNCTION
	return false;
}

bool CItemData::Init(CMysqlConnection* pDB, ST_ITEMINFO& info)
{
__ENTER_FUNCTION
	m_pType	= ItemTypeSet()->QueryObj(info.idType);
	if (!m_pType)
	{
		LOGERROR("UNKNOW itemtype:{}", info.idType);
		return false;
	}

	m_pRecord = pDB->MakeRecord(TBLD_ITEM::table_name);
	if (!m_pRecord)
		return false;
	m_pRecord->Field(TBLD_ITEM::ID)				= info.idItem;
	m_pRecord->Field(TBLD_ITEM::ITEMTYPE)		= info.idType;
	m_pRecord->Field(TBLD_ITEM::OWNER_ID)		= info.idOwner;
	m_pRecord->Field(TBLD_ITEM::POSITION)		= info.nPosition;
	m_pRecord->Field(TBLD_ITEM::GRID)			= info.nGrid;
	m_pRecord->Field(TBLD_ITEM::ADDITION_LEV)	= info.nAddition;
	m_pRecord->Field(TBLD_ITEM::PILENUM)		= info.nNum;
	if(m_pType->GetExpireTime() > 0)
		m_pRecord->Field(TBLD_ITEM::EXPIRE_TIME)= uint32_t(TimeGetSecond())+m_pType->GetExpireTime();
	else
		m_pRecord->Field(TBLD_ITEM::EXPIRE_TIME)= 0;
	m_pRecord->Field(TBLD_ITEM::DURA)			= m_pType->GetDura();
	m_pRecord->Field(TBLD_ITEM::DURA_LIMIT)		= m_pType->GetDuraLimit();
	m_pRecord->Field(TBLD_ITEM::FLAG)			= info.dwFlag;
	

	if (m_pRecord->Update() == false)
		return false;
	uint32_t nAddition = GetAddition();
	if (nAddition > 0)
	{
		m_pAddition = ItemAdditionSet()->QueryItemAddition(GetType(),nAddition);
	}

	return true;
__LEAVE_FUNCTION
	return false;
}

bool CItemData::Init(CMysqlConnection* pDB,uint64_t idPlayer, uint32_t idItemType, uint32_t nNum, uint32_t dwFlag , uint32_t nPosition)
{
__ENTER_FUNCTION
	m_pType	= ItemTypeSet()->QueryObj(idItemType);
	if (!m_pType)
	{
		LOGERROR("UNKNOW itemtype:{}", idItemType);
		return false;
	}

	m_pRecord = pDB->MakeRecord(TBLD_ITEM::table_name);
	if (!m_pRecord)
		return false;
	m_pRecord->Field(TBLD_ITEM::ITEMTYPE)		= idItemType;
	m_pRecord->Field(TBLD_ITEM::OWNER_ID)		= idPlayer;
	m_pRecord->Field(TBLD_ITEM::PILENUM)		= nNum;
	m_pRecord->Field(TBLD_ITEM::POSITION)		= nPosition;
	m_pRecord->Field(TBLD_ITEM::FLAG)			= dwFlag;

	return true;
__LEAVE_FUNCTION
	return false;
}

bool CItemData::DelRecord()
{
__ENTER_FUNCTION
	m_pRecord->DeleteRecord();
	m_pRecord.reset();
	m_pType = nullptr;
	m_pAddition = nullptr;
__LEAVE_FUNCTION
	return false;
}

bool CItemData::DelRecordStatic(CMysqlConnection* pDB,OBJID idItem)
{
__ENTER_FUNCTION
	CHECKF(pDB);
	pDB->SyncExec(fmt::format("DELETE FROM {} WHERE id={}", TBLD_ITEM::table_name, idItem));
__LEAVE_FUNCTION
	return false;
}

void CItemData::SetOwnerID(OBJID idOwner, bool bUpdate/*=true*/)
{
__ENTER_FUNCTION
	m_pRecord->Field(TBLD_ITEM::OWNER_ID) = idOwner;
	if(bUpdate)
		m_pRecord->Update();
__LEAVE_FUNCTION
}

void CItemData::SetTypeID(OBJID idType, bool bUpdate/*=true*/)
{
__ENTER_FUNCTION
	m_pRecord->Field(TBLD_ITEM::ITEMTYPE) = idType;
	if(bUpdate)
		m_pRecord->Update();
__LEAVE_FUNCTION
}

void CItemData::SetPosition(UCHAR ucPosition, bool bUpdate/*=true*/)
{
__ENTER_FUNCTION
	m_pRecord->Field(TBLD_ITEM::POSITION) = ucPosition;
	if(bUpdate)
		m_pRecord->Update();
__LEAVE_FUNCTION
}

void CItemData::SetGrid(uint32_t nData, bool bUpdate/*=true*/)
{
__ENTER_FUNCTION
	m_pRecord->Field(TBLD_ITEM::GRID) = nData;
	if(bUpdate)
		m_pRecord->Update();
__LEAVE_FUNCTION
}

void CItemData::SetPileNum(uint32_t nNum, bool bUpdate/*=false*/)
{
__ENTER_FUNCTION
	m_pRecord->Field(TBLD_ITEM::PILENUM) = nNum;
	if(bUpdate)
		m_pRecord->Update();
__LEAVE_FUNCTION
}

void CItemData::AddPileNum(uint32_t nNum, bool bUpdate /*= false*/)
{
__ENTER_FUNCTION
	uint32_t nFNum = GetPileNum() + nNum;
	if(nFNum > ItemTypePtr()->GetPileLimit())
		SetPileNum(ItemTypePtr()->GetPileLimit(), bUpdate);
	else
		SetPileNum(nFNum, bUpdate);
__LEAVE_FUNCTION
}

void CItemData::DecPileNum(uint32_t nNum, bool bUpdate /*= false*/)
{
__ENTER_FUNCTION
	if(GetPileNum() > nNum)
		SetPileNum(GetPileNum() - nNum, bUpdate);
	else
		SetPileNum(0, bUpdate);
__LEAVE_FUNCTION
}

void CItemData::SetExpireTime(uint32_t nExpireTime, bool bUpdate/*=false*/)
{
__ENTER_FUNCTION
	m_pRecord->Field(TBLD_ITEM::EXPIRE_TIME) = nExpireTime;
	if(bUpdate)
		m_pRecord->Update();
__LEAVE_FUNCTION
}

void CItemData::SetAddition(uint32_t nAddition, bool bUpdate/*=false*/)
{
__ENTER_FUNCTION
	m_pRecord->Field(TBLD_ITEM::ADDITION_LEV) = nAddition;
	if(bUpdate)
		m_pRecord->Update();
__LEAVE_FUNCTION
}

void CItemData::SetDura(uint32_t nDura, bool bUpdate/*=false*/)
{
__ENTER_FUNCTION
	m_pRecord->Field(TBLD_ITEM::DURA) = nDura;
	if(bUpdate)
		m_pRecord->Update();
__LEAVE_FUNCTION
}

void CItemData::SetDuraLimit(uint32_t nDuraLimit, bool bUpdate/*=false*/)
{
__ENTER_FUNCTION
	m_pRecord->Field(TBLD_ITEM::DURA_LIMIT) = nDuraLimit;
	if(bUpdate)
		m_pRecord->Update();
__LEAVE_FUNCTION
}

void CItemData::SetFlag(uint32_t dwFlag, bool bUpdate/*=false*/)
{
__ENTER_FUNCTION
	m_pRecord->Field(TBLD_ITEM::FLAG) = dwFlag;
	if(bUpdate)
		m_pRecord->Update();
__LEAVE_FUNCTION
}

void CItemData::SetExtra(int nIdx, uint32_t nExtra, bool bUpdate/*=false*/)
{
__ENTER_FUNCTION
	m_pRecord->Field(TBLD_ITEM::EXTRA0+nIdx) = nExtra;
	if(bUpdate)
		m_pRecord->Update();
__LEAVE_FUNCTION
}

