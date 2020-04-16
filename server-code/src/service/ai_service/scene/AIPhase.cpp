#include "AIPhase.h"

#include "AIScene.h"
#include "AIActor.h"
#include "AIService.h"
#include "AIActorManager.h"
#include "MapManager.h"

CAIPhase::CAIPhase() {}

CAIPhase::~CAIPhase()
{
    while(m_setActor.empty() == false)
    {
        CAIActor* pActor = static_cast<CAIActor*>(m_setActor.begin()->second);
        LeaveMap(pActor);
        AIActorManager()->DelActor(pActor);
    }
}

bool CAIPhase::Init(CAIScene* pScene, const SceneID& idScene, uint64_t idPhase, const PhaseData* pPhaseData)
{
    m_idPhase = idPhase;
    CSceneBase::Init(idScene, MapManager());
    m_MonsterGen.Init(this);
    if(pPhaseData)
    {   
        uint64_t idPhaseLink = pPhaseData->link_phase();
        auto pPhase = pScene->QueryPhaseByID(idPhaseLink);
        if(pPhase)
        {
            LinkSceneTree(pPhase);
        }
        else
        {
            CPos2D vBasePos{pPhaseData->left(), pPhaseData->top()};
            float fWidth = pPhaseData->right() - pPhaseData->left();
            float fHeight = pPhaseData->bottom() - pPhaseData->top();
            InitSceneTree(vBasePos, fWidth, fHeight, pPhaseData->viewgrid_width());
        }
    }
    else
    {
        InitSceneTree({0.0f,0.0f}, 0.0f, 0.0f, 0);    
    }
    return true;
}