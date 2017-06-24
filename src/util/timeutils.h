/*
 * boblight
 * Copyright (C) Bob  2009 
 * 
 * boblight is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * boblight is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TIMEUTILS
#define TIMEUTILS

#include "inclstdint.h"
#include <time.h>

#if defined(_MSC_VER)
#include <windows.h>
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
#include "config.h"
#include <sys/time.h>
#endif

#if defined(_MSC_VER)
struct timezone
{
	int  tz_minuteswest; /* minutes W of Greenwich */
	int  tz_dsttime;     /* type of dst correction */
};

inline int gettimeofday(struct timeval *tv, struct timezone *tz)
{
	FILETIME ft;
	unsigned __int64 tmpres = 0;
	static int tzflag;

	if (NULL != tv)
	{
		GetSystemTimeAsFileTime(&ft);

		tmpres |= ft.dwHighDateTime;
		tmpres <<= 32;
		tmpres |= ft.dwLowDateTime;

		/*converting file time to unix epoch*/
		tmpres -= DELTA_EPOCH_IN_MICROSECS;
		tmpres /= 10;  /*convert into microseconds*/
		tv->tv_sec = (long)(tmpres / 1000000UL);
		tv->tv_usec = (long)(tmpres % 1000000UL);
	}

	if (NULL != tz)
	{
		if (!tzflag)
		{
			_tzset();
			tzflag++;
		}
		tz->tz_minuteswest = _timezone / 60;
		tz->tz_dsttime = _daylight;
	}

	return 0;
}
#endif


inline int64_t GetTimeUs()
{
#if defined(HAVE_CLOCK_GETTIME) && defined(CLOCK_MONOTONIC)
  struct timespec time;
  clock_gettime(CLOCK_MONOTONIC, &time);
  return ((int64_t)time.tv_sec * 1000000LL) + (int64_t)(time.tv_nsec + 500) / 1000LL;
#else
  struct timeval time;
  gettimeofday(&time, NULL);
  return ((int64_t)time.tv_sec * 1000000LL) + (int64_t)time.tv_usec;
#endif
}

template <class T> 
inline T GetTimeSec()
{
  return (T)GetTimeUs() / (T)1000000.0;
}

void USleep(int64_t usecs, volatile bool* stop = NULL);

#endif //TIMEUTILS
