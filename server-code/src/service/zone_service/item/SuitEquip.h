#ifndef SUITEQUIP_H
#define SUITEQUIP_H

#include "ActorAttrib.h"
#include "BaseCode.h"
#include "Cfg_Suit.pb.h"
class CSuitEquipData : public Noncopyable<CSuitEquipData>
{
    CSuitEquipData() {}
public:
    CreateNewImpl(CSuitEquipData);
public:
    ~CSuitEquipData() {}
    
    bool Init(const Cfg_Suit_Row& row)
    {
        m_nEquipNum = row.num();
        for(int32_t i = 0; i < row.attrib_change_list_size(); i++)
        {
            m_AttribChangeList.push_back(CActorAttribChange{row.attrib_change_list(i)});
        }
        return true;
    }

    uint32_t GetEquipNum() const { return m_nEquipNum; }

public:
    const std::vector<CActorAttribChange>& GetAttribChangeList() const { return m_AttribChangeList; }

private:
    uint32_t                        m_nEquipNum;
    std::vector<CActorAttribChange> m_AttribChangeList;
};

//////////////////////////////////////////////////////////////////////
class CSuitEquipSet : public Noncopyable<CSuitEquipSet>
{
    CSuitEquipSet();
public:
    CreateNewImpl(CSuitEquipSet);
public:
    virtual ~CSuitEquipSet();

public:
    
    bool Init(const char* szFileName);
    bool Reload(const char* szFileName);
    void Destroy();
    // 根据itemtype, 追加等级，查询对应的追加数据
    const CSuitEquipData* QuerySuitEquip(uint32_t idSuit, uint32_t nEquipNum);

public:
    typedef std::vector<CSuitEquipData*>            LEVEL_SET;
    typedef std::unordered_map<uint32_t, LEVEL_SET> TYPE_SET;

protected:
    TYPE_SET m_setSuitType;
};
#endif /* SUITEQUIP_H */
