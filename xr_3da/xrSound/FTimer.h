#ifndef _FTIMER_H_
#define _FTIMER_H_
#pragma once

class XRCORE_API CTimer {
private:
	u64			qwStartTime;
	float		fResult;
public:
	IC void		Start			()		{	qwStartTime = CPU::GetCycleCount(); }
	IC float	Stop			()		{	return (fResult = GetElapsed_sec()); }
	IC float	Get				()		{	return fResult; }
	IC u64		GetElapsed_clk	()		{	return CPU::GetCycleCount()-qwStartTime-CPU::cycles_overhead; }
	IC u32		GetElapsed_ms	()		{	return u32(u64(GetElapsed_clk())/u64(CPU::cycles_per_milisec)); }
	IC float	GetElapsed_sec	()		{	return float(GetElapsed_clk())*CPU::cycles2seconds; }
	IC void		Dump			()
	{
		Msg("* Elapsed time (sec): %f",GetElapsed_sec());
	}
};

#endif
