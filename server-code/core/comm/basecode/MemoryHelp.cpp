#include "MemoryHelp.h"

#ifdef USE_JEMALLOC
extern "C"
{
#define JEMALLOC_MANGLE
#include "jemalloc/jemalloc.h"
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

uint64_t get_thread_memory_allocted()
{
#ifdef USE_JEMALLOC
    uint64_t allocated = 0;
    // uint64_t deallocated = 0;
    size_t len = sizeof(uint64_t);
    je_mallctl("thread.allocated", &allocated, &len, NULL, 0);
    return allocated;
    // je_mallctl("thread.deallocated", &deallocated, &len, NULL, 0);
    // if(allocated > deallocated)
    // {
    //     return allocated - deallocated;
    // }
    // else
    // {
    //     return 0;
    // }
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