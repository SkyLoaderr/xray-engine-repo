#include "stdafx.h"
#include "xrSheduler.h"
#include "xr_object.h"
#include "xr_creator.h"

ENGINE_API float		psUpdateFar	= 200.f;

CSheduled::CSheduled	()	
{
	shedule_Min			= 20;
	shedule_Max			= 1000;
	shedule_TimeStamp	= 0;
	shedule_Register	();
}

CSheduled::~CSheduled	()
{
	shedule_Unregister	();
}
void	CSheduled::shedule_Register			(DWORD priority)
{
	pCreator->Objects.sheduled.Register		(this, priority);
}
void	CSheduled::shedule_Unregister		()
{
	pCreator->Objects.sheduled.Unregister	(this);
}

//-------------------------------------------------------------------------------------
void CSheduler::Register		(CSheduled* O, DWORD priority)
{

	// Fill item structure
	Item	TNext;
	TNext.dwTimeForExecute		= Device.dwTimeGlobal+O->shedule_Min;
	TNext.dwTimeOfLastExecute	= Device.dwTimeGlobal;
	TNext.Object				= O;
	
	// Insert into priority Queue
	Push						(priority, TNext);
}

void CSheduler::Unregister		(CSheduled* O)
{
	for (DWORD P=0; P<3; P++) 
	{
		for (DWORD i=0; i<Items.size(); i++)
		{
			if (Items[i].Object==O) {
				Items.erase(Items.begin()+i);
				return;
			}
		}
	}
}

void CSheduler::UpdateLevel			(DWORD Priority, DWORD mcs)
{
	DWORD	dwTime					= Device.dwTimeGlobal;
	DWORD	dwCount					= 0;
	if (Items[Priority].empty())	return;

	while (Top().dwTimeForExecute < dwTime)
	{
		dwCount	++;

		// Update
		Item	T			= Top(Priority);
		DWORD	Elapsed		= dwTime-T.dwTimeOfLastExecute;

		// Calc next update interval
		DWORD	dwMin		= T.Object->shedule_Min;
		DWORD	dwMax		= T.Object->shedule_Max;
		float	scale		= Device.vCameraPosition.distance_to(T.Object->Position())/psUpdateFar;
		DWORD	dwUpdate	= dwMin+iFloor(float(dwMax-dwMin)*scale);
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
		if (T.Object->Ready())		T.Object->Update(Elapsed);
	}
}
void CSheduler::Update				()
{
	UpdateLevel		(0, 500);	// LOW
	UpdateLevel		(1,3000);	// NORMAL
	UpdateLevel		(2,3000);	// HIGH
}
