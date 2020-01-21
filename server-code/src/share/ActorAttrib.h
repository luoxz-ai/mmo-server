#ifndef ACTORATTRIB_H
#define ACTORATTRIB_H

#include <array>
#include <unordered_map>
#include <vector>

#include "AttribChangeDataProto.pb.h"
#include "BaseCode.h"
#include "Common.pb.h"
#include "game_common_def.h"

constexpr uint32_t MAX_ATTR_LAYER = 50;

// val = (val + nValModify) * (nValPercent+10000) / 10000
//将属性变化储存在一个uint64中， 前32位代表数值的增删, 后32位代表数值的变化系数(万分比)
struct CActorAttribChangeVal
{
public:
	CActorAttribChangeVal& operator+=(const CActorAttribChangeVal& rht)
	{
		nValModify += rht.nValModify;
		nValFactor += rht.nValFactor;
		return *this;
	}
	CActorAttribChangeVal& operator-=(const CActorAttribChangeVal& rht)
	{
		nValModify -= rht.nValModify;
		nValFactor -= rht.nValFactor;
		return *this;
	}
	
	CActorAttribChangeVal operator+(const CActorAttribChangeVal& rht) const
	{
		return CActorAttribChangeVal{nValModify + rht.nValModify, nValFactor + rht.nValFactor};
	}

	bool empty() const
	{
		return nValModify == 0 && nValFactor == 0;
	}


	int32_t nValModify = 0;
	int32_t nValFactor = 0;
};

struct CActorAttribChange
{
public:
	CActorAttribChange(uint32_t nAttribIdx = 0, uint32_t nOpLev = 0, int32_t nValModify = 0, int32_t nValFactor = 0)
		: m_nAttribIdx(nAttribIdx)
		, m_nLayer(nOpLev)
		, m_AttribChangeVal{nValModify, nValFactor}
	{
	}
	CActorAttribChange(const AttribChangeDataProto& v)
		: m_nAttribIdx(v.attrib())
		, m_nLayer(v.oplev())
		, m_AttribChangeVal{v.val(), v.factor()}
	{
	}

	uint32_t			  m_nAttribIdx = 0;
	uint32_t			  m_nLayer	   = 0;
	CActorAttribChangeVal m_AttribChangeVal;
};
using AttribList_t = std::array<uint32_t, ATTRIB_MAX>;
using ValModifyLayer_t = std::unordered_map<uint32_t, CActorAttribChangeVal>;
using ActorAttribChangeList_t = std::vector<CActorAttribChange>;
//最终数据
class CActorAttrib
{
public:
	~CActorAttrib() {}
	CActorAttrib()
	{
		memset(&m_setAttribBase, 0, sizeof(m_setAttribBase));
		memset(&m_setAttrib, 0, sizeof(m_setAttrib));
	}
	
	uint32_t  operator[](size_t nIdx) const { return get(nIdx); }
	uint32_t& operator[](size_t nIdx) { return get(nIdx); }

	uint32_t get(size_t nIdx) const
	{
		CHECKF(nIdx < ATTRIB_MAX);
		return m_setAttrib[nIdx];
	}
	uint32_t& get(size_t nIdx)
	{
		CHECKFR(nIdx < ATTRIB_MAX, m_setAttrib[nIdx]);
		return m_setAttrib[nIdx];
	}
	uint32_t get_base(size_t nIdx) const
	{
		CHECKF(nIdx < ATTRIB_MAX);
		return m_setAttribBase[nIdx];
	}
	uint32_t& get_base(size_t nIdx)
	{
		CHECKFR(nIdx < ATTRIB_MAX, m_setAttribBase[nIdx]);
		return m_setAttribBase[nIdx];
	}

	void SetBase(const AttribList_t& base)
	{
		m_setAttribBase = base;
	}

	void Store(const CActorAttribChange& valModify) 
	{ 
		CActorAttribChangeVal& change = m_mapValModify[valModify.m_nLayer][valModify.m_nAttribIdx]; 
		change += valModify.m_AttribChangeVal;
		m_nDirtyLayer = std::min<uint32_t>(m_nDirtyLayer, valModify.m_nLayer);
	}

	void Store(const ActorAttribChangeList_t& list) 
	{
		for(const auto& v : list)
		{
			Store(v);
		}
	}

	void Remove(const CActorAttribChange& valModify)
	{
		CActorAttribChangeVal& change = m_mapValModify[valModify.m_nLayer][valModify.m_nAttribIdx]; 
		change -= valModify.m_AttribChangeVal;
		m_nDirtyLayer = std::min<uint32_t>(m_nDirtyLayer, valModify.m_nLayer);
		if(change.empty())
		{
			m_mapValModify[valModify.m_nLayer].erase(valModify.m_nAttribIdx);
			if(m_mapValModify[valModify.m_nLayer].empty())
			{
				m_mapValModify.erase(valModify.m_nLayer);
			}
		}
	}

	void Remove(const ActorAttribChangeList_t& list) 
	{
		for(const auto& v : list)
		{
			Remove(v);
		}
	}
	

	void Apply(bool bRecalcAll)
	{
		if(bRecalcAll)
		{
			m_setAttrib = m_setAttribBase;
			for(const auto&[nLayer, stValModifyLayer] : m_mapValModify)
			{
				_CalcValLayer(nLayer, stValModifyLayer);
			}
			m_nDirtyLayer = MAX_ATTR_LAYER;
		}
		else if(m_nDirtyLayer != MAX_ATTR_LAYER)
		{
			uint32_t nCachedLayer = GetLowerBoundCacheLayer(m_nDirtyLayer);
			if(nCachedLayer == (uint32_t)-1)
			{
				Apply(true);
				return;
			}
			else
			{
				m_setAttrib = m_setCache[nCachedLayer];
				
				auto it_begin = m_mapValModify.find(nCachedLayer);
				for(auto it = it_begin; it != m_mapValModify.end(); it++)
				{
					const auto& nLayer = it->first;
					const auto& stValModifyLayer = it->second;
					_CalcValLayer(nLayer, stValModifyLayer);
				}

				m_nDirtyLayer = MAX_ATTR_LAYER;
			}
		}
		
	}


	template<class T>
	void load_from(const T& row)
	{
		get_base(ATTRIB_HP_MAX)	  = row.hp_max();
		get_base(ATTRIB_MP_MAX)	  = row.mp_max();
		get_base(ATTRIB_MOVESPD)  = row.movespd();
		get_base(ATTRIB_MIN_ATK)  = row.min_atk();
		get_base(ATTRIB_MAX_ATK)  = row.max_atk();
		get_base(ATTRIB_MIN_DEF)  = row.min_def();
		get_base(ATTRIB_MAX_DEF)  = row.max_def();
		get_base(ATTRIB_MIN_MATK) = row.min_matk();
		get_base(ATTRIB_MAX_MATK) = row.max_matk();
		get_base(ATTRIB_MIN_MDEF) = row.min_mdef();
		get_base(ATTRIB_MAX_MDEF) = row.max_mdef();
		get_base(ATTRIB_HIT)	  = row.hit();
		get_base(ATTRIB_DODGE)	  = row.dodge();
	}
	
	template<class T>
	static void load_from(AttribList_t& list, const T& row)
	{
		list[ATTRIB_HP_MAX]	  = row.hp_max();
		list[ATTRIB_MP_MAX]	  = row.mp_max();
		list[ATTRIB_MOVESPD]  = row.movespd();
		list[ATTRIB_MIN_ATK]  = row.min_atk();
		list[ATTRIB_MAX_ATK]  = row.max_atk();
		list[ATTRIB_MIN_DEF]  = row.min_def();
		list[ATTRIB_MAX_DEF]  = row.max_def();
		list[ATTRIB_MIN_MATK] = row.min_matk();
		list[ATTRIB_MAX_MATK] = row.max_matk();
		list[ATTRIB_MIN_MDEF] = row.min_mdef();
		list[ATTRIB_MAX_MDEF] = row.max_mdef();
		list[ATTRIB_HIT]	  = row.hit();
		list[ATTRIB_DODGE]	  = row.dodge();
	}
private:

	void _CalcValLayer(uint32_t nLayer, const ValModifyLayer_t& stValModifyLayer)
	{
		for(const auto&[nAttribIdx,stValChange] : stValModifyLayer)
		{
			//final =(final+val)*percent
			if(stValChange.nValFactor == 0)
			{
				m_setAttrib[nAttribIdx] =m_setAttrib[nAttribIdx] + stValChange.nValModify;
			}
			else
			{
				m_setAttrib[nAttribIdx] = MulDiv(m_setAttrib[nAttribIdx] + stValChange.nValModify, 100+stValChange.nValFactor, 100);
			}
		}

		if(NeedCacheLayer(nLayer))
		{
			m_setCache[nLayer] = m_setAttrib;
		}
	}

	static uint32_t GetLowerBoundCacheLayer(uint32_t nLayer)
	{
		auto it = std::lower_bound(NEED_CACHE_LAYER.begin(), NEED_CACHE_LAYER.end(), nLayer);
		if(it != NEED_CACHE_LAYER.end())
		{
			return *it;
		}
		return -1;
	}
	static bool NeedCacheLayer(uint32_t nLayer)
	{
		return std::binary_search(NEED_CACHE_LAYER.begin(), NEED_CACHE_LAYER.end(), nLayer);
	}
	static constexpr std::array<uint32_t,4> NEED_CACHE_LAYER = {1,5,8,10};


private:
	
	AttribList_t m_setAttribBase;
	AttribList_t m_setAttrib;
	
	std::unordered_map<uint32_t, ValModifyLayer_t > m_mapValModify;
	std::unordered_map<uint32_t, AttribList_t > m_setCache;
	uint32_t m_nDirtyLayer = MAX_ATTR_LAYER;
};
#endif /* ACTORATTRIB_H */
