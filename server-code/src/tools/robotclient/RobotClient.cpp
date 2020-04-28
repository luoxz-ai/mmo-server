#include "RobotClient.h"

#include "NetMSGProcess.h"
#include "ProtobuffUtil.h"
#include "RobotClientManager.h"
#include "msg/ts_cmd.pb.h"
#include "msg/world_service.pb.h"
#include "msg/zone_service.pb.h"
#include "pb_luahelper.h"
#include "NetServerSocket.h"

RobotClient::RobotClient(RobotClientManager* pManager)
    : m_pManager(pManager)
    , m_pServerSocket(nullptr)
    , m_idClient(0)
{
}

RobotClient::~RobotClient()
{
    if(m_pServerSocket)
        m_pServerSocket->Interrupt();
}

void RobotClient::initInLua(struct lua_State* L)
{
    lua_tinker::class_add<RobotClient>(L, "RobotClient");
    lua_tinker::class_def<RobotClient>(L, "IsConnectServer", &RobotClient::IsConnectServer);
    lua_tinker::class_def<RobotClient>(L, "DisconnectServer", &RobotClient::DisconnectServer);
    lua_tinker::class_def<RobotClient>(L, "GetClientID", &RobotClient::GetClientID);
    lua_tinker::class_def<RobotClient>(L, "SetClientID", &RobotClient::SetClientID);
    lua_tinker::class_def<RobotClient>(L, "SendProtobufToServer", &RobotClient::SendProtobufToServer);
    lua_tinker::class_def<RobotClient>(L, "AddEventCallBack", &RobotClient::AddEventCallBack);

    pb_luahelper::init_lua(L);
}

void RobotClient::OnConnected(CNetSocket* pSocket)
{
    m_pServerSocket = pSocket;

    // call lua
}

void RobotClient::OnConnectFailed(CNetSocket*)
{
    LOGDEBUG("RobotClient::OnConnectFailed");
}

void RobotClient::OnDisconnected(CNetSocket*)
{
    m_pServerSocket = nullptr;

    m_pManager->ExecScript<void>("OnDisconnected", this);
}

void RobotClient::OnAccepted(CNetSocket*) {}

void RobotClient::OnRecvData(CNetSocket* pSocket, byte* pBuffer, size_t len)
{
    MSG_HEAD* pHeader = (MSG_HEAD*)pBuffer;
    switch(pHeader->usCmd)
    {
        case CMD_INTERRUPT:
        {
            LOGDEBUG("INITATIVE_CLOSE:{}", m_idClient);
            static_cast<CServerSocket*>(pSocket)->SetReconnect(false);
            DisconnectServer();
        }
        break;
        case CMD_SC_KEY:
        {
            SC_KEY msg;
            if(msg.ParseFromArray(pBuffer + sizeof(MSG_HEAD), len - sizeof(MSG_HEAD)) == false)
            {
                LOGERROR("ParseFromArray Fail:{}", CMD_SC_KEY);
            }

            pSocket->InitEncryptor(msg.key());
            LOGMESSAGE("recv SC_KEY");
            m_pManager->ExecScript<void>("OnConnect", this);
        }
        break;
        default:
        {
            // m_pManager->GetNetMessageProcess()->Process(pHeader->usCmd, pBuffer + sizeof(MSG_HEAD), len -
            // sizeof(MSG_HEAD), VirtualSocket(0, pSocket->GetSocketIdx()), 0, 0);
            std::string func_name = m_pManager->GetProcessCMD(pHeader->usCmd);
            if(func_name.empty() == false)
            {
                LOGDEBUG("process net_msg:{}", pHeader->usCmd);
                m_pManager->ExecScript<void>(func_name.c_str(),
                                             this,
                                             pBuffer + sizeof(MSG_HEAD),
                                             len - sizeof(MSG_HEAD));
                // m_pManager->GetScriptManager()->FullGC();
            }
            else
            {

                LOGDEBUG("not find processer net_msg:{}", pHeader->usCmd);
            }
        }
        break;
    }
}

void RobotClient::OnProcessMessage(CNetworkMessage*) {}

void RobotClient::OnRecvTimeout(CNetSocket*) {}

void RobotClient::AddEventCallBack(uint32_t nWaitMs, const std::string& func_name, bool bPersist)
{
    m_pManager->GetEventManager()->ScheduleEvent(
        0,
        [func_name, pThis = this]() { pThis->m_pManager->ExecScript<void>(func_name.c_str(), pThis); },
        nWaitMs,
        bPersist,
        m_Event);
}

bool RobotClient::IsConnectServer()
{
    return m_pServerSocket != nullptr;
}

void RobotClient::DisconnectServer()
{
    if(m_pServerSocket)
        m_pServerSocket->Interrupt();
    m_pManager->DelClient(this);
}

void RobotClient::SendToServer(CNetworkMessage& msg)
{
    if(m_pServerSocket)
        m_pServerSocket->SendSocketMsg(msg.GetBuf(), msg.GetSize());
}

void RobotClient::SendToServer(byte* buf, size_t len)
{
    if(m_pServerSocket)
        m_pServerSocket->SendSocketMsg(buf, len);
}

void RobotClient::SendProtobufToServer(uint16_t cmd, google::protobuf::Message* pMsg)
{
    CHECK(cmd != 0);
    CHECK(pMsg);
    CNetworkMessage _msg(cmd, *pMsg);
    SendToServer(_msg);
}
