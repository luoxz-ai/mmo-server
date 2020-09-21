#include "User.h"

#include "Account.h"
#include "MapData.h"
#include "MapManager.h"
#include "WorldService.h"
#include "msg/world_service.pb.h"
#include "server_msg/server_side.pb.h"

OBJECTHEAP_IMPLEMENTATION(CUser, s_heap);

CUser::CUser() {}
CUser::~CUser() {}

bool CUser::Init(CAccount* pAccount, ST_ROLE_INFO* pInfo)
{
    //查询角色信息
    CHECKF(pAccount);
    CHECKF(pInfo);
    m_pAccount = pAccount;
    m_pInfo    = pInfo;
    m_nLev     = m_pInfo->GetLev();
    m_idTeam   = m_pInfo->GetTeamID();
    m_idGuild  = m_pInfo->GetGuildID();
    return true;
}

const VirtualSocket& CUser::GetSocket() const
{
    return m_pAccount->GetSocket();
}

void CUser::EnterZone()
{
    __ENTER_FUNCTION
    LOGLOGIN("UserEnterZone:{}", GetID());

    SceneIdx idRecordScene = m_pInfo->GetRecordSceneIdx();
    uint64_t idTargetScene = idRecordScene;
    //通过地图id查找玩家所在的Zone
    uint16_t idZone = idRecordScene.GetZoneID();
    if(idZone == 0)
    {
        auto pMap = MapManager()->QueryMap(idRecordScene.GetMapID());
        CHECK(pMap);
        idZone = pMap->GetZoneID();
    }
    if(idZone == 0)
    {
        SceneIdx idHomeScene = m_pInfo->GetHomeSceneIdx();
        idTargetScene        = idHomeScene;
        idZone               = idHomeScene.GetZoneID();
        if(idZone == 0)
        {
            auto pMap = MapManager()->QueryMap(idHomeScene.GetMapID());
            CHECK(pMap);
            idZone = pMap->GetZoneID();
        }
    }

    CHECK(idZone != 0);

    //通知对应的Zone开始Loading玩家数据
    {
        ServerMSG::PlayerEnterZone msg;
        msg.set_idplayer(GetID());
        msg.set_socket(m_pAccount->GetSocket());
        msg.set_target_scene_idx(idTargetScene);
        msg.set_posx(m_pInfo->GetRecordPosX());
        msg.set_posy(m_pInfo->GetRecordPosY());
        msg.set_face(m_pInfo->GetRecordFace());

        ServerPort zone_port(WorldService()->GetWorldID(), SCENE_SERVICE, idZone);
        WorldService()->SendProtoMsgToZonePort(zone_port,  msg);
    }

    //通知Socket，玩家消息指向新的Zone
    {
        ServerMSG::SocketChangeDest sock_msg;
        sock_msg.set_destport(ServerPort(WorldService()->GetWorldID(), SCENE_SERVICE, idZone));
        sock_msg.set_vs(GetSocket());

        WorldService()->SendProtoMsgToZonePort(GetSocket().GetServerPort(), sock_msg);
    }

    m_idZone = idZone;
    __LEAVE_FUNCTION
}

void CUser::OnChangeZone(uint16_t idZone)
{
    __ENTER_FUNCTION
    LOGLOGIN("UserChangeZone:{}", GetID());

    //通知Socket，玩家消息指向新的Zone
    {
        ServerMSG::SocketChangeDest sock_msg;
        sock_msg.set_destport(ServerPort(WorldService()->GetWorldID(), SCENE_SERVICE, idZone));
        sock_msg.set_vs(GetSocket());

        WorldService()->SendProtoMsgToZonePort(GetSocket().GetServerPort(), sock_msg);
    }

    m_idZone = idZone;
    __LEAVE_FUNCTION
}

void CUser::Logout()
{
    __ENTER_FUNCTION
    LOGLOGIN("UserLogout:{}", GetID());

    //通知对应的Zone 卸载对应的Player
    {
        ServerMSG::PlayerLogout msg;
        msg.set_idplayer(GetID());
        msg.set_socket(GetSocket());
        ServerPort zone_port(WorldService()->GetWorldID(), SCENE_SERVICE, m_idZone);
        WorldService()->SendProtoMsgToZonePort(zone_port,  msg);
    }
    //通知Socket， 玩家消息指向回World
    {
        ServerMSG::SocketChangeDest sock_msg;
        sock_msg.set_destport(WorldService()->GetServerPort());
        sock_msg.set_vs(GetSocket());

        WorldService()->SendProtoMsgToZonePort(GetSocket().GetServerPort(), sock_msg);
    }
    __LEAVE_FUNCTION
}

bool CUser::SendMsg(const proto_msg_t& msg)
{
    return WorldService()->SendMsgToVirtualSocket(GetSocket(), msg);
}

uint32_t CUser::GetLev() const
{
    return m_nLev;
}

const std::string& CUser::GetName() const
{
    return m_pInfo->GetName();
}

uint64_t CUser::GetTeamID() const
{
    return m_idTeam;
}

uint64_t CUser::GetGuildID() const
{
    return m_idGuild;
}

bool CUser::ChangeName(const std::string& new_name)
{
    return true;
}

void CUser::OnLevChg(uint32_t v)
{
    m_nLev = v;
}
void CUser::ClearMate()
{
    m_pInfo->ClearMate();
    // send msg to zone
}

OBJID CUser::GetID() const
{
    return m_pInfo->GetID();
}

uint16_t CUser::GetZoneID() const
{
    return m_idZone;
}
