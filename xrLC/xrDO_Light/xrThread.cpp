#include "stdafx.h"
#include "xrThread.h"

void __cdecl CThread::startup(void* P)
{
	CThread* T = (CThread*)P;

	Msg("* THREAD #%d: Started.",T->thID);
	FPU::m24r		();
	T->Execute		();
	T->thCompleted	= TRUE;
	Msg("* THREAD #%d: Task Completed.",T->thID);
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
	for (;;)
	{
		Sleep	(sleep_time);
		
		float	sumProgress=0;
		DWORD	sumComplete=0;
		for (DWORD ID=0; ID<threads.size(); ID++)
		{
			sumProgress += threads[ID]->thProgress;
			sumComplete	+= threads[ID]->thCompleted?1:0;
		}
		
		Progress(sumProgress/float(threads.size()));
		if (sumComplete == threads.size())	break;
	}
	
	// Delete threads
	for (DWORD thID=0; thID<threads.size(); thID++)
		_DELETE(threads[thID]);
	threads.clear	();
}
