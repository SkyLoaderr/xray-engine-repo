#include "stdafx.h"
#include "xrSheduler.h"

ENGINE_API float		psUpdateFar		= 200.f;
LPVOID 					fiber_main		= 0;
LPVOID					fiber_thread	= 0;

CSheduled::CSheduled	()	
{
	shedule_Min			= 20;
	shedule_Max			= 1000;
	shedule_TimeStamp	= 0;
	shedule_PMON		= 0;
	shedule_Register	();
}

CSheduled::~CSheduled	()
{
	shedule_Unregister	();
}

void	CSheduled::shedule_Register			()
{
	Engine.Sheduler.Register	(this);
}

void	CSheduled::shedule_Unregister		()
{
	Engine.Sheduler.Unregister	(this);
}

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
	R_ASSERT			(Items.empty());
	DeleteFiber			(fiber_thread);
}

void CSheduler::RegisterRT		(CSheduled* O)
{
	// Fill item structure
	Item						TNext;
	TNext.dwTimeForExecute		= Device.dwTimeGlobal+O->shedule_Min;
	TNext.dwTimeOfLastExecute	= Device.dwTimeGlobal;
	TNext.Object				= O;

	ItemsRT.push_back			(TNext);
}

void CSheduler::Register		(CSheduled* O)
{
	// Fill item structure
	Item						TNext;
	TNext.dwTimeForExecute		= Device.dwTimeGlobal+O->shedule_Min;
	TNext.dwTimeOfLastExecute	= Device.dwTimeGlobal;
	TNext.Object				= O;
	
	// Insert into priority Queue
	Push						(TNext);
}

void CSheduler::UnregisterRT	(CSheduled* O)
{
	for (DWORD i=0; i<ItemsRT.size(); i++)
	{
		if (ItemsRT[i].Object==O) {
			ItemsRT.erase(ItemsRT.begin()+i);
			return;
		}
	}
}

void CSheduler::Unregister		(CSheduled* O)
{
	for (DWORD i=0; i<Items.size(); i++)
	{
		if (Items[i].Object==O) {
			Items.erase(Items.begin()+i);
			return;
		}
	}
}

void CSheduler::Push		(Item& I)
{
	Items.push_back	(I);
	push_heap		(Items.begin(), Items.end());
}

void CSheduler::Pop		()
{
	pop_heap		(Items.begin(), Items.end());
	Items.pop_back	();
}

void CSheduler::ProcessStep			()
{
	DWORD	dwTime					= Device.dwTimeGlobal;

	// Normal priority
	if (Items.empty())				return;
	while (Top().dwTimeForExecute < dwTime)
	{
		// Update
		Item	T					= Top	();
		DWORD	Elapsed				= dwTime-T.dwTimeOfLastExecute;

		// Calc next update interval
		DWORD	dwMin				= T.Object->shedule_Min;
		DWORD	dwMax				= T.Object->shedule_Max;
		float	scale				= T.Object->shedule_Scale(); 
		DWORD	dwUpdate			= dwMin+iFloor(float(dwMax-dwMin)*scale);
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
		if (T.Object->Ready())		T.Object->Update		(Elapsed);

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

void CSheduler::Update				(DWORD mcs)
{
	DWORD	dwTime					= Device.dwTimeGlobal;

	// Realtime priority
	for (u32 it=0; it<ItemsRT.size(); it++)
	{
		Item&	T					= ItemsRT[it];
		DWORD	Elapsed				= dwTime-T.dwTimeOfLastExecute;
		if (T.Object->Ready())		T.Object->Update(Elapsed);
		T.dwTimeOfLastExecute		= dwTime;
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
