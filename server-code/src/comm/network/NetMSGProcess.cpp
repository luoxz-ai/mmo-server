#include "NetMSGProcess.h"

#include "LoggingMgr.h"

void CNetMSGProcess::Process(CNetworkMessage* pMsg, bool bLogNoPorcess) const
{
    __ENTER_FUNCTION
    CHECK(pMsg);
    auto itFind = m_FuncMap.find(pMsg->GetCmd());
    if(itFind == m_FuncMap.end())
    {
        if(m_funcDefault)
        {
            return m_funcDefault(pMsg->GetCmd(), pMsg);
        }
        if(bLogNoPorcess)
        {
            LOGERROR("CMD {} didn't have ProcessHandler", pMsg->GetCmd());
        }
        return;
    }

    return (itFind->second)(pMsg);
    __LEAVE_FUNCTION
}
