#pragma once

class ENGINE_API CThread
{
	static void __cdecl startup(void* P);
public:
	volatile DWORD		thID;
	volatile float		thProgress;
	volatile BOOL		thCompleted;
	volatile BOOL		thMessages;
	volatile BOOL		thMonitor;
	volatile float		thPerformance;
	volatile BOOL		thDestroyOnComplete;

	CThread				(DWORD _ID)	
	{
		thID				= _ID;
		thProgress			= 0;
		thCompleted			= FALSE;
		thMessages			= TRUE;
		thMonitor			= FALSE;
		thDestroyOnComplete	= TRUE;
	}
	void				Start	()
	{
		_beginthread(startup,0,this);
	}
	virtual		void	Execute	()	= 0;
};

class ENGINE_API CThreadManager
{
	xr_vector<CThread*>	threads;
public:
	void				start	(CThread*	T);
	void				wait	(DWORD		sleep_time=1000);
};