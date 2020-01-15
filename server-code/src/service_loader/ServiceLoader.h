#ifndef ServiceLoader_h__
#define ServiceLoader_h__

#include <map>
#include <set>
#include <string>
#include <vector>

#include <dlfcn.h>

#include "BaseType.h"
class IService;
class ServiceLoader
{
  public:
	ServiceLoader();
	~ServiceLoader();

	bool Load(const std::string& setting_filename, uint16_t nWorldID, const std::set<uint16_t>& create_service_set);
	void Destory();
	bool _StartService(const std::string& dll_name, uint16_t idWorld, uint16_t idService);

  private:
	std::vector<IService*>		   m_setService;
	std::map<std::string, HMODULE> m_setModule;
};
#endif // ServiceLoader_h__