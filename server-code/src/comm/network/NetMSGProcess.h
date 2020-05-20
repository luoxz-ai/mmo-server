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
    using MessageHandler        = std::function<void(CNetworkMessage*)>;
    using DefaultMessageHandler = std::function<void(uint32_t cmd, CNetworkMessage*)>;
    template<class Func>
    void Register(uint16_t cmd, Func&& func)
    {
        m_FuncMap.emplace(cmd, std::forward<Func>(func));
        LOGNETDEBUG("RegisterMsgProc:{}", cmd);
    }

    template<class Func>
    void Register(uint16_t cmd, const char* cmd_name, Func&& func)
    {
        m_FuncMap.emplace(cmd, std::forward<Func>(func));
        LOGNETDEBUG("RegisterMsgProc:{}", cmd_name);
    }

    bool Process(CNetworkMessage* pMsg) const;
    void SetDefaultFunc(DefaultMessageHandler&& func) { m_funcDefault = std::move(func); }

private:
    std::unordered_map<uint16_t, MessageHandler> m_FuncMap;
    DefaultMessageHandler                        m_funcDefault;
};
#endif /* NETMSGPROCESS_H */
