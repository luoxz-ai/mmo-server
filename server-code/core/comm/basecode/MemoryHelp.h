#ifndef MEMORYHELP_H
#define MEMORYHELP_H

#include "BaseType.h"

struct memory_status
{
    size_t allocted    = 0;
    size_t active      = 0;
    size_t metadata    = 0;
    size_t resident    = 0;
    size_t mapped      = 0;
    size_t retained    = 0;
    size_t num_threads = 0;
};

uint64_t      get_thread_memory_allocted();
memory_status get_memory_status();

void start_jemalloc_backgroud_thread();
void stop_jemalloc_backgroud_thread();

#endif /* MEMORYHELP_H */
