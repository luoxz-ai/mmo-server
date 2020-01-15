#include "ServiceLoader.h"

#include <dlfcn.h>

#include "BaseCode.h"
#include "IService.h"
#include "MemoryHeap.h"
#include "MessageRoute.h"
#include "SettingMap.h"
#include "event2/event.h"
#include "event2/thread.h"
#include "loging_manager.h"
//#include "brpc/global.h"
void log_cb(int severity, const char* msg)
{
	LOGNETERROR("{}", msg);
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
	for(auto it = m_setService.rbegin(); it != m_setService.rend(); it++)
	{
		__ENTER_FUNCTION
		IService* pService = *it;
		pService->Release();
		__LEAVE_FUNCTION
	}
	m_setService.clear();
	for(auto it = m_setModule.begin(); it != m_setModule.end(); it++)
	{
		dlclose(it->second);
	}
	m_setModule.clear();
	LOGMESSAGE("service_loader end:{}", getpid());
	ReleaseMessageRoute();
	__LEAVE_FUNCTION
}

bool ServiceLoader::_StartService(const std::string& dll_name, uint16_t idWorld, uint16_t idService)
{
	__ENTER_FUNCTION
	if(dll_name.empty())
	{
		// log error
		LOGFATAL("Service In service.xml Need ServiceLib=xxx ");
		return false;
	}
	// load dll/so
	HMODULE hService  = nullptr;
	auto	it_moudle = m_setModule.find(dll_name);
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

	typedef IService* (*ServiceCreateFunc)(uint16_t, uint16_t);

	ServiceCreateFunc func	   = (ServiceCreateFunc)dlsym(hService, "ServiceCreate");
	IService*		  pService = func(idWorld, idService);
	if(pService)
	{
		m_setService.push_back(pService);
	}
	else
	{
		// log err
		LOGFATAL("ServiceCreate {} fail:{}", idService, dll_name.c_str());
		return false;
	}

	LOGDEBUG("after {}[{}] memory allocated: {}", dll_name.c_str(), idService, get_memory_status().allocted);
	return true;
	__LEAVE_FUNCTION
	return false;
}

bool ServiceLoader::Load(const std::string& setting_filename, uint16_t nWorldID, const std::set<uint16_t>& create_service_set)
{

	//先将所有的Service存储到MessagePort中，这样当Service开启后，收到的ServiceMsg:service_addr如果没有收录就是新Service
	if(GetMessageRoute()->LoadServiceSetting(setting_filename, nWorldID) == false)
	{
		return false;
	}

	GetMessageRoute()->CreateAllMessagePort(nWorldID, create_service_set);

	//开启service
	{
		if(create_service_set.empty())
		{
			//全部启动在本loader上
			bool bSucc = false;
			GetMessageRoute()->ForeachServiceInfoByWorldID(nWorldID, false, [&bSucc, nWorldID, pThis = this](const ServerAddrInfo* pServerAddrInfo) {
				bSucc = pThis->_StartService(pServerAddrInfo->lib_name, nWorldID, pServerAddrInfo->idService);
				if(bSucc == false)
					return false;
				return true;
			});
			if(bSucc == false)
				return false;
		}
		else
		{
			for(uint16_t idService: create_service_set)
			{
				auto pServerAddrInfo = GetMessageRoute()->QueryServiceInfo(ServerPort(nWorldID, idService));
				if(pServerAddrInfo == nullptr)
				{
					LOGFATAL("World:{} Service:{} QueryServiceInfo Error", nWorldID, idService);
					continue;
				}
				bool bSucc = _StartService(pServerAddrInfo->lib_name, nWorldID, idService);
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
}
