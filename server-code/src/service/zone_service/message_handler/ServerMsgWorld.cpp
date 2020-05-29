#include <regex>

#include "LoadingThread.h"
#include "MsgZoneProcess.h"
#include "Player.h"
#include "ZoneService.h"
#include "MessageRoute.h"
#include "msg/ts_cmd.pb.h"
#include "msg/zone_service.pb.h"
#include "server_msg/server_side.pb.h"

//////////////////////////////////////////////////////////////////////////
ON_SERVERMSG(CZoneService, PlayerEnterZone)
{
    __ENTER_FUNCTION
    
    CHECK(msg.idplayer() != 0);
    CHECK(msg.socket() != 0);
    CHECK(msg.target_scene_idx() != 0);
    CHECK(msg.posx() != 0);
    CHECK(msg.posy() != 0);

    ZoneService()->GetLoadingThread()->CancleWaiting(msg.idplayer());

    LOGLOGIN("AddLoginPlayer: {}", msg.idplayer());
    ST_LOADINGTHREAD_PROCESS_DATA data;
    data.nPorcessType = LPT_LOADING;
    data.idPlayer     = msg.idplayer();
    data.bChangeZone  = false;
    data.socket       = msg.socket();
    data.idTargetScene = msg.target_scene_idx();
    data.fPosX        = msg.posx();
    data.fPosY        = msg.posy();
    data.fRange       = 0.0f;
    data.fFace        = msg.face();
    data.pPlayer      = nullptr;

    ZoneService()->GetLoadingThread()->AddLoginPlayer(std::move(data));

    ZoneService()->CreateSocketMessagePool(msg.socket());


    __LEAVE_FUNCTION
}

ON_SERVERMSG(CZoneService, PlayerChangeZone)
{
    __ENTER_FUNCTION

    CHECK(msg.idplayer() != 0);
    CHECK(msg.socket() != 0);
    CHECK(msg.idzone() != 0);
    CHECK(msg.target_scene() != 0);
    CHECK(msg.posx() >= 0);
    CHECK(msg.posy() >= 0);
    CHECK(msg.range() > 0);

    ZoneService()->GetLoadingThread()->CancleWaiting(msg.idplayer());

    ST_LOADINGTHREAD_PROCESS_DATA data;
    data.nPorcessType = LPT_LOADING;
    data.idPlayer     = msg.idplayer();
    data.bChangeZone  = true;
    data.socket       = msg.socket();
    data.idTargetScene = msg.target_scene();
    data.fPosX        = msg.posx();
    data.fPosY        = msg.posy();
    data.fRange       = msg.range();
    data.fFace        = msg.face();
    data.pPlayer      = nullptr;

    ZoneService()->GetLoadingThread()->AddLoginPlayer(std::move(data));
    ZoneService()->CreateSocketMessagePool(msg.socket());

    __LEAVE_FUNCTION
}

ON_SERVERMSG(CZoneService, PlayerChangeZone_Data)
{
    __ENTER_FUNCTION

    CHECK(msg.idplayer() != 0);
    CHECK(msg.socket() != 0);

    ZoneService()->PushMsgToMessagePool(msg.socket(), pMsg);

    __LEAVE_FUNCTION
}

ON_SERVERMSG(CZoneService, PlayerLogout)
{
    __ENTER_FUNCTION

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


    __LEAVE_FUNCTION
}

ON_SERVERMSG(CZoneService, ServiceRegister)
{
    __ENTER_FUNCTION

    ServerPort server_port{msg.serverport()};
    GetMessageRoute()->SetWorldReady(server_port.GetWorldID(), true);
    GetMessageRoute()->ReloadServiceInfo(msg.update_time(), server_port.GetWorldID());


    __LEAVE_FUNCTION
}
