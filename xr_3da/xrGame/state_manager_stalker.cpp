////////////////////////////////////////////////////////////////////////////
//	Module 		: state_manager_stalker.h
//	Created 	: 13.01.2004
//  Modified 	: 13.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker state manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "state_manager_stalker.h"

CStateManagerStalker::CStateManagerStalker	()
{
	Init					();
}

CStateManagerStalker::~CStateManagerStalker	()
{
}

void CStateManagerStalker::Init				()
{
//	add						(xr_new<CStateManagerDeath>(this),	eStalkerStateDeath,		0);
//	add						(xr_new<CStateManagerNoALife>(this),eStalkerStateNoALife,	3);
//	add						(xr_new<CStateManagerALife>(this),	eStalkerStateALife,		2);
//	add						(xr_new<CStateManagerCombat>(this),	eStalkerStateCombat,	1);
}

void CStateManagerStalker::Load				(LPCSTR section)
{
	inherited::Load			(section);
}

void CStateManagerStalker::reinit			()
{
	inherited::reinit		();
}

void CStateManagerStalker::reload			(LPCSTR section)
{
	inherited::reload		(section);
}

void CStateManagerStalker::update			(u32 time_delta)
{

}
