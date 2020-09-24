#include "RobotClient.h"

#include "NetMSGProcess.h"
#include "NetServerSocket.h"
#include "ProtobuffUtil.h"
#include "RobotClientManager.h"
#include "msg/ts_cmd.pb.h"
#include "msg/world_service.pb.h"
#include "msg/zone_service.pb.h"
#include "pb_luahelper.h"

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
                m_pManager->ExecScript<void>(func_name.c_str(), this, pBuffer + sizeof(MSG_HEAD), len - sizeof(MSG_HEAD));
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
    CEventEntryCreateParam param;
    param.evType = 0;
    param.cb     = [func_name, pThis = this]() {
        pThis->m_pManager->ExecScript<void>(func_name.c_str(), pThis);
    };
    param.tWaitTime = nWaitMs;
    param.bPersist  = bPersist;
    m_pManager->GetEventManager()->ScheduleEvent(param, m_Event);
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

void RobotClient::SendToServer(const CNetworkMessage& msg)
{
    if(m_pServerSocket)
        m_pServerSocket->SendNetworkMessage(msg);
}

inline uint32_t to_cmd(const proto_msg_t& msg)
{
    __ENTER_FUNCTION
    auto msg_desc      = msg.GetDescriptor();
    auto options       = msg_desc->options();
    auto cmd_extension = options.GetExtension(cmd);
    if(cmd_extension.ts_cmd() != 0)
    {
        CHECK_DEBUG("CMD_" + msg_desc->name() == TS_CMD_Name(cmd_extension.ts_cmd()));
        return cmd_extension.ts_cmd();
    }
    if(cmd_extension.sc_cmd() != 0)
    {
        CHECK_DEBUG("CMD_" + msg_desc->name() == SC_CMD_Name(cmd_extension.sc_cmd()));
        return cmd_extension.sc_cmd();
    }
    if(cmd_extension.cs_cmd() != 0)
    {
        CHECK_DEBUG("CMD_" + msg_desc->name() == CS_CMD_Name(cmd_extension.cs_cmd()));
        return cmd_extension.cs_cmd();
    }

    __LEAVE_FUNCTION
    return 0;
}

void RobotClient::SendProtobufToServer(proto_msg_t* pMsg)
{
    CHECK(pMsg);
    CNetworkMessage _msg(to_cmd(*pMsg), *pMsg);
    SendToServer(_msg);
}
