#include "MsgZoneProcess.h"

#include <regex>

#include "Player.h"
#include "ZoneService.h"
#include "msg/ts_cmd.pb.h"
#include "msg/zone_service.pb.h"

//////////////////////////////////////////////////////////////////////////
void OnMsg_PlayerEnterZone(const ServerMSG::PlayerEnterZone& msg, CNetworkMessage* pMsg)
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

void OnMsg_PlayerChangeZone(const ServerMSG::PlayerChangeZone& msg, CNetworkMessage* pMsg)
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

void OnMsg_PlayerChangeZone_Data(const ServerMSG::PlayerChangeZone_Data& msg, CNetworkMessage* pMsg)
{
    CHECK(msg.idplayer() != 0);
    CHECK(msg.socket() != 0);

    ZoneService()->PushMsgToMessagePool(msg.socket(), pMsg);
}

void OnMsg_PlayerLogout(const ServerMSG::PlayerLogout& msg, CNetworkMessage* pMsg)
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

    ZoneItemMessageHandlerRegister();
    ZoneMapMessageHandlerRegister();
    ZoneSkillMessageHandlerRegister();
    ZoneTaskMessageHandlerRegister();
    ZoneTeamMessageHandlerRegister();
    ZonePlayerMessageHandlerRegister();

    auto pNetMsgProcess = ZoneService()->GetNetMsgProcess();
#define REGISTER_SERVERMSG(MsgT) \
    pNetMsgProcess->Register(    \
        ServerMSG::MsgID_##MsgT, \
        std::bind(&ProcessMsg<ServerMSG::MsgT, decltype(OnMsg_##MsgT)>, std::placeholders::_1, &OnMsg_##MsgT));

    REGISTER_SERVERMSG(PlayerEnterZone);
    REGISTER_SERVERMSG(PlayerChangeZone);
    REGISTER_SERVERMSG(PlayerChangeZone_Data);
    REGISTER_SERVERMSG(PlayerLogout);

#undef REGISTER_MSG

    __LEAVE_FUNCTION
}
