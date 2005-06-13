#include "stdafx.h"
#include "snork.h"
#include "snork_state_manager.h"
#include "../../../level.h"
#include "../../../level_debug.h"
#include "../states/monster_state_rest.h"
#include "../states/monster_state_attack.h"
#include "../states/monster_state_panic.h"
#include "../states/monster_state_eat.h"
#include "../states/monster_state_hear_int_sound.h"
#include "../states/monster_state_hear_danger_sound.h"
#include "../states/monster_state_hitted.h"
#include "../states/state_look_point.h"
#include "../states/state_test_look_actor.h"
#include "../critical_action_info.h"
#include "../../../entitycondition.h"

CStateManagerSnork::CStateManagerSnork(CSnork *obj) : inherited(obj)
{
	add_state(eStateRest,				xr_new<CStateMonsterRest<CSnork> >					(obj));
	add_state(eStatePanic,				xr_new<CStateMonsterPanic<CSnork> >					(obj));
	add_state(eStateAttack,				xr_new<CStateMonsterAttack<CSnork> >				(obj));
	add_state(eStateEat,				xr_new<CStateMonsterEat<CSnork> >					(obj));
	add_state(eStateHearInterestingSound,	xr_new<CStateMonsterHearInterestingSound<CSnork> >	(obj));
	add_state(eStateHearDangerousSound,		xr_new<CStateMonsterHearDangerousSound<CSnork> >	(obj));
	add_state(eStateHitted,				xr_new<CStateMonsterHitted<CSnork> >				(obj));

	add_state(eStateFindEnemy,			xr_new<CStateMonsterTurnAwayFromActor<CSnork> >			(obj));
	//add_state(eStateFakeDeath,			xr_new<CStateMonsterLookActor<CSnork> >			(obj));
}

CStateManagerSnork::~CStateManagerSnork()
{
}

void CStateManagerSnork::execute()
{
	u32 state_id = u32(-1);

	const CEntityAlive* enemy	= object->EnemyMan.get_enemy();

	if (enemy) {
		switch (object->EnemyMan.get_danger_type()) {
				case eStrong:	state_id = eStatePanic; break;
				case eWeak:		state_id = eStateAttack; break;
		}
	} else if (object->HitMemory.is_hit()) {
		state_id = eStateHitted;
	} else if (object->hear_dangerous_sound) {
		state_id = eStateHearDangerousSound;
	} else if (object->hear_interesting_sound) {
		state_id = eStateHearInterestingSound;
	} else {
		if (can_eat())	state_id = eStateEat;
		else			state_id = eStateRest;
	}

//	state_id = eStateFindEnemy;

	if (state_id == eStateAttack) {
		object->try_to_jump();
	}

	if (object->CriticalActionInfo->is_fsm_locked()) return;

	select_state(state_id); 

	// выполнить текущее состояние
	get_state_current()->execute();

	prev_substate = current_substate;

}

