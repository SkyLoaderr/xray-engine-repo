/************************************************************************

  Routines for measuring time.

  $Id: time.cxx,v 1.1 2000/04/26 18:56:26 garland Exp $

 ************************************************************************/

#include <gfx/gfx.h>

#if defined(WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// Only Windows NT supports getting proper time usage information.
// In Windows 95, we have to settle for measuring real time.
double get_cpu_time()
{
    FILETIME start, end, kernel, user;

    if( !GetThreadTimes(GetCurrentThread(), &start, &end, &kernel, &user) )
    {
	// We're running under Windows 95 instead of NT.
	// Just get the current time and be done with it.
	SYSTEMTIME now;
	GetSystemTime(&now);
	SystemTimeToFileTime(&now, &user);
    }

    // Convert into something we can do math on
    LARGE_INTEGER i;
    i.LowPart = user.dwLowDateTime;
    i.HighPart = user.dwHighDateTime;

#ifdef __GNUC__

    // The Win32 headers shipped with GCC don't define the QuadPart
    // accessor for the LARGE_INTEGER type.  So we have to build it
    // directly.
    long long quad = i.HighPart;
    quad = (quad << 32) + i.LowPart;
    return (double)quad / 1e7;
#else
    // Convert to seconds and return
    return (double)(i.QuadPart) / 1e7;
#endif
}

#elif defined(HAVE_GETRUSAGE)
#include <sys/time.h>
#include <sys/resource.h>

double get_cpu_time()
{
    struct rusage t;

    getrusage(RUSAGE_SELF, &t);

    return (double)t.ru_utime.tv_sec + (double)t.ru_utime.tv_usec/1000000;
}

#elif defined(HAVE_TIMES)

double get_cpu_time()
{
    struct tms t;

    times(&t);

    return (double)(t.tms_utime) / (double)CLK_TCK;
}

#else

#error "No supported timing mechanism available."

#endif
