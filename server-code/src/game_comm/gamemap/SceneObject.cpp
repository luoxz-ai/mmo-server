#include "SceneObject.h"

#include "SceneBase.h"
#include "taskflow/taskflow.hpp"

CSceneObject::CSceneObject() {}

CSceneObject::~CSceneObject() {}

void CSceneObject::SetPos(const Vector2& pos)
{
	__ENTER_FUNCTION
	if(m_Pos.x == pos.x && m_Pos.y == pos.y)
		return;
	CHECK(Math::isNaN(pos.x) == false);
	CHECK(Math::isNaN(pos.y) == false);

	m_Pos = pos;
	if(m_pScene)
	{
		SetSceneNode(m_pScene->GetSceneNodeByPos(GetPosX(), GetPosY()));
		SetCollisionNode(m_pScene->GetCollisionNodeByPos(GetPosX(), GetPosY(), GetActorType()));
	}
	__LEAVE_FUNCTION
}

void CSceneObject::FaceTo(const Vector2& pos)
{
	if(pos != m_Pos)
		SetFace(GameMath::getAngleFrom((pos - m_Pos).normalisedCopy()));
}

void CSceneObject::OnEnterMap(CSceneBase* pScene)
{
	m_pScene = pScene;
	SetSceneNode(pScene->GetSceneNodeByPos(GetPosX(), GetPosY()));
	SetCollisionNode(pScene->GetCollisionNodeByPos(GetPosX(), GetPosY(), GetActorType()));
}

void CSceneObject::OnLeaveMap(uint64_t idTargetScene)
{
	SetSceneNode(nullptr);
	SetCollisionNode(nullptr);
	m_pScene = nullptr;
}

void CSceneObject::SetHideCoude(int nHideCount)
{
	if(nHideCount == 0)
	{
		m_nHideCount = nHideCount;
		SetSceneNode(GetCurrentScene()->GetSceneNodeByPos(GetPosX(), GetPosY()));
		SetCollisionNode(GetCurrentScene()->GetCollisionNodeByPos(GetPosX(), GetPosY(), GetActorType()));
	}
	else
	{
		SetSceneNode(nullptr);
		SetCollisionNode(nullptr);
		m_nHideCount = nHideCount;
	}
}

void CSceneObject::AddHide()
{
	SetSceneNode(nullptr);
	SetCollisionNode(nullptr);

	m_nHideCount++;
}

void CSceneObject::RemoveHide()
{
	m_nHideCount--;
	if(m_nHideCount == 0)
	{
		SetSceneNode(m_pScene->GetSceneNodeByPos(GetPosX(), GetPosY()));
		SetCollisionNode(m_pScene->GetCollisionNodeByPos(GetPosX(), GetPosY(), GetActorType()));
	}
}

uint32_t CSceneObject::GetCurrentViewActorCount() const
{
	return m_ViewActors.size();
}

uint32_t CSceneObject::GetCurrentViewPlayerCount()
{
	return m_ViewActorsByType[ACT_PLAYER].size();
}

uint32_t CSceneObject::GetCurrentViewMonsterCount()
{
	return m_ViewActorsByType[ACT_MONSTER].size();
}

void CSceneObject::SetSceneNode(CSceneNode* val)
{
	__ENTER_FUNCTION

	if(m_pSceneNode == val)
		return;
	if(m_nHideCount != 0)
		return;

	if(m_pSceneNode)
		m_pSceneNode->RemoveActor(this);
	m_pSceneNode = val;
	if(m_pSceneNode)
		m_pSceneNode->AddActor(this);
	__LEAVE_FUNCTION
}

void CSceneObject::SetCollisionNode(CSceneCollisionNode* val)
{
	__ENTER_FUNCTION

	if(m_pCollisionNode == val)
		return;
	if(m_nHideCount > 0)
		return;

	if(m_pCollisionNode)
		m_pCollisionNode->RemoveActor(this);
	m_pCollisionNode = val;
	if(m_pCollisionNode)
		m_pCollisionNode->AddActor(this);
	__LEAVE_FUNCTION
}

bool CSceneObject::IsEnemy(CSceneObject* pTarget) const
{
	return false;
}

bool CSceneObject::IsInViewActor(CSceneObject* pActor) const
{
	if(!pActor)
	{
		return false;
	}
	if(pActor == this)
	{
		return true;
	}

	uint64_t id = pActor->GetID();
	return std::binary_search(m_ViewActors.begin(), m_ViewActors.end(), id);
}

bool CSceneObject::IsInViewActorByID(OBJID idActor) const
{
	return std::binary_search(m_ViewActors.begin(), m_ViewActors.end(), idActor);
}

//////////////////////////////////////////////////////////////////////
void CSceneObject::RemoveFromViewList(CSceneObject* pActor, OBJID idActor, bool bErase)
{
	// 通知自己对方消失
	if(pActor)
	{
		m_ViewActorsByType[pActor->GetActorType()].erase(pActor->GetID());
	}
	else
	{
		for(auto& refList: m_ViewActorsByType)
		{
			refList.second.erase(idActor);
		}
	}

	if(bErase)
	{
		BROADCAST_SET::iterator it = std::lower_bound(m_ViewActors.begin(), m_ViewActors.end(), idActor);
		if(it != m_ViewActors.end() && !(idActor < *it))
		{
			m_ViewActors.erase(it);
		}
	}
}

//////////////////////////////////////////////////////////////////////
void CSceneObject::AddToViewList(CSceneObject* pActor, bool bChkDuplicate, bool bSendShow)
{
	if(pActor == nullptr)
		return;

	if(bChkDuplicate)
	{
		if(binary_search(m_ViewActors.begin(), m_ViewActors.end(), pActor->GetID()))
		{
			return;
		}
		m_ViewActors.insert(std::lower_bound(m_ViewActors.begin(), m_ViewActors.end(), pActor->GetID()), pActor->GetID());
	}

	m_ViewActorsByType[pActor->GetActorType()].insert(pActor->GetID());
}

//////////////////////////////////////////////////////////////////////
bool CSceneObject::UpdateViewList()
{
	//////////////////////////////////////////////////////////////////////////
	// 为了减少重新搜索广播集的次数，这里采用的策略是划分3*3格的逻辑格子，
	// 只有首次进入地图或逻辑格子发生变化的时候才重新进行搜索

	// 寻找新的目标集
	BROADCAST_SET setBCActor;
	ACTOR_MAP	  mapAllViewActor;
	struct ACTOR_MAP_BY_DIS_DATA
	{
		uint32_t	  dis;
		CSceneObject* pActor;
	};
	typedef std::deque<ACTOR_MAP_BY_DIS_DATA> ACTOR_MAP_BY_DIS;
	ACTOR_MAP_BY_DIS						  sortedAllViewActorByDist;

	uint32_t viewcount_max		   = GetCurrentScene()->GetViewCount();
	uint32_t view_range_in_square  = GetCurrentScene()->GetViewRangeInSquare();
	uint32_t view_range_out_square = GetCurrentScene()->GetViewRangeOutSquare();
	// 广播集算法修改测试
	//////////////////////////////////////////////////////////////////////////
	// step1: 获取当前广播集范围内的对象
	{
		GetCurrentScene()->foreach_SceneNodeInSight(
			GetPosX(), GetPosY(),
			[thisActor = this, &setBCActor, &mapAllViewActor, &sortedAllViewActorByDist, view_range_in_square, viewcount_max](CSceneNode* pSceneNode) {
				const auto& actor_list = *pSceneNode;
				for(CSceneObject* pActor: actor_list)
				{
					if(pActor == thisActor)
						continue;

					// 判断目标是否需要加入广播集
					if(thisActor->IsNeedAddToBroadCastSet(pActor) == false)
						continue;

					//! 目标进入视野，需要加入广播集
					uint32_t distance_square = 0;

					if(view_range_in_square > 0) //距离优先判断
					{
						distance_square = GameMath::simpleDistance(thisActor->GetPos(), pActor->GetPos());
					}
					else
					{
						// view_in == 0时,使用麦哈顿距离进行判断
						distance_square = GameMath::manhattanDistance(thisActor->GetPos(), pActor->GetPos());
					}

					if(view_range_in_square <= 0 || distance_square < view_range_in_square)
					{

						if(viewcount_max > 0)
						{
							if(thisActor->IsMustAddToBroadCastSet(pActor) == true)
							{
								setBCActor.push_back(pActor->GetID());
							}
							else
							{
								sortedAllViewActorByDist.emplace_back(ACTOR_MAP_BY_DIS_DATA{distance_square, pActor});
							}
						}
						else
						{
							setBCActor.push_back(pActor->GetID());
						}

						mapAllViewActor.emplace(std::make_pair(pActor->GetID(), pActor));
					}
				}
			});
	}

	if(viewcount_max > 0)
	{
		//需要视野裁剪
		//如果当前视野人数已经超过视野人数上限
		if(setBCActor.size() < viewcount_max)
		{
			//对视野集合进行排序,只取距离自己最近的N个,作为最新广播集
			int nCanInsert = viewcount_max - setBCActor.size();
			struct comp
			{
				bool operator()(const ACTOR_MAP_BY_DIS_DATA& lft, uint32_t rht) const { return lft.dis < rht; }

				bool operator()(uint32_t lft, const ACTOR_MAP_BY_DIS_DATA& rht) const { return lft < rht.dis; }

				bool operator()(const ACTOR_MAP_BY_DIS_DATA& lft, const ACTOR_MAP_BY_DIS_DATA& rht) const { return lft.dis < rht.dis; }
			};
			std::nth_element(sortedAllViewActorByDist.begin(), sortedAllViewActorByDist.begin() + nCanInsert - 1, sortedAllViewActorByDist.end(), comp());

			int i = 0;
			for(auto it = sortedAllViewActorByDist.begin(); i < nCanInsert && it != sortedAllViewActorByDist.end(); it++, i++)
			{
				CSceneObject* pActor = it->pActor;
				uint64_t	  id	 = pActor->GetID();
				setBCActor.push_back(id);
			}
		}
	}

	// 广播集必须先做好排序
	sort(setBCActor.begin(), setBCActor.end());

	//////////////////////////////////////////////////////////////////////////
	// setp2: 计算当前广播集与旧广播集的差集——这部分是新进入视野的
	BROADCAST_SET::iterator result;

	BROADCAST_SET setBCActorAdd(setBCActor.size(), 0);
	result = set_difference(setBCActor.begin(), setBCActor.end(), m_ViewActors.begin(), m_ViewActors.end(), setBCActorAdd.begin());
	setBCActorAdd.erase(result, setBCActorAdd.end());

	//////////////////////////////////////////////////////////////////////////
	// step3: 计算旧广播集与当前广播集的差集——这部分是可能需要离开视野的
	BROADCAST_SET setBCActorDel(m_ViewActors.size(), 0);
	result = set_difference(m_ViewActors.begin(), m_ViewActors.end(), setBCActor.begin(), setBCActor.end(), setBCActorDel.begin());
	setBCActorDel.erase(result, setBCActorDel.end());

	//计算待删除列表还可以保留多少个
	int nCanReserveDelCount = setBCActorDel.size();
	if(viewcount_max > 0)
	{
		//需要视野裁剪
		//如果当前视野人数已经超过视野人数上限
		nCanReserveDelCount = viewcount_max - ((m_ViewActors.size() - setBCActorDel.size()) + setBCActorAdd.size());
	}

	// step4: 需要离开视野的角色Remove
	AOIProcessActorRemoveFromAOI(setBCActorDel, setBCActor, nCanReserveDelCount, view_range_out_square);

	// 设置角色广播集=当前广播集-离开视野的差集
	m_ViewActors = setBCActor;
	AOIProcessPosUpdate();

	//////////////////////////////////////////////////////////////////////////
	// step5: 新进入视野的角色和地图物品Add
	AOIProcessActorAddToAOI(setBCActorAdd, mapAllViewActor);

	return true;
}

void CSceneObject::ForeachViewActorList(const std::function<void(OBJID)>& func)
{
	__ENTER_FUNCTION
	std::for_each(m_ViewActors.begin(), m_ViewActors.end(), func);
	__LEAVE_FUNCTION
}