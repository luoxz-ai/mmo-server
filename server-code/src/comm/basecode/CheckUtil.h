#ifndef CHECKUTIL_H
#define CHECKUTIL_H

#include <functional>
#include <optional>
#include "LoggingMgr.h"

#define CHECK(expr)                                                              \
    {                                                                            \
        if(!(expr))                                                              \
        {                                                                        \
            LOGASSERT("ASSERT:" #expr);                                          \
            LOGASSERT("CallStack：{}", GetStackTraceString(CallFrameMap(2, 7))); \
            return;                                                              \
        }                                                                        \
    }
#define CHECKF(expr)                                                             \
    {                                                                            \
        if(!(expr))                                                              \
        {                                                                        \
            LOGASSERT("ASSERT:" #expr);                                          \
            LOGASSERT("CallStack：{}", GetStackTraceString(CallFrameMap(2, 7))); \
            return 0;                                                            \
        }                                                                        \
    }
#define CHECKFR(expr, r)                                                         \
    {                                                                            \
        if(!(expr))                                                              \
        {                                                                        \
            LOGASSERT("ASSERT:" #expr);                                          \
            LOGASSERT("CallStack：{}", GetStackTraceString(CallFrameMap(2, 7))); \
            return r;                                                            \
        }                                                                        \
    }

#define CHECKFSR(expr, R)                                                        \
    {                                                                            \
        if(!(expr))                                                              \
        {                                                                        \
            LOGASSERT("ASSERT:" #expr);                                          \
            LOGASSERT("CallStack：{}", GetStackTraceString(CallFrameMap(2, 7))); \
            static R r;                                                          \
            return r;                                                            \
        }                                                                        \
    }

#define CHECK_M(expr, msg)                                                       \
    {                                                                            \
        if(!(expr))                                                              \
        {                                                                        \
            LOGASSERT("ASSERT:" #expr " msg:{}", msg);                           \
            LOGASSERT("CallStack：{}", GetStackTraceString(CallFrameMap(2, 7))); \
            return;                                                              \
        }                                                                        \
    }
#define CHECKF_M(expr, msg)                                                      \
    {                                                                            \
        if(!(expr))                                                              \
        {                                                                        \
            LOGASSERT("ASSERT:" #expr " msg:{}", msg);                           \
            LOGASSERT("CallStack：{}", GetStackTraceString(CallFrameMap(2, 7))); \
            return 0;                                                            \
        }                                                                        \
    }
#define CHECKFR_M(expr, r, msg)                                                  \
    {                                                                            \
        if(!(expr))                                                              \
        {                                                                        \
            LOGASSERT("ASSERT:" #expr " msg:{}", msg);                           \
            LOGASSERT("CallStack：{}", GetStackTraceString(CallFrameMap(2, 7))); \
            return r;                                                            \
        }                                                                        \
    }
#define CHECKFSR_M(expr, R, msg)                                                 \
    {                                                                            \
        if(!(expr))                                                              \
        {                                                                        \
            LOGASSERT("ASSERT:" #expr " msg:{}", msg);                           \
            LOGASSERT("CallStack：{}", GetStackTraceString(CallFrameMap(2, 7))); \
            static R r;                                                          \
            return r;                                                            \
        }                                                                        \
    }

#define CHECK_V(expr, v)                                                         \
    {                                                                            \
        if(!(expr))                                                              \
        {                                                                        \
            LOGASSERT("ASSERT:" #expr " " #v ":{}", v);                          \
            LOGASSERT("CallStack：{}", GetStackTraceString(CallFrameMap(2, 7))); \
            return;                                                              \
        }                                                                        \
    }
#define CHECKF_V(expr, v)                                                        \
    {                                                                            \
        if(!(expr))                                                              \
        {                                                                        \
            LOGASSERT("ASSERT:" #expr " " #v ":{}", v);                          \
            LOGASSERT("CallStack：{}", GetStackTraceString(CallFrameMap(2, 7))); \
            return 0;                                                            \
        }                                                                        \
    }
#define CHECKFR_V(expr, r, v)                                                    \
    {                                                                            \
        if(!(expr))                                                              \
        {                                                                        \
            LOGASSERT("ASSERT:" #expr " " #v ":{}", v);                          \
            LOGASSERT("CallStack：{}", GetStackTraceString(CallFrameMap(2, 7))); \
            return r;                                                            \
        }                                                                        \
    }
#define CHECKFSR_V(expr, R, v)                                                   \
    {                                                                            \
        if(!(expr))                                                              \
        {                                                                        \
            LOGASSERT("ASSERT:" #expr " " #v ":{}", v);                          \
            LOGASSERT("CallStack：{}", GetStackTraceString(CallFrameMap(2, 7))); \
            static R r;                                                          \
            return r;                                                            \
        }                                                                        \
    }

#define CHECK_FMT(expr, fmt_msg, ...)                                            \
    {                                                                            \
        if(!(expr))                                                              \
        {                                                                        \
            LOGASSERT("ASSERT:" #expr " msg:" fmt_msg, ##__VA_ARGS__);           \
            LOGASSERT("CallStack：{}", GetStackTraceString(CallFrameMap(2, 7))); \
            return;                                                              \
        }                                                                        \
    }
#define CHECKF_FMT(expr, fmt_msg, ...)                                           \
    {                                                                            \
        if(!(expr))                                                              \
        {                                                                        \
            LOGASSERT("ASSERT:" #expr " msg:" fmt_msg, ##__VA_ARGS__);           \
            LOGASSERT("CallStack：{}", GetStackTraceString(CallFrameMap(2, 7))); \
            return 0;                                                            \
        }                                                                        \
    }
#define CHECKFR_FMT(expr, r, fmt_msg, ...)                                       \
    {                                                                            \
        if(!(expr))                                                              \
        {                                                                        \
            LOGASSERT("ASSERT:" #expr " msg:" fmt_msg, ##__VA_ARGS__);           \
            LOGASSERT("CallStack：{}", GetStackTraceString(CallFrameMap(2, 7))); \
            return r;                                                            \
        }                                                                        \
    }
#define CHECKFSR_FMT(expr, R, fmt_msg, ...)                                      \
    {                                                                            \
        if(!(expr))                                                              \
        {                                                                        \
            LOGASSERT("ASSERT:" #expr " msg:" fmt_msg, ##__VA_ARGS__);           \
            LOGASSERT("CallStack：{}", GetStackTraceString(CallFrameMap(2, 7))); \
            static R r;                                                          \
            return r;                                                            \
        }                                                                        \
    }

#define __ENTER_FUNCTION \
    {                    \
        try              \
        {
#define __LEAVE_FUNCTION                                                   \
    }                                                                      \
    catch(const std::runtime_error& e)                                     \
    {                                                                      \
        LOGERROR("catch_execpetion:{}", e.what());                         \
        LOGERROR("CallStack:{}", GetStackTraceString(CallFrameMap(1, 6))); \
    }                                                                      \
    catch(const std::exception& e)                                         \
    {                                                                      \
        LOGERROR("catch_execpetion:{}", e.what());                         \
        LOGERROR("CallStack:{}", GetStackTraceString(CallFrameMap(1, 6))); \
    }                                                                      \
    catch(...) { LOGSTACK("catch_error"); }                                \
    }

template<class Func, class... Args>
static inline std::optional<std::string> attempt(Func&& func, Args&&... args)
{
    try
    {
        std::invoke(std::forward<Func>(func), std::forward<Args>(args)...);
    }
    catch(std::exception e)
    {
        return {e.what()};
    }
    return {};
}


#endif /* CHECKUTIL_H */
