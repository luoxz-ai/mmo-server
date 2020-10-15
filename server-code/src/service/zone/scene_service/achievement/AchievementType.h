#ifndef ACHIEVEMENTTYPE_H
#define ACHIEVEMENTTYPE_H

#include "BaseCode.h"
#include "T_GameDataMap.h"
#include "config/Cfg_Achievement.pb.h"

class CAchievementType : public NoncopyableT<CAchievementType>
{
    CAchievementType() {}
    bool Init(const Cfg_Achievement& row)
    {
        m_Data = row;
        return true;
    }

public:
    CreateNewImpl(CAchievementType);

public:
    ~CAchievementType() {}

    using PB_T = Cfg_Achievement;
    static uint32_t GetKey(const Cfg_Achievement& row) { return row.id(); }

    uint32_t           GetID() const { return m_Data.id(); }
    const std::string& GetName() const { return m_Data.name(); }
    uint32_t           GetLevReq() const { return m_Data.lev_req(); }
    uint32_t           GetProfReq() const { return m_Data.prof_req(); }
    uint32_t           GetGroupID() const { return m_Data.group_id(); }

    decltype(auto) GetCheckData() const { return m_Data.check_data(); }

    uint32_t       GetAwardExp() const { return m_Data.award_exp(); }
    uint32_t       GetAwardAchiPoint() const { return m_Data.award_achipoint(); }
    decltype(auto) GetAwardItemList() const { return m_Data.award_list(); }

public:
    const Cfg_Achievement& GetDataRef() const { return m_Data; }

private:
    Cfg_Achievement m_Data;
};

//////////////////////////////////////////////////////////////////////
class CAchievementTypeSet : public CGameDataContainer<CAchievementType>
{
    CAchievementTypeSet();

public:
    CreateNewImpl(CAchievementTypeSet);

public:
    virtual ~CAchievementTypeSet();

public:
    typedef std::unordered_map<uint32_t, CAchievementType*> DATA_TYPE;
    typedef std::vector<CAchievementType*>                  SUB_DATA_LIST;
    typedef std::unordered_map<uint32_t, SUB_DATA_LIST>     DATA_MAP_BY_CHECKTYPE;
    typedef std::unordered_map<uint32_t, SUB_DATA_LIST>     DATA_MAP_BY_GROUPID;

public:
    virtual void Clear() override;
    virtual void BuildIndex() override;

    // 根据itemtype, 追加等级，查询对应的追加数据
    const CAchievementType* GetData(uint32_t idType) const;
    const DATA_TYPE&        GetData() const;
    const SUB_DATA_LIST*    QueryAchiemenetByGroupID(uint32_t idGroup) const;
    const SUB_DATA_LIST*    QueryAchiemenetByCheckType(uint32_t idType) const;

protected:
    DATA_TYPE             m_setData;
    DATA_MAP_BY_GROUPID   m_setDataByGroupID;
    DATA_MAP_BY_CHECKTYPE m_setDataByCheckType;
};
#endif /* ACHIEVEMENTTYPE_H */
