#ifndef _FTIMER_H_
#define _FTIMER_H_
#pragma once

class XRCORE_API CTimer {
private:
	u64		qwStartTime;
	float	fResult;
public:
	IC void		Start			()		{	qwStartTime = CPU::GetCycleCount(); }
	IC float	Stop			()		{	return (fResult = GetAsync()); }
	IC float	Get				()		{	return fResult; }
	IC float	GetAsync		()		{	return float(GetElapsed())*CPU::cycles2seconds; }
	IC u64		GetElapsed		()		{	return CPU::GetCycleCount()-qwStartTime-CPU::cycles_overhead; }
	IC u32		GetElapsed_ms	()		{	return u32(u64(GetElapsed())/u64(CPU::cycles_per_milisec)); }
	IC void		Dump			()
	{
		Msg("* Elapsed time (sec): %f",GetAsync());
	}
};

#endif
