#include "stdafx.h"
#include "poltergeist.h"
#include "poltergeist_state_manager.h"
#include "../states/monster_state_rest.h"
#include "../states/monster_state_rest_sleep.h"
#include "../states/monster_state_rest_walk_graph.h"

#include "../../ai_monster_debug.h"

CStateManagerPoltergeist::CStateManagerPoltergeist(CPoltergeist *obj) : inherited(obj)
{
	add_state(
		eStateRest, 
		xr_new<CStateMonsterRest<CPoltergeist> > (obj, 
			xr_new<CStateMonsterRestSleep<CPoltergeist> >(obj), 
			xr_new<CStateMonsterRestWalkGraph<CPoltergeist> >(obj)
		)
	);

	//add_state(
	//	eStateAttack, 
	//	xr_new<CStateControllerAttack<CController> > (obj,
	//		xr_new<CStateMonsterAttackRun<CController> >(obj), 
	//		xr_new<CStateMonsterAttackMelee<CController> >(obj)
	//	)
	//);


	//add_state(
	//	eStateEat,
	//	xr_new<CStateMonsterEat<CController> >(obj,
	//	xr_new<CStateMonsterEating<CController> >(obj)
	//	)
	//	);

	//add_state(
	//	eStatePanic,
	//	xr_new<CStateMonsterPanic<CController> >(obj)
	//	);

	//add_state(
	//	eStateHitted,
	//	xr_new<CStateMonsterHitted<CController> >(obj)
	//	);

	//add_state(
	//	eStateInterestingSound,
	//	xr_new<CStateMonsterHearInterestingSound<CController> >(obj)
	//	);

	//add_state(
	//	eStateDangerousSound,
	//	xr_new<CStateMonsterHearDangerousSound<CController> >(obj)
	//	);


	//add_state(
	//	eStateFindEnemy, 
	//	xr_new<CStateMonsterFindEnemy<CController> > (obj,
	//	xr_new<CStateMonsterFindEnemyRun<CController> >(obj), 
	//	xr_new<CStateMonsterFindEnemyLook<CController> >(obj,
	//	xr_new<CStateMonsterMoveToPoint<CController> >(obj), 
	//	xr_new<CStateMonsterCustomAction<CController> >(obj),
	//	xr_new<CStateMonsterLookToPoint<CController> >(obj)),
	//	xr_new<CStateMonsterFindEnemyAngry<CController> >(obj), 
	//	xr_new<CStateMonsterFindEnemyWalkAround<CController> >(obj)
	//	)
	//	);
}

CStateManagerPoltergeist::~CStateManagerPoltergeist()
{
}


void CStateManagerPoltergeist::execute()
{
	u32 state_id = u32(-1);

	//const CEntityAlive* enemy	= object->EnemyMan.get_enemy();
	//const CEntityAlive* corpse	= object->CorpseMan.get_corpse();

	//if (enemy) {
	//	if (object->m_hidden) {
	//		state_id = eStateAttackHidden;
	//	} else 
	//		state_id = eStateAttack;
	//}

	state_id = eStateRest;
	
	select_state(state_id); 

	// выполнить текущее состояние
	get_state_current()->execute();

	prev_substate = current_substate;
}
