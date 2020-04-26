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
        SetSceneTile(m_pScene->GetSceneTree()->GetSceneTileByPos(GetPosX(), GetPosY()));
        SetCollisionTile(m_pScene->GetSceneTree()->GetCollisionTileByPos(GetPosX(), GetPosY(), GetActorType()));
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
    SetSceneTile(pScene->GetSceneTree()->GetSceneTileByPos(GetPosX(), GetPosY()));
    SetCollisionTile(pScene->GetSceneTree()->GetCollisionTileByPos(GetPosX(), GetPosY(), GetActorType()));
}

void CSceneObject::OnLeaveMap(uint64_t idTargetScene)
{
    SetSceneTile(nullptr);
    SetCollisionTile(nullptr);
    m_pScene = nullptr;
}

void CSceneObject::SetHideCoude(int32_t nHideCount)
{
    if(nHideCount == 0)
    {
        m_nHideCount = nHideCount;
        SetSceneTile(GetCurrentScene()->GetSceneTree()->GetSceneTileByPos(GetPosX(), GetPosY()));
        SetCollisionTile(
            GetCurrentScene()->GetSceneTree()->GetCollisionTileByPos(GetPosX(), GetPosY(), GetActorType()));
    }
    else
    {
        SetSceneTile(nullptr);
        SetCollisionTile(nullptr);
        m_nHideCount = nHideCount;
    }
}

void CSceneObject::AddHide()
{
    SetSceneTile(nullptr);
    SetCollisionTile(nullptr);

    m_nHideCount++;
}

void CSceneObject::RemoveHide()
{
    m_nHideCount--;
    if(m_nHideCount == 0)
    {
        SetSceneTile(m_pScene->GetSceneTree()->GetSceneTileByPos(GetPosX(), GetPosY()));
        SetCollisionTile(m_pScene->GetSceneTree()->GetCollisionTileByPos(GetPosX(), GetPosY(), GetActorType()));
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

void CSceneObject::SetSceneTile(CSceneTile* val)
{
    __ENTER_FUNCTION

    if(m_pSceneTile == val)
        return;
    if(m_nHideCount != 0)
        return;

    if(m_pSceneTile)
        m_pSceneTile->RemoveActor(this);
    m_pSceneTile = val;
    if(m_pSceneTile)
        m_pSceneTile->AddActor(this);
    __LEAVE_FUNCTION
}

void CSceneObject::SetCollisionTile(CSceneCollisionTile* val)
{
    __ENTER_FUNCTION

    if(m_pCollisionTile == val)
        return;
    if(m_nHideCount > 0)
        return;

    if(m_pCollisionTile)
        m_pCollisionTile->RemoveActor(this);
    m_pCollisionTile = val;
    if(m_pCollisionTile)
        m_pCollisionTile->AddActor(this);
    __LEAVE_FUNCTION
}

void CSceneObject::ChangePhase(uint64_t idPhaseID)
{
    if(GetPhaseID() == idPhaseID)
    {
        return;
    }

    _SetPhaseID(idPhaseID);
    UpdateViewList();
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
        m_ViewActors.erase(idActor);
    }
}

//////////////////////////////////////////////////////////////////////
void CSceneObject::AddToViewList(CSceneObject* pActor)
{
    if(pActor == nullptr)
        return;

    if(m_ViewActors.find(pActor->GetID()) != m_ViewActors.end())
    {
        return;
    }
    m_ViewActors.insert(pActor->GetID());
    m_ViewActorsByType[pActor->GetActorType()].insert(pActor->GetID());
}

//////////////////////////////////////////////////////////////////////
bool CSceneObject::UpdateViewList()
{
    CHECKF(GetCurrentScene());
    CHECKF(GetSceneTile());
    //////////////////////////////////////////////////////////////////////////
    // 为了减少重新搜索广播集的次数，这里采用的策略是划分3*3格的逻辑格子，
    // 只有首次进入地图或逻辑格子发生变化的时候才重新进行搜索

    // 寻找新的目标集
    BROADCAST_SET setBCActor;
    ACTOR_MAP     mapAllViewActor;
    struct ACTOR_MAP_BY_DIS_DATA
    {
        uint32_t      dis;
        CSceneObject* pActor;
    };
    typedef std::deque<ACTOR_MAP_BY_DIS_DATA> ACTOR_MAP_BY_DIS;
    ACTOR_MAP_BY_DIS                          sortedAllViewActorByDist;

    uint32_t viewcount_max         = GetCurrentScene()->GetSceneTree()->GetViewCount();
    uint32_t view_range_in_square  = GetCurrentScene()->GetSceneTree()->GetViewRangeInSquare();
    uint32_t view_range_out_square = GetCurrentScene()->GetSceneTree()->GetViewRangeOutSquare();
    // 广播集算法修改测试
    //////////////////////////////////////////////////////////////////////////
    // step1: 获取当前广播集范围内的对象
    {
        GetCurrentScene()->GetSceneTree()->foreach_SceneTileInSight(
            GetPosX(),
            GetPosY(),
            [thisActor = this,
             &setBCActor,
             &mapAllViewActor,
             &sortedAllViewActorByDist,
             view_range_in_square,
             viewcount_max](CSceneTile* pSceneTile) {
                const auto& actor_list = *pSceneTile;
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
                                setBCActor.insert(pActor->GetID());
                            }
                            else
                            {
                                sortedAllViewActorByDist.emplace_back(ACTOR_MAP_BY_DIS_DATA{distance_square, pActor});
                            }
                        }
                        else
                        {
                            setBCActor.insert(pActor->GetID());
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
            int32_t nCanInsert = viewcount_max - setBCActor.size();
            struct comp
            {
                bool operator()(const ACTOR_MAP_BY_DIS_DATA& lft, uint32_t rht) const { return lft.dis < rht; }

                bool operator()(uint32_t lft, const ACTOR_MAP_BY_DIS_DATA& rht) const { return lft < rht.dis; }

                bool operator()(const ACTOR_MAP_BY_DIS_DATA& lft, const ACTOR_MAP_BY_DIS_DATA& rht) const
                {
                    return lft.dis < rht.dis;
                }
            };
            std::nth_element(sortedAllViewActorByDist.begin(),
                             sortedAllViewActorByDist.begin() + nCanInsert - 1,
                             sortedAllViewActorByDist.end(),
                             comp());

            int32_t i = 0;
            for(auto it = sortedAllViewActorByDist.begin(); i < nCanInsert && it != sortedAllViewActorByDist.end();
                it++, i++)
            {
                CSceneObject* pActor = it->pActor;
                uint64_t      id     = pActor->GetID();
                setBCActor.insert(id);
            }
        }
    }

    // 广播集必须先做好排序
    // sort(setBCActor.begin(), setBCActor.end());

    //////////////////////////////////////////////////////////////////////////
    // setp2: 计算当前广播集与旧广播集的差集——这部分是新进入视野的
    BROADCAST_SET setBCActorAdd;
    set_difference(setBCActor.begin(),
                   setBCActor.end(),
                   m_ViewActors.begin(),
                   m_ViewActors.end(),
                   std::insert_iterator(setBCActorAdd, setBCActorAdd.begin()));

    //////////////////////////////////////////////////////////////////////////
    // step3: 计算旧广播集与当前广播集的差集——这部分是可能需要离开视野的
    BROADCAST_SET setBCActorDel;
    set_difference(m_ViewActors.begin(),
                   m_ViewActors.end(),
                   setBCActor.begin(),
                   setBCActor.end(),
                   std::insert_iterator(setBCActorDel, setBCActorDel.begin()));

    //计算待删除列表还可以保留多少个
    int32_t nCanReserveDelCount = setBCActorDel.size();
    if(viewcount_max > 0)
    {
        //需要视野裁剪
        //如果当前视野人数已经超过视野人数上限
        nCanReserveDelCount = viewcount_max - ((m_ViewActors.size() - setBCActorDel.size()) + setBCActorAdd.size());
    }

    // step4: 需要离开视野的角色Remove
    OnAOIProcess_ActorRemoveFromAOI(setBCActorDel, setBCActor, nCanReserveDelCount, view_range_out_square);

    // 设置角色广播集=当前广播集-离开视野的差集
    m_ViewActors = setBCActor;
    OnAOIProcess_PosUpdate();

    //////////////////////////////////////////////////////////////////////////
    // step5: 新进入视野的角色和地图物品Add
    OnAOIProcess_ActorAddToAOI(setBCActorAdd, mapAllViewActor);

    return true;
}

void CSceneObject::ForeachViewActorList(const std::function<void(OBJID)>& func)
{
    __ENTER_FUNCTION
    std::for_each(m_ViewActors.begin(), m_ViewActors.end(), func);
    __LEAVE_FUNCTION
}