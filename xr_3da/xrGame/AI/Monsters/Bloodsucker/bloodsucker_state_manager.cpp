#include "stdafx.h"
#include "bloodsucker_state_manager.h"
#include "bloodsucker.h"
#include "../ai_monster_debug.h"
#include "../states/monster_state_rest.h"
#include "../states/monster_state_attack.h"
#include "../states/monster_state_panic.h"
#include "../states/monster_state_eat.h"
#include "../states/monster_state_hear_int_sound.h"
#include "../states/monster_state_hitted.h"

#include "bloodsucker_vampire.h"

CStateManagerBloodsucker::CStateManagerBloodsucker(CAI_Bloodsucker *monster) : inherited(monster)
{
	add_state(eStateRest,				xr_new<CStateMonsterRest<CAI_Bloodsucker> >					(monster));
	add_state(eStatePanic,				xr_new<CStateMonsterPanic<CAI_Bloodsucker> >				(monster));
	add_state(eStateAttack,				xr_new<CStateMonsterAttack<CAI_Bloodsucker> >				(monster));
	add_state(eStateEat,				xr_new<CStateMonsterEat<CAI_Bloodsucker> >					(monster));
	add_state(eStateInterestingSound,	xr_new<CStateMonsterHearInterestingSound<CAI_Bloodsucker> >	(monster));
	add_state(eStateHitted,				xr_new<CStateMonsterHitted<CAI_Bloodsucker> >				(monster));

	add_state(eStateVampire,			xr_new<CStateBloodsuckerVampire<CAI_Bloodsucker> >			(monster));	
}

void CStateManagerBloodsucker::execute()
{
	u32 state_id = u32(-1);

	const CEntityAlive* enemy	= object->EnemyMan.get_enemy();

	if (enemy) {
		
		bool set_vampire = false;
		if (prev_substate == eStateVampire) {
			if (!get_state_current()->check_completion())			set_vampire = true;
		} else {
			if (get_state(eStateVampire)->check_start_conditions()) set_vampire = true;
		}
		
		if (set_vampire) {
			state_id = eStateVampire;
		} else {
			
			switch (object->EnemyMan.get_danger_type()) {
				case eVeryStrong:	state_id = eStatePanic; break;
				case eStrong:		
				case eNormal:
				case eWeak:			state_id = eStateAttack; break;
			}

		}

	} else if (object->HitMemory.is_hit()) {
		state_id = eStateHitted;
	} else if (object->hear_dangerous_sound || object->hear_interesting_sound) {
		state_id = eStateInterestingSound;
	} else {
		if (can_eat())	state_id = eStateEat;
		else			state_id = eStateRest;
	}

	select_state(state_id); 

	// выполнить текущее состояние
	get_state_current()->execute();

	prev_substate = current_substate;

	// установить агрессивность
	bool aggressive = (current_substate != eStateEat) && (current_substate != eStateRest);
	object->CEnergyHolder::set_aggressive(aggressive);
}
