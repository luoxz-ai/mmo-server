#include "MsgZoneProcess.h"

#include "Player.h"
#include "ZoneService.h"

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
