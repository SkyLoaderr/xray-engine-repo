#include "stdafx.h"
#include "controller.h"


CController::CController()
{
	stateRest			= xr_new<CBitingRest>(this);
	CurrentState		= stateRest;

	Init();
}

CController::~CController()
{
	xr_delete(stateRest);
}


void CController::Init()
{
	inherited::Init();

	CurrentState					= stateRest;
	CurrentState->Reset				();
}

void CController::Load(LPCSTR section)
{
	inherited::Load	(section);

	BEGIN_LOAD_SHARED_MOTION_DATA();

	END_LOAD_SHARED_MOTION_DATA();
}

void CController::StateSelector()
{	
	SetState(stateRest);
}

