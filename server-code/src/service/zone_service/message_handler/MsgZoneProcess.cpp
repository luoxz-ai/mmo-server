#include "MsgProcessRegister.h"
#include "ZoneService.h"


void ZoneMessageHandlerRegister()
{
    __ENTER_FUNCTION

    auto pNetMsgProcess = ZoneService()->GetNetMsgProcess();
    for(const auto& [k, v]: MsgProcRegCenter<CZoneService>::instance().m_MsgProc)
    {
        pNetMsgProcess->Register(k, std::get<0>(v), std::get<1>(v));
    }

    __LEAVE_FUNCTION
}
