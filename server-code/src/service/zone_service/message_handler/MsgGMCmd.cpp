#include "GMManager.h"
#include "Player.h"

struct GMCMDRegister
{
	GMCMDRegister(const std::string& cmd, CGMManager::GMCmdHandle&& handle)
	{
		s_DataMap.emplace(cmd, std::move(handle));
	}
	static std::unordered_map<std::string, CGMManager::GMCmdHandle> s_DataMap;
};
std::unordered_map<std::string, CGMManager::GMCmdHandle> GMCMDRegister::s_DataMap;

#define DEFINE_GMCMD(MsgType)                                                                                  \
	static void OnGMCMD_##MsgType(CPlayer* pPlayer, const std::vector<std::string>& vecCMD); \
	static GMCMDRegister s_GMCMDRegister_##MsgType(#MsgType, &OnGMCMD_##MsgType);     \
	void OnGMCMD_##MsgType(CPlayer* pPlayer, const std::vector<std::string>& vecCMD)

DEFINE_GMCMD(help)
{
	__ENTER_FUNCTION
	__LEAVE_FUNCTION
}

DEFINE_GMCMD(additem)
{
	__ENTER_FUNCTION
	CHECK(pPlayer);
	if(vecCMD.size() < 3)
		return;

	uint32_t idItemType = std::stoul(vecCMD[1]);
	uint32_t nCount		= std::stoul(vecCMD[2]);
	uint32_t dwFlag		= 0;
	if(vecCMD.size() >= 4)
		dwFlag = std::stoul(vecCMD[3]);
	pPlayer->AwardItem(0, idItemType, nCount, dwFlag);
	__LEAVE_FUNCTION
}

DEFINE_GMCMD(chgmap)
{
	__ENTER_FUNCTION
	CHECK(pPlayer);
	if(vecCMD.size() < 3)
		return;

	uint32_t idMapID = std::stoul(vecCMD[1]);
	uint32_t posx	 = std::stoul(vecCMD[2]);
	uint32_t posy	 = std::stoul(vecCMD[3]);

	pPlayer->FlyMap(idMapID, posx, posy, 0, 0);
	__LEAVE_FUNCTION
}

void CGMManager::GMCmdHandlerRegister()
{
	for(auto& [k,v] : GMCMDRegister::s_DataMap)
	{
		RegisterGMCmd(k, std::move(v));
		v = nullptr;
	}
	GMCMDRegister::s_DataMap.clear();
}