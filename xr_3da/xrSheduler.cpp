#include "stdafx.h"
#include "xrSheduler.h"
#include "xr_object.h"

float			psShedulerCurrent		= 10.f	;
float			psShedulerTarget		= 10.f	;
const	float	psShedulerReaction		= 0.1f	;
/*
LPVOID 		fiber_main				= 0;
LPVOID		fiber_thread			= 0;
*/

//-------------------------------------------------------------------------------------
/*
VOID CALLBACK t_process			(LPVOID p)
{
	Engine.Sheduler.Process		();
}
*/
void CSheduler::Initialize		()
{
//	fiber_main			= ConvertThreadToFiber	(0);
//	fiber_thread		= CreateFiber			(0,t_process,0);
//	fibered				= FALSE;
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
#ifdef DEBUG	
	if (!Items.empty())
	{
		string1024	_objects; _objects[0]=0;
		string64	_hex;
		for (u32 it=0; it<Items.size(); it++)
		{
			CObject*	O	= dynamic_cast<CObject*> (Items[it].Object);
			if (O)			strconcat(_objects,*O->cName(),":",*O->cNameSect(),",");
			else			{ 
				sprintf		(_hex,"%X",size_t(Items[it].Object));
				strconcat	(_objects,"unknown:",_hex,",");
			}
		}
		Debug.fatal		("Sheduler work-list is not empty\n%s",_objects);
	}
#endif

//	DeleteFiber			(fiber_thread);
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
	// Normal priority
	u32		dwTime					= Device.dwTimeGlobal;
	CTimer							eTimer;
	while (!Items.empty() && Top().dwTimeForExecute < dwTime)
	{
		u32		delta_ms			= dwTime - Top().dwTimeForExecute;

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
		u32		dwMin				= _max(u32(30),T.Object->shedule.t_min);
		u32		dwMax				= (1000+T.Object->shedule.t_max)/2;
		float	scale				= T.Object->shedule_Scale	(); 
		u32		dwUpdate			= dwMin+iFloor(float(dwMax-dwMin)*scale);
		clamp	(dwUpdate,u32(_max(dwMin,u32(20))),dwMax);

		// Fill item structure
		Item	TNext;
		TNext.dwTimeForExecute		= dwTime+dwUpdate;
		TNext.dwTimeOfLastExecute	= dwTime;
		TNext.Object				= T.Object;

		// Insert into priority Queue
		Pop							();
		ItemsProcessed.push_back	(TNext);

		// Real update call
		// Msg						("------- %d:",Device.dwFrame);
#ifdef DEBUG
		T.Object->dbg_startframe	= Device.dwFrame;
		eTimer.Start				();
#endif

		T.Object->shedule.b_locked	= TRUE;
		T.Object->shedule_Update	(Elapsed);
		T.Object->shedule.b_locked	= FALSE;

#ifdef DEBUG
		u32	execTime				= eTimer.GetElapsed_ms		();
		//void*	dbgaddr				= dynamic_cast<void*>		(T.Object);
		CObject*	O				= dynamic_cast<CObject*>	(T.Object);
		LPCSTR		N				= O?O->cName().c_str():"unknown";
		VERIFY3						(T.Object->dbg_update_shedule == T.Object->dbg_startframe, "Broken sequence of calls to 'shedule_Update'", O?*O->cName():"unknown object" );
		if (delta_ms> 3*dwUpdate)	{
			//Msg	("! xrSheduler: failed to shedule object [%s] (%dms)",		N, delta_ms	);
		}
		if (execTime> 10)			{
			Msg	("! xrSheduler: too much time consumed by object [%s] (%dms)",	N, execTime	);
		}
#endif

		// 
		if (CPU::GetCycleCount() > cycles_limit)	{
			// we have maxed out the load - increase heap
			psShedulerTarget	+= psShedulerReaction * 2;
			break;
		}
	}

	// Push "processed" back
	while (ItemsProcessed.size())	{
		Push	(ItemsProcessed.back())	;
		ItemsProcessed.pop_back		()	;
	}

	// always try to decrease target
	psShedulerTarget	-= psShedulerReaction;
}
/*
void CSheduler::Switch				()
{
	if (fibered)	
	{
		fibered						= FALSE;
		SwitchToFiber				(fiber_main);
	}
}
*/
void CSheduler::Update				()
{
	u32	dwTime						= Device.dwTimeGlobal;

	// Log				("------------- CSheduler::Update: ",u32(Device.dwFrame));

	// Realtime priority
	for (u32 it=0; it<ItemsRT.size(); it++)
	{
		Item&	T						= ItemsRT[it];
		u32	Elapsed						= dwTime-T.dwTimeOfLastExecute;
#ifdef DEBUG
		T.Object->dbg_startframe		= Device.dwFrame;
#endif
		T.Object->shedule_Update		(Elapsed);
		T.dwTimeOfLastExecute			= dwTime;
	}

	// Normal (sheduled)
	Device.Statistic.Sheduler.Begin	();
	cycles_start					= CPU::GetCycleCount			();
	cycles_limit					= CPU::cycles_per_milisec * u64	(iCeil(psShedulerCurrent)) + cycles_start;
	ProcessStep						();
	clamp							(psShedulerTarget,3.f,50.f);
	psShedulerCurrent				= 0.9f*psShedulerCurrent + 0.1f*psShedulerTarget;
	Device.Statistic.Sheduler.End	();
}
