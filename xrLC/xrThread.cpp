#include "stdafx.h"
#include "xrThread.h"

void __cdecl CThread::startup(void* P)
{
	CThread* T = (CThread*)P;

	Msg("* THREAD #%d: Started.",T->ID);
	T->Execute		();
	T->bCompleted	= TRUE;
	Msg("* THREAD #%d: Task Completed.",T->ID);
}
