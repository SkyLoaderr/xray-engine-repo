#include "stdafx.h"
#include "xrThread.h"

void __cdecl CThread::startup(void* P)
{
	CThread* T = (CThread*)P;

	if (T->thMessages)	Msg("* THREAD #%d: Started.",T->thID);
	FPU::m64r		();
	T->Execute		();
	T->thCompleted	= TRUE;
	if (T->thMessages)	Msg("* THREAD #%d: Task Completed.",T->thID);
}

void	CThreadManager::start	(CThread*	T)
{
	R_ASSERT			(T);
	threads.push_back	(T);
	T->Start			();
}

void	CThreadManager::wait	(DWORD	sleep_time)
{
	// Wait for completition
	char		perf			[1024];
	for (;;)
	{
		Sleep	(sleep_time);
		
		perf[0]					=0;
		float	sumProgress		=0;
		float	sumPerformance	=0;
		DWORD	sumComplete		=0;
		for (DWORD ID=0; ID<threads.size(); ID++)
		{
			sumProgress			+= threads[ID]->thProgress;
			sumComplete			+= threads[ID]->thCompleted?1:0;
			sumPerformance		+= threads[ID]->thPerformance;

			char				P[64];
			if (ID)				sprintf	(P,"*%3.1f",threads[ID]->thPerformance);
			else				sprintf	(P," %3.1f",threads[ID]->thPerformance);
			strcat				(perf,P);
		}
		if (threads[0]->thMonitor)
		{
			Status	("Performance: %3.1f :%s",sumPerformance,perf);
		}
		Progress(sumProgress/float(threads.size()));
		if (sumComplete == threads.size())	break;
	}
	
	// Delete threads
	for (DWORD thID=0; thID<threads.size(); thID++)
		_DELETE(threads[thID]);
	threads.clear	();
}
