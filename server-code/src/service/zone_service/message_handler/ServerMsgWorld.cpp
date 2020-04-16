#include <regex>

#include "MsgZoneProcess.h"
#include "Player.h"
#include "ZoneService.h"
#include "msg/ts_cmd.pb.h"
#include "msg/zone_service.pb.h"

//////////////////////////////////////////////////////////////////////////
ON_SERVERMSG(PlayerEnterZone)
{
    CHECK(msg.idplayer() != 0);
    CHECK(msg.socket() != 0);
    CHECK(msg.idscene() != 0);
    CHECK(msg.posx() != 0);
    CHECK(msg.posy() != 0);

    ZoneService()->GetLoadingThread()->CancleWaiting(msg.idplayer());

    LOGLOGIN("AddLoginPlayer: {}", msg.idplayer());
    ST_LOADINGTHREAD_PROCESS_DATA data;
    data.nPorcessType = LPT_LOADING;
    data.idPlayer     = msg.idplayer();
    data.bChangeZone  = false;
    data.socket       = msg.socket();
    data.idScene      = msg.idscene();
    data.fPosX        = msg.posx();
    data.fPosY        = msg.posy();
    data.fRange       = 0.0f;
    data.fFace        = msg.face();
    data.pPlayer      = nullptr;

    ZoneService()->GetLoadingThread()->AddLoginPlayer(std::move(data));

    ZoneService()->CreateSocketMessagePool(msg.socket());
}

ON_SERVERMSG(PlayerChangeZone)
{
    CHECK(msg.idplayer() != 0);
    CHECK(msg.socket() != 0);
    CHECK(msg.idzone() != 0);
    CHECK(msg.idscene() != 0);
    CHECK(msg.posx() >= 0);
    CHECK(msg.posy() >= 0);
    CHECK(msg.range() > 0);

    ZoneService()->GetLoadingThread()->CancleWaiting(msg.idplayer());


    ST_LOADINGTHREAD_PROCESS_DATA data;
    data.nPorcessType = LPT_LOADING;
    data.idPlayer     = msg.idplayer();
    data.bChangeZone  = true;
    data.socket       = msg.socket();
    data.idScene      = msg.idscene();
    data.fPosX        = msg.posx();
    data.fPosY        = msg.posy();
    data.fRange       = msg.range();
    data.fFace        = msg.face();
    data.pPlayer      = nullptr;

    ZoneService()->GetLoadingThread()->AddLoginPlayer(std::move(data));
    ZoneService()->CreateSocketMessagePool(msg.socket());

    return;
}

ON_SERVERMSG(PlayerChangeZone_Data)
{
    CHECK(msg.idplayer() != 0);
    CHECK(msg.socket() != 0);

    ZoneService()->PushMsgToMessagePool(msg.socket(), pMsg);
}

ON_SERVERMSG(PlayerLogout)
{
    CHECK(msg.idplayer() != 0);
    CHECK(msg.socket() != 0);

    ZoneService()->GetLoadingThread()->CancleWaiting(msg.idplayer());

    CActor* pActor = ActorManager()->QueryActor(msg.idplayer());
    if(pActor == nullptr)
    {
        // log error
        return;
    }

    CPlayer* pPlayer = pActor->CastTo<CPlayer>();
    pPlayer->OnLogout();
}