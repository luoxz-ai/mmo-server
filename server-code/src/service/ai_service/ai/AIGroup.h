#ifndef AIGROUP_H
#define AIGROUP_H

#include <unordered_set>
#include <functional>
#include <memory>

#include "BaseCode.h"
#include "export_lua.h"

class CAIActor;
class CAIPhase;

export_lua class CAIGroup  : public NoncopyableT<CAIGroup>
{
    CAIGroup();

    bool Init(uint32_t group_id, CAIPhase* pScene);

public:
    CreateNewImpl(CAIGroup);
    ~CAIGroup();

public:
    export_lua void AddMember(CAIActor* pActor);
    export_lua void RemoveMember(CAIActor* pActor);
    export_lua void Foreach(std::function<void(CAIActor*)> func);
    export_lua void FindIF(std::function<bool(CAIActor*)> func);
private:
    uint32_t m_GroupID = 0;
    CAIPhase* m_pScene = nullptr;
    std::unordered_set<CAIActor*> m_setMember;
};


export_lua class CAIGroupManager: public NoncopyableT<CAIGroupManager>
{
    CAIGroupManager();

    bool Init(CAIPhase* pScene);

public:
    CreateNewImpl(CAIGroupManager);
    ~CAIGroupManager();

public:
    export_lua CAIGroup* GetGroup(uint32_t group_id);
private:
    CAIPhase* m_pScene = nullptr;
    std::unordered_map<uint32_t, std::unique_ptr<CAIGroup> > m_setGroup;
};

#endif /* AIGROUP_H */
