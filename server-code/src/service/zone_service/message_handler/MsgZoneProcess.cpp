#include "MsgZoneProcess.h"

#include <regex>

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
    ZoneService()
        ->GetLoadingThread()
        ->AddLoginPlayer(msg.idplayer(), msg.socket(), false, msg.idscene(), msg.posx(), msg.posy(), 0.0f, msg.face());

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

    ZoneService()->GetLoadingThread()->AddLoginPlayer(msg.idplayer(),
                                                      msg.socket(),
                                                      true,
                                                      msg.idscene(),
                                                      msg.posx(),
                                                      msg.posy(),
                                                      msg.range(),
                                                      msg.face());

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

    CPlayer* pPlayer = pActor->ConvertToDerived<CPlayer>();
    pPlayer->OnLogout();
}

void ZoneMessageHandlerRegister()
{
    __ENTER_FUNCTION

    auto pNetMsgProcess = ZoneService()->GetNetMsgProcess();
    for(const auto& [k,v] : ZoneMsgProcRegCenter::instance().m_MsgProc)
    {
        pNetMsgProcess->Register(k,v);
    }
    
    __LEAVE_FUNCTION
}
