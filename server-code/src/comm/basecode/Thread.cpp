#include "Thread.h"

#define RESUME_SIG	SIGUSR2
#define SUSPEND_SIG SIGUSR1

static sigset_t			wait_mask;
static thread_local int suspended = 0; // per-thread flag
void					resume_handler(int sig, siginfo_t* pInfo, void* pVoid)
{
	suspended = 0;
}

void suspend_handler(int sig, siginfo_t* pInfo, void* pVoid)
{
	if(suspended)
		return;
	suspended = 1;
	do
	{
		sigsuspend(&wait_mask);
	} while(suspended);
}

CNormalThread::CNormalThread(int32_t					nWorkIntervalMS,
							 const std::string&			thread_name /*= std::string()*/,
							 on_thread_event_function_t on_thread_process_func /*= on_thread_event_function_t()*/,
							 on_thread_event_function_t on_thread_create_func /*= on_thread_event_function_t()*/,
							 on_thread_event_function_t on_thread_finish_func /*= on_thread_event_function_t() */)
	: m_nWorkIntervalMS(nWorkIntervalMS)
	, m_bIsReady(false)
	, m_bStop(false)
{
	m_Thread =
		std::unique_ptr<std::thread>(new std::thread([pThis					  = this,
													  _on_thread_create_func  = std::move(on_thread_create_func),
													  _on_thread_process_func = std::move(on_thread_process_func),
													  _on_thread_finish_func  = std::move(on_thread_finish_func),
													  _thread_name			  = thread_name]() {
			__ENTER_FUNCTION
			pThis->SetTid(pthread_self());
			struct sigaction sa;

			sigfillset(&wait_mask);
			sigdelset(&wait_mask, SUSPEND_SIG);
			sigdelset(&wait_mask, RESUME_SIG);

			sigfillset(&sa.sa_mask);
			sa.sa_flags		= SA_SIGINFO;
			sa.sa_sigaction = &resume_handler;
			sigaction(RESUME_SIG, &sa, NULL);

			sa.sa_sigaction = &suspend_handler;
			sigaction(SUSPEND_SIG, &sa, NULL);

			if(_thread_name.empty() == false)
			{
				pthread_setname_np(pthread_self(), _thread_name.c_str());
			}

			if(_on_thread_create_func)
				_on_thread_create_func();
			TimeGetCacheCreate();
			pThis->m_bIsReady = true;
			while(!pThis->m_bStop)
			{
				__ENTER_FUNCTION
				auto beginTime = std::chrono::high_resolution_clock::now();
				TimeGetCacheUpdate();
				_on_thread_process_func();
				auto endTime = std::chrono::high_resolution_clock::now();

				std::chrono::milliseconds costTime =
					std::chrono::duration_cast<std::chrono::milliseconds>(endTime - beginTime);
				if(pThis->m_nWorkIntervalMS > 0)
				{
					std::chrono::milliseconds waitTime = std::chrono::milliseconds(pThis->m_nWorkIntervalMS) - costTime;
					if(waitTime.count() > 0)
					{
						usleep(waitTime.count() * 1000);
					}
					else
					{
						std::this_thread::yield();
					}
				}
				else
				{
					std::this_thread::yield();
				}
				__LEAVE_FUNCTION
			}

			if(_on_thread_finish_func)
				_on_thread_finish_func();
			__LEAVE_FUNCTION
		}));
}

CNormalThread::~CNormalThread()
{
	Stop();
	Join();
}

void CNormalThread::Stop()
{
	m_bStop = true;
}

void CNormalThread::Join()
{
	if(m_Thread)
	{
		m_Thread->join();
		m_Thread.reset();
	}
}

void CNormalThread::Suspend()
{
	pthread_kill(m_tid, SUSPEND_SIG);
}

void CNormalThread::Resume()
{
	pthread_kill(m_tid, RESUME_SIG);
}

bool CNormalThread::IsReady() const
{
	return m_bIsReady;
}

CWorkerThread::CWorkerThread(const std::string&			thread_name /*= std::string()*/,
							 on_thread_event_function_t on_thread_create_func /*= on_thread_event_function_t()*/,
							 on_thread_event_function_t on_thread_finish_func /*= on_thread_event_function_t()*/)
	: m_bIsReady(false)
	, m_bStop(false)
{
	m_Thread =
		std::unique_ptr<std::thread>(new std::thread([pThis					 = this,
													  _thread_name			 = thread_name,
													  _on_thread_create_func = std::move(on_thread_create_func),
													  _on_thread_finish_func = std::move(on_thread_finish_func)]() {
			__ENTER_FUNCTION
			if(_thread_name.empty() == false)
			{
				pthread_setname_np(pthread_self(), _thread_name.c_str());
			}

			if(_on_thread_create_func)
				_on_thread_create_func();
			pThis->m_bIsReady = true;
			while(pThis->m_bStop.load() == false)
			{
				__ENTER_FUNCTION
				std::function<void()> job_func;
				while(pThis->m_JobList.get(job_func))
				{
					job_func();
				}
				if(pThis->m_bStop)
				{
					break;
				}
				std::unique_lock<std::mutex> lk(pThis->m_csCV);
				pThis->m_cv.wait(lk);
				__LEAVE_FUNCTION
			}

			if(_on_thread_finish_func)
				_on_thread_finish_func();
			__LEAVE_FUNCTION
		}));
}

CWorkerThread::~CWorkerThread()
{
	Stop();
	Join();
}

void CWorkerThread::Stop()
{
	m_bStop = true;
	m_cv.notify_one();
}

void CWorkerThread::Join()
{
	if(m_Thread)
	{
		m_Thread->join();
		m_Thread.reset();
	}
}

void CWorkerThread::AddJob(job_function_t&& job_func)
{
	// call by main thread
	m_JobList.push(std::move(job_func));
	m_cv.notify_one();
}

void CWorkerThread::_AddResult(result_function_t&& result_func)
{
	// call by worker thread
	m_ResultList.push(std::move(result_func));
}

void CWorkerThread::PorcessResult()
{
	// call by main thread

	std::function<void()> result_func;
	while(m_ResultList.get(result_func))
	{
		result_func();
	}
}

bool CWorkerThread::IsReady() const
{
	return m_bIsReady;
}
