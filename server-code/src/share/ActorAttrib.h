#ifndef ACTORATTRIB_H
#define ACTORATTRIB_H


#include <unordered_map>

#include "AttribChangeDataProto.pb.h"
#include "BaseCode.h"
#include "Common.pb.h"
#include "game_common_def.h"

// val = (val + nValModify) * (nValPercent+10000) / 10000
//将属性变化储存在一个uint64中， 前32位代表数值的增删, 后32位代表数值的变化系数(万分比)
struct CActorAttribChangeVal
{
  public:
	CActorAttribChangeVal(int32_t nValModify, int32_t nValFactor)
		: m_Data{nValModify, nValFactor}
	{
	}

	union {
		struct
		{
			int32_t nValModify;
			int32_t nValFactor;
		} m_Data;
		uint64_t m_nData32 = 0;
	};
};

//计算过程中的临时变量
struct CActorAttribChangeValTmp
{
  public:
	CActorAttribChangeValTmp& operator+=(const CActorAttribChangeVal& rht)
	{
		nValModify += rht.m_Data.nValModify;
		nValFactor += rht.m_Data.nValFactor;
		return *this;
	}
	CActorAttribChangeValTmp& operator+=(const CActorAttribChangeValTmp& rht)
	{
		nValModify += rht.nValModify;
		nValFactor += rht.nValFactor;
		return *this;
	}

	CActorAttribChangeValTmp operator+(const CActorAttribChangeVal& rht) const
	{
		return CActorAttribChangeValTmp{nValModify + rht.m_Data.nValModify, nValFactor + rht.m_Data.nValFactor};
	}
	int32_t nValModify = 0;
	int32_t nValFactor = 0;
};

struct CActorAttribChange
{
  public:
	CActorAttribChange(uint32_t nAttribIdx = 0, uint32_t nOpLev = 0, int16_t nValModify = 0, int16_t nValFactor = 0)
		: m_nAttribIdx(nAttribIdx)
		, m_nOpLev(nOpLev)
		, m_AttribChangeVal{nValModify, nValFactor}
	{
	}
	CActorAttribChange(const AttribChangeDataProto& v)
		: m_nAttribIdx(v.attrib())
		, m_nOpLev(v.oplev())
		, m_AttribChangeVal{v.val(), v.factor()}
	{
	}

	uint32_t			  m_nAttribIdx = 0;
	uint32_t			  m_nOpLev	   = 0;
	CActorAttribChangeVal m_AttribChangeVal;
};

// RoleAttr计算过程中的数据缓存
class CActorAttribCalc
{
  public:
	typedef std::unordered_map<size_t, CActorAttribChangeValTmp> CHANGE_VAL_MAP;
	CActorAttribCalc() {}
	~CActorAttribCalc() {}

	CActorAttribCalc& operator+=(const CActorAttribChange& modify)
	{
		auto& data = m_setData[modify.m_nOpLev][modify.m_nAttribIdx];
		data += modify.m_AttribChangeVal;
		return *this;
	}

	void Merge(const CActorAttribCalc& rht)
	{
		for(auto it_map = rht.m_setData.begin(); it_map != rht.m_setData.end(); it_map++)
		{
			const auto& ref_list = it_map->second;
			uint32_t	oplev	 = it_map->first;
			for(auto it = ref_list.begin(); it != ref_list.end(); it++)
			{
				size_t							nAttribIdx = it->first;
				const CActorAttribChangeValTmp& refData	   = it->second;
				m_setData[oplev][nAttribIdx] += refData;
			}
		}
	}

	void Clear() { m_setData.clear(); }

	CHANGE_VAL_MAP&							  _GetDataSet(uint32_t oplev) { return m_setData[oplev]; }
	std::map<uint32_t, CHANGE_VAL_MAP>&		  _GetDataSet() { return m_setData; }
	const std::map<uint32_t, CHANGE_VAL_MAP>& _GetDataSet() const { return m_setData; }

  private:
	std::map<uint32_t, CHANGE_VAL_MAP> m_setData;
};

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
	CActorAttrib(const CActorAttrib& rht)
		: m_setAttribBase(rht.m_setAttribBase)
		, m_Calc(rht.m_Calc)
	{
		memset(&m_setAttrib, 0, sizeof(m_setAttrib));
	}

	CActorAttrib& operator=(const CActorAttrib& rht)
	{
		m_setAttribBase = rht.m_setAttribBase;
		m_Calc			= rht.m_Calc;

		return *this;
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

	void Store(const CActorAttribCalc& valModify_All) { m_Calc.Merge(valModify_All); }
	void StoreTmp(const CActorAttribCalc& valModify_All) { m_CalcTmp.Merge(valModify_All); }

	void Apply()
	{
		m_CalcTmp.Merge(m_Calc);
		m_setAttrib = m_setAttribBase;
		for(const auto& [oplev, ref_list]: m_CalcTmp._GetDataSet())
		{
			for(const auto& [nAttribIdx, change]: ref_list)
			{
				auto& val = m_setAttrib[nAttribIdx];
				if(change.nValModify == 0)
				{
					val = (val + change.nValModify);
				}
				else
				{
					val = MulDiv(val + change.nValModify, change.nValFactor + 10000, 10000);
				}
			}
		}
		m_CalcTmp.Clear();
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

  private:
	std::array<uint32_t, ATTRIB_MAX> m_setAttribBase;
	std::array<uint32_t, ATTRIB_MAX> m_setAttrib;
	CActorAttribCalc				 m_Calc;
	CActorAttribCalc				 m_CalcTmp;
};
#endif /* ACTORATTRIB_H */
