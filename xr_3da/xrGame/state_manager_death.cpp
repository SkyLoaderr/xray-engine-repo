////////////////////////////////////////////////////////////////////////////
//	Module 		: state_manager_death.cpp
//	Created 	: 14.01.2004
//  Modified 	: 14.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker state manager death
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "state_manager_death.h"

CStateManagerDeath::CStateManagerDeath	()
{
	Init					();
}

CStateManagerDeath::~CStateManagerDeath	()
{
}

void CStateManagerDeath::Init				()
{
}

void CStateManagerDeath::Load				(LPCSTR section)
{
	inherited::Load			(section);
}

void CStateManagerDeath::reinit			(CAI_Stalker *object)
{
	inherited::reinit		(object);
}

void CStateManagerDeath::reload			(LPCSTR section)
{
	inherited::reload		(section);
}

void CStateManagerDeath::initialize		()
{
	inherited::initialize	();
}

void CStateManagerDeath::execute			()
{
	inherited::execute		();
}

void CStateManagerDeath::finalize			()
{
	inherited::finalize		();
}

void CStateManagerDeath::update			(u32 time_delta)
{
	inherited::update		(time_delta);
}
