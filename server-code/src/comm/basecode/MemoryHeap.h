#ifndef __MEMORYHEAP_H__
#define __MEMORYHEAP_H__

#include <atomic>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>

#include "BaseType.h"
#include "CallStackDumper.h"

struct memory_status
{
	size_t allocted	   = 0;
	size_t active	   = 0;
	size_t metadata	   = 0;
	size_t resident	   = 0;
	size_t mapped	   = 0;
	size_t retained	   = 0;
	size_t num_threads = 0;
};

size_t		  get_alloc_from_memory_heap();
size_t		  get_thread_memory_allocted();
memory_status get_memory_status();

void start_jemalloc_backgroud_thread();
void stop_jemalloc_backgroud_thread();

//#ifdef DEBUG
//#define HEAP_DEBUG
//#endif

class CMemoryHeap
{
public:
	CMemoryHeap(const std::string& szClassName);
	virtual ~CMemoryHeap();

	bool   IsValidPt(void* p);
	void*  Alloc(size_t size);
	void   Free(void* ptr);
	size_t GetAllocedSize();

protected:
	std::string			  m_strName;
	std::atomic<uint32_t> m_lNumAllocsInHeap;
	std::atomic<uint32_t> m_lMaxAllocsInHeap;

#if defined(HEAP_DEBUG)
	std::mutex								   m_mutexDebugInfo;
	CALLFRAME_NODE*							   m_setCallFrame;
	std::unordered_map<void*, CALLFRAME_NODE*> m_setDebugInfo;
#endif
};

#define MEMORYHEAP_DECLARATION(VAR)                               \
public:                                                           \
	void  operator delete(void* p) { VAR.Free(p); }               \
	void* operator new(size_t size) { return VAR.Alloc(size); }   \
	void  operator delete[](void* p) { VAR.Free(p); }             \
	void* operator new[](size_t size) { return VAR.Alloc(size); } \
                                                                  \
public:                                                           \
	static bool IsValidPt(void* p) { return VAR.IsValidPt(p); }   \
                                                                  \
public:                                                           \
	static CMemoryHeap VAR;

#define MEMORYHEAP_IMPLEMENTATION(T, VAR) CMemoryHeap T::VAR(#T);

#endif // __MEMORYHEAP_H__
