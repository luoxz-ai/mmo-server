#include "AIActor.h"
#include "AIActorManager.h"
#include "AIMonster.h"
#include "AIPlayer.h"
#include "AIScene.h"
#include "AIService.h"

//////////////////////////////////////////////////////////////////////
void CAIActor::AddToViewList(CSceneObject* pActor)
{
    __ENTER_FUNCTION
    CSceneObject::AddToViewList(pActor);

    //如果自己是怪物
    if(IsMonster())
    {
        CastTo<CAIMonster>()->SetIsAISleep(false);
    }

    __LEAVE_FUNCTION
}
