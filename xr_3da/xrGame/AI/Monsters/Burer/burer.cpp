#include "stdafx.h"
#include "burer.h"


CBurer::CBurer()
{
	stateRest			= xr_new<CBitingRest>(this);
	CurrentState		= stateRest;

	Init();
}

CBurer::~CBurer()
{
	xr_delete(stateRest);
}


void CBurer::Init()
{
	inherited::Init();

	CurrentState					= stateRest;
	CurrentState->Reset				();
}

void CBurer::Load(LPCSTR section)
{
	inherited::Load	(section);

	BEGIN_LOAD_SHARED_MOTION_DATA();

	END_LOAD_SHARED_MOTION_DATA();
}

void CBurer::StateSelector()
{	
	SetState(stateRest);
}

