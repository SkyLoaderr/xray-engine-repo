#include "stdafx.h"
#include "ai_biting.h"

//#include "state_manager_stalker.h"
//#include "state_manager_death.h"
//#include "state_manager_no_alife.h"
//#include "state_manager_alife.h"
//#include "state_manager_combat.h"
//#include "ai/stalker/ai_stalker.h"

CStateManagerBiting::CStateManagerBiting	(LPCSTR state_name) : inherited(state_name)
{
	Init					();
}

CStateManagerBiting::~CStateManagerBiting	()
{
}

void CStateManagerBiting::Init				()
{
}

void CStateManagerBiting::Load				(LPCSTR section)
{
//	add_state				(xr_new<CStateManagerDeath>("DeathManager"),	eStalkerStateDeath,		0);
//	add_state				(xr_new<CStateManagerNoALife>("NoALife"),		eStalkerStateNoALife,	3);
//	add_state				(xr_new<CStateManagerCombat>("Combat"),			eStalkerStateCombat,	1);
//	//	add						(xr_new<CStateManagerALife>(),					eStalkerStateALife,		2);
//	add_transition			(eStalkerStateNoALife,eStalkerStateDeath,1);
//	add_transition			(eStalkerStateNoALife,eStalkerStateCombat,1,1);

	inherited::Load			(section);
}

void CStateManagerBiting::reinit			(CAI_Biting *object)
{
	inherited::reinit		(object);

//	set_current_state		(eStateRest);
//	set_dest_state			(eStateRest);
}

void CStateManagerBiting::reload			(LPCSTR section)
{
	inherited::reload		(section);
}

void CStateManagerBiting::initialize		()
{
	inherited::initialize	();
}

void CStateManagerBiting::execute			()
{

	
	//	if (m_object->g_Alive())
//		if (!m_object->enemy())
//			set_dest_state	(eStalkerStateNoALife);
//		else
//			set_dest_state	(eStalkerStateCombat);
//	else
//		set_dest_state		(eStalkerStateDeath);

	inherited::execute		();
}

void CStateManagerBiting::finalize			()
{
	inherited::finalize		();
}

