#ifndef IService_h__
#define IService_h__

#include <string>

class IService
{
public:
    IService() {}
    virtual ~IService() {}
    virtual void Release() = 0;
    virtual const std::string& GetServiceName() const = 0;
};

#endif // IService_h__
