#ifndef FTimerH
#define FTimerH
#pragma once

class CTimer;

class XRCORE_API pauseMngr
{
	xr_vector<CTimer*>	m_timers;
	bool				m_paused;
public:
	pauseMngr():m_paused(false){}
	bool Paused(){return m_paused;};
	void Pause(bool b);
	void Register (CTimer* t);
	void UnRegister (CTimer* t);
};

extern XRCORE_API pauseMngr	g_pauseMngr;

class XRCORE_API CTimer {
private:
	u64			qwStartTime;
	u64			qwPaused_time;
	float		fResult;
	bool		bPause;
public:
	CTimer						(bool bCanBePaused=false):qwPaused_time(0),bPause(false){if(bCanBePaused)g_pauseMngr.Register(this);}
	~CTimer						()				{g_pauseMngr.UnRegister(this);}
	IC void		Start			()		{	qwStartTime = CPU::GetCycleCount(); }
	IC float	Stop			()		{	return (fResult = GetElapsed_sec()); }
	IC float	Get				()		{	return fResult; }
	IC u64		GetElapsed_clk	()		{	if(bPause) return qwPaused_time; else return CPU::GetCycleCount()-qwStartTime-CPU::cycles_overhead-qwPaused_time; }
	IC u32		GetElapsed_ms	()		{	return u32(u64(GetElapsed_clk())/u64(CPU::cycles_per_milisec)); }
	IC float	GetElapsed_sec	()		{	return float(GetElapsed_clk())*CPU::cycles2seconds; }
	IC void		Dump			()
	{
		Msg("* Elapsed time (sec): %f",GetElapsed_sec());
	}
	IC bool		Paused			()		{	return bPause; }

	IC void		Pause			(bool b){
		if(bPause==b)return;
		
		if( b ){
			qwPaused_time			+= GetElapsed_clk();
		}else{
			u64 qwtmp				= CPU::GetCycleCount()-qwStartTime-CPU::cycles_overhead;
			qwPaused_time			= qwtmp - qwPaused_time;
		}
		bPause = b;
	}
};

class XRCORE_API CStatTimer
{
public:
	CTimer		T;
	__int64		accum;
	float		result;
	u32			count;
public:
				CStatTimer		();
	void		FrameStart		();
	void		FrameEnd		();

	IC void		Begin			()		{	count++; T.Start(); }
	IC void		End				()		{	accum += T.GetElapsed_clk(); }

	IC u64		GetElapsed_clk	()		{	return accum; }
	IC u32		GetElapsed_ms	()		{	return u32(u64(GetElapsed_clk())/u64(CPU::cycles_per_milisec)); }
	IC float	GetElapsed_sec	()		{	return float(GetElapsed_clk())*CPU::cycles2seconds; }
	IC float	GetFrame_sec	()		{	return result; }
};

#endif // FTimerH
