#include "stdafx.h"

#include "Timer.h"

#include <windows.h>

namespace MiniBall
{
	Timer::Timer()
	{
	}

	double Timer::seconds()
	{
		__int64 currentTime;
		__int64 frequency;

		QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
		QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

		return (double)currentTime / (double)frequency;
	}

	double Timer::ticks()
	{
		return 4800.0 * seconds();
	}

	void Timer::pause(double p)
	{
		double stopTime = Timer::seconds() + p;

		while(Timer::seconds() < stopTime);
	}
}