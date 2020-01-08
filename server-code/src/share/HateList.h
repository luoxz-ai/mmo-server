#pragma once

#include "BaseCode.h"
#include <vector>
#include <unordered_map>
#include <functional>

struct ST_HATE_DATA
{
	OBJID idTarget;
	float fHate;
	time_t tNextInvaildTime;
};


class CHateList
{
public:
	CHateList(){}
	~CHateList(){}
	
	void ClearHateList()
	{
		for(auto pData : m_HateListOrderByHate)
		{
			SAFE_DELETE(pData);
		}
		m_HateListOrderByHate.clear();
		m_HateList.clear();
	}

	float AddHate(OBJID idTarget, int32_t nHate)
	{
		auto& pHateData = m_HateList[idTarget];
		if(pHateData == nullptr)
		{
			pHateData = new ST_HATE_DATA{idTarget};
			m_HateListOrderByHate.push_back(pHateData);
		}

		time_t now = TimeGetSecond();
		if(pHateData->tNextInvaildTime < now)
			pHateData->fHate = 0.0f;
		pHateData->fHate += nHate;
		pHateData->tNextInvaildTime = now + 5 * 60;

		return pHateData->fHate;
	}

	ST_HATE_DATA* GetHate(OBJID idTarget)
	{	
		return m_HateList[idTarget];
	}

	void FindIF(std::function<bool(ST_HATE_DATA*)> func)
	{
		//将失效数据分离
		auto it_end = std::stable_partition(m_HateListOrderByHate.begin(), m_HateListOrderByHate.end(), [timeNow = TimeGetSecond()](const ST_HATE_DATA* left)->bool
		{
			return left->tNextInvaildTime < timeNow;
		});

		//排序Hate列表
		std::stable_sort(m_HateListOrderByHate.begin(), it_end, [](const ST_HATE_DATA* left, const ST_HATE_DATA* right)->bool
		{
			return left->fHate < right->fHate;
		});

		//找到第一个在范围内的敌人
		for(auto it = m_HateListOrderByHate.begin(); it != it_end; it++)
		{
			auto pHateData = *it;
			if(func(pHateData) == false)
				continue;
			else
				return;
		}
	}


public:
	std::unordered_map<OBJID, ST_HATE_DATA*> m_HateList;
	std::vector<ST_HATE_DATA*> m_HateListOrderByHate;

};