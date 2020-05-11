#include "AIPhase.h"

#include "AIActor.h"
#include "AIActorManager.h"
#include "AIScene.h"
#include "AIService.h"
#include "MapManager.h"

CAIPhase::CAIPhase() {}

CAIPhase::~CAIPhase()
{
    LOGDEBUG("PhaseDestory:{} {} idPhase:{}", GetSceneIdx().GetMapID(), GetSceneIdx().GetPhaseIdx(), m_idPhase);
    while(m_setActor.empty() == false)
    {
        CAIActor* pActor = static_cast<CAIActor*>(m_setActor.begin()->second);
        LeaveMap(pActor);
        AIActorManager()->DelActor(pActor);
    }
}

bool CAIPhase::Init(CAIScene* pScene, const SceneIdx& idxScene, uint64_t idPhase, const PhaseData* pPhaseData)
{
    m_idPhase = idPhase;
    CHECKF(CSceneBase::Init(idxScene, MapManager()));
    
    m_MonsterGen.Init(this);
    if(pPhaseData)
    {
        uint64_t idPhaseLink = pPhaseData->link_phase();
        auto     pPhase      = pScene->QueryPhaseByID(idPhaseLink);
        if(pPhase)
        {
            CHECKF(LinkSceneTree(pPhase));
        }
        else
        {
            CPos2D vBasePos{pPhaseData->left(), pPhaseData->top()};
            float  fWidth  = pPhaseData->right() - pPhaseData->left();
            float  fHeight = pPhaseData->bottom() - pPhaseData->top();
            CHECKF(InitSceneTree(vBasePos, fWidth, fHeight, pPhaseData->viewgrid_width()));
        }
    }
    else
    {
        CHECKF(InitSceneTree({0.0f, 0.0f}, 0.0f, 0.0f, 0));
    }

    LOGDEBUG("CAIPhase {} Created Map:{} Idx:{}", idPhase, idxScene.GetMapID(), idxScene.GetPhaseIdx());
    return true;
}