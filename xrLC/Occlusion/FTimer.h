#ifndef _FTIMER_H_
#define _FTIMER_H_
#pragma once

class ENGINE_API CTimer {
private:
	__int64 qwStartTime;
	float	fResult;
	float	fTimerFreq;
public:
	CTimer()
	{
		__int64	qwTimerFreq;
        if (QueryPerformanceFrequency((LARGE_INTEGER *)&qwTimerFreq)==0) {
			THROW;
        } else {
            fTimerFreq = float(qwTimerFreq);
        }
	}
	void	Start	(void)
	{
		QueryPerformanceCounter((LARGE_INTEGER *)&qwStartTime);
	}
	float	Stop	(void)
	{
		__int64 qwEndTime;
		QueryPerformanceCounter((LARGE_INTEGER *)&qwEndTime);
		fResult = float(qwEndTime-qwStartTime)/fTimerFreq;
		return fResult;
	}
	float	Get		(void)
	{
		return fResult;
	}
	float	GetAsync(void)
	{
		__int64 qwEndTime;
		QueryPerformanceCounter((LARGE_INTEGER *)&qwEndTime);
		return float(qwEndTime-qwStartTime)/fTimerFreq;
	}
	void	Log		(void)
	{
//		::Log("* Elapsed time (sec):",GetAsync());
	}
};

#endif
