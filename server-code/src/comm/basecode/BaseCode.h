#ifndef BaseCode_h__
#define BaseCode_h__

#include <chrono>
#include <cstdio>
#include <cstring>
#include <deque>
#include <exception>
#include <functional>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <utility>

#include <dirent.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <unistd.h>

#include "BaseType.h"
#include "GameMath.h"
#include "StringAlgo.h"
#include "export_lua.h"
#include "LoggingMgr.h"
#include "TimeUtil.h"
#include "CheckUtil.h"
#include "IntUtil.h"
#include "Misc.h"

export_lua enum SYNC_TYPE { SYNC_FALSE, SYNC_TRUE, SYNC_ALL, SYNC_ALL_DELAY };
export_lua constexpr bool UPDATE_TRUE  = true;
export_lua constexpr bool UPDATE_FALSE = false;

#define CREATE_NEW_IMPL(Type)                                   \
	template<typename... Args>                                  \
	static inline Type* CreateNew(Args&&... args)               \
	{                                                           \
		Type* newT = nullptr;                                   \
		__ENTER_FUNCTION                                        \
		{                                                       \
			newT = new Type();                                  \
			if(newT && newT->Init(std::forward<Args>(args)...)) \
			{                                                   \
				return newT;                                    \
			}                                                   \
		}                                                       \
		__LEAVE_FUNCTION                                        \
		SAFE_DELETE(newT);                                      \
		return nullptr;                                         \
	}




#endif // BaseCode_h__
