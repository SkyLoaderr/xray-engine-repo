#include "stdafx.h"
#include "poltergeist.h"
#include "poltergeist_state_manager.h"
#include "../../ai_monster_utils.h"
#include "../states/monster_state_rest.h"
#include "../states/monster_state_rest_sleep.h"
#include "../states/monster_state_rest_walk_graph.h"
#include "poltergeist_state_attack_hidden.h"

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

	add_state(
		eStateAttackHidden,
		xr_new<CStatePoltergeistAttackHidden<CPoltergeist> > (obj)
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

void CStateManagerPoltergeist::initialize()
{
	inherited::initialize();
	
	time_next_flame_attack	= 0;
	time_next_tele_attack	= 0;
	time_next_scare_attack	= 0;

}

void CStateManagerPoltergeist::execute()
{
	u32 state_id = u32(-1);

	const CEntityAlive* enemy	= object->EnemyMan.get_enemy();
	//const CEntityAlive* corpse	= object->CorpseMan.get_corpse();

	if (enemy) {
		if (object->is_hidden()) {
			state_id = eStateAttackHidden;
		} 	
		state_id = eStateAttackHidden;
	} else
		state_id = eStateRest;
	
	
	if (state_id == eStateAttackHidden) polter_attack();
	
	select_state(state_id); 

	// выполнить текущее состояние
	get_state_current()->execute();

	prev_substate = current_substate;
}

#define FLAME_DELAY_MIN			6000
#define FLAME_DELAY_NORMAL		10000
#define FLAME_DELAY_AGGRESSIVE  8000

#define TELE_DELAY_MIN			500
#define TELE_DELAY_NORMAL		2000
#define TELE_DELAY_AGGRESSIVE	1000

#define SCARE_DELAY_MIN			100
#define SCARE_DELAY_NORMAL		1000
#define SCARE_DELAY_AGGRESSIVE	500

void CStateManagerPoltergeist::polter_attack()
{
	u32 cur_time = Level().timeServer();
	const CEntityAlive* enemy	= object->EnemyMan.get_enemy();
	
	bool b_aggressive = object->GetHealth() < 0.5f;

	if (time_next_flame_attack < cur_time) {
		object->FireFlame(enemy);
		time_next_flame_attack = cur_time + Random.randI(FLAME_DELAY_MIN, (b_aggressive) ? FLAME_DELAY_AGGRESSIVE : FLAME_DELAY_NORMAL);
	}

	if (time_next_tele_attack < cur_time) {
		object->ProcessTelekinesis(enemy);
		time_next_tele_attack = cur_time + Random.randI(TELE_DELAY_MIN, (b_aggressive) ? TELE_DELAY_AGGRESSIVE : TELE_DELAY_NORMAL);
	}

	if (time_next_scare_attack < cur_time) {
		if (Random.randI(2))
			object->PhysicalImpulse(enemy->Position());
		else 
			object->StrangeSounds(enemy->Position());
		
		time_next_scare_attack = cur_time + Random.randI(SCARE_DELAY_MIN, (b_aggressive) ? SCARE_DELAY_AGGRESSIVE : SCARE_DELAY_NORMAL);
	}
}
