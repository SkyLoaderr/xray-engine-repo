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

	CThread				(DWORD _ID)	
	{
		thID			= _ID;
		thProgress		= 0;
		thCompleted		= FALSE;
		thMessages		= TRUE;
		thMonitor		= FALSE;
	}
	void				Start	()
	{
		_beginthread(startup,0,this);
	}
	virtual		void	Execute	()	= 0;
};

class ENGINE_API CThreadManager
{
	vector<CThread*>	threads;
public:
	void				start	(CThread*	T);
	void				wait	(DWORD		sleep_time=1000);
};