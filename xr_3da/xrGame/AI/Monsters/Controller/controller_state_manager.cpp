#include "stdafx.h"
#include "controller.h"
#include "controller_state_manager.h"
#include "../../controlled_entity.h"

#include "../../../phmovementcontrol.h"
#include "../../../PhysicsShell.h"
#include "../../../PHCharacter.h"
#include "../../../phcapture.h"

#include "../../ai_monster_debug.h"

#include "../states/monster_state_rest.h"
#include "../states/monster_state_rest_sleep.h"
#include "../states/monster_state_rest_walk_graph.h"
#include "controller_state_attack.h"
#include "../states/monster_state_attack_melee.h"
#include "../states/monster_state_attack_run.h"
#include "../states/monster_state_eat.h"
#include "../states/monster_state_eat_eat.h"

//#include "../states/monster_state_find_enemy.h"
//#include "../states/monster_state_find_enemy_run.h"
//#include "../states/monster_state_find_enemy_look.h"
//#include "../states/monster_state_find_enemy_angry.h"
//#include "../states/monster_state_find_enemy_walk.h"
//#include "../states/state_move_to_point.h"
//#include "../states/state_look_point.h"
//#include "../states/state_custom_action.h"

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

	//add_state(
	//	eStateFindEnemy, xr_new<CStateMonsterFindEnemy<CChimera> > (obj,
	//	xr_new<CStateMonsterFindEnemyRun<CChimera> >(obj), 
	//	xr_new<CStateMonsterFindEnemyLook<CChimera> >(obj,
	//	xr_new<CStateMonsterMoveToPoint<CChimera> >(obj), 
	//	xr_new<CStateMonsterCustomAction<CChimera> >(obj),
	//	xr_new<CStateMonsterLookToPoint<CChimera> >(obj)),
	//	xr_new<CStateMonsterFindEnemyAngry<CChimera> >(obj), 
	//	xr_new<CStateMonsterFindEnemyWalkAround<CChimera> >(obj)
	//	)
	//	);


}

CStateManagerController::~CStateManagerController()
{
}

void CStateManagerController::execute()
{
	u32 state_id = u32(-1);
		
	const CEntityAlive* enemy = object->EnemyMan.get_enemy();

	//if (enemy) {
	//	if (object->is_able_to_control(enemy)) {
	//		object->take_under_control(enemy);
	//		state_id = eStateRest;
	//	} else {
	//		switch (object->EnemyMan.get_danger_type()) {
	//			case eVeryStrong:	state_id = eStatePanic; break;
	//			case eStrong:		
	//			case eNormal:
	//			case eWeak:			state_id = eStateAttack; break;
	//		}
	//	}
	//} else if (object->hear_dangerous_sound || object->hear_interesting_sound) {
	//	if (object->hear_interesting_sound)	state_id = eStateInterestingSound;	
	//	if (object->hear_dangerous_sound)	state_id = eStateDangerousSound;		
	//} else if (object->CorpseMan.get_corpse() && ((object->GetSatiety() < object->get_sd()->m_fMinSatiety) || object->flagEatNow))
	//	state_id = eStateEat;	
	//else state_id = eStateRest;

	const CEntityAlive* corpse = object->CorpseMan.get_corpse();
	bool can_eat = true;

	if ((prev_substate == eStateEat) && (object->GetSatiety() < object->get_sd()->m_fMaxSatiety)) {
		can_eat = true;		
	}

	if ((prev_substate != eStateEat) && (object->GetSatiety() < object->get_sd()->m_fMinSatiety)) {
		can_eat = true;		
	}


	if (enemy) {
		state_id = eStateAttack;
		//object->set_controlled_task(eTaskAttack);
	} else if (corpse && can_eat){
		state_id = eStateEat;

#ifdef DEBUG
		//object->HDebug->M_Add(0, "[Eat State]", D3DCOLOR_XRGB(255,0,0));	
#endif
		
	} else {
		state_id = eStateRest;

#ifdef DEBUG
		//object->HDebug->M_Add(0, "[Rest State]", D3DCOLOR_XRGB(255,0,0));	
#endif
		//object->set_controlled_task(eTaskFollow);
	}


	select_state(state_id); 

	// выполнить текущее состояние
	get_state_current()->execute();

	prev_substate = current_substate;
}


