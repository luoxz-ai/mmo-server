#ifndef SCENETREE_H
#define SCENETREE_H

#include "BaseCode.h"

class CSceneObject;

class CSceneNode : public std::unordered_set<CSceneObject*>
{
public:
	CSceneNode() = default;

	void AddActor(CSceneObject* pActor) { this->insert(pActor); }
	void RemoveActor(CSceneObject* pActor) { this->erase(pActor); }
};

class CSceneCollisionNode
{
public:
	CSceneCollisionNode() = default;

	void								 AddActor(CSceneObject* pActor);
	void								 RemoveActor(CSceneObject* pActor);
	size_t								 size(uint32_t actor_type) const;
	bool								 find_if(std::function<bool(uint32_t, size_t)> func) const;
	std::unordered_map<uint32_t, size_t> m_setCount;
};

class CGameMap;
class CSceneTree
{
protected:
	CSceneTree();

public:
	virtual ~CSceneTree();

	bool InitSceneTree(uint32_t nNodeGridRange = 0);

	CGameMap* GetMap() { return m_pMap; }

	uint32_t GetViewRangeIn() const { return m_nViewRangeIn; }
	void	 SetViewRangeIn(uint32_t val)
	{
		m_nViewRangeIn		 = val;
		m_nViewRangeInSquare = m_nViewRangeIn * m_nViewRangeIn;
	}
	uint32_t GetViewRangeInSquare() const { return m_nViewRangeInSquare; }

	uint32_t GetViewRangeOut() const { return m_nViewRangeOut; }
	void	 SetViewRangeOut(uint32_t val)
	{
		m_nViewRangeOut		  = val;
		m_nViewRangeOutSquare = m_nViewRangeOut * m_nViewRangeOut;
	}
	uint32_t GetViewRangeOutSquare() const { return m_nViewRangeOutSquare; }

	uint32_t GetViewCount() const { return m_nViewCount; }
	void	 SetViewCount(uint32_t val) { m_nViewCount = val; }

	uint32_t GetCurNodeDynamicLevel() const { return m_nCurNodeDynamicLevel; }
	void	 SetCurNodeDynamicLevel(uint32_t val) { m_nCurNodeDynamicLevel = val; }

	// AOI相关的
	CSceneNode* GetSceneNodeByPos(float x, float y);
	uint32_t	GetSceneNodeIndexByPos(float x, float y);
	// 遍历自己视野内的SceneNode
	bool foreach_SceneNodeInSight(float x, float y, std::function<void(CSceneNode*)>&& func);

	void	   CheckNeedResizeSceneNode(uint32_t nPlayerCount);
	export_lua CSceneCollisionNode* GetCollisionNodeByPos(float x, float y, uint32_t actor_type);
	export_lua bool					CollisionTest(float x, float y, uint32_t actor_type) const;

private:
	void _SetSceneNodeGridRange(uint32_t v);

protected:
	std::deque<CSceneNode>			m_setNode;
	std::deque<CSceneCollisionNode> m_setCollision;

	CGameMap* m_pMap = nullptr;
	//左上角坐标
	CPos2D m_BasePos;

	uint32_t m_nNodeDefaultGridRange = 0;
	uint32_t m_nNodeGridRange		 = 0;
	uint32_t m_nNodeWidth			 = 0;
	uint32_t m_nNodeHeight			 = 0;

	uint32_t m_nViewRangeIn		  = 0;
	uint32_t m_nViewRangeInSquare = 0;

	uint32_t m_nViewRangeOut	   = 0;
	uint32_t m_nViewRangeOutSquare = 0;
	uint32_t m_nViewCount		   = 0;
	uint32_t m_nViewCountDefault   = 0;

	uint32_t m_nCurNodeDynamicLevel	   = 0;
	bool	 m_bDynamicAdjustNodeLevel = false;
};
#endif /* SCENETREE_H */
