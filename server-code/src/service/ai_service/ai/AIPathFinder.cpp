#include "AIPathFinder.h"

#include "AIActor.h"
#include "AIScene.h"
#include "AIPhase.h"
CAIPathFinder::CAIPathFinder(CAIActor* pActor)
    : m_pActor(pActor)
{
}

Vector2 CAIPathFinder::SearchStep(const Vector2& dest, float move_spd)
{
    // src->dest ddlline
    // 计算行走方向
    Vector2 dis = (dest - m_pActor->GetPos());
    if(dis.length() < move_spd)
    {
        //可以行走到了
        //检查目标点是否可行走
        if(m_pActor->GetCurrentScene()->IsPassDisable(dest.x, dest.y, m_pActor->GetActorType()) == false)
        {
            return dest;
        }
        else
        {
            //从dest反向查找第一个可移动的格子
            return m_pActor->GetCurrentScene()->GetMap()->LineFindCanStand(dest, m_pActor->GetPos());
        }
    }

    auto dir     = dis.normalisedCopy();
    auto new_pos = dir * move_spd;
    if(m_pActor->GetCurrentScene()->IsPassDisable(new_pos.x, new_pos.y, m_pActor->GetActorType()) == false)
    {
        return new_pos;
    }
    else
    {
        //检查正向180度内是否有位置可以移动
        for(int32_t i = 0; i < 6; i++)
        {
            auto new_dir = GameMath::rotate(dir, 30.0f * i * ((i % 2) ? 1 : -1));
            new_pos      = new_dir * move_spd;
            if(m_pActor->GetCurrentScene()->IsPassDisable(new_pos.x, new_pos.y, m_pActor->GetActorType()) == false)
            {
                return new_pos;
            }
        }
        return m_pActor->GetPos();
    }

    // record pos
}
