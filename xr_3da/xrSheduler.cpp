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

void	CSheduled::shedule_Register			(DWORD priority)
{
	Engine.Sheduler.Register	(this, priority);
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
}
void CSheduler::Destroy			()
{
	R_ASSERT			(Items.empty());
	DeleteFiber			(fiber_thread);
	DeleteFiber			(fiber_main);
}

void CSheduler::Register		(CSheduled* O, DWORD priority)
{
	// Fill item structure
	Item						TNext;
	TNext.dwTimeForExecute		= Device.dwTimeGlobal+O->shedule_Min;
	TNext.dwTimeOfLastExecute	= Device.dwTimeGlobal;
	TNext.Object				= O;
	
	// Insert into priority Queue
	Push						(TNext);
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
	if (Items.empty())				return;

	DWORD	dwTime					= Device.dwTimeGlobal;
	while (Top().dwTimeForExecute < dwTime)
	{
		// Update
		Item	T					= Top	(Priority);
		DWORD	Elapsed				= dwTime-T.dwTimeOfLastExecute;

		// Calc next update interval
		DWORD	dwMin				= T.Object->shedule_Min;
		DWORD	dwMax				= T.Object->shedule_Max;
		float	scale				= Device.vCameraPosition.distance_to(T.Object->Position())/psUpdateFar;
		DWORD	dwUpdate			= dwMin+iFloor(float(dwMax-dwMin)*scale);
		clamp	(dwUpdate,dwMin,dwMax);

		// Fill item structure
		Item	TNext;
		TNext.dwTimeForExecute		= dwTime+dwUpdate;
		TNext.dwTimeOfLastExecute	= dwTime;
		TNext.Object				= T.Object;

		// Insert into priority Queue
		Pop							(Priority);
		Push						(Priority, TNext);

		// Real update call
		if (T.Object->Ready())		T.Object->Update		(Elapsed);

		Slice						();
		dwTime						= Device.dwTimeGlobal;
	}
}

void CSheduler::Switch				()
{
	SwitchToFiber					(fiber_main);
}

void CSheduler::Update				(DWORD mcs)
{
	Device.Statistic.Sheduler.Begin	();
	cycles_limit					= CPU::cycles_per_microsec * u64(mcs);
	cycles_start					= CPU::GetCycleCount();
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
