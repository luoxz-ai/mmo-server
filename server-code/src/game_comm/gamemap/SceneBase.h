#ifndef SCENEBASE_H
#define SCENEBASE_H

#include "BaseCode.h"
#include "DynaRegion.h"
#include "MapManager.h"
#include "SceneTree.h"

class CSceneObject;

class CSceneBase : NoncopyableT<CSceneBase>
{
protected:
    CSceneBase();

public:
    CreateNewImpl(CSceneBase);
    virtual ~CSceneBase();

public:
    bool                    Init(const SceneID& idScene, CMapManager* pMapManager);
    export_lua virtual bool IsStatic() const { return true; }

    export_lua OBJID    GetID() const { return m_idScene; }
    export_lua uint32_t GetMapID() const { return m_pMap->GetMapID(); }

    export_lua const SceneID& GetSceneID() const { return m_idScene; }

    const CGameMap* GetMap() const { return m_pMap; }
    CSceneTree*     GetSceneTree() const { return m_pSceneTree.get(); }
    bool            InitSceneTree(const CPos2D& vBasePos, float fWidth, float fHeight, uint32_t nTileGridRange);
    bool            LinkSceneTree(CSceneBase* pLinkScene);

    virtual bool      EnterMap(CSceneObject* pActor, float fPosX, float fPosY, float fFace);
    virtual void      LeaveMap(CSceneObject* pActor, uint64_t idTargetScene = 0);
    export_lua size_t GetActorCount() const { return m_setActor.size(); }
    export_lua size_t GetPlayerCount() const { return m_setPlayer.size(); }

    export_lua virtual Vector2 FindPosNearby(const Vector2& pos, float range) const;
    export_lua virtual bool    IsPassDisable(float x, float y, uint32_t actor_type) const;
    export_lua virtual bool    IsPvPDisable(float x, float y) const;
    export_lua virtual bool    IsStallDisable(float x, float y) const;
    export_lua virtual bool    IsPlaceDisable(float x, float y) const;
    export_lua virtual bool    IsRecordDisable(float x, float y) const;
    export_lua virtual bool    IsDropDisable(float x, float y) const;
    export_lua virtual bool    IsPvPFree(float x, float y) const;
    export_lua virtual bool    IsDeadNoDrop(float x, float y) const;

    export_lua virtual uint32_t GetSPRegionIdx(float x, float y) const;
    export_lua virtual float    GetHigh(float x, float y) const;

    export_lua virtual void AddDynaRegion(uint32_t nRegionType, const FloatRect& rect);
    export_lua virtual void ClearDynaRegion(uint32_t nRegionType);

protected:
    const CGameMap*                          m_pMap = nullptr;
    SceneID                                  m_idScene;
    std::shared_ptr<CSceneTree>              m_pSceneTree;
    std::unordered_map<OBJID, CSceneObject*> m_setPlayer;
    std::unordered_map<OBJID, CSceneObject*> m_setActor;

    std::unordered_map<uint32_t, CDynaRegion> m_DynaRegionDataSet;
};

#endif /* SCENEBASE_H */
