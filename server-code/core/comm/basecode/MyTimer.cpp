#include "MyTimer.h"

//////////////////////////////////////////////////////////////////////
// CMyTimer
//////////////////////////////////////////////////////////////////////
// 带时间积累的定时时钟
bool CMyTimer::ToNextTick(int32_t nSecs)
{
    if(IsTimeOut(nSecs))
    {
        if(time(NULL) >= m_tUpdateTime + nSecs * 2)
            return Update(), true;
        else
            return (m_tUpdateTime += nSecs), true;
    }
    else
        return false;
}

//////////////////////////////////////////////////////////////////////
// CMyTimerMS
//////////////////////////////////////////////////////////////////////
// 带时间积累的定时时钟
bool CMyTimerMS::ToNextTick(int32_t nMilliSecs)
{
    if(IsTimeOut(nMilliSecs))
    {
        if(clock() >= (clock_t)(m_tUpdateTime + nMilliSecs * 2))
            return Update(), true;
        else
            return (m_tUpdateTime += nMilliSecs), true;
    }
    else
        return false;
}
