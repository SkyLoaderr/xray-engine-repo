#include "stdafx.h"
#include "zombie.h"
#include "zombie_state_manager.h"
#include "../states/monster_state_rest.h"
#include "../states/monster_state_rest_sleep.h"
#include "../states/monster_state_rest_walk_graph.h"

#include "../../ai_monster_debug.h"

CStateManagerZombie::CStateManagerZombie(CZombie *obj) : inherited(obj)
{
	add_state(
		eStateRest, 
			xr_new<CStateMonsterRest<CZombie> > (obj, 
			xr_new<CStateMonsterRestSleep<CZombie> >(obj), 
			xr_new<CStateMonsterRestWalkGraph<CZombie> >(obj)
			)
		);


	//add_state(
	//	eStateAttack, 
	//	xr_new<CStateControllerAttack<CZombie> > (obj,
	//		xr_new<CStateMonsterAttackRun<CZombie> >(obj), 
	//		xr_new<CStateMonsterAttackMelee<CZombie> >(obj)
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

CStateManagerZombie::~CStateManagerZombie()
{
}

void CStateManagerZombie::initialize()
{
	inherited::initialize();

}

void CStateManagerZombie::execute()
{
	//u32 state_id = u32(-1);

	select_state(eStateRest); 

	// выполнить текущее состояние
	get_state_current()->execute();

	prev_substate = current_substate;
}

