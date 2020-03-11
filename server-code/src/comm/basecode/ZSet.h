#ifndef ZSET_H
#define ZSET_H

#include <map>
#include <vector>

#include "SkipList.h"

class CZset
{
private:
	CSkipList					 m_SkipList;
	std::map<uint64_t, uint64_t> m_MemScoreMap;

public:
	CZset() {}
	~CZset() {}

	void zAdd(uint64_t member, uint32_t score)
	{
		auto it = m_MemScoreMap.find(member);
		if(it != m_MemScoreMap.end())
		{
			m_SkipList.Delete(it->second, member);
			m_MemScoreMap.erase(it);
		}
		auto _data = MAKE64(score, _TimeGetMonotonic());
		m_SkipList.Insert(_data, member);
		m_MemScoreMap[member] = _data;
	}

	int64_t zScore(uint64_t member) const
	{
		auto it = m_MemScoreMap.find(member);
		if(it != m_MemScoreMap.end())
		{
			return GetHighFromU64(it->second);
		}
		else
		{
			return 0;
		}
	}

	uint32_t zCount() const { return m_SkipList.m_length; }

	uint32_t zRank(uint64_t member) const
	{
		auto it = m_MemScoreMap.find(member);
		if(it != m_MemScoreMap.end())
		{
			return m_SkipList.GetRank(it->second, member);
		}
		else
		{
			return 0;
		}
	}

	uint32_t zRevRank(uint64_t member) const
	{
		uint32_t r = zRank(member);
		if(r != 0)
			return _reverse_rank(r);
		return r;
	}

	bool zsetIsInRange(double s1, double s2) const { return m_SkipList.IsInRange(MAKE64(s1, 0), MAKE64(s2, 0)); }

	void zRange(uint32_t r1, uint32_t r2, const std::function<void(uint32_t, uint64_t, uint32_t)>& func) const
	{
		bool	reverse	 = false;
		int32_t rangelen = 0;
		if(r1 < 1)
			r1 = 1;
		if(r2 < 1)
			r2 = 1;
		if(r1 > zCount())
			r1 = zCount();
		if(r2 > zCount())
			r2 = zCount();

		if(r1 <= r2)
		{
			reverse	 = false;
			rangelen = r2 - r1 + 1;
		}
		else
		{
			reverse	 = true;
			rangelen = r1 - r2 + 1;
		}

		auto	node = m_SkipList.GetNodeByRank(r1);
		int32_t n	 = 0;
		while(node && n < rangelen)
		{
			uint32_t nRank = (reverse) ? r1 - n : r1 + n;
			func(nRank, node->m_member, GetHighFromU64(node->m_score));
			node = node->getNext(reverse);
			n++;
		}
	}

	void zRevRange(uint32_t r1, uint32_t r2, const std::function<void(uint32_t, uint64_t, uint32_t)>& func) const
	{
		if(r1 < 1)
			r1 = 1;
		if(r2 < 1)
			r2 = 1;
		if(r1 > zCount())
			r1 = zCount();
		if(r2 > zCount())
			r2 = zCount();

		return zRange(_reverse_rank(r1), _reverse_rank(r2), func);
	}

	void zRangeByScore(uint32_t s1, uint32_t s2, const std::function<void(uint64_t, uint32_t)>& func) const
	{
		bool					  reverse = false;
		CSkipList::CSkipListNode* node;
		/*if(std::isnan(s1) || std::isnan(s2) )
			return;*/

		if(s1 <= s2)
		{
			reverse = false;
			node	= m_SkipList.FirstInRange(MAKE64(s1, 0), MAKE64(s2, 0));
		}
		else
		{
			reverse = true;
			node	= m_SkipList.LastInRange(MAKE64(s2, 0), MAKE64(s1, 0));
		}

		while(node)
		{
			auto score = GetHighFromU64(node->m_score);
			if(reverse)
			{
				if(score < s2)
					break;
			}
			else
			{
				if(score > s2)
					break;
			}

			func(node->m_member, score);
			node = node->getNext(reverse);
		}
	}

	void zRem(uint64_t member)
	{
		auto it = m_MemScoreMap.find(member);
		if(it != m_MemScoreMap.end())
		{
			m_SkipList.Delete(it->second, member);
			m_MemScoreMap.erase(it);
		}
	}

	void zRemRangeByRank(uint32_t r1, uint32_t r2)
	{
		m_SkipList.DeleteByRank(r1, r2, [this](uint64_t member, uint32_t score) { m_MemScoreMap.erase(member); });
	}

	void for_each(const std::function<void(uint32_t, uint64_t, uint32_t)>& func) const
	{
		m_SkipList.for_each([func](uint32_t r, uint64_t m, uint64_t s) { func(r, m, GetHighFromU64(s)); });
	}

	uint32_t _reverse_rank(uint32_t r) const { return zCount() - r + 1; }
};

#endif /* ZSET_H */
