#include "MemoryHeap.h"

#include <algorithm>
#include <deque>

#include "BaseCode.h"
#include "LoggingMgr.h"

#ifdef USE_JEMALLOC
extern "C"
{
#define JEMALLOC_MANGLE
#include <jemalloc/jemalloc.h>
}
#endif
//
//
// void   operator delete	(void* p)
//{
//	je_free(p);
//}
//
// void*  operator new	(size_t size)
//{
//	return je_malloc(size);
//}
//
// void   operator delete[](void* p)
//{
//	je_free(p);
//}
//
// void*  operator new[](size_t size)
//{
//	return je_malloc(size);
//}
//
//
// void JEMALLOC_Initer()
//{
//	bool option;
//	option = true;
//	je_mallctl("opt.background_thread", nullptr, nullptr, &option, sizeof(bool));
//	option = true;
//	je_mallctl("opt.junk", nullptr, nullptr, &option, sizeof(bool));
//	option = true;
//	je_mallctl("opt.prof_leak", nullptr, nullptr, &option, sizeof(bool));
//}

#ifdef USE_JEMALLOC
namespace
{

inline uint32_t arena_alloc()
{
    uint32_t    arena;
    std::size_t arena_byte_size = sizeof(arena);

    // Extend number of arenas
    if(je_mallctl("arenas.create", (void*)&arena, &arena_byte_size, nullptr, 0))
    {
        throw std::runtime_error("Failed to get new arena");
    }
    return arena;
}

inline void arena_free(unsigned arena)
{
    je_mallctl(fmt::format("arena.{}.destroy", arena).c_str(), 0, 0, 0, 0);
}

} // namespace
#endif

size_t get_thread_memory_allocted()
{
#ifdef USE_JEMALLOC
    uint64_t allocated   = 0;
    uint64_t deallocated = 0;
    size_t   len         = sizeof(uint64_t);
    je_mallctl("thread.allocated", &allocated, &len, NULL, 0);
    je_mallctl("thread.deallocated", &deallocated, &len, NULL, 0);
    if(allocated > deallocated)
    {
        return allocated - deallocated;
    }
    else
    {
        return 0;
    }
#else
    return 0;
#endif
}

memory_status get_memory_status()
{
    memory_status result;
#ifdef USE_JEMALLOC
    size_t   len   = sizeof(size_t);
    uint64_t epoch = 1;
    je_mallctl("epoch", &epoch, &len, &epoch, len);

    je_mallctl("stats.allocated", &result.allocted, &len, NULL, 0);
    je_mallctl("stats.active", &result.active, &len, NULL, 0);
    je_mallctl("stats.metadata", &result.metadata, &len, NULL, 0);
    je_mallctl("stats.resident", &result.resident, &len, NULL, 0);
    je_mallctl("stats.mapped", &result.mapped, &len, NULL, 0);
    je_mallctl("stats.retained", &result.retained, &len, NULL, 0);
    je_mallctl("stats.background_thread.num_threads", &result.num_threads, &len, NULL, 0);
#endif
    return result;
}

void start_jemalloc_backgroud_thread()
{
#ifdef USE_JEMALLOC
    bool option;
    option = true;
    je_mallctl("background_thread", nullptr, nullptr, &option, sizeof(bool));
    option = true;
    je_mallctl("opt.background_thread", nullptr, nullptr, &option, sizeof(bool));
#endif
}

void stop_jemalloc_backgroud_thread()
{
#ifdef USE_JEMALLOC
    bool option;
    option = false;
    je_mallctl("background_thread", nullptr, nullptr, &option, sizeof(bool));
    option = false;
    je_mallctl("opt.background_thread", nullptr, nullptr, &option, sizeof(bool));
#endif
}

CMemoryHeap::CMemoryHeap(const std::string& szClassName)
    : m_strName(szClassName)
    , m_lNumAllocsInHeap(0)
    , m_lMaxAllocsInHeap(0)
{
    BaseCode::MyLogMsg("log/debug", "MemoryHeap {} create", m_strName.c_str());

#if defined(HEAP_DEBUG)
    m_setCallFrame = new CALLFRAME_NODE;
#endif
}

CMemoryHeap::~CMemoryHeap()
{
    if(m_lNumAllocsInHeap != 0)
    {
        BaseCode::MyLogMsg("log/error", "MemoryHeap {} was not clear:{}", m_strName.c_str(), m_lNumAllocsInHeap.load());
#if defined(HEAP_DEBUG)
        {
            for(auto it = m_setDebugInfo.begin(); it != m_setDebugInfo.end(); it++)
            {
                const CALLFRAME_NODE* pFrame = it->second;
                DumpStack(pFrame);
            }
            m_setDebugInfo.clear();
        }
#endif
    }
    BaseCode::MyLogMsg("log/debug", "MemoryHeap {} destory", m_strName.c_str());
}

bool CMemoryHeap::IsValidPt(void* p)
{
#ifdef USE_JEMALLOC
    return je_sallocx(p, 0) != 0;
#else
    return true;
#endif
}

void* CMemoryHeap::Alloc(size_t size)
{
#ifdef USE_JEMALLOC
    void* result = je_malloc(size);
#else
    void* result = malloc(size);
#endif
    if(result == nullptr)
    {
        throw std::bad_alloc();
    }

    m_lNumAllocsInHeap++;
    if(m_lNumAllocsInHeap > m_lMaxAllocsInHeap)
        m_lMaxAllocsInHeap = m_lNumAllocsInHeap.load();

#if defined(HEAP_DEBUG)
    std::lock_guard<std::mutex> lock(m_mutexDebugInfo);
    m_setDebugInfo[result] = m_setCallFrame->MakeCallFrame(1);
#endif

    return result;
}

void CMemoryHeap::Free(void* ptr)
{
#ifdef USE_JEMALLOC
    je_free(ptr);
#else
    free(ptr);
#endif
    m_lNumAllocsInHeap--;

#if defined(HEAP_DEBUG)
    std::lock_guard<std::mutex> lock(m_mutexDebugInfo);
    auto                        it = m_setDebugInfo.find(p);
    if(it != m_setDebugInfo.end())
        m_setDebugInfo.erase(it);
#endif
}

size_t CMemoryHeap::GetAllocedSize()
{
    return m_lNumAllocsInHeap;
}
