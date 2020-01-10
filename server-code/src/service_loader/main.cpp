#include "ServiceLoader.h"
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <thread>
#include <set>
#include "get_opt.h"
#include "FileLock.h"
#include "loging_manager.h"
#include "StringAlgo.h"
#include "MemoryHeap.h"
#include "BaseCode.h"
#include "segvcatch.h"
#include "fmt/format.h"
#include <google/protobuf/stubs/common.h>
#ifdef USE_JEMALLOC
extern "C" {
#define JEMALLOC_MANGLE
#include <jemalloc/jemalloc.h>
}
#endif
//std::atomic<uint64_t> g_GlobalAllocSize(0);
//inline void   operator delete	(void* p)
//{
//	if(p)
//	{
//		size_t size_mem = je_sallocx(p, 0);
//		g_GlobalAllocSize -= size_mem;
//		je_free(p);
//	}
//}
//
//inline void*  operator new	(size_t size)
//{
//	g_GlobalAllocSize += size;
//	return je_malloc(size);
//}
//
//inline void   operator delete[](void* p)
//{
//	size_t size_mem = je_sallocx(p, 0);
//	g_GlobalAllocSize -= size_mem;
//	je_free(p);
//}
//
//inline void*  operator new[](size_t size)
//{
//	g_GlobalAllocSize += size;
//	return je_malloc(size);
//}


void ProtobufLogHandler(google::protobuf::LogLevel level, const char* file, int line, const std::string& msg) 
{
	LOGERROR("PBError: {} in {}:{}", msg.c_str(), file, line);
}

struct PB_Initer
{
public:
	PB_Initer()
	{
		GOOGLE_PROTOBUF_VERIFY_VERSION;
		google::protobuf::SetLogHandler(ProtobufLogHandler);
	}
	~PB_Initer()
	{
	}
}const s_PB_Initer;

void print_help()
{
	std::cout << " [-h|--help] [--config=service.xml] [--worldid=0] [--start] [--start=1,2,3,4,5,6] [--stop] [--daemon]" << std::endl;
}

ServiceLoader* g_pLoader;
std::unique_ptr<file_lock> plock;
std::mutex g_tem_mutex;
//////////////////////////////////////////////////////////////////////////
void sig_term(int signo)
{
__ENTER_FUNCTION
	std::unique_lock<std::mutex> lock(g_tem_mutex,std::try_to_lock);
	if(lock.owns_lock() == false)
	{
		return;
	}
	if(g_pLoader)
	{
		g_pLoader->Destory();
		stop_jemalloc_backgroud_thread();
		std::cerr << "service destory." << std::endl;
		plock->unlock();
		plock.reset();
		BaseCode::StopLog();
		SAFE_DELETE(g_pLoader);
	}
__LEAVE_FUNCTION
	exit(1);
	//std::quick_exit(1);
}



//int daemon_init()
//{
//	int		i;
//	pid_t	pid;
//
//	if ((pid = fork()) < 0)
//		return (-1);
//	else if (pid)
//		_exit(0);			/* parent terminates */
//
//							/* child 1 continues... */
//
//	if (setsid() < 0)			/* become session leader */
//		return (-1);
//
//	signal(SIGHUP, SIG_IGN);
//	if ((pid = fork()) < 0)
//		return (-1);
//	else if (pid)
//		_exit(0);			/* child 1 terminates */
//
//	/* child 2 continues... */
//
//	/* close off file descriptors */
//	for (i = 0; i < 64; i++)
//		close(i);
//
//	return (0);				/* success */
//}

void PurgeJemalloc()
{
#ifdef USE_JEMALLOC
   mallctl("arenas.4096.purge", 0, 0, 0, 0);
#endif
}

int main(int argc, char *argv[])
{
	get_opt opt(argc, (const char**)argv);
	//请小心使用daemon/fork,这样会导致在main函数之前创建的线程被干掉
	if (opt.has("--daemon") || opt.has("-d"))
	{
		daemon(1, 1);
		//daemon_init();
	}

	G_INITSEGV();
	setlocale(LC_ALL, "en_US.UTF-8");
	tzset();
	std::string setting_filename = "res/service.xml";
	if (opt.has("--config"))
	{
		setting_filename = opt["--config"];
	}
	uint16_t nWorldID = 0;
	if (opt.has("--worldid"))
	{
		nWorldID = std::atoi(opt["--worldid"].c_str());
	}

	std::string logpath = "./log/";
	if (opt.has("--logpath"))
	{
		logpath = opt["--logpath"];
	}


	if (opt.has("--stop"))
	{
		file_lock lock("service_loader_" +std::to_string(nWorldID)+"_"+ opt["--stop"]);
		lock.kill(SIGQUIT);
		exit(0);
	}
	else if (opt.has("--start") == false)
	{
		print_help();
		exit(0);
	}
	

	BaseCode::InitLog(logpath);
	g_pLoader = new ServiceLoader();
	std::string start_service_set;
	if(opt.has("--start"))
		start_service_set = opt["--start"];
	

	plock.reset(new file_lock("service_loader_"+std::to_string(nWorldID)+"_"+ start_service_set));
	if (plock->lock() == false)
	{
		std::cerr << "only can start one instance." << std::endl;
		exit(-1);
	}

	

	auto vec_start_service = split_string(start_service_set, ",");
	std::set<uint16_t> create_service_set;
	for (const auto& v : vec_start_service)
	{
		if(v.empty() == false && v != "" && v != "\001")
			create_service_set.insert(std::stol(v));
	}



	if (g_pLoader->Load(setting_filename, nWorldID, create_service_set) == false)
	{
		std::cerr << "service load fail." << std::endl;
		exit(-1);
	}
	std::cout << fmt::format("service {} load succ.",start_service_set) << std::endl;

#ifndef STDIN_FILENO
	/* Standard file descriptors. */
#define STDIN_FILENO 0 /* Standard input. */
#define STDOUT_FILENO 1 /* Standard output. */
#define STDERR_FILENO 2 /* Standard error output. */
#endif

	FILE*	pStdOutFile = fopen("stdout.log", "w+");
	if (pStdOutFile == NULL)
	{
		exit(-1);
	}
	dup2(fileno(pStdOutFile), STDOUT_FILENO);
	dup2(fileno(pStdOutFile), STDERR_FILENO);
	fclose(pStdOutFile);
	pStdOutFile = NULL;

	
	signal(SIGTERM, sig_term);
	signal(SIGINT, sig_term);
	signal(SIGQUIT, sig_term);

	start_jemalloc_backgroud_thread();

	BaseCode::InitMonitorLog("comm");
	while (true)
	{
	__ENTER_FUNCTION
		//std::this_thread::yield();
		//PurgeJemalloc();
		auto result = get_memory_status();
		LOGMONITOR("Allocated: %.2f, active: %.2f, metadata: %.2f, resident: %.2f, mapped: %.2f, retained: %.2f, num_threads: {}", 
					 result.allocted/1024.0f/1024.0f, 
					 result.active/1024.0f/1024.0f, 
					 result.metadata/1024.0f/1024.0f, 
					 result.resident/1024.0f/1024.0f,
					 result.mapped/1024.0f/1024.0f, 
					 result.retained/1024.0f/1024.0f, 
					 result.num_threads);

		sleep(60);
	__LEAVE_FUNCTION
	
	}

}
