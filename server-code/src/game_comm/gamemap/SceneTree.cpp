#include "SceneTree.h"
#include "SceneObject.h"
#include "GameMap.h"
static const int CONST_SCENE_NODE_GRIDRANGE = 30;

CSceneTree::CSceneTree()
{

}

CSceneTree::~CSceneTree()
{
	m_setNode.clear();
	m_setCollision.clear();
	m_pMap = nullptr;
}


bool CSceneTree::InitSceneTree(uint32_t nNodeGridRange)
{
	m_nNodeDefaultGridRange = nNodeGridRange; 
	if(m_nNodeDefaultGridRange == 0)
		m_nNodeDefaultGridRange = CONST_SCENE_NODE_GRIDRANGE;
	_SetSceneNodeGridRange(m_nNodeDefaultGridRange);
	if(m_pMap->HasMapFlag(MAPFLAG_COLLISION_ENABLE) == true)
	{
		m_setCollision.resize(m_pMap->GetMapData()->GetGirdCount());
	}
	return true;
}

CSceneNode* CSceneTree::GetSceneNodeByPos(float x, float y)
{
	uint32_t tower_idx = GetSceneNodeIndexByPos(x, y);
	if (tower_idx >= m_setNode.size())
		return nullptr;
	return &m_setNode[tower_idx];
}

uint32_t CSceneTree::GetSceneNodeIndexByPos(float x, float y)
{
	float dx = x - m_BasePos.x;
	float dy = y - m_BasePos.y;

	uint32_t cellx = (dx < 0.0f) ? 0 : ceil( dx / float(m_nNodeGridRange) );
	uint32_t celly = (dy < 0.0f) ? 0 : ceil( dy / float(m_nNodeGridRange));
	if (cellx >= m_nNodeWidth)
		cellx = m_nNodeWidth - 1;
	if (celly >= m_nNodeHeight)
		celly = m_nNodeHeight - 1;


	return cellx + celly*m_nNodeWidth;
}

bool CSceneTree::foreach_SceneNodeInSight(float x, float y, std::function<void(CSceneNode*)>&& func)
{
	float dx = x - m_BasePos.x;
	float dy = y - m_BasePos.y;

	uint32_t cellx = (dx < 0.0f) ? 0 : ceil(dx / float(m_nNodeGridRange));
	uint32_t celly = (dy < 0.0f) ? 0 : ceil(dy / float(m_nNodeGridRange));
	if (cellx >= m_nNodeWidth)
		cellx = m_nNodeWidth - 1;
	if (celly >= m_nNodeHeight)
		celly = m_nNodeHeight - 1;

	uint32_t xx_min = __max(0, cellx - 1);
	uint32_t xx_max = __min(m_nNodeWidth, cellx + 1);

	uint32_t yy_min = __max(0, celly - 1);
	uint32_t yy_max = __min(m_nNodeHeight, celly + 1);

	for (uint32_t xx = xx_min; xx < xx_max; xx++)
	{
		for (uint32_t yy = yy_min; yy < yy_max; yy++)
		{
			CSceneNode* pNode = &m_setNode[xx + yy * m_nNodeWidth];
			func(pNode);
		}
	}

	return true;
}

struct TOWER_RESIZE_WATER_MARK
{
	uint32_t low_water_mark;
	uint32_t hight_water_mark;
	uint32_t tower_gridrange;
	uint32_t view_count;
	bool view_in;
	bool view_out;
};
TOWER_RESIZE_WATER_MARK TOWER_RESIZE_WATER_MARK_ARRAY[] =
{
	{ 0, 300, CONST_SCENE_NODE_GRIDRANGE, 0, true, true },
	{ 200, 600, 25, 0, true, true },
	{ 400, 1000, 20, 0, true, true },
	{ 700, 1500, 18, 80, true, false },
	{ 1100, 1900, 15, 80, true, false },
	{ 1500, 2200, 12, 80, true, false },
	{ 1800, 2500, 10, 40, true, false },
	{ 2100, 30000, 8, 40, false, false },
};


void CSceneTree::CheckNeedResizeSceneNode(uint32_t nPlayerCount)
{
	if (m_bDynamicAdjustNodeLevel == false)
		return;

	if (nPlayerCount < TOWER_RESIZE_WATER_MARK_ARRAY[m_nCurNodeDynamicLevel].low_water_mark)
	{
		//dec_tower_size
		if (m_nCurNodeDynamicLevel > 0)
		{
			m_nCurNodeDynamicLevel--;
		}
		if (m_nCurNodeDynamicLevel == 0)
		{
			_SetSceneNodeGridRange(m_nNodeDefaultGridRange);
		}
		else
		{
			_SetSceneNodeGridRange(TOWER_RESIZE_WATER_MARK_ARRAY[m_nCurNodeDynamicLevel].tower_gridrange);
		}

		SetViewCount(TOWER_RESIZE_WATER_MARK_ARRAY[m_nCurNodeDynamicLevel].view_count);

		if (TOWER_RESIZE_WATER_MARK_ARRAY[m_nCurNodeDynamicLevel].view_out == false)
		{
			SetViewRangeOut(0);
		}
		if (TOWER_RESIZE_WATER_MARK_ARRAY[m_nCurNodeDynamicLevel].view_in == false)
		{
			SetViewRangeIn(0);
		}

	}
	else if (nPlayerCount > TOWER_RESIZE_WATER_MARK_ARRAY[m_nCurNodeDynamicLevel].hight_water_mark)
	{
		//inc_tower_size
		if (m_nCurNodeDynamicLevel < sizeOfArray(TOWER_RESIZE_WATER_MARK_ARRAY))
		{
			m_nCurNodeDynamicLevel++;
		}
		_SetSceneNodeGridRange(TOWER_RESIZE_WATER_MARK_ARRAY[m_nCurNodeDynamicLevel].tower_gridrange);
		SetViewCount(TOWER_RESIZE_WATER_MARK_ARRAY[m_nCurNodeDynamicLevel].view_count);

		if (TOWER_RESIZE_WATER_MARK_ARRAY[m_nCurNodeDynamicLevel].view_out == false)
		{
			SetViewRangeOut(0);
		}
		if (TOWER_RESIZE_WATER_MARK_ARRAY[m_nCurNodeDynamicLevel].view_in == false)
		{
			SetViewRangeIn(0);
		}
	}
}

void CSceneTree::_SetSceneNodeGridRange(uint32_t val)
{
	if (val == 0)
	{
		return;
	}

	//先将所有的角色保存好,并将它们从Tower中移除
	std::unordered_set<CSceneObject*> actors;
	for (uint32_t i = 0; i != m_nNodeWidth * m_nNodeHeight; ++i)
	{
		const std::unordered_set<CSceneObject*>& this_actors = m_setNode[i];
		for (CSceneObject* actor : this_actors)
		{
			if (actor != nullptr)
			{
				actor->SetSceneNode(nullptr);
				actors.insert(actor);
			}
		}
	}

	//删除所有的Tower
	m_setNode.clear();

	//重新分配
	m_nNodeGridRange = val;
	m_nViewRangeIn = m_nNodeGridRange;
	m_nViewRangeOut = m_nViewRangeIn + m_nViewRangeIn / 4;
	m_nViewRangeInSquare = m_nViewRangeIn * m_nViewRangeIn;
	m_nViewRangeOutSquare = m_nViewRangeOut * m_nViewRangeOut;

	auto pMapData = m_pMap->GetMapData();
	m_nNodeWidth = (pMapData->GetWidthMap() + m_nNodeGridRange - 1) / m_nNodeGridRange;
	m_nNodeHeight = (pMapData->GetHeightMap() + m_nNodeGridRange - 1) / m_nNodeGridRange;


	if (m_nNodeWidth <= 0 || m_nNodeHeight <= 0)
	{
		LOGERROR("map size error!!!!! width = {}, height = {}", pMapData->GetWidth(), pMapData->GetHeight());
		return;
	}

	LOGDEBUG("resize node {}", m_nNodeWidth*m_nNodeHeight);

	m_setNode.resize(m_nNodeWidth*m_nNodeHeight);

	for (CSceneObject* actor : actors)
	{
		actor->SetSceneNode(GetSceneNodeByPos(actor->GetPosX(), actor->GetPosY()));
	}
}

CSceneCollisionNode* CSceneTree::GetCollisionNodeByPos(float x, float y, uint32_t actor_type)
{
	if(m_pMap->HasMapFlag(MAPFLAG_COLLISION_ENABLE) == false)
		return nullptr;
	uint32_t nIdx = m_pMap->GetMapData()->Pos2Idx(x, y);
	CHECKF(nIdx < m_setCollision.size());

	return &(m_setCollision[nIdx]);
}

bool CSceneTree::CollisionTest(float x, float y, uint32_t actor_type) const
{
	if(m_pMap->HasMapFlag(MAPFLAG_COLLISION_ENABLE) == false)
		return false;
	uint32_t nIdx = m_pMap->GetMapData()->Pos2Idx(x, y);
	CHECKF(nIdx < m_setCollision.size());

	const auto& refData = m_setCollision[nIdx];
	return refData.find_if([this,actor_type](uint32_t k, size_t v)
	{
		if(m_pMap->GetMapData()->CanCollision(actor_type, k) == true)
		{
			if(v > 0)
				return true;
		}
		return false;
	});
}


void CSceneCollisionNode::AddActor(CSceneObject* pActor)
{
	m_setCount[pActor->GetActorType()]++;
}

void CSceneCollisionNode::RemoveActor(CSceneObject* pActor)
{
	m_setCount[pActor->GetActorType()]--;
}

size_t CSceneCollisionNode::size(uint32_t actor_type) const
{
	auto it = m_setCount.find(actor_type);
	if(it == m_setCount.end())
		return 0;
	else
		return it->second;
}


bool CSceneCollisionNode::find_if(std::function<bool(uint32_t, size_t)> func) const
{
	for(const auto&[k,v] : m_setCount)
	{
		if(func(k,v) == true)
			return true;
	}
	return false;
}