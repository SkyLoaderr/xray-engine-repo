#include "stdafx.h"
#include "controller.h"
#include "controller_state_manager.h"
#include "../controlled_entity.h"

#include "../../../phmovementcontrol.h"
#include "../../../PhysicsShell.h"
#include "../../../PHCharacter.h"
#include "../../../phcapture.h"

#include "../ai_monster_debug.h"

#include "../states/monster_state_rest.h"
#include "../states/monster_state_rest_sleep.h"
#include "../states/monster_state_rest_walk_graph.h"
#include "controller_state_attack.h"
#include "../states/monster_state_attack_melee.h"
#include "../states/monster_state_attack_run.h"
#include "../states/monster_state_eat.h"
#include "../states/monster_state_eat_eat.h"
#include "../states/monster_state_panic.h"
#include "../states/monster_state_hear_int_sound.h"
#include "../states/monster_state_hear_danger_sound.h"
#include "../states/monster_state_hitted.h"

#include "../states/monster_state_find_enemy.h"
#include "../states/monster_state_find_enemy_run.h"
#include "../states/monster_state_find_enemy_look.h"
#include "../states/monster_state_find_enemy_angry.h"
#include "../states/monster_state_find_enemy_walk.h"
#include "../states/state_move_to_point.h"
#include "../states/state_look_point.h"
#include "../states/state_custom_action.h"

CStateManagerController::CStateManagerController(CController *obj) : inherited(obj)
{
	add_state(
		eStateRest, 
		xr_new<CStateMonsterRest<CController> > (obj, 
			xr_new<CStateMonsterRestSleep<CController> >(obj), 
			xr_new<CStateMonsterRestWalkGraph<CController> >(obj)
		)
	);

	add_state(
		eStateAttack, 
		xr_new<CStateControllerAttack<CController> > (obj,
			xr_new<CStateMonsterAttackRun<CController> >(obj), 
			xr_new<CStateMonsterAttackMelee<CController> >(obj)
		)
	);


	add_state(
		eStateEat,
		xr_new<CStateMonsterEat<CController> >(obj,
			xr_new<CStateMonsterEating<CController> >(obj)
		)
	);

	add_state(
		eStatePanic,
		xr_new<CStateMonsterPanic<CController> >(obj)
	);

	add_state(
		eStateHitted,
		xr_new<CStateMonsterHitted<CController> >(obj)
	);

	add_state(
		eStateInterestingSound,
		xr_new<CStateMonsterHearInterestingSound<CController> >(obj)
	);

	add_state(
		eStateDangerousSound,
		xr_new<CStateMonsterHearDangerousSound<CController> >(obj)
	);


	add_state(
		eStateFindEnemy, 
		xr_new<CStateMonsterFindEnemy<CController> > (obj,
			xr_new<CStateMonsterFindEnemyRun<CController> >(obj), 
			xr_new<CStateMonsterFindEnemyLook<CController> >(obj,
				xr_new<CStateMonsterMoveToPoint<CController> >(obj), 
				xr_new<CStateMonsterCustomAction<CController> >(obj),
				xr_new<CStateMonsterLookToPoint<CController> >(obj)),
			xr_new<CStateMonsterFindEnemyAngry<CController> >(obj), 
			xr_new<CStateMonsterFindEnemyWalkAround<CController> >(obj)
		)
	);
}

CStateManagerController::~CStateManagerController()
{
}

#define FIND_ENEMY_TIME_ENEMY_HIDDEN	5000
#define FIND_ENEMY_MAX_DISTANCE			10.f

void CStateManagerController::execute()
{
	u32 state_id = u32(-1);
		
	const CEntityAlive* enemy	= object->EnemyMan.get_enemy();
	const CEntityAlive* corpse	= object->CorpseMan.get_corpse();

	if (enemy) {
		switch (object->EnemyMan.get_danger_type()) {
			case eVeryStrong:	state_id = eStatePanic; break;
			case eStrong:		
			case eNormal:
			case eWeak:			state_id = eStateAttack; break;
		}
		
		if (state_id == eStateAttack) {
			if (object->EnemyMan.get_enemy_time_last_seen() + FIND_ENEMY_TIME_ENEMY_HIDDEN < object->m_current_update) {
				if (prev_substate == eStateFindEnemy) state_id = eStateFindEnemy;
				else {
					if (object->EnemyMan.get_enemy_position().distance_to(object->Position()) < FIND_ENEMY_MAX_DISTANCE) state_id = eStateFindEnemy;
				}
			}
		}
	} else if (object->HitMemory.is_hit()) {
		state_id = eStateHitted;
	} else if (object->hear_dangerous_sound) {
		state_id = eStateDangerousSound;
	} else if (object->hear_interesting_sound) {
		state_id = eStateInterestingSound;
	} else {
		bool can_eat = false;
		if (corpse) {
			
			if (prev_substate == eStateEat) {
				if (!get_state_current()->check_completion()) can_eat = true;
			}
			
			if ((prev_substate != eStateEat) && (object->GetSatiety() < object->get_sd()->m_fMinSatiety)) 
				can_eat = true;		
		}
		
		if (can_eat) state_id = eStateEat;
		else state_id = eStateRest;
	}
	
	if (enemy) object->set_controlled_task(eTaskAttack);
	else object->set_controlled_task(eTaskFollow);

	select_state(state_id); 

	// выполнить текущее состояние
	get_state_current()->execute();

	prev_substate = current_substate;
}
