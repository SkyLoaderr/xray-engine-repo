////////////////////////////////////////////////////////////////////////////
//	Module 		: state_manager_combat.cpp
//	Created 	: 14.01.2004
//  Modified 	: 14.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker state manager combat
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "state_manager_combat.h"

CStateManagerCombat::CStateManagerCombat	()
{
	Init					();
}

CStateManagerCombat::~CStateManagerCombat	()
{
}

void CStateManagerCombat::Init			()
{
}

void CStateManagerCombat::Load			(LPCSTR section)
{
	inherited::Load			(section);
}

void CStateManagerCombat::reinit		(CAI_Stalker *object)
{
	inherited::reinit		(object);
}

void CStateManagerCombat::reload		(LPCSTR section)
{
	inherited::reload		(section);
}

void CStateManagerCombat::initialize	()
{
	inherited::initialize	();
}

void CStateManagerCombat::execute		()
{
	inherited::execute		();
}

void CStateManagerCombat::finalize		()
{
	inherited::finalize		();
}

void CStateManagerCombat::update		(u32 time_delta)
{
	inherited::update		(time_delta);
}
