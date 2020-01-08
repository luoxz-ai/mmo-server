#pragma once

#include "BaseCode.h"
#include "AchievementType.h"
#include "DBRecord.h"

class CPlayer;
export_lua class CPlayerAchievement
{
	CPlayerAchievement();

public:
	virtual ~CPlayerAchievement();
	CREATE_NEW_IMPL(CPlayerAchievement);
	bool	Init(CPlayer* pPlayer);

public:
	export_lua void SyncAll();
	export_lua bool CheckAchiCondition(uint32_t nConditionType, uint32_t nVal0, uint32_t nVal1=0, uint32_t nVal2=0);
	
	export_lua bool TakeReward(uint32_t idAchiType);
	export_lua bool TakeAll();
	void SaveAll();
private:
	bool _CheckAchiCondition(CAchievementType* pType, uint32_t nVal0, uint32_t nVal1, uint32_t nVal2);
	void FinishAchievement(CAchievementType* pType);
	void SendAchiToClient(uint32_t idAchiType, bool bTake);
	bool IsFinish(uint32_t idAchiType);
private:
	CPlayer* m_pOwner;
	std::unordered_map<uint32_t, CDBRecordPtr> m_setFinish;
	std::unordered_set<uint32_t> m_setAward;
};