#include "stdafx.h"
#include "burer.h"
#include "../States/monster_state_rest.h"
#include "../States/monster_state_rest_sleep.h"
#include "../States/monster_state_attack_melee.h"
#include "burer_state_attack.h"
#include "burer_state_attack_tele.h"


CStateManagerBurer::CStateManagerBurer(LPCSTR state_name) : inherited(state_name)
{
}

CStateManagerBurer::~CStateManagerBurer()
{
}

void CStateManagerBurer::Load(LPCSTR section)
{
	// set hierarchy
	add_state(
		xr_new<CStateMonsterRest<CBurer> > ("State Rest",
			xr_new<CStateMonsterRestSleep<CBurer> >("StateSleep"), 
			xr_new<CStateMonsterRestSleep<CBurer> >("StateSleep")),
		eStateRest, 1
		);

	add_state(
		xr_new<CStateBurerAttack<CBurer> > ("State Attack",
			xr_new<CStateBurerAttackTele<CBurer> >("StateAttackTele"), 
			xr_new<CStateBurerAttackTele<CBurer> >("StateAttackTele"),
			xr_new<CStateMonsterAttackMelee<CBurer> >("StateAttackMelee")),
		eStateAttack, 1
		);

	add_transition			(eStateRest,	eStateAttack,	1, 1);

	inherited::Load			(section);
}

void CStateManagerBurer::reinit			(CBurer *object)
{
	inherited::reinit		(object);

	set_current_state		(eStateRest);
	set_dest_state			(eStateRest);
}


void CStateManagerBurer::initialize		()
{
	inherited::initialize	();
}

void CStateManagerBurer::execute			()
{
	(m_object->EnemyMan.get_enemy()) ? set_dest_state(eStateAttack) : set_dest_state(eStateRest);

	inherited::execute		();
}

void CStateManagerBurer::finalize			()
{
	inherited::finalize		();
}

