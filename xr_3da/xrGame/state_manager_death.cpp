////////////////////////////////////////////////////////////////////////////
//	Module 		: state_manager_death.cpp
//	Created 	: 14.01.2004
//  Modified 	: 14.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker state manager death
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "state_manager_death.h"
#include "state_death_recently_dead.h"
#include "state_death_already_dead.h"

CStateManagerDeath::CStateManagerDeath	()
{
	Init					();
}

CStateManagerDeath::~CStateManagerDeath	()
{
}

void CStateManagerDeath::Init			()
{
	add						(xr_new<CStateDeathRecentlyDead>(),	eDeathStateRecentlyDead,	0);
	add						(xr_new<CStateDeathAlreadyDead>(),	eDeathStateAlreadyDead,		0);
	graph().add_edge		(eDeathStateRecentlyDead,eDeathStateAlreadyDead,1);
}

void CStateManagerDeath::Load			(LPCSTR section)
{
	inherited::Load			(section);
}

void CStateManagerDeath::reinit			(CAI_Stalker *object)
{
	inherited::reinit		(object,eDeathStateRecentlyDead);
}

void CStateManagerDeath::reload			(LPCSTR section)
{
	inherited::reload		(section);
}

void CStateManagerDeath::initialize		()
{
	inherited::initialize	();
}

void CStateManagerDeath::execute		()
{
	set_dest_vertex_id		(eDeathStateAlreadyDead);
	inherited::execute		();
}

void CStateManagerDeath::finalize		()
{
	inherited::finalize		();
}