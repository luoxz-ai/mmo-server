#include "Thread.h"

#define RESUME_SIG  SIGUSR2
#define SUSPEND_SIG SIGUSR1

static thread_local sigset_t wait_mask;
static thread_local int32_t  suspended = 0; // per-thread flag

void resume_handler(int32_t sig, siginfo_t* pInfo, void* pVoid)
{
    suspended = 0;
}

void suspend_handler(int32_t sig, siginfo_t* pInfo, void* pVoid)
{
    if(suspended)
        return;
    suspended = 1;
    do
    {
        sigsuspend(&wait_mask);
    } while(suspended);
}

CNormalThread::CNormalThread(int32_t                    nWorkIntervalMS,
                             const std::string&         thread_name /*= std::string()*/,
                             on_thread_event_function_t on_thread_process_func /*= on_thread_event_function_t()*/,
                             on_thread_event_function_t on_thread_create_func /*= on_thread_event_function_t()*/,
                             on_thread_event_function_t on_thread_finish_func /*= on_thread_event_function_t() */)
    : m_nWorkIntervalMS(nWorkIntervalMS)
    , m_ThreadName(thread_name)
    , m_funcThreadProcess(std::move(on_thread_process_func))
    , m_funcThreadCreate(std::move(on_thread_create_func))
    , m_funcThreadFinish(std::move(on_thread_finish_func))
    , m_Thread{std::make_unique<std::thread>(std::bind(&CNormalThread::ThreadFunc, this))}
{
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

void CNormalThread::ThreadFunc()
{
    __ENTER_FUNCTION
    SetTid(pthread_self());
    struct sigaction sa;

    sigfillset(&wait_mask);
    sigdelset(&wait_mask, SUSPEND_SIG);
    sigdelset(&wait_mask, RESUME_SIG);

    sigfillset(&sa.sa_mask);
    sa.sa_flags     = SA_SIGINFO;
    sa.sa_sigaction = &resume_handler;
    sigaction(RESUME_SIG, &sa, NULL);

    sa.sa_sigaction = &suspend_handler;
    sigaction(SUSPEND_SIG, &sa, NULL);

    if(m_ThreadName.empty() == false)
    {
        pthread_setname_np(pthread_self(), m_ThreadName.c_str());
    }
    BaseCode::SetNdc(m_ThreadName);
    LOGDEBUG("ThreadCreate:{} ThreadID:{}", m_ThreadName, get_cur_thread_id());
    if(m_funcThreadCreate)
    {
        m_funcThreadCreate();
    }

    TimeGetCacheCreate();
    m_bIsReady = true;
    while(!m_bStop)
    {
        __ENTER_FUNCTION
        auto beginTime = std::chrono::high_resolution_clock::now();
        TimeGetCacheUpdate();
        m_funcThreadProcess();
        auto endTime = std::chrono::high_resolution_clock::now();

        std::chrono::milliseconds costTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - beginTime);
        if(m_nWorkIntervalMS > 0)
        {
            std::chrono::milliseconds waitTime = std::chrono::milliseconds(m_nWorkIntervalMS) - costTime;
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

    if(m_funcThreadFinish)
    {
        m_funcThreadFinish();
    }

    LOGDEBUG("ThreadExit:{} ThreadID:{}", m_ThreadName, get_cur_thread_id());
    BaseCode::ClearNdc();
    __LEAVE_FUNCTION
}

CWorkerThread::CWorkerThread(const std::string&         thread_name /*= std::string()*/,
                             on_thread_event_function_t on_thread_create_func /*= on_thread_event_function_t()*/,
                             on_thread_event_function_t on_thread_finish_func /*= on_thread_event_function_t()*/)
    : m_ThreadName(thread_name)
    , m_funcThreadCreate(std::move(on_thread_create_func))
    , m_funcThreadFinish(std::move(on_thread_finish_func))
    , m_Thread{std::make_unique<std::thread>(std::bind(&CWorkerThread::ThreadFunc, this))}
{
}

CWorkerThread::~CWorkerThread()
{
    Stop();
    Join();
}

void CWorkerThread::ThreadFunc()
{
    __ENTER_FUNCTION
    if(m_ThreadName.empty() == false)
    {
        pthread_setname_np(pthread_self(), m_ThreadName.c_str());
    }

    BaseCode::SetNdc(m_ThreadName);
    LOGDEBUG("ThreadCreate:{} ThreadID:{}", m_ThreadName, get_cur_thread_id());

    if(m_funcThreadCreate)
    {
        m_funcThreadCreate();
    }
    m_bIsReady = true;
    while(m_bStop.load() == false)
    {
        __ENTER_FUNCTION
        std::function<void()> job_func;
        while(m_JobList.get(job_func))
        {
            job_func();
        }
        if(m_bStop)
        {
            break;
        }
        std::unique_lock<std::mutex> lk(m_csCV);
        m_cv.wait(lk);
        __LEAVE_FUNCTION
    }

    if(m_funcThreadFinish)
    {
        m_funcThreadFinish();
    }
    LOGDEBUG("ThreadExit:{} ThreadID:{}", m_ThreadName, get_cur_thread_id());
    BaseCode::ClearNdc();
    __LEAVE_FUNCTION
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

void CWorkerThread::PorcessResult(int32_t nMaxProcess)
{
    // call by main thread
    int32_t               nProcessed = 0;
    std::function<void()> result_func;
    while(m_ResultList.get(result_func))
    {
        result_func();
        if(nMaxProcess > 0 && nProcessed >= nMaxProcess)
        {
            return;
        }
    }
}

bool CWorkerThread::IsReady() const
{
    return m_bIsReady;
}
