////////////////////////////////////////////////////////////////////////////
//	Module 		: state_manager_stalker.h
//	Created 	: 13.01.2004
//  Modified 	: 13.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker state manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "state_manager_stalker.h"
#include "state_manager_death.h"
#include "state_manager_no_alife.h"
#include "state_manager_alife.h"
#include "state_manager_combat.h"
#include "ai/stalker/ai_stalker.h"

CStateManagerStalker::CStateManagerStalker	()
{
	Init					();
}

CStateManagerStalker::~CStateManagerStalker	()
{
}

void CStateManagerStalker::Init				()
{
	add_state				(xr_new<CStateManagerDeath>(),	eStalkerStateDeath,		0);
	add_state				(xr_new<CStateManagerNoALife>(),eStalkerStateNoALife,	3);
	add_transition			(eStalkerStateNoALife,eStalkerStateDeath,1);
//	add						(xr_new<CStateManagerALife>(),	eStalkerStateALife,		2);
//	add						(xr_new<CStateManagerCombat>(),	eStalkerStateCombat,	1);
}

void CStateManagerStalker::Load				(LPCSTR section)
{
	inherited::Load			(section);
}

void CStateManagerStalker::reinit			(CAI_Stalker *object)
{
	inherited::reinit		(object,eStalkerStateNoALife);
}

void CStateManagerStalker::reload			(LPCSTR section)
{
	inherited::reload		(section);
}

void CStateManagerStalker::initialize		()
{
	inherited::initialize	();
}

void CStateManagerStalker::execute			()
{
	if (m_object->g_Alive())
		set_dest_state		(eStalkerStateNoALife);
	else
		set_dest_state		(eStalkerStateDeath);
	inherited::execute		();
}

void CStateManagerStalker::finalize			()
{
	inherited::finalize		();
}
