#include "stdafx.h"
#include "ftimer.h"
#include "process.h"

LPVOID	fiber_sheduler		= 0;
LPVOID	fiber_processor		= 0;

volatile int	dummy		= 0;

VOID	CALLBACK		t_process	(PVOID P)
{
	for (;;)
	{
		int	rnd = ::Random.randI		();
		int lcp = _max(rnd,10);
		for (int it=0; it<lcp; it++)	dummy += rnd;
	}
}

void	t_sheduler		()
{
	SwitchToFiber		(fiber_processor);
}

volatile u64 test		= 0;
void __cdecl t_thread	(void * P)
{
	for (;;) test++;
}

#define TESTS	1000

void	t_test			()
{
	CTimer				T;
	SetPriorityClass	(GetCurrentProcess(),HIGH_PRIORITY_CLASS);

	// Make sure the thread is started
	_beginthread		(t_thread,0,0);
	Sleep				(5);

	u64		C			= 0;
	u64		iter		= 0;
	for (int i=0; i<TESTS; i++)
	{
		T.Start				();
		test				=	0;
		Sleep				(0);
		iter				+=	test;
		C					+=	T.GetElapsed	();
	}
	SetPriorityClass	(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);
	float mcs			=  float(u64(C)/u64(TESTS))*CPU::cycles2milisec;
	iter				/= u64(TESTS);
	printf				("%f, it: %d\n",mcs,int(iter));

	// Create fibers
	fiber_sheduler		= ConvertThreadToFiber	(0);
	fiber_processor		= CreateFiber			(0,t_process,0);

	// Process tests
	for (int i=0; i<100; i++)
	{
		Sleep		(5);
		t_sheduler	();
	}
}
