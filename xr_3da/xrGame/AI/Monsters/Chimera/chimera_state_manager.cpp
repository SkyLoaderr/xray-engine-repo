#include "stdafx.h"
#include "chimera.h"
#include "../States/monster_state_rest.h"
#include "../States/monster_state_rest_sleep.h"
#include "../States/monster_state_rest_walk_graph.h"
#include "../States/monster_state_attack.h"
#include "../States/monster_state_attack_run.h"
#include "../States/monster_state_attack_melee.h"

#include "chimera_state_threaten.h"
#include "../States/monster_state_threaten_walk.h"
#include "../States/monster_state_threaten_threaten.h"

CStateManagerChimera::CStateManagerChimera	(LPCSTR state_name) : inherited(state_name)
{
}

CStateManagerChimera::~CStateManagerChimera	()
{
}

void CStateManagerChimera::Load				(LPCSTR section)
{
	// set hierarchy
	add_state(
		xr_new<CStateMonsterRest<CChimera> > ("State Rest",
			xr_new<CStateMonsterRestSleep<CChimera> >("StateSleep"), 
			xr_new<CStateMonsterRestWalkGraph<CChimera> >("StateWalkGraph")),
		eStateRest, 1
	);
	
	add_state(
		xr_new<CStateMonsterAttack<CChimera> > ("State Attack",
			xr_new<CStateMonsterAttackRun<CChimera> >("State Attack Run"), 
			xr_new<CStateMonsterAttackMelee<CChimera> >("State Attack Melee")),
		eStateAttack, 1
	);

	add_state(
		xr_new<CStateChimeraThreaten<CChimera> > ("State Threaten",
			xr_new<CStateMonsterThreatenWalk<CChimera> >("State Threaten Walk"), 
			xr_new<CStateMonsterThreatenThreaten<CChimera> >("State Threaten Threaten")),
		eStateThreaten, 1
	);

	add_transition			(eStateRest,	eStateAttack,	1, 1);
	add_transition			(eStateRest,	eStateThreaten,	1, 1);
	add_transition			(eStateThreaten,eStateAttack,	1, 1);

	inherited::Load			(section);
}

void CStateManagerChimera::reinit			(CChimera *object)
{
	inherited::reinit		(object);

	set_current_state		(eStateRest);
	set_dest_state			(eStateRest);
}


void CStateManagerChimera::initialize		()
{
	inherited::initialize	();
}

void CStateManagerChimera::execute			()
{
	(m_object->m_tEnemy.obj) ? set_dest_state(eStateThreaten) : set_dest_state(eStateRest);
	
	inherited::execute		();
}

void CStateManagerChimera::finalize			()
{
	inherited::finalize		();
}

