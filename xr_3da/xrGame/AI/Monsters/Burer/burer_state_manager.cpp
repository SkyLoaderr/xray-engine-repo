#include "stdafx.h"
#include "burer.h"
#include "../States/monster_state_rest.h"
#include "../States/monster_state_rest_sleep.h"
#include "../States/monster_state_attack.h"
#include "../States/monster_state_attack_run.h"
#include "../States/monster_state_attack_melee.h"


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
		xr_new<CStateMonsterAttack<CBurer> > ("State Attack",
			xr_new<CStateMonsterAttackRun<CBurer> >("State Attack Run"), 
			xr_new<CStateMonsterAttackMelee<CBurer> >("State Attack Melee")),
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
	(m_object->m_tEnemy.obj) ? set_dest_state(eStateAttack) : set_dest_state(eStateRest);

	inherited::execute		();
}

void CStateManagerBurer::finalize			()
{
	inherited::finalize		();
}

