#include "stdafx.h"

LPVOID	fiber_sheduler		= 0;
LPVOID	fiber_processor		= 0;

volatile int	dummy		= 0;

VOID	CALLBACK		terminator	(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	SwitchToFiber		(fiber_sheduler);
}

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
	timeSetEvent		(10,0,terminator,0,TIME_ONESHOT | TIME_CALLBACK_FUNCTION);
	SwitchToFiber		(fiber_processor);
}

void	t_test()
{
	// 
	fiber_sheduler		= ConvertThreadToFiber	(0);
	fiber_processor		= CreateFiber			(0,t_process,0);

	// Process tests
	for (int i=0; i<100; i++)
	{
		Sleep		(5);
		t_sheduler	();
	}
}
