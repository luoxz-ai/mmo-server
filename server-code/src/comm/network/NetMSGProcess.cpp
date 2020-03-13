#include "NetMSGProcess.h"

#include "LoggingMgr.h"

void CNetMSGProcess::Register(uint16_t cmd, MessageHandler&& func)
{
	__ENTER_FUNCTION
	m_FuncMap.emplace(cmd, std::move(func));
	__LEAVE_FUNCTION
}

void CNetMSGProcess::Process(CNetworkMessage* pMsg, bool bLogNoPorcess) const
{
	__ENTER_FUNCTION
	CHECK(pMsg);
	auto itFind = m_FuncMap.find(pMsg->GetCmd());
	if(itFind == m_FuncMap.end())
	{
		if(bLogNoPorcess)
			LOGERROR("CMD {} didn't have ProcessHandler", pMsg->GetCmd());
		return;
	}

	return (itFind->second)(pMsg);
	__LEAVE_FUNCTION
}
