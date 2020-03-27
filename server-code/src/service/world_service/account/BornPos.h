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
class CBornPos: public Noncopyable<CBornPos>
{
    CBornPos() {}
    bool Init(const Cfg_BornPos_Row& row)
    {
        m_row = row;
        return true;
    }
public:
    CreateNewImpl(CBornPos);
public:
    ~CBornPos() {}

public:
    

    uint32_t GetID() const { return m_row.id(); }
    uint32_t GetProf() const { return m_row.prof(); }
    uint32_t GetMapID() const { return m_row.mapid(); }
    float    GetPosX() const { return m_row.posx(); }
    float    GetPoxY() const { return m_row.posy(); }
    float    GetRange() const { return m_row.range(); }
    float    GetFace() const { return m_row.face(); }

protected:
    Cfg_BornPos::Row m_row;
};

class CBornPosSet: public Noncopyable<CBornPosSet>
{
    CBornPosSet() {}
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
             
            m_vecData[pData->GetProf()].push_back(std::unique_ptr<CBornPos>{pData});
        }
        return true;
    }
 public:
    CreateNewImpl(CBornPosSet);
public:
    
    ~CBornPosSet() { Clear(); }

public:
    void Clear()
    {
    }

    
    bool Reload(const char* pszFileName)
    {
        Clear();
        return Init(pszFileName);
    }

    const CBornPos* RandGet(uint32_t dwProf)const
    {
        auto it = m_vecData.find(dwProf);
        if(it == m_vecData.end())
        {
            return nullptr;
        }
        const auto& refVecData = it->second;
        return refVecData[random_uint32_range(0, refVecData.size() - 1)].get();
    }

private:
    std::unordered_map<uint32_t, std::vector<std::unique_ptr<CBornPos>>> m_vecData;
};

#endif /* BORNPOS_H */
