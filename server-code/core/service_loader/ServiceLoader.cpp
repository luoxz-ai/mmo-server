#include "ServiceLoader.h"

#include <dlfcn.h>

#include "BaseCode.h"
#include "IService.h"
#include "LoggingMgr.h"
#include "MemoryHelp.h"
#include "MessageRoute.h"
#include "ObjectHeap.h"

#include "event2/event.h"
#include "event2/thread.h"
//#include <brpc/global.h>
void log_cb(int32_t severity, const char* msg)
{
    LOGNETERROR("libevent：{}", msg);
}

ServiceLoader::ServiceLoader()
{
    event_enable_debug_mode();
    evthread_use_pthreads();
    event_set_log_callback(log_cb);

    CreateMessageRoute();
}

ServiceLoader::~ServiceLoader()
{
    Destory();
}

void ServiceLoader::Destory()
{
    __ENTER_FUNCTION
    LOGMESSAGE("service_loader StartDestory:{}", getpid());
    for(auto it = m_setService.rbegin(); it != m_setService.rend(); it++)
    {
        __ENTER_FUNCTION
        IService* pService = *it;

        auto oldNdc = BaseCode::SetNdc(pService->GetServiceName());
        pService->Release();
        BaseCode::SetNdc(oldNdc);
        __LEAVE_FUNCTION
    }
    m_setService.clear();
    for(auto it = m_setModule.begin(); it != m_setModule.end(); it++)
    {
#ifndef USE_ASAN
        dlclose(it->second);
#endif
    }
    m_setModule.clear();
    LOGMESSAGE("service_loader StopDestory:{}", getpid());
    ReleaseMessageRoute();
    __LEAVE_FUNCTION
}

bool ServiceLoader::_StartService(const std::string& dll_name, WorldID_t idWorld, ServiceType_t idServiceType, ServiceIdx_t idServiceIdx)
{
    __ENTER_FUNCTION
    LOGMESSAGE("_StartService {} World:{} Service:{} {} ", dll_name, idWorld, idServiceType, idServiceIdx);
    if(dll_name.empty())
    {
        // log error
        LOGFATAL("Service In service.xml Need ServiceLib=xxx ");
        return false;
    }
    // load dll/so
    HMODULE hService  = nullptr;
    auto    it_moudle = m_setModule.find(dll_name);
    if(it_moudle == m_setModule.end())
    {
        hService = dlopen(dll_name.c_str(), RTLD_LOCAL | RTLD_NOW); // RTLD_GLOBAL|RTLD_LAZY);
        if(hService == nullptr)
        {
            // log err
            LOGFATAL("dlopen fail:{}:{}", dll_name.c_str(), dlerror());
            return false;
        }
        m_setModule[dll_name] = hService;
    }
    else
    {
        hService = it_moudle->second;
    }

    typedef IService* (*ServiceCreateFunc)(WorldID_t, ServiceType_t, ServiceIdx_t);

    ServiceCreateFunc func = (ServiceCreateFunc)dlsym(hService, "ServiceCreate");
    if(func == nullptr)
    {
        // log err
        LOGFATAL("ServiceCreateFunc null {} fail:{}", idServiceType, dll_name.c_str());
        return false;
    }
    LOGDEBUG("{} : ServiceCreate start ", dll_name);
    IService* pService = func(idWorld, idServiceType, idServiceIdx);
    if(pService)
    {
        m_setService.push_back(pService);
    }
    else
    {
        // log err
        LOGFATAL("ServiceCreate {}-{} fail:{}", idServiceType, idServiceIdx, dll_name.c_str());
        return false;
    }
    float alloced = get_memory_status().allocted / 1024.0f / 1024.0f;
    LOGDEBUG("after {}[{}-{}] memory allocated: {:.2f}M", dll_name.c_str(), idServiceType, idServiceIdx, alloced);
    return true;

    __LEAVE_FUNCTION
    return false;
}

bool ServiceLoader::Load(const std::string& setting_filename, WorldID_t nWorldID, const std::set<ServiceID>& create_service_set)
{
    __ENTER_FUNCTION
    LOGMESSAGE("LoadServiceSetting Start file:{} world:{}", setting_filename, nWorldID);
    //先将所有的Service存储到MessagePort中，这样当Service开启后，收到的ServiceMsg:service_addr如果没有收录就是新Service
    if(GetMessageRoute()->LoadServiceSetting(setting_filename, nWorldID) == false)
    {
        return false;
    }
    LOGDEBUG("start CreateAllMessagePort");
    GetMessageRoute()->CreateAllMessagePort(nWorldID, create_service_set);

    //开启service
    {
        if(create_service_set.empty())
        {
            LOGDEBUG("start Loader AllService");
            //全部启动在本loader上
            bool bSucc = false;
            GetMessageRoute()->ForeachServiceInfoByWorldID(nWorldID, false, [&bSucc, nWorldID, pThis = this](const ServerAddrInfo* pServerAddrInfo) {
                bSucc = pThis->_StartService(pServerAddrInfo->lib_name, nWorldID, pServerAddrInfo->idServiceType, pServerAddrInfo->idServiceIdx);
                if(bSucc == false)
                    return false;
                return true;
            });
            if(bSucc == false)
                return false;
        }
        else
        {
            LOGDEBUG("start Loader create_service_set");
            for(const auto& service_id: create_service_set)
            {
                auto pServerAddrInfo =
                    GetMessageRoute()->QueryServiceInfo(ServerPort(nWorldID, service_id.GetServiceType(), service_id.GetServiceIdx()));
                if(pServerAddrInfo == nullptr)
                {
                    LOGFATAL("World:{} Service:{} {} QueryServiceInfo Error", nWorldID, service_id.GetServiceType(), service_id.GetServiceIdx());
                    continue;
                }

                bool bSucc = _StartService(pServerAddrInfo->lib_name, nWorldID, service_id.GetServiceType(), service_id.GetServiceIdx());
                if(bSucc == false)
                    return false;
            }
        }
    }

    if(m_setService.empty())
    {
        std::cerr << "m_setService.empty()" << std::endl;
        return false;
    }
    return true;

    __LEAVE_FUNCTION
    return false;
}
