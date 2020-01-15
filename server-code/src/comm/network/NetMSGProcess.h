#ifndef NETMSGPROCESS_H
#define NETMSGPROCESS_H

#include <functional>
#include <unordered_map>

#include "NetworkDefine.h"
#include "NetworkMessage.h"
class CNetMSGProcess
{
public:
	CNetMSGProcess()  = default;
	~CNetMSGProcess() = default;

public:
	using MessageHandler = std::function<void(CNetworkMessage*)>;

	void Register(uint16_t cmd, MessageHandler&& func);
	void Process(CNetworkMessage* pMsg, bool bLogNoPorcess = true) const;

private:
	std::unordered_map<uint16_t, MessageHandler> m_FuncMap;
};
#endif /* NETMSGPROCESS_H */
