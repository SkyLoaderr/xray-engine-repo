#include "stdafx.h"
#include "xrSheduler.h"
#include "xr_object.h"

float					psUpdateFar		= 200.f;
int						psSheduler		= 3000;
LPVOID 					fiber_main		= 0;
LPVOID					fiber_thread	= 0;

//-------------------------------------------------------------------------------------
VOID CALLBACK t_process			(LPVOID p)
{
	Engine.Sheduler.Process		();
}

void CSheduler::Initialize		()
{
	fiber_main			= ConvertThreadToFiber	(0);
	fiber_thread		= CreateFiber			(0,t_process,0);
	fibered				= FALSE;
}

void CSheduler::Destroy			()
{
	for (u32 it=0; it<Items.size(); it++)
	{
		if (0==Items[it].Object)	
		{
			Items.erase(Items.begin()+it);
			it	--;
		}
	}

	R_ASSERT			(Items.empty()); 
	DeleteFiber			(fiber_thread);
}

void CSheduler::Register		(ISheduled* O, BOOL RT)
{
	if (RT)
	{
		// Fill item structure
		Item						TNext;
		TNext.dwTimeForExecute		= Device.dwTimeGlobal+O->shedule.t_min;
		TNext.dwTimeOfLastExecute	= Device.dwTimeGlobal;
		TNext.Object				= O;
		O->shedule.b_RT				= TRUE;

		ItemsRT.push_back			(TNext);
	} else {
		// Fill item structure
		Item						TNext;
		TNext.dwTimeForExecute		= Device.dwTimeGlobal+O->shedule.t_min;
		TNext.dwTimeOfLastExecute	= Device.dwTimeGlobal;
		TNext.Object				= O;
		O->shedule.b_RT				= FALSE;

		// Insert into priority Queue
		Push						(TNext);
	}
}

void CSheduler::Unregister		(ISheduled* O)
{
	if (O->shedule.b_RT)
	{
		for (u32 i=0; i<ItemsRT.size(); i++)
		{
			if (ItemsRT[i].Object==O) {
				ItemsRT.erase(ItemsRT.begin()+i);
				return;
			}
		}
	} else {
		for (u32 i=0; i<Items.size(); i++)
		{
			if (Items[i].Object==O) {
				Items[i].Object	= NULL;
				return;
			}
		}
	}
}

void CSheduler::EnsureOrder	(ISheduled* Before, ISheduled* After)
{
	VERIFY(Before->shedule.b_RT && After->shedule.b_RT);

	for (u32 i=0; i<ItemsRT.size(); i++)
	{
		if (ItemsRT[i].Object==After) 
		{
			Item	A			= ItemsRT[i];
			ItemsRT.erase		(ItemsRT.begin()+i);
			ItemsRT.push_back	(A);
			return;
		}
	}
}

void CSheduler::Push		(Item& I)
{
	Items.push_back	(I);
	std::push_heap	(Items.begin(), Items.end());
}

void CSheduler::Pop		()
{
	std::pop_heap	(Items.begin(), Items.end());
	Items.pop_back	();
}

void CSheduler::ProcessStep			()
{
	u32	dwTime					= Device.dwTimeGlobal;

	// Normal priority
	CTimer						eTimer;
	while (!Items.empty() && Top().dwTimeForExecute < dwTime)
	{
		// Update
		Item	T					= Top	();
		u32		Elapsed				= dwTime-T.dwTimeOfLastExecute;
		if (NULL==T.Object)			
		{
			// Erase element
			Pop						();
			continue;
		}

		// Calc next update interval
		u32		dwMin				= T.Object->shedule.t_min;
		u32		dwMax				= T.Object->shedule.t_max;
		float	scale				= T.Object->shedule_Scale	(); 
		u32		dwUpdate			= dwMin+iFloor(float(dwMax-dwMin)*scale);
		clamp	(dwUpdate,dwMin,dwMax);

		// Fill item structure
		Item	TNext;
		TNext.dwTimeForExecute		= dwTime+dwUpdate;
		TNext.dwTimeOfLastExecute	= dwTime;
		TNext.Object				= T.Object;

		// Insert into priority Queue
		Pop							();
		Push						(TNext);

		// Real update call
		eTimer.Start				();
		T.Object->shedule.b_locked	= TRUE;
		T.Object->shedule_Update	(Elapsed);
		T.Object->shedule.b_locked	= FALSE;
		u32	execTime				= eTimer.GetElapsed_ms		();
		if (execTime>3)
		{
			CObject*	O				= dynamic_cast<CObject*> (T.Object);
			if (O)						Msg	("! xrSheduler: object exceed timelimit (%s / %dms)",O->cName(),execTime);
			else						Msg	("! xrSheduler: object exceed timelimit (%x / %dms)",T.Object,execTime);
		}	

		Slice						();
	}
}

void CSheduler::Switch				()
{
	if (fibered)	
	{
		fibered						= FALSE;
		SwitchToFiber				(fiber_main);
	}
}

void CSheduler::Update				()
{
	u32	mcs						= psSheduler;
	u32	dwTime					= Device.dwTimeGlobal;

	// Realtime priority
	for (u32 it=0; it<ItemsRT.size(); it++)
	{
		Item&	T						= ItemsRT[it];
		u32	Elapsed						= dwTime-T.dwTimeOfLastExecute;
		T.Object->shedule_Update		(Elapsed);
		T.dwTimeOfLastExecute			= dwTime;
	}

	// Normal (sheduled)
	Device.Statistic.Sheduler.Begin	();
	cycles_limit					= CPU::cycles_per_microsec * u64(mcs);
	cycles_start					= CPU::GetCycleCount();
	fibered							= TRUE;
	SwitchToFiber					(fiber_thread);
	Device.Statistic.Sheduler.End	();
}

void CSheduler::Process				()
{
	for (;;)
	{
		ProcessStep	();
		Switch		();
	}
}
