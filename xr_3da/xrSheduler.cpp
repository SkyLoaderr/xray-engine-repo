#include "stdafx.h"
#include "xrSheduler.h"
#include "xr_object.h"
#include "xr_creator.h"

ENGINE_API float	psUpdateFar	= 200.f;

CSheduled::CSheduled()	
{
	dwMinUpdate	= 20;
	dwMaxUpdate	= 500;
	dwTimeStamp = 0;
	pCreator->Objects.sheduled.Register(this);
}

CSheduled::~CSheduled()
{
	pCreator->Objects.sheduled.Unregister(this);
}

//-------------------------------------------------------------------------------------
void CSheduler::Register(CSheduled* O)
{
	// Fill item structure
	Item	TNext;
	TNext.dwTimeForExecute		= Device.dwTimeGlobal+O->dwMinUpdate;
	TNext.dwTimeOfLastExecute	= Device.dwTimeGlobal;
	TNext.Object				= O;
	
	// Insert into priority Queue
	Push						(TNext);
}
void CSheduler::Unregister(CSheduled* O)
{
	for (DWORD i=0; i<Items.size(); i++)
	{
		if (Items[i].Object==O) {
			Items.erase(Items.begin()+i);
			return;
		}
	}
}

void CSheduler::Update()
{
	DWORD	dwTime		= Device.dwTimeGlobal;
	DWORD	dwCount		= 0;
	if (Items.empty())	return;

	while (Top().dwTimeForExecute < dwTime)
	{
		dwCount++;

		// Update
		Item	T			= Top();
		DWORD	Elapsed		= dwTime-T.dwTimeOfLastExecute;

		// Calc next update interval
		DWORD	dwMin		= T.Object->dwMinUpdate;
		DWORD	dwMax		= T.Object->dwMaxUpdate;
		float	scale		= Device.vCameraPosition.distance_to(T.Object->Position())/psUpdateFar;
		DWORD	dwUpdate	= dwMin+iFloor(float(dwMax-dwMin)*scale);
		clamp	(dwUpdate,dwMin,dwMax);

		// Fill item structure
		Item	TNext;
		TNext.dwTimeForExecute		= dwTime+dwUpdate;
		TNext.dwTimeOfLastExecute	= dwTime;
		TNext.Object				= T.Object;

		// Insert into priority Queue
		Pop					();
		Push				(TNext);

		// Real update call
		if (T.Object->Ready())	T.Object->Update(Elapsed);
	}
}
