#include "stdafx.h"
#include "chimera.h"

CChimera::CChimera()
{
	stateRest			= xr_new<CBitingRest>(this);
	CurrentState		= stateRest;

	Init();
}

CChimera::~CChimera()
{
	xr_delete(stateRest);
}


void CChimera::Init()
{
	inherited::Init();

	CurrentState					= stateRest;
	CurrentState->Reset				();
}

void CChimera::Load(LPCSTR section)
{
	inherited::Load	(section);

	BEGIN_LOAD_SHARED_MOTION_DATA();

	END_LOAD_SHARED_MOTION_DATA();
}

void CChimera::StateSelector()
{	
	SetState(stateRest);
}

