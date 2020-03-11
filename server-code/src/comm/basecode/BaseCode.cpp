#include "BaseCode.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include "loging_manager.h"

int32_t MulDivSign(int32_t a, int32_t b, int32_t c)
{
	return ((int32_t)(((int64_t)(a) * (int64_t)(b)) / (int64_t)(c)));
}

uint32_t MulDiv(uint32_t a, uint32_t b, uint32_t c)
{
	return ((uint32_t)(((uint64_t)(a) * (uint64_t)(b)) / (uint64_t)(c)));
}

uint32_t hex_set(uint32_t dwFlag, uint8_t nHex, uint8_t ucVal)
{
	CHECKF(nHex < 8);
	return (dwFlag & ~(0xF << (nHex * 4))) | ((ucVal & 0xF) << (nHex * 4));
}

uint8_t hex_get(uint32_t dwFlag, uint8_t nHex)
{
	CHECKF(nHex < 8);
	return (dwFlag >> (nHex * 4)) & 0xF;
}

bool bit_test(uint32_t dwFlag, uint8_t nBit)
{
	CHECKF(nBit < 32);
	return (dwFlag & ((uint32_t)1 << nBit)) != 0;
}

uint32_t bit_flip(uint32_t dwFlag, uint8_t nBit)
{
	CHECKF(nBit < 32);
	return dwFlag ^ ((uint32_t)1 << nBit);
}

uint32_t bit_set(uint32_t dwFlag, uint8_t nBit, bool bVal)
{
	CHECKF(nBit < 32);
	if(bVal)
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

	int32_t clock_gettime(clockid_t clk_id, struct timespec *tp);
	clk_id:
	CLOCK_REALTIME				系统实时时间
	CLOCK_MONOTONIC				从系统启动到这一刻所经过的时间，不受系统时间被用户改变的影响
	CLOCK_PROCESS_CPUTIME_ID	本进程到当前所花费的系统CPU时间
	CLOCK_THREAD_CPUTIME_ID		本线程到当前所花费的系统CPU时间
	*/
	timespec _ts;
	if(clock_gettime(CLOCK_MONOTONIC, &_ts) != 0)
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
	return gmt2local(_TimeGetSecond());
}

struct TimeGetCacheData
{
	bool   bUserCache		 = false;
	time_t Last				 = 0;
	time_t Now				 = 0;
	time_t NowOffset		 = 0;
	time_t NowOffsetSec		 = 0;
	time_t NowOffsetLocalSec = 0;
};
thread_local static TimeGetCacheData g_TimeGetCacheData;
static std::atomic<time_t>			 g_TimeOffset = 0;

void TimeOffset(time_t offset)
{
	g_TimeOffset += offset;
}

void TimeGetCacheCreate()
{
	g_TimeGetCacheData.bUserCache = true;
	g_TimeGetCacheData.Last		  = _TimeGetMillisecond();
	g_TimeGetCacheData.Now		  = g_TimeGetCacheData.Last;
}

void TimeGetCacheUpdate()
{
	if(g_TimeGetCacheData.bUserCache)
	{
		time_t now = _TimeGetMillisecond();
		if(now > g_TimeGetCacheData.Last)
		{
			g_TimeGetCacheData.Now += now - g_TimeGetCacheData.Last;
		}
		g_TimeGetCacheData.Last				 = now;
		g_TimeGetCacheData.NowOffset		 = g_TimeGetCacheData.Now + g_TimeOffset;
		g_TimeGetCacheData.NowOffsetSec		 = g_TimeGetCacheData.NowOffset / 1000;
		g_TimeGetCacheData.NowOffsetLocalSec = gmt2local(g_TimeGetCacheData.NowOffsetSec);
	}
}

time_t TimeGetMonotonic()
{
	if(g_TimeGetCacheData.bUserCache)
		return g_TimeGetCacheData.NowOffset;
	else
		return _TimeGetMonotonic();
}

time_t TimeGetSecond()
{
	if(g_TimeGetCacheData.bUserCache)
		return g_TimeGetCacheData.NowOffsetSec;
	else
		return _TimeGetSecond();
}

time_t TimeGetMillisecond()
{
	if(g_TimeGetCacheData.bUserCache)
		return g_TimeGetCacheData.NowOffset;
	else
		return _TimeGetMillisecond();
}

time_t TimeGetSecondLocal()
{
	if(g_TimeGetCacheData.bUserCache)
		return g_TimeGetCacheData.NowOffsetLocalSec;
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
bool CheckSameDay(time_t time1, time_t time2)
{
	struct tm tm1;
	if(0 != localtime_r(&time1, &tm1)) /* Convert to local time. */
		return false;

	struct tm tm2;
	if(0 != localtime_r(&time2, &tm2)) /* Convert to local time. */
		return false;

	return (tm1.tm_yday == tm2.tm_yday) && (tm1.tm_mon == tm2.tm_mon) && (tm1.tm_year == tm2.tm_year);
}

/////////////////////////////////////////////////////////////////////////////
int32_t DateDiffLocal(time_t time1, time_t time2)
{
	__ENTER_FUNCTION
	struct tm tm1;
	if(0 != localtime_r(&time1, &tm1)) /* Convert to local time. */
		return 0;

	struct tm tm2;
	if(0 != localtime_r(&time2, &tm2)) /* Convert to local time. */
		return 0;

	// int32_t nLeapYear = isleap(tm2.tm_year) ? 1 : 0;

	int32_t nYears = tm2.tm_year - tm1.tm_year;
	int32_t nDays  = tm2.tm_yday - tm1.tm_yday;

	int32_t nTmpYear = tm1.tm_year + 1900;

	if(nYears > 0)
	{
		while(nYears > 0)
		{
			if(isleap(nTmpYear))
				nDays += 366;
			else
				nDays += 365;
			nTmpYear++;
			nYears--;
		}
	}
	else if(nYears < 0)
	{
		while(nYears < 0)
		{
			if(isleap(nTmpYear))
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
	// t = local2gmt(t);
	return t;
}

time_t GetNextDayBeginTime()
{
	static thread_local time_t s_NextDayBeginTime = 0;

	time_t now = TimeGetSecond();
	if(now < s_NextDayBeginTime)
		return s_NextDayBeginTime;

	struct tm curtime;
	if(0 != localtime_r(&now, &curtime)) /* Convert to local time. */
	{
		curtime.tm_hour	   = 0;
		curtime.tm_min	   = 0;
		curtime.tm_sec	   = 0;
		s_NextDayBeginTime = mktime(&curtime) + 86400; // 得到次日0点时间戳
		return s_NextDayBeginTime;
	}
	return now;
}
