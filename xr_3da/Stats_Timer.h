#ifndef STATS_TIMER_H
#define STATS_TIMER_H
#pragma once

class CStatTimer
{
	friend class CStats;
	friend class CRenderDevice;
private:
	CTimer		T;
	__int64		accum;
	float		result;
	u32		count;
public:
	CStatTimer()
	{
		accum	= 0;
		result	= 0;
		count	= 0;
	}
	
	IC void		FrameStart	()
	{
		accum	= 0;
		count	= 0;
	}
	IC void		FrameEnd	()
	{
		result	= 0.97f*result + 0.03f*float(accum)*CPU::cycles2milisec;
	}
	IC void		Begin()
	{	count++; T.Start(); }
	IC void		End()
	{	accum += T.GetElapsed(); }
};

#endif