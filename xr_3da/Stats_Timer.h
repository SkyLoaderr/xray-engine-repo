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
	u32			count;
public:
	CStatTimer();
	void		FrameStart	();
	void		FrameEnd	();

	IC void		Begin()
	{	count++; T.Start(); }
	IC void		End()
	{	accum += T.GetElapsed(); }
};

#endif