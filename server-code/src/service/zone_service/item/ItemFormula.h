#ifndef ITEMFORMULA_H
#define ITEMFORMULA_H

#include "BaseCode.h"
#include "Cfg_ItemFormula.pb.h"
#include "T_GameDataMap.h"

//////////////////////////////////////////////////////////////////////
class CItemFormulaData
{
	CItemFormulaData() {}

public:
	virtual ~CItemFormulaData() {}
	using PB_T = Cfg_ItemFormula;
	CREATE_NEW_IMPL(CItemFormulaData);
	bool Init(const Cfg_ItemFormula_Row& row)
	{
		m_Data = row;
		return true;
	}
	static uint32_t getIDFromPBRow(const Cfg_ItemFormula_Row& row) { return row.id(); }

public:
	uint32_t	   GetID() const { return m_Data.id(); }
	uint32_t	   GetSort() const { return m_Data.sort(); }
	uint32_t	   GetTargetType() const { return m_Data.target_itemtype(); }
	uint32_t	   GetTargetFlag() const { return m_Data.target_flag(); }
	uint32_t	   GetTargetNum() const { return m_Data.target_itemnum(); }
	uint32_t	   GetLevReq() const { return m_Data.lev_req(); }
	uint32_t	   GetSuccRate() const { return m_Data.succ_rate(); }
	uint32_t	   GetRumor() const { return m_Data.rumor(); }
	decltype(auto) GetReqData() const { return m_Data.req_data_list(); } // 需要材料itemtypeid

public:
	Cfg_ItemFormula_Row m_Data;
};

typedef CGameDataMap<CItemFormulaData> CItemFormulaDataSet;

#endif /* ITEMFORMULA_H */
