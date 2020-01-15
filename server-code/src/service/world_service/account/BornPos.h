#ifndef BORNPOS_H
#define BORNPOS_H

#include "BaseCode.h"
#include "DBRecord.h"
#include "ProtobuffUtil.h"
#include "RandomGet.h"
#include "T_GameDataMap.h"
#include "config/Cfg_BornPos.pb.h"
//////////////////////////////////////////////////////////////////////
//
class CBornPos
{
	CBornPos() {}

public:
	~CBornPos() {}
	CREATE_NEW_IMPL(CBornPos);

public:
	bool Init(const Cfg_BornPos_Row& row)
	{
		m_row = row;
		return true;
	}

	uint32_t GetID() const { return m_row.id(); }
	uint32_t GetProf() const { return m_row.prof(); }
	uint32_t GetMapID() const { return m_row.mapid(); }
	float	 GetPosX() const { return m_row.posx(); }
	float	 GetPoxY() const { return m_row.posy(); }
	float	 GetRange() const { return m_row.range(); }
	float	 GetFace() const { return m_row.face(); }

protected:
	Cfg_BornPos::Row m_row;
};

class CBornPosSet
{
	CBornPosSet() {}

public:
	CREATE_NEW_IMPL(CBornPosSet);
	~CBornPosSet() { Clear(); }

public:
	void Clear()
	{
		for(auto& refMap: m_vecData)
		{
			for(auto& v: refMap.second)
			{
				SAFE_DELETE(v);
			}
		}
		m_vecData.clear();
	}

	bool Init(const char* pszFileName)
	{
		Cfg_BornPos cfg;
		if(pb_util::LoadFromBinaryFile(pszFileName, cfg) == false)
		{
			LOGERROR("InitFromFile {} Fail.", pszFileName);
			return false;
		}

		for(const auto& iter: cfg.rows())
		{
			CBornPos* pData = CBornPos::CreateNew(iter);
			if(pData == nullptr)
			{
				return false;
			}

			m_vecData[pData->GetProf()].push_back(pData);
		}
		return true;
	}
	bool Reload(const char* pszFileName)
	{
		Clear();
		return Init(pszFileName);
	}

	CBornPos* RandGet(uint32_t dwProf)
	{
		const auto& refVecData = m_vecData[dwProf];
		return refVecData[random_uint32_range(0, refVecData.size() - 1)];
	}

private:
	std::unordered_map<uint32_t, std::vector<CBornPos*>> m_vecData;
};

#endif /* BORNPOS_H */
