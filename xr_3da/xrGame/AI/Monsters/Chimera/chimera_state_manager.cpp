#include "stdafx.h"
#include "chimera.h"
#include "chimera_state_manager.h"
#include "../../ai_monster_debug.h"
#include "../../../actor.h"
#include "../../../level.h"
#include "../states/monster_state_rest.h"
#include "../states/monster_state_rest_sleep.h"
#include "../states/monster_state_rest_walk_graph.h"
#include "../states/monster_state_attack.h"
#include "../states/monster_state_attack_melee.h"
#include "../states/monster_state_attack_run.h"
#include "../states/monster_state_find_enemy.h"
#include "../states/monster_state_find_enemy_run.h"
#include "../states/monster_state_find_enemy_look.h"
#include "../states/monster_state_find_enemy_angry.h"
#include "../states/monster_state_find_enemy_walk.h"
#include "../states/state_move_to_point.h"
#include "../states/state_look_point.h"
#include "../states/state_custom_action.h"
#include "chimera_state_threaten.h"

CStateManagerChimera::CStateManagerChimera(CChimera *obj) : inherited(obj)
{
	//add_state(
	//	eStateRest, 
	//	xr_new<CStateMonsterRest<CChimera> > (obj, 
	//		xr_new<CStateMonsterRestSleep<CChimera> >(obj), 
	//		xr_new<CStateMonsterRestWalkGraph<CChimera> >(obj)
	//	)
	//);

	//add_state(
	//	eStateAttack, xr_new<CStateMonsterAttack<CChimera> > (obj,
	//		xr_new<CStateMonsterAttackRun<CChimera> >(obj), 
	//		xr_new<CStateMonsterAttackMelee<CChimera> >(obj)
	//	)
	//);

	//add_state(
	//	eStateFindEnemy, xr_new<CStateMonsterFindEnemy<CChimera> > (obj,
	//		xr_new<CStateMonsterFindEnemyRun<CChimera> >(obj), 
	//		xr_new<CStateMonsterFindEnemyLook<CChimera> >(obj,
	//			xr_new<CStateMonsterMoveToPoint<CChimera> >(obj), 
	//			xr_new<CStateMonsterCustomAction<CChimera> >(obj),
	//			xr_new<CStateMonsterLookToPoint<CChimera> >(obj)),
	//		xr_new<CStateMonsterFindEnemyAngry<CChimera> >(obj), 
	//		xr_new<CStateMonsterFindEnemyWalkAround<CChimera> >(obj)
	//	)
	//);
	

	add_state(eStateRest, xr_new<CStateChimeraTest<CChimera> > (obj));


//	add_state(
//		eStateThreaten, xr_new<CStateChimeraThreaten<CChimera> > (
//			xr_new<CStateMonsterThreatenWalk<CChimera> >, 
//			xr_new<CStateMonsterThreatenThreaten<CChimera> >
//		)	
//	);
//
//	add_state(
//		eStatePanic, xr_new<CStateMonsterPanic<CChimera> > (
//			xr_new<CStateMonsterPanicRun<CChimera> > 
//		)
//	);
}

CStateManagerChimera::~CStateManagerChimera()
{
}

void CStateManagerChimera::execute()
{
	u32 state_id = u32(-1);

	//if (object->EnemyMan.get_enemy()) {
	//	
	//	if (object->EnemyMan.get_enemy_time_last_seen() != object->m_current_update) {
	//		float dist = object->EnemyMan.get_enemy_position().distance_to(object->Position());

	//		if (prev_substate == eStateFindEnemy) state_id = eStateFindEnemy;
	//		else if (dist > 10.f) state_id = eStateFindEnemy;
	//	}
	//	if (state_id != eStateFindEnemy) state_id = eStateAttack;
	//}else state_id = eStateRest;

	state_id = eStateRest;

	select_state(state_id); 

	// выполнить текущее состояние
	get_state_current()->execute();

	prev_substate = current_substate;
}

