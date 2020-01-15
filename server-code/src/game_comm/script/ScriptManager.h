#ifndef script_manager_h__
#define script_manager_h__

#include <map>
#include <unordered_map>

#include "BaseCode.h"
#include "MemoryHeap.h"
#include "fmt/format.h"
#include "lua_tinker.h"

export_lua enum ScriptCallBack {
	SCB_MONSTER_ONBORN = 1,
	SCB_MONSTER_ONATTACK,
	SCB_MONSTER_ONBEATTACK,
	SCB_MONSTER_ONBEKILL,

	SCB_MAP_ONCREATE = 100,
	SCB_MAP_ONENTERMAP,
	SCB_MAP_ONLEAVEMAP,
	SCB_MAP_ONACTORBEKILL,

	SCB_STATUS_ONATTACH = 200,
	SCB_STATUS_ONDETACH,
	SCB_STATUS_ONEFFECT,
	SCB_STATUS_ONSKILL,
	SCB_STATUS_ONATTACK,
	SCB_STATUS_ONBEATTACK,
	SCB_STATUS_ONDEAD,
	SCB_STATUS_ONMOVE,
	SCB_STATUS_ONLEAVEMAP,
	SCB_STATUS_ONLOGIN,
	SCB_STATUS_ONLOGOUT,

	SCB_ITEM_ONUSE			 = 300,
	SCB_ITEM_ONBATCHUSECHECK = 301,
	SCB_ITEM_ONTIMEOUT		 = 302,

	SCB_SKILL_DOINTONE = 400,
	SCB_SKILL_DOLAUNCH = 401,
	SCB_SKILL_DOAPPLY  = 402,
	SCB_SKILL_DOSTUN   = 403,
	SCB_SKILL_ISTARGET = 404,
	SCB_SKILL_DODAMAGE = 405,

	SCB_TASK_CAN_ACCEPT		 = 501,
	SCB_TASK_ON_ACCEPT		 = 502,
	SCB_TASK_CAN_COMMIT		 = 503,
	SCB_TASK_ON_COMMIT		 = 504,
	SCB_TASK_ON_GIVEUP		 = 505,
	SCB_TASK_SHOW_TASKDIALOG = 506,

	SCB_AI_ONUNDERATTACK  = 601,
	SCB_AI_SEARCHENEMY	  = 602,
	SCB_AI_FINDNEXTENEMY  = 603,
	SCB_AI_PROCESS_ATTACK = 604,
	SCB_AI_TO_IDLE		  = 605,
	SCB_AI_TO_GOBACK	  = 606,

	SCB_NPC_ONBORN	 = 700,
	SCB_NPC_ONACTIVE = 701,
};

class CLUAScriptManager
{
public:
	//每个lua被创建时都会调用一下该函数来向lua注册一些必备的c++函数,比如类注册函数等
	typedef void (*InitRegisterFunc)(lua_State* pL, void* pInitParam);

private:
	CLUAScriptManager();

public:
	~CLUAScriptManager();
	CREATE_NEW_IMPL(CLUAScriptManager);
	bool Init(const std::string& name, InitRegisterFunc func, void* pInitParam, const char* search_path = "script", bool bExecMain = true);

	void Destory();
	void Reload(const std::string& name, bool bExecMain);

	void LoadFile(uint64_t idScript, const std::string& filename);
	void LoadFilesInDir(const std::string& dir, bool bRecursive);

	void RegistFile(uint64_t idScript, const std::string& filename);
	void OnTimer(time_t tTick);
	void FullGC();

public:
	int	 GetLuaGCStep() const { return m_nLuaGCStep; }
	void SetLuaGCStep(int val) { m_nLuaGCStep = val; }
	int	 GetLuaGCStepTick() const { return m_nLuaGCStepTick; }
	void SetLuaGCStepTick(int val) { m_nLuaGCStepTick = val; }

public:
	template<typename RVal, typename... Args>
	RVal ExecScript(uint64_t idScript, const char* pszFuncName, Args&&... args)
	{
		__ENTER_FUNCTION
		if(pszFuncName == nullptr)
			return RVal();

		auto itFindMap = m_Data.find(idScript);
		if(itFindMap == m_Data.end())
			return RVal();

		return lua_tinker::call<RVal>(m_pLua, fmt::format(FMT_STRING("x{}_{}"), idScript, pszFuncName).c_str(), std::forward<Args>(args)...);
		__LEAVE_FUNCTION
		return RVal();
	}

	//注册一个函数回调名
	export_lua void RegistFucName(uint64_t idScript, uint32_t idxCallBackType, const std::string& FuncName) { m_Data[idScript].CallBackData[idxCallBackType] = FuncName; }
	export_lua const std::string& QueryFunc(uint64_t idScript, uint32_t idxCallBackType) const
	{
		static const std::string s_Empty;
		auto					 itFindMap = m_Data.find(idScript);
		if(itFindMap == m_Data.end())
			return s_Empty;

		const auto& refData = itFindMap->second;

		auto itFind = refData.CallBackData.find(idxCallBackType);
		if(itFind == refData.CallBackData.end())
			return s_Empty;

		return itFind->second;
	}

	template<typename RVal, typename... Args>
	RVal TryExecScript(uint64_t idScript, uint32_t idxCallBackType, Args&&... args)
	{
		const std::string& funcName = QueryFunc(idScript, idxCallBackType);
		if(funcName.empty())
			return RVal();
		return _ExecScript<RVal>(funcName.c_str(), std::forward<Args>(args)...);
	}

public:
	template<typename RVal, typename... Args>
	RVal _ExecScript(const char* pszFuncName, Args&&... args)
	{
		__ENTER_FUNCTION
		if(pszFuncName == nullptr)
			return RVal();

		return lua_tinker::call<RVal>(m_pLua, pszFuncName, std::forward<Args>(args)...);
		__LEAVE_FUNCTION
		return RVal();
	}

public:
	operator lua_State*() const { return m_pLua; }
	operator lua_State*() { return m_pLua; }

private:
	lua_State* m_pLua;
	int		   m_nLuaGCStep;	  //每次GCStep执行几步GC
	int		   m_nLuaGCStepTick;  //每多少Tick执行1次GCStep
	time_t	   m_tNextGCStepTime; //下1次执行GC的Time

	std::string		 m_search_path;
	InitRegisterFunc m_pInitRegisterFunc;
	void*			 m_pInitParam;

private:
	struct ScriptFileData
	{
		std::string								  FileName;
		std::unordered_map<uint32_t, std::string> CallBackData;
	};
	std::unordered_map<uint64_t, ScriptFileData> m_Data;
};

/*
lua_tinker使用：
因为lua_tinker对于引用的解析有部分问题，可能导致解析为传值，所以def函数时请避免

//向lua中设置一个全局变量,尽量不要,保持脚本干净
lua_tinker::set(L, "g_test", &g_test);
//从lua中获取一个全局变量
int lua_int = lua_tinker::get<int>(L, "lua_int");

//将test函数注册给lua,命名为ctest
lua_tinker::def(L, "ctest", test);

//调用lua中的function test,参数为1,返回void
lua_tinker::call<void>(L, "test", 1);

//向lua中注册一个类LUAScript
lua_tinker::class_add<LUAScript>(m_pLua, "LUAScript");
//向lua中注册一个类LUAScript2,继承自LUAScript
lua_tinker::class_inh<LUAScript2, LUAScript>(L);
//注册类构造函数,一般来说没必要在lua中产生c++对象,因为内存管理不同步,
//可以用在struct上,比如Vector2,Point之类的
lua_tinker::class_con<Vector2>(L,lua_tinker::constructor<int,int>());
//向lua中注册一个类成员函数LUAScript::Reload
lua_tinker::class_def<LUAScript>(L, "Reload", &LUAScript::Reload);
//向lua中注册一个成员变量
lua_tinker::class_mem<LUAScript>(L, "m_pLua", &LUAScript::m_pLua);


*/
/*
注册函数使用:

void InitFreeFunction(lua_State* L)
{
	lua_tinker::def(L, "rand", &rand);
	lua_tinker::def(L, "TimdDiff", &TimdDiff);
	lua_tinker::def(L, "TimeGet", &TimeGet);
}
void InitUser(lua_State* L)
{
	lua_tinker::class_add<CRole>(L, "CRole");
	lua_tinker::class_def<CRole>(L, "Attack", &CRole::Attack);
	lua_tinker::class_add<CUser>(L, "CUser");
	lua_tinker::class_inh<CUser, CRole>(L);
	lua_tinker::class_def<CUser>(L, "Attack", &CUser::Attack);

	typedef void(CUser::*overload_func1)(const char*);
	typedef void(CUser::*overload_func2)(std::string);

	lua_tinker::class_def<CUser>(L, "SetName", (overload_func1)&CUser::SetName);
	lua_tinker::class_def<CUser>(L, "SetNameString", (overload_func2)&CUser::SetName);
}
void InitRegisterFunction(lua_State* L)
{
   lua_tinker::def(L, "InitFreeFunc", &InitFreeFunc);
   lua_tinker::def(L, "InitUser", &InitUser);
   lua_tinker::def(L, "InitNpc", &InitNpc);
   lua_tinker::def(L, "InitZoneService", &InitZoneService);
}

LUAScriptManager::CreateNew(&InitRegisterFunction);

*/
/*
task_example.lua

--当KillNpc时c++会回调该函数,传入的参数为CTaskData*,CUser*,CRole*三个指针
function OnKillNpc(pTask,pUser,pDeadRole)
	if pDeadRole:GetID() == 30113310 then
		local data = pTask:GetData(0);		--所有变量前必须加local作为局部变量，一般情况下不允许使用全局变量
		pTask:SetData(0,  data + 1);		--调用成员函数
		pTask.state = 1;					--设置成员变量state=1
	end
end
*/

#endif // script_manager_h__
