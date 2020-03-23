#ifndef __MYTIMER_H__
#define __MYTIMER_H__

#include <time.h>

#include "BaseCode.h"
#include "export_lua.h"
//////////////////////////////////////////////////////////////////////
// 超时类，以“秒”为单位
export_lua class CMyTimer
{
public:
    export_lua CMyTimer(int32_t nIntervalSecs = 0)
    {
        m_tInterval   = nIntervalSecs;
        m_tUpdateTime = 0;
    } // =0 : 用于数组初始化
      //	virtual ~CMyTimer() {}

public:
    // 设置时间标签
    export_lua void Update() { m_tUpdateTime = ::TimeGetSecond(); /*time(NULL);*/ }
    // 检查是否超时
    export_lua bool IsTimeOut() { return ::TimeGetSecond() >= m_tUpdateTime + m_tInterval; }
    // 用于定时触发的连续时钟。每次触发时为true。
    export_lua bool ToNextTime()
    {
        if(IsTimeOut())
            return Update(), true;
        else
            return false;
    }
    // 初始化时间片。(但不启动)
    export_lua void SetInterval(int32_t nSecs) { m_tInterval = nSecs; }

    // 开始启动时钟(同SetInterval)
    export_lua void Startup(int32_t nSecs)
    {
        m_tInterval = nSecs;
        Update();
    }
    // 时钟是否触发(只触发一次)，与Startup配合使用，可实现只触发一次的时钟。
    export_lua bool TimeOver()
    {
        if(IsActive() && IsTimeOut())
            return Clear(), true;
        return false;
    }
    export_lua void Clear()
    {
        m_tInterval   = 0;
        m_tUpdateTime = 0;
    }
    export_lua bool IsActive() { return m_tUpdateTime != 0; }
    export_lua void IncInterval(int32_t nSecs, int32_t nLimit) { m_tInterval = __min(m_tInterval + nSecs, nLimit); }
    export_lua void DecInterval(int32_t nSecs) { m_tInterval = __max(m_tInterval - nSecs, 0); }

    // 检查是否超时了指定时间片。用于同一时间标签检查多个不同超时事件。(可实现一个对象控制多个时钟)
    export_lua bool IsTimeOut(int32_t nSecs) { return ::TimeGetSecond() >= m_tUpdateTime + nSecs; }
    // 用于变长的超时事件。
    export_lua bool ToNextTime(int32_t nSecs)
    {
        if(IsTimeOut(nSecs))
            return Update(), true;
        else
            return false;
    }
    // 时钟会积累，但积累值不超过间隔值。其它同ToNextTime(...)
    export_lua bool ToNextTick(int32_t nSecs);
    //	void	AppendInterval	(int32_t nSecs)			{ if(ToNextTime()) m_nInterval=nSecs; else m_nInterval+=nSecs; }
    //// ToNextTime(): 保证至少有nSecs秒

public: // get
    /**
     * 返回剩余时间
     */
    export_lua int32_t GetRemain()
    {
        return (int32_t)(m_tUpdateTime
                             ? __min(__max(m_tInterval - (::TimeGetSecond() - (int32_t)m_tUpdateTime), 0), m_tInterval)
                             : 0);
    }
    /**
     * 返回timeout时刻时间戳
     */
    export_lua time_t GetTimeOut()
    {
        if(m_tUpdateTime)
            return m_tUpdateTime + m_tInterval;
        return 0;
    }

protected:
    time_t m_tInterval;
    time_t m_tUpdateTime;
};

//////////////////////////////////////////////////////////////////////
// 超时类，以“毫秒”为单位
class CMyTimerMS
{
public:
    CMyTimerMS(int32_t nIntervalSecs = 0)
    {
        m_tInterval   = nIntervalSecs;
        m_tUpdateTime = 0;
    } // =0 : 用于数组初始化
      //	virtual ~CMyTimerMS() {}

public:
    void Update() { m_tUpdateTime = ::TimeGetMonotonic(); }
    bool IsTimeOut() { return ::TimeGetMonotonic() >= m_tUpdateTime + m_tInterval; }
    bool ToNextTime()
    {
        if(IsTimeOut())
            return Update(), true;
        else
            return false;
    }
    // 	void	WaitNextTime()						{ clock_t tmNow = ::TimeGet(); if(m_tUpdateTime+m_nInterval > tmNow)
    // Sleep(m_tUpdateTime+m_nInterval-tmNow); Update(); }
    void SetInterval(int32_t nMilliSecs) { m_tInterval = nMilliSecs; }

    void Startup(int32_t nMilliSecs)
    {
        m_tInterval = nMilliSecs;
        Update();
    }
    bool TimeOver()
    {
        if(IsActive() && IsTimeOut())
            return Clear(), true;
        return false;
    }
    void Clear()
    {
        m_tInterval   = 0;
        m_tUpdateTime = 0;
    }
    bool IsActive() { return m_tUpdateTime != 0; }
    void IncInterval(int32_t nMilliSecs, int32_t nLimit)
    {
        m_tInterval = __min(m_tInterval + nMilliSecs, (uint32_t)nLimit);
    }
    void DecInterval(int32_t nMilliSecs) { m_tInterval = __max(m_tInterval - nMilliSecs, 0); }

    bool IsTimeOut(int32_t nMilliSecs) { return ::TimeGetMonotonic() >= m_tUpdateTime + nMilliSecs; }
    bool ToNextTime(int32_t nMilliSecs)
    {
        if(IsTimeOut(nMilliSecs))
            return Update(), true;
        else
            return false;
    }
    bool ToNextTick(int32_t nMilliSecs);
    //	void	AppendInterval	(int32_t nMilliSecs)	{ if(ToNextTime()) m_nInterval=nMilliSecs; else
    // m_nInterval+=nMilliSecs; }	// ToNextTime(): 保证至少有nSecs秒

public: // get
    int32_t  GetInterval() { return (int32_t)m_tInterval; }
    time_t   GetUpdateTime() { return m_tUpdateTime; }
    uint32_t GetRemain()
    {
        return uint32_t(
            m_tUpdateTime ? __min(__max(m_tInterval - (::TimeGetMonotonic() - m_tUpdateTime), 0), m_tInterval) : 0);
    }
    time_t GetTimeOut()
    {
        if(m_tUpdateTime)
            return m_tUpdateTime + m_tInterval;
        return 0;
    }

protected:
    time_t m_tInterval;
    time_t m_tUpdateTime;
};

#endif // __MYTIMER_H__
