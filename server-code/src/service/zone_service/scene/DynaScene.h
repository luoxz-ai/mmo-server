#ifndef DYNASCENE_H
#define DYNASCENE_H

#include "EventManager.h"
#include "Scene.h"
class CDynaScene : public CScene
{
protected:
    CDynaScene();

public:
    virtual ~CDynaScene();
    CREATE_NEW_IMPL(CDynaScene);

public:
    bool Init(const SceneID& idScene);
    void AddOwnerID(OBJID idOwner);
    void SetOwnerID(const std::vector<OBJID>& setPlayerID);
    void ClearOwner(OBJID idOwner);
    bool IsOwner(OBJID idOwner) const;
    void _DestoryThisScene();

    virtual bool IsDynaScene() const override { return true; }
    virtual void EnterMap(CSceneObject* pActor, float fPosX, float fPosY, float fFace) override;
    virtual void LeaveMap(CSceneObject* pActor, uint64_t idTargetScene = 0) override;

protected:
    std::map<OBJID, bool> m_OwnerIDSet;
    CEventEntryPtr        m_DelEvent;
};
#endif /* DYNASCENE_H */
