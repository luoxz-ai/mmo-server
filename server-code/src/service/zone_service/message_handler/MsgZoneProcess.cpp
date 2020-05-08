#include "MsgProcessRegister.h"
#include "ZoneService.h"
#include "proto_help.h"

void ZoneMessageHandlerRegister()
{
    __ENTER_FUNCTION

    auto pNetMsgProcess = ZoneService()->GetNetMsgProcess();
    for(const auto& [k, v]: MsgProcRegCenter<CZoneService>::instance().m_MsgProc)
    {
        pNetMsgProcess->Register(k, v, cmd_to_enum_name(k));
    }

    __LEAVE_FUNCTION
}
