#pragma once


#include "BaseCode.h"
#include "config/Cfg_Achievement.pb.h"
#include "T_GameDataMap.h"


class CAchievementType
{
	CAchievementType() {}
public:
	~CAchievementType() {}
	CREATE_NEW_IMPL(CAchievementType);
	bool Init(const Cfg_Achievement_Row& row)
	{
		m_Data = row;
		return true;
	}


	using PB_T = Cfg_Achievement;
	static uint32_t getIDFromPBRow(const Cfg_Achievement_Row& row)
	{
		return row.id();
	}

	uint32_t GetID() const { return m_Data.id(); }
	const std::string& GetName() const {return m_Data.name(); }
	uint32_t GetLevReq() const {return m_Data.lev_req(); }
	uint32_t GetProfReq() const {return m_Data.prof_req(); }
	uint32_t GetGroupID() const {return m_Data.group_id(); }
	
	decltype(auto) GetCheckData() const {return m_Data.check_data(); }


	uint32_t GetAwardExp() const {return m_Data.award_exp(); }
	uint32_t GetAwardAchiPoint() const {return m_Data.award_achipoint(); }
	decltype(auto) GetAwardItemList() const {return m_Data.award_list(); }
	

public:
	const Cfg_Achievement_Row& GetDataRef() const {	return m_Data; }
private:
	Cfg_Achievement_Row m_Data;
};


//////////////////////////////////////////////////////////////////////
class CAchievementTypeSet
{
	CAchievementTypeSet();
public:
	virtual ~CAchievementTypeSet();

public:
	CREATE_NEW_IMPL(CAchievementTypeSet);
	bool	Init(const char* szFileName);
	bool	Reload(const char* szFileName);
	void	Destroy();
	// 根据itemtype, 追加等级，查询对应的追加数据
	CAchievementType* GetData(uint32_t idType) const;
	const std::map<uint32_t, CAchievementType*>& GetData() const;
	const std::vector<CAchievementType*>* QueryAchiemenetByGroupID(uint32_t idGroup)const;
	const std::vector<CAchievementType*>* QueryAchiemenetByCheckType(uint32_t idType)const;

public:
	typedef std::unordered_map<uint32_t, std::vector<CAchievementType*> > DATA_MAP_BY_CHECKTYPE;
	typedef std::unordered_map<uint32_t, std::vector<CAchievementType*> > DATA_MAP_BY_GROUPID;


protected:
	std::map<uint32_t, CAchievementType*> m_setData;
	DATA_MAP_BY_GROUPID			m_setDataByGroupID;
	DATA_MAP_BY_CHECKTYPE		m_setDataByCheckType;
};