#ifndef IService_h__
#define IService_h__

class IService
{
public:
	IService() {}
	virtual ~IService() {}
	virtual void Release() = 0;
};

#endif // IService_h__
