#ifndef ITEMADDITION_H
#define ITEMADDITION_H

#include "ActorAttrib.h"
#include "BaseCode.h"
#include "Cfg_ItemAddition.pb.h"

class CItemAdditionData
{
    CItemAdditionData() {}

public:
    ~CItemAdditionData() {}
    CREATE_NEW_IMPL(CItemAdditionData);
    bool Init(const Cfg_ItemAddition_Row& row)
    {
        for(int32_t i = 0; i < row.attrib_change_list_size(); i++)
        {
            m_AttribChangeList.push_back(CActorAttribChange{row.attrib_change_list(i)});
        }
        return true;
    }

public:
    const std::vector<CActorAttribChange>& GetAttrib() const { return m_AttribChangeList; }

private:
    std::vector<CActorAttribChange> m_AttribChangeList;
};

//////////////////////////////////////////////////////////////////////
class CItemAdditionSet
{
    CItemAdditionSet();

public:
    virtual ~CItemAdditionSet();

public:
    CREATE_NEW_IMPL(CItemAdditionSet);
    bool Init(const char* szFileName);
    bool Reload(const char* szFileName);
    void Destroy();
    // 根据itemtype, 追加等级，查询对应的追加数据
    const CItemAdditionData* QueryItemAddition(uint32_t idType, int32_t nLevel);

public:
    typedef std::vector<CItemAdditionData*>      LEVEL_SET;
    typedef std::unordered_map<OBJID, LEVEL_SET> TYPE_SET;

protected:
    TYPE_SET m_setAddition;
};
#endif /* ITEMADDITION_H */
