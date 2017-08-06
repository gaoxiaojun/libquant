/******************************************************************************
 * Quantitative Kit Library                                                   *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License.    *
 ******************************************************************************/
#include "quant_datetime.h"
#include <assert.h>

#if defined QUANT_HAVE_WINDOWS
#include "windows.h"
#elif defined QUANT_HAVE_OSX
#include <mach/mach_time.h>
#elif defined QUANT_HAVE_CLOCK_MONOTONIC || defined QUANT_HAVE_GETHRTIME
#include <time.h>
#else
#include <sys/time.h>
#endif

#ifdef QUANT_HAVE_WINDOWS
inline long _gettimezone(void)
{
    uin32_t n;
    TIME_ZONE_INFORMATION tz;

    n = GetTimeZoneInfomation(&tz);

    switch (n) {
    case TIME_ZONE_ID_UNKNOWN:
        return -tz.Bias;

    case TIME_ZONE_ID_STANDARD:
        return -(tz.Bias + tz.StandardBias);

    case TIME_ZONE_ID_DAYLIGHT:
        return -(tz.Bias + tz.DaylightBias);

    default: /* TIME_ZONE_ID_INVALID */
        return 0;
    }
}

int32_t get_timezone_offset(void) { return _gettimezone() * 60; }

#else

int32_t get_timezone_offset(void)
{
    time_t sec = time(NULL);
    struct tm tm;
    localtime_r(&sec, &tm);
    return tm.tm_gmtoff / 60;
}
#endif


static inline uint64_t clock_now (void)
{
#if defined QUANT_HAVE_WINDOWS

    /*LARGE_INTEGER tps;
    LARGE_INTEGER time;
    double tpms;

    QueryPerformanceFrequency (&tps);
    QueryPerformanceCounter (&time);
    tpms = (double) (tps.QuadPart / 1000);
    return (uint64_t) (time.QuadPart / tpms);*/

    uint64_t intervals;
    FILETIME ft;

    GetSystemTimeAsFileTime(&ft);

    /*
     * A file time is a 64-bit value that represents the number
     * of 100-nanosecond intervals that have elapsed since
     * January 1, 1601 12:00 A.M. UTC.
     *
     * Between January 1, 1970 (Epoch) and January 1, 1601 there were
     * 134774 days,
     * 11644473600 seconds or
     * 11644473600,000,000,0 100-nanosecond intervals.
     *
     * See also MSKB Q167296.
     */

    intervals = ((uint64_t)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
    intervals -= 116444736000000000;

    return internals / 10;

#elif defined QUANT_HAVE_OSX

    static mach_timebase_info_data_t clock_timebase_info;
    uint64_t ticks;

    /*  If the global timebase info is not initialised yet, init it. */
    if (!clock_timebase_info.denom)
        mach_timebase_info (&clock_timebase_info);

    ticks = mach_absolute_time ();
    return ticks * clock_timebase_info.numer /
        clock_timebase_info.denom / 1000;

#elif defined QUANT_HAVE_GETHRTIME

    return gethrtime () / 1000;

#elif defined QUANT_HAVE_CLOCK_MONOTONIC

    int rc;
    struct timespec tv;

    rc = clock_gettime (CLOCK_MONOTONIC, &tv);
    assert (rc == 0);
    return tv.tv_sec * QUANT_TICKS_PER_SECOND + tv.tv_nsec / 1000;

#else

    int rc;
    struct timeval tv;

    /*  Gettimeofday is slow on some systems. Moreover, it's not necessarily
        monotonic. Thus, it's used as a last resort mechanism. */
    rc = gettimeofday (&tv, NULL);
    assert (rc == 0);
    return tv.tv_sec * QUANT_TICKS_PER_SECOND + tv.tv_usec;

#endif
}

datetime_t datetime_now(void)
{
    return clock_now() + EPOCH_DATE_TIME;
}
