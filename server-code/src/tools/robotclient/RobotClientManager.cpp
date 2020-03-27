#include "RobotClientManager.h"

#include <iostream>

#include "NetMSGProcess.h"
#include "NetworkMessage.h"
#include "RobotClient.h"
#include "msg/ts_cmd.pb.h"
#include "pb_luahelper.h"

void export_to_lua(lua_State* L, void* pManager)
{
    RobotClient::initInLua(L);
    lua_tinker::class_add<RobotClientManager>(L, "RobotClientManager");
    lua_tinker::class_def<RobotClientManager>(L, "ConnectServer", &RobotClientManager::ConnectServer);
    lua_tinker::class_def<RobotClientManager>(L, "DelClient", &RobotClientManager::DelClient);
    lua_tinker::class_def<RobotClientManager>(L, "RegisterCMD", &RobotClientManager::RegisterCMD);
    lua_tinker::class_def<RobotClientManager>(L, "GetProcessCMD", &RobotClientManager::GetProcessCMD);
    lua_tinker::set(L, "robot_manager", (RobotClientManager*)pManager);

    pb_luahelper::export_protobuf_enum_to_lua(L, TS_CMD_descriptor());
}

RobotClientManager::RobotClientManager(uint32_t nRobStart, uint32_t nRobAmount)
    : m_pNetMsgProcess(std::make_unique<CNetMSGProcess>())
{
    m_pEventManager.reset( CEventManager::CreateNew(GetEVBase()) );

    m_pScriptManager.reset(CLUAScriptManager::CreateNew("script", export_to_lua, this, "robot_client", false));
    m_pScriptManager->_ExecScript<void>("main", nRobStart, nRobAmount);
    m_pEventManager->ScheduleEvent(
        0,
        [pScriptManager = m_pScriptManager.get()]() {
            // pScriptManager->OnTimer(TimeGetMonotonic());
            pScriptManager->FullGC();
        },
        200,
        true,
        m_Event);
}

RobotClientManager::~RobotClientManager()
{
    Stop();
    JoinIOThread();
    for(RobotClient* pClient: m_setClient)
    {
        SAFE_DELETE(pClient);
    }
    m_setClient.clear();
    Destroy();

}

RobotClient* RobotClientManager::ConnectServer(const char* addr, int32_t port)
{
    RobotClient* pClient = new RobotClient(this);
    if(AsyncConnectTo(addr, port, pClient) == nullptr)
    {
        SAFE_DELETE(pClient);
        return nullptr;
    }
    return pClient;
}

void RobotClientManager::DelClient(RobotClient* pClient)
{
    m_setClient.erase(pClient);
    SAFE_DELETE(pClient);
}
