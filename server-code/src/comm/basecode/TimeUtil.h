#ifndef TIMEUTIL_H
#define TIMEUTIL_H

#include <cstddef>
#include <ctime>
#include <string>

#include "export_lua.h"

void TimeGetCacheCreate();
void TimeGetCacheUpdate();

export_lua time_t TimeGetMonotonic();
export_lua time_t TimeGetMillisecond();
export_lua time_t TimeGetSecond();
export_lua time_t TimeGetSecondLocal();

export_lua time_t _TimeGetMonotonic();
export_lua time_t _TimeGetSecond();
export_lua time_t _TimeGetMillisecond();
export_lua time_t _TimeGetSecondLocal();

export_lua time_t gmt2local(time_t tNow);
export_lua time_t local2gmt(time_t tNow);

//检查是否是同一天,loclatime
export_lua bool CheckSameDay(time_t time1, time_t time2);
//返回两个时间戳之间的日期差
export_lua int32_t DateDiffLocal(time_t time1, time_t time2);
export_lua time_t  GetNextDayBeginTime();

export_lua time_t GetTimeFromString(const std::string& time_str);

export_lua inline int32_t isleap(uint32_t year)
{
    return ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0));
}

#endif /* TIMEUTIL_H */
