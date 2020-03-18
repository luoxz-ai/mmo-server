#include "ScriptManager.h"

#include "BaseCode.h"
#include "lua_tinker.h"
#include "MD5.h"
#include "FileUtil.h"

//////////////////////////////////////////////////////////////////////////////////////
// static void *my_l_alloc (void *ud, void *ptr, size_t osize, size_t nsize)
//{
//    (void)osize;
//    if (nsize == 0)
//    {
//        if(ptr == NULL)
//            return NULL;
//		je_free(ptr);
//        return NULL;
//    }
//	else
//	{
//		return je_realloc(ptr, nsize)
//	}
//}

///////////////////////////////////////////////////////////////////////////////////////
static int32_t my_panic(lua_State* L)
{
	(void)L; /* to avoid warnings */
	LOGERROR("PANIC: unprotected error in call to Lua API ({})", lua_tostring(L, -1));
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////
LUALIB_API lua_State* my_luaL_newstate()
{
	// lua_State *L = lua_newstate(my_l_alloc, (void*)pManager);
	lua_State* L = luaL_newstate();
	if(L)
		lua_atpanic(L, &my_panic);
	return L;
}

//为decoda打开lua调试函数
#ifdef _DEBUG
#pragma comment(linker, "/include:_luaL_ref")
#pragma comment(linker, "/include:_luaL_unref")
#pragma comment(linker, "/include:_luaL_newmetatable")
#pragma comment(linker, "/include:_lua_cpcall")
#endif

///////////////////////////////////////////////////////////////////////////////////////
CLUAScriptManager::CLUAScriptManager()
	: m_pLua(nullptr)
	, m_nLuaGCStep(1)
	, m_nLuaGCStepTick(100)
	, m_tNextGCStepTime(0)
{
}

///////////////////////////////////////////////////////////////////////////////////////
CLUAScriptManager::~CLUAScriptManager()
{
	Destory();
}

void InitBaseCodeInLua(lua_State* L)
{
	extern void basecode2lua(lua_State*);
	basecode2lua(L);
}

void LogLuaError(const char* txt)
{
	LOGLUAERROR("{}", txt);
}

void LogLuaDebug(const char* txt)
{
#ifdef DEBUG
	LOGLUADEBUG("{}", txt);
#endif
}

///////////////////////////////////////////////////////////////////////////////////////
bool CLUAScriptManager::Init(const std::string& name, InitRegisterFunc func, void* pInitParam, const char* search_path /*= "script"*/, bool bExecMain)
{
	m_pInitRegisterFunc = func;
	m_pInitParam		= pInitParam;

	if(m_pLua == nullptr)
	{
		m_pLua = my_luaL_newstate();
		CHECKF(m_pLua);

		luaL_openlibs(m_pLua);
		lua_tinker::init(m_pLua);
		lua_tinker::class_add<lua_State>(m_pLua, "lua_State");
		lua_tinker::set(m_pLua, "this_lua", m_pLua);

		lua_tinker::class_add<CLUAScriptManager>(m_pLua, "CLUAScriptManager");
		lua_tinker::class_def<CLUAScriptManager>(m_pLua, "RegistFucName", &CLUAScriptManager::RegistFucName);
		lua_tinker::class_def<CLUAScriptManager>(m_pLua, "RegistFile", &CLUAScriptManager::RegistFile);
		lua_tinker::class_def<CLUAScriptManager>(m_pLua, "LoadFile", &CLUAScriptManager::LoadFile);
		lua_tinker::class_def<CLUAScriptManager>(m_pLua, "LoadFilesInDir", &CLUAScriptManager::LoadFilesInDir);

		lua_tinker::def(m_pLua, "InitBaseCodeInLua", &InitBaseCodeInLua);
		lua_tinker::def(m_pLua, "_ALERT", &LogLuaError);
		lua_tinker::def(m_pLua, "logdebug", &LogLuaDebug);

		lua_tinker::set(m_pLua, "script_manager", this);
	}
	CHECKF(m_pLua);

	if(m_pInitRegisterFunc)
	{
		m_pInitRegisterFunc(m_pLua, m_pInitParam);
	}

	if(search_path)
	{
		lua_tinker::table_onstack table(m_pLua, "package");
		std::string				  load_path = table.get<std::string>("path");
		table.set("path", load_path + ";./" + search_path + "/?.lua");

		lua_tinker::dofile(m_pLua, (std::string(search_path) + "/main.lua").c_str());
		m_search_path = std::string("./") + search_path;
	}
	else
	{
		lua_tinker::dofile(m_pLua, "main.lua");
	}
	// 调用初始化函数
	if(bExecMain)
		lua_tinker::call<void>(m_pLua, "main");

	return true;
}

void CLUAScriptManager::Destory()
{
	if(m_pLua)
	{
		lua_close(m_pLua);
		m_pLua = nullptr;
	}
}

///////////////////////////////////////////////////////////////////////////////////////
void CLUAScriptManager::Reload(const std::string& name, bool bExecMain)
{
	Init(name, m_pInitRegisterFunc, m_pInitParam, m_search_path.c_str(), bExecMain);
}

void CLUAScriptManager::LoadFile(uint64_t idScript, const std::string& filename)
{
	m_Data[idScript].FileName = filename;
	lua_tinker::dofile(m_pLua, filename.c_str());
}

void CLUAScriptManager::LoadFilesInDir(const std::string& dir, bool bRecursive)
{
	//遍历文件夹
	scan_dir(m_search_path, dir, bRecursive, [pThis = this](const std::string& dirname, const std::string& filename) {
		if(GetFileExt(filename) != "lua")
			return;
		try
		{
			uint64_t id = std::stoull(GetFileNameWithoutExt(filename));
			pThis->LoadFile(id, dirname + "/" + filename);
		}
		catch(...)
		{
		}
	});
}

void CLUAScriptManager::RegistFile(uint64_t idScript, const std::string& filename)
{
	//获得当前lua路径
	LOGLUA("register script:%ld {}", idScript, filename.c_str());
	m_Data[idScript].FileName = filename;
}

void CLUAScriptManager::OnTimer(time_t tTick)
{
	if(m_pLua)
	{
		if(tTick > m_tNextGCStepTime)
		{
			m_tNextGCStepTime = tTick + m_nLuaGCStepTick;
			lua_gc(m_pLua, LUA_GCSTEP, m_nLuaGCStep);
		}
	}
}

void CLUAScriptManager::FullGC()
{
	lua_gc(m_pLua, LUA_GCCOLLECT, 0);
}
