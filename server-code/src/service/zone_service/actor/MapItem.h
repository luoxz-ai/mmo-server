#ifndef MAPITEM_H
#define MAPITEM_H

#include "Actor.h"
#include "BaseCode.h"

export_lua class CMapItem : public CActor
{
protected:
    CMapItem();

public:
    virtual ~CMapItem();

    CREATE_NEW_IMPL(CMapItem);
    bool                Init(uint32_t idType);
    export_lua uint32_t GetTypeID() const { return m_idType; }

public:
    export_lua virtual ActorType GetActorType() const override { return ActorType::ACT_MAPITEM; }
    export_lua static ActorType  GetActorTypeStatic() { return ActorType::ACT_MAPITEM; }
    virtual void                 MakeShowData(SC_AOI_NEW& msg) override;
    virtual void                 OnEnterMap(CSceneBase* pScene) override;
    virtual void                 OnLeaveMap(uint64_t idTargetScene) override;

public:
    MEMORYHEAP_DECLARATION(s_heap);

private:
    uint32_t m_idType;
};
#endif /* MAPITEM_H */
