#include "stdafx.h"
#include "xrThread.h"

void __cdecl CThread::startup(void* P)
{
	CThread* T = (CThread*)P;

	Msg("* THREAD #%d: Started.",T->thID);
	T->Execute		();
	T->thCompleted	= TRUE;
	Msg("* THREAD #%d: Task Completed.",T->thID);
}
