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

export_lua enum SYNC_TYPE { SYNC_FALSE, SYNC_TRUE, SYNC_ALL, SYNC_ALL_DELAY };
export_lua constexpr bool UPDATE_TRUE  = true;
export_lua constexpr bool UPDATE_FALSE = false;


template<class T>
void SAFE_DELETE(T*& p)
{
	if(p)
	{
		delete p;
		p = nullptr;
	}
}
template<class T>
void SAFE_DELETE_ARRAY(T*& p)
{
	if(p)
	{
		delete[] p;
		p = nullptr;
	}
}
template<class T>
void SAFE_RELEASE(T*& p)
{
	if(p)
	{
		p->Release();
		p = nullptr;
	}
}
template<class T, size_t N>
constexpr inline size_t sizeOfArray(T (&array)[N])
{
	return N;
}

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

template<class... Args>
struct type_list
{
	template<std::size_t N>
	using type = typename std::tuple_element<N, std::tuple<Args...>>::type;
};

class scope_guards
{
public:
	scope_guards()					  = default;
	scope_guards(const scope_guards&) = delete;
	void operator=(const scope_guards&) = delete;
	scope_guards(scope_guards&&)		= delete;
	void operator=(scope_guards&&) = delete;

	template<class Callable>
	scope_guards& operator+=(Callable&& undo_func)
	{
		m_data.emplace_front(std::forward<Callable>(undo_func));
		return *this;
	}

	~scope_guards()
	{
		for(auto& f: m_data)
		{
			try
			{
				f(); // must not throw
			}
			catch(...)
			{
			}
		}
	}

	void dismiss() noexcept { m_data.clear(); }

private:
	std::deque<std::function<void()>> m_data;
};






#endif // BaseCode_h__
