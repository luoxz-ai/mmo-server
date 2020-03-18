#ifndef CHECKUTIL_H
#define CHECKUTIL_H

#include "LoggingMgr.h"

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

#define CHECKFSR(x, R)               \
	{                                \
		if(!(x))                     \
		{                            \
			LOGASSERT("ASSERT:" #x); \
			static R r;              \
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
#define CHECKFSR_M(x, R, msg)                       \
	{                                               \
		if(!(x))                                    \
		{                                           \
			LOGASSERT("ASSERT:" #x " msg:{}", msg); \
			static R r;                             \
			return r;                               \
		}                                           \
	}

#define CHECK_FMT(x, fmt_msg, ...)                                  \
	{                                                               \
		if(!(x))                                                    \
		{                                                           \
			LOGASSERT("ASSERT:" #x " msg:" fmt_msg, ##__VA_ARGS__); \
			return;                                                 \
		}                                                           \
	}
#define CHECKF_FMT(x, fmt_msg, ...)                                 \
	{                                                               \
		if(!(x))                                                    \
		{                                                           \
			LOGASSERT("ASSERT:" #x " msg:" fmt_msg, ##__VA_ARGS__); \
			return 0;                                               \
		}                                                           \
	}
#define CHECKFR_FMT(x, r, fmt_msg, ...)                             \
	{                                                               \
		if(!(x))                                                    \
		{                                                           \
			LOGASSERT("ASSERT:" #x " msg:" fmt_msg, ##__VA_ARGS__); \
			return r;                                               \
		}                                                           \
	}
#define CHECKFSR_FMT(x, R, fmt_msg, ...)                            \
	{                                                               \
		if(!(x))                                                    \
		{                                                           \
			LOGASSERT("ASSERT:" #x " msg:" fmt_msg, ##__VA_ARGS__); \
			static R r;                                             \
			return r;                                               \
		}                                                           \
	}

#define __ENTER_FUNCTION \
	{                    \
		try              \
		{
#define __LEAVE_FUNCTION                                                               	  \
		}                                                                                  \
		catch(const std::runtime_error& e) { LOGASSERT("catch_execpetion:{}", e.what()); DumpStackFile(CallFrameMap(1)); } \
		catch(const std::exception& e) { LOGASSERT("catch_execpetion:{}", e.what()); DumpStackFile(CallFrameMap(1)); }     \
		catch(...) { LOGASSERT("catch_error"); }                                           \
	}

#endif /* CHECKUTIL_H */
