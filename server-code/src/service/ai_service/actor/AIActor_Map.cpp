#include "AIActor.h"
#include "AIMonster.h"
#include "AIPlayer.h"
#include "AIScene.h"
#include "AIService.h"

//////////////////////////////////////////////////////////////////////
void CAIActor::AddToViewList(CSceneObject* pActor, bool bChkDuplicate, bool bSendShow)
{
    CSceneObject::AddToViewList(pActor, bChkDuplicate, bSendShow);

    //����Լ��ǹ���
    if(GetActorType() == ACT_MONSTER)
    {
        ConvertToDerived<CAIMonster>()->SetIsAISleep(false);
    }
}

//////////////////////////////////////////////////////////////////////
void CAIActor::ClearViewList(bool bSendMsgToSelf)
{
    for(uint64_t id: m_ViewActors)
    {
        // ֪ͨ�Է��Լ���ʧ
        CAIActor* pActor = AIActorManager()->QueryActor(id);
        if(pActor)
        {
            pActor->RemoveFromViewList(this, GetID(), true);
        }
    }
    //����ɾ����

    m_ViewActorsByType.clear();
    m_ViewActors.clear();
}

bool CAIActor::IsNeedAddToBroadCastSet(CSceneObject* pActor)
{
    //���������Ҫ��������������Լ���Ұ��,���ﴦ��
    if(GetActorType() == ACT_MONSTER)
    {
        CAIMonster* pMonster = this->ConvertToDerived<CAIMonster>();
        // ֻ�������˵�λ
        return pMonster->IsEnemy(pActor);
    }
    else
    {
        // player need add all to view list
        return true;
    }
}

void CAIActor::AOIProcessActorRemoveFromAOI(const BROADCAST_SET& setBCActorDel,
                                            BROADCAST_SET&       setBCActor,
                                            int32_t              nCanReserveDelCount,
                                            uint32_t             view_range_out_square)
{
    for(auto it = setBCActorDel.begin(); it != setBCActorDel.end(); it++)
    {
        // ����Է�����������Ұ��Χ�ڣ���ɾ��
        uint64_t  id     = *it;
        CAIActor* pActor = AIActorManager()->QueryActor(id);
        if(pActor == nullptr)
        {
            //����Է��Ѿ���ʧ,���Ƴ�
            RemoveFromViewList(nullptr, id, false);
            continue;
        }
        if(nCanReserveDelCount > 0)
        {
            if(view_range_out_square > 0)
            {
                uint32_t distance_square = GameMath::simpleDistance(GetPos(), pActor->GetPos());
                if(distance_square < view_range_out_square) // ��������Ұ�뾶�ڵģ�����Ҫ�뿪�㲥����)
                {
                    nCanReserveDelCount--;
                    setBCActor.insert(std::lower_bound(setBCActor.begin(), setBCActor.end(), id), id);
                    continue;
                }
            }
        }

        // �Է���������Ұ

        // ֪ͨ�Լ��Է���ʧ
        //����Ҫ���Լ���m_ViewActors�Ƴ�,��Ϊ���»�һ�����Ƴ�,
        //Ϊ�˼��ٷ��ʹ���,���͸��Լ����Ƴ���Ϣһ���Է���
        RemoveFromViewList(pActor, id, false);
        // ֪ͨ�Է��Լ���ʧ,
        pActor->RemoveFromViewList(this, this->GetID(), true);
    }
}

//////////////////////////////////////////////////////////////////////////
//
void CAIActor::AOIProcessActorAddToAOI(BROADCAST_SET& setBCActorAdd, const ACTOR_MAP& mapAllViewActor)
{
    for(auto it = setBCActorAdd.begin(); it != setBCActorAdd.end(); it++)
    {
        auto itr = mapAllViewActor.find(*it);
        if(itr != mapAllViewActor.end())
        {
            CAIActor* pActor = static_cast<CAIActor*>(itr->second);

            // ֪ͨ�Լ�,��Ҫ���ͶԷ�����Ϣ���Լ�
            AddToViewList(pActor, false, true);

            // ֪ͨĿ��,��Ҫ�����Լ�����Ϣ���Է�,�����ͳһ�㲥
            pActor->AddToViewList(this, true, false);
        }
    }
}
