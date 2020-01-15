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

#include <dirent.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <unistd.h>

#include "BaseType.h"
#include "GameMath.h"
#include "StringAlgo.h"
#include "export_lua.h"
#include "loging_manager.h"

export_lua enum SYNC_TYPE { SYNC_FALSE, SYNC_TRUE, SYNC_ALL, SYNC_ALL_DELAY };

export_lua inline bool HasFlag(uint32_t flag, uint32_t mask)
{
	return (flag & mask) != 0;
}

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

export_lua inline int64_t MakeINT64(int32_t left, int32_t right)
{
	return static_cast<int64_t>(left) << 32 | static_cast<int64_t>(right);
}

export_lua inline uint64_t MakeUINT64(uint32_t left, uint32_t right)
{
	return static_cast<uint64_t>(left) << 32 | static_cast<uint64_t>(right);
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

inline FILE* fopen_s(FILE** fp, const char* path, const char* mode)
{
	if(fp == nullptr)
		return nullptr;
	*fp = fopen(path, mode);
	return *fp;
}

export_lua inline int isleap(unsigned int year)
{
	return ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0));
}

export_lua inline BYTE toHex(const BYTE& x)
{
	return x > 9 ? x + 55 : x + 48;
}

#define CHECK(x)                     \
	{                                \
		if(!(x))                     \
		{                            \
			LOGASSERT("ASSERT:" #x); \
			return;                  \
		}                            \
	}
#define CHECKF(x)                    \
	{                                \
		if(!(x))                     \
		{                            \
			LOGASSERT("ASSERT:" #x); \
			return 0;                \
		}                            \
	}
#define CHECKFR(x, r)                \
	{                                \
		if(!(x))                     \
		{                            \
			LOGASSERT("ASSERT:" #x); \
			return r;                \
		}                            \
	}

#define CHECK_M(x, msg)                             \
	{                                               \
		if(!(x))                                    \
		{                                           \
			LOGASSERT("ASSERT:" #x " msg:{}", msg); \
			return;                                 \
		}                                           \
	}
#define CHECKF_M(x, msg)                            \
	{                                               \
		if(!(x))                                    \
		{                                           \
			LOGASSERT("ASSERT:" #x " msg:{}", msg); \
			return 0;                               \
		}                                           \
	}
#define CHECKFR_M(x, r, msg)                        \
	{                                               \
		if(!(x))                                    \
		{                                           \
			LOGASSERT("ASSERT:" #x " msg:{}", msg); \
			return r;                               \
		}                                           \
	}

#define __ENTER_FUNCTION \
	{                    \
		try              \
		{
#define __LEAVE_FUNCTION                                                               \
	}                                                                                  \
	catch(const std::runtime_error& e) { LOGASSERT("catch_execpetion:{}", e.what()); } \
	catch(const std::exception& e) { LOGASSERT("catch_execpetion:{}", e.what()); }     \
	catch(...) { LOGASSERT("catch_error"); }                                           \
	}

// a*b/c
export_lua int MulDiv(int a, int b, int c);

export_lua inline uint32_t MAKE32(uint32_t a, uint32_t b)
{
	return (a << 16) | (b & 0xFFFF);
}

export_lua inline uint64_t MAKE64(uint64_t a, uint64_t b)
{
	return (a << 32) | (b & 0xFFFFFFFF);
}

export_lua inline uint32_t GetHighFromU64(uint64_t v)
{
	return (v >> 32) & 0xFFFFFFFF;
}

export_lua inline uint32_t GetLowFromU64(uint64_t v)
{
	return (v)&0xFFFFFFFF;
}

//十六进制get/set
export_lua uint32_t hex_set(uint32_t dwFlag, int nHex, UCHAR ucVal);
export_lua UCHAR	hex_get(uint32_t dwFlag, int nHex);

//位操作
export_lua bool		bit_test(uint32_t dwFlag, int nBit);
export_lua uint32_t bit_flip(uint32_t dwFlag, int nBit);
export_lua uint32_t bit_set(uint32_t dwFlag, int nBit, bool bVal);

void TimeGetCacheCreate();
void TimeGetCacheUpdate();

export_lua time_t TimeGetMonotonic();
export_lua time_t TimeGetMillisecond();
export_lua time_t TimeGetSecond();
export_lua time_t TimeGetSecondLocal();
export_lua time_t _TimeGetMonotonic();
export_lua time_t _TimeGetSecond();
export_lua time_t _TimeGetMillisecond();
export_lua time_t _TimeGetSecondLocal();

export_lua time_t gmt2local(time_t tNow);
export_lua time_t local2gmt(time_t tNow);

//检查是否是同一天,loclatime
export_lua bool CheckSameDay(time_t time1, time_t time2);
//返回两个时间戳之间的日期差
export_lua int	  DateDiffLocal(time_t time1, time_t time2);
export_lua time_t GetNextDayBeginTime();

export_lua time_t GetTimeFromString(const std::string& time_str);

export_lua constexpr bool UPDATE_TRUE  = true;
export_lua constexpr bool UPDATE_FALSE = false;

export_lua inline bool scan_dir(const std::string&											parent_path,
								const std::string&											path,
								bool														bRecursive,
								std::function<void(const std::string&, const std::string&)> func)
{
	std::string cur_dir = parent_path + "/" + path;
	DIR*		dp		= opendir(cur_dir.c_str());
	if(dp == nullptr)
	{
		return false;
	}

	struct dirent* entry = readdir(dp);
	while(entry != nullptr)
	{
		struct stat statbuf;
		memset(&statbuf, 0, sizeof(statbuf));
		lstat(entry->d_name, &statbuf); // 获取下一级成员属性
		if(S_IFDIR & statbuf.st_mode)	// 判断下一级成员是否是目录
		{
			if(std::string(".") == entry->d_name || std::string("..") == entry->d_name)
			{
				entry = readdir(dp);
				continue;
			}

			if(bRecursive)
			{
				scan_dir(cur_dir, entry->d_name, bRecursive, func); // 递归调用自身，扫描下一级目录的内容
			}
		}
		else
		{
			func(cur_dir, entry->d_name);
		}

		entry = readdir(dp);
	}
	closedir(dp);
	return true;
}

#endif // BaseCode_h__
