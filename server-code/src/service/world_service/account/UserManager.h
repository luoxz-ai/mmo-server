#pragma once

#include <unordered_map>
#include "BaseCode.h"
#include "NetworkDefine.h"
class CUser;
class CNetworkMessage;

class CUserManager
{
public:
	CUserManager();
	~CUserManager();


	CUser* QueryUser(OBJID uid);
	CUser* QueryUserBySocket(const VirtualSocket& vs);
	CUser* CreateUser(class CAccount* pAccount, struct ST_ROLE_INFO* pInfo);
	void RemoveUser(CUser*& pRemoveUser);
	void Destory();
public:

private:
	std::unordered_map<OBJID, CUser*> m_setUser;
	std::unordered_map<VirtualSocket, CUser*> m_setUserBySocket;

};