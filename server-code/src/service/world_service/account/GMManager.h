#pragma once
#include "BaseCode.h"

class CGMManager
{
	CGMManager();
public:
	~CGMManager();
	CREATE_NEW_IMPL(CGMManager);
	bool Init();
	void Destory();
	void Reload();

	uint32_t GetGMLevel(const std::string& openid) const;
private:
	std::map<std::string, uint32_t> m_GMList;
};