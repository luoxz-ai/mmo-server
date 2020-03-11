#ifndef THREAD_H
#define THREAD_H

#include <atomic>
#include <condition_variable>
#include <memory>
#include <thread>

#include <unistd.h>

#include "BaseCode.h"
#include "LockfreeQueue.h"
//普通的按照时间间隔运行的Thread
class CNormalThread
{
public:
	typedef std::function<void()> on_thread_event_function_t;

public:
	CNormalThread(int32_t					 nWorkIntervalMS,
				  const std::string&		 thread_name			= std::string(),
				  on_thread_event_function_t on_thread_process_func = on_thread_event_function_t(),
				  on_thread_event_function_t on_thread_create_func	= on_thread_event_function_t(),
				  on_thread_event_function_t on_thread_finish_func	= on_thread_event_function_t());
	~CNormalThread();
	uint32_t GetTid() const { return m_tid; }
	void	 SetTid(uint32_t val) { m_tid = val; }

	void Stop();
	void Join();
	void Suspend();
	void Resume();

	bool IsReady() const;

private:
	std::unique_ptr<std::thread> m_Thread;
	int32_t						 m_nWorkIntervalMS;
	std::atomic<bool>			 m_bStop;
	std::atomic<bool>			 m_bIsReady;
	uint32_t					 m_tid;
};

//放入Job进行Process的Thread
class CWorkerThread
{
public:
	typedef std::function<void()> on_thread_event_function_t;
	typedef std::function<void()> job_function_t;
	typedef std::function<void()> result_function_t;

public:
	CWorkerThread(const std::string&		 thread_name		   = std::string(),
				  on_thread_event_function_t on_thread_create_func = on_thread_event_function_t(),
				  on_thread_event_function_t on_thread_finish_func = on_thread_event_function_t());
	~CWorkerThread();

	void Stop();

	void Join();

	void AddJob(job_function_t&& job_func);

	void _AddResult(result_function_t&& result_func);

	void PorcessResult();

	bool IsReady() const;

private:
	std::unique_ptr<std::thread> m_Thread;
	MPSCQueue<job_function_t>	 m_JobList;
	MPSCQueue<result_function_t> m_ResultList;

	std::atomic<bool>		m_bStop;
	std::atomic<bool>		m_bIsReady;
	std::mutex				m_csCV;
	std::condition_variable m_cv;
};

inline int32_t get_cur_thread_id()
{
	constexpr int32_t ___NR_gettid = 186;
	return (int32_t)syscall(___NR_gettid);
}
#endif /* THREAD_H */
