#ifndef _FTIMER_H_
#define _FTIMER_H_
#pragma once

class XRCORE_API CTimer {
private:
	u64		qwStartTime;
	float	fResult;
public:
	IC void	Start	(void)
	{	qwStartTime = CPU::GetCycleCount(); }
	IC float Stop	(void)
	{	return (fResult = GetAsync()); }
	IC float Get	(void)
	{	return fResult; }
	IC float GetAsync(void)
	{	return float(GetElapsed())*CPU::cycles2seconds; }
	IC u64	GetElapsed	()
	{	return CPU::GetCycleCount()-qwStartTime-CPU::cycles_overhead; }

	void	Dump	(void)
	{
		Msg("* Elapsed time (sec): %f",GetAsync());
	}
};

#endif
