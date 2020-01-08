#include "BaseCode.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <math.h>
#include "loging_manager.h"




int MulDiv(int a, int b, int c)
{
	return ((int)(((int64_t)(a) * (int64_t)(b)) / (int64_t)(c)));
}




uint32_t hex_set(uint32_t dwFlag, int nHex, UCHAR ucVal)
{
	CHECKF(nHex >= 0 && nHex < 8);
	return (dwFlag & ~(0xF << (nHex * 4))) | ((ucVal & 0xF) << (nHex * 4));
}

UCHAR hex_get(uint32_t dwFlag, int nHex)
{
	CHECKF(nHex >= 0 && nHex < 8);
	return (dwFlag >> (nHex * 4)) & 0xF;
}

bool bit_test(uint32_t dwFlag, int nBit)
{
	CHECKF(nBit >= 0 && nBit < 32);
	return (dwFlag & ((uint32_t)1 << nBit)) != 0;
}

uint32_t bit_flip(uint32_t dwFlag, int nBit)
{
	CHECKF(nBit >= 0 && nBit < 32);
	return dwFlag ^ ((uint32_t)1 << nBit);
}

uint32_t bit_set(uint32_t dwFlag, int nBit, bool bVal)
{
	CHECKF(nBit >= 0 && nBit < 32);
	if (bVal)
		return dwFlag | (uint32_t)1 << nBit;
	else
		return dwFlag & ~((uint32_t)1 << nBit);
}



time_t timeGetTime()
{
	// 	timeval cTime;
	// 	gettimeofday(&cTime,NULL);
	// 	return (1000*(time_t)cTime.tv_sec + cTime.tv_usec/1000);

	/*
	struct timespec {
	time_t   tv_sec;        // seconds
	long     tv_nsec;       // nanoseconds
	};

	int clock_gettime(clockid_t clk_id, struct timespec *tp);
	clk_id:
	CLOCK_REALTIME				系统实时时间
	CLOCK_MONOTONIC				从系统启动到这一刻所经过的时间，不受系统时间被用户改变的影响
	CLOCK_PROCESS_CPUTIME_ID	本进程到当前所花费的系统CPU时间
	CLOCK_THREAD_CPUTIME_ID		本线程到当前所花费的系统CPU时间
	*/
	timespec _ts;
	if (clock_gettime(CLOCK_MONOTONIC, &_ts) != 0)
	{
		// error
		return 0;
	}
	time_t _tick = (time_t)_ts.tv_sec * 1000 + _ts.tv_nsec / 1000000;
	return _tick;
}


time_t _TimeGetMonotonic()
{
	return timeGetTime();
}

time_t _TimeGetSecond()
{
	return ::time(NULL);
}

time_t _TimeGetMillisecond()
{
	timespec _ts;
	if(clock_gettime(CLOCK_REALTIME, &_ts) != 0)
	{
		return 0;
	}
	time_t _tick = (time_t)_ts.tv_sec * 1000 + _ts.tv_nsec / 1000000;
	return _tick;
}

time_t _TimeGetSecondLocal()
{
	return gmt2local(TimeGetSecond());
}

struct TimeGetCacheData
{
	bool bUserCache = false;
	time_t cache_TimeGet = 0;
	time_t cache_TimeGetSecond = 0;
	time_t cache_TimeGetMillisecond = 0;
	time_t cache_TimeGetSecondLocal = 0;
};
thread_local static TimeGetCacheData g_TimeGetCacheData;

void TimeGetCacheCreate()
{
	g_TimeGetCacheData.bUserCache = true;
	TimeGetCacheUpdate();
}

void TimeGetCacheUpdate()
{
	if(g_TimeGetCacheData.bUserCache)
	{
		g_TimeGetCacheData.cache_TimeGet = _TimeGetMonotonic();
		g_TimeGetCacheData.cache_TimeGetSecond = _TimeGetSecond();
		g_TimeGetCacheData.cache_TimeGetMillisecond = _TimeGetMillisecond();
		g_TimeGetCacheData.cache_TimeGetSecondLocal = _TimeGetSecondLocal();	
	}
}

time_t TimeGetMonotonic()
{
	if(g_TimeGetCacheData.bUserCache)
		return g_TimeGetCacheData.cache_TimeGet;
	else
		return _TimeGetMonotonic();
}

time_t TimeGetSecond()
{
	if(g_TimeGetCacheData.bUserCache)
		return g_TimeGetCacheData.cache_TimeGetSecond;
	else
		return _TimeGetSecond();
}

time_t TimeGetMillisecond()
{
	if(g_TimeGetCacheData.bUserCache)
		return g_TimeGetCacheData.cache_TimeGetMillisecond;
	else
		return _TimeGetMillisecond();
}

time_t TimeGetSecondLocal()
{
	if(g_TimeGetCacheData.bUserCache)
		return g_TimeGetCacheData.cache_TimeGetSecondLocal;
	else
		return _TimeGetSecondLocal();
}




time_t gmt2local(time_t tNow)
{
#if defined(WIN32)
	return tNow + _timezone;
#else
	return tNow - timezone;
#endif
}


time_t local2gmt(time_t tNow)
{
#if defined(WIN32)
	return tNow - _timezone;
#else
	return tNow + timezone;
#endif
}




/////////////////////////////////////////////////////////////////////////////
bool	CheckSameDay(time_t time1, time_t time2)
{
	struct tm tm1;
	if (0 != localtime_r(&time1, &tm1)) /* Convert to local time. */
		return false;

	struct tm tm2;
	if (0 != localtime_r(&time2, &tm2)) /* Convert to local time. */
		return false;

	return (tm1.tm_yday == tm2.tm_yday) && (tm1.tm_mon == tm2.tm_mon) && (tm1.tm_year == tm2.tm_year);
}



/////////////////////////////////////////////////////////////////////////////
int		DateDiffLocal(time_t time1, time_t time2)
{
__ENTER_FUNCTION
	struct tm tm1;
	if (0 != localtime_r(&time1, &tm1)) /* Convert to local time. */
		return 0;

	struct tm tm2;
	if (0 != localtime_r(&time2, &tm2)) /* Convert to local time. */
		return 0;

	//int nLeapYear = isleap(tm2.tm_year) ? 1 : 0;

	int nYears = tm2.tm_year - tm1.tm_year;
	int nDays = tm2.tm_yday - tm1.tm_yday;

	int nTmpYear = tm1.tm_year + 1900;

	if (nYears > 0)
	{
		while (nYears > 0)
		{
			if (isleap(nTmpYear))
				nDays += 366;
			else
				nDays += 365;
			nTmpYear++;
			nYears--;
		}
	}
	else if (nYears < 0)
	{
		while (nYears < 0)
		{
			if (isleap(nTmpYear))
				nDays -= 366;
			else
				nDays -= 365;
			nTmpYear--;
			nYears++;
		}
	}

	return nDays;
__LEAVE_FUNCTION
	return 0;
}


time_t GetTimeFromString(const std::string& time_str)
{
	struct tm _tm;
	memset(&_tm, 0, sizeof(_tm));
	strptime(time_str.c_str(), "%Y-%m-{} %H:%M:{}", &_tm);

	time_t t = mktime(&_tm);
	//t = local2gmt(t);
	return t;
}



time_t GetNextDayBeginTime()
{
	static thread_local time_t s_NextDayBeginTime = 0;
	time_t now = TimeGetSecond();
	if(now < s_NextDayBeginTime)
		return s_NextDayBeginTime;

	struct tm curtime;
	if (0 != localtime_r(&now, &curtime)) /* Convert to local time. */
	{
		curtime.tm_hour = 0;
		curtime.tm_min = 0;
		curtime.tm_sec = 0;
		s_NextDayBeginTime = mktime(&curtime) + 86400;	// 得到次日0点时间戳
		return s_NextDayBeginTime;
	}
	return now;
}

