#include "stdafx.h"
#include "flesh.h"
#include "flesh_state_manager.h"
#include "../../../level.h"
#include "../../../level_debug.h"
#include "../states/monster_state_rest.h"
#include "../states/monster_state_attack.h"
#include "../states/monster_state_panic.h"
#include "../states/monster_state_eat.h"
#include "../states/monster_state_hear_int_sound.h"
#include "../states/monster_state_hear_danger_sound.h"
#include "../states/monster_state_hitted.h"
#include "../../../entitycondition.h"

CStateManagerFlesh::CStateManagerFlesh(CAI_Flesh *monster) : inherited(monster)
{
	add_state(eStateRest,				xr_new<CStateMonsterRest<CAI_Flesh> >				(monster));
	add_state(eStatePanic,				xr_new<CStateMonsterPanic<CAI_Flesh> >				(monster));
	add_state(eStateAttack,				xr_new<CStateMonsterAttack<CAI_Flesh> >				(monster));
	add_state(eStateEat,				xr_new<CStateMonsterEat<CAI_Flesh> >				(monster));
	add_state(eStateInterestingSound,	xr_new<CStateMonsterHearInterestingSound<CAI_Flesh> >(monster));
	add_state(eStateDangerousSound,		xr_new<CStateMonsterHearDangerousSound<CAI_Flesh> >	(monster));
	add_state(eStateHitted,				xr_new<CStateMonsterHitted<CAI_Flesh> >				(monster));
	//add_state(eStateControlled,				xr_new<CBaseMonsterControlled>	(monster));
}

void CStateManagerFlesh::execute()
{
	//if (m_object->is_under_control()) {
	//	set_state(eStateControlled);
	//	return;
	//}

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

	} else if (object->HitMemory.is_hit()) {
		state_id = eStateHitted;
	} else if (object->hear_interesting_sound) {
		state_id = eStateInterestingSound;
	} else if (object->hear_dangerous_sound) {
		state_id = eStateHearDangerousSound;	
	} else {
		bool can_eat = false;
		if (corpse) {
			if (prev_substate == eStateEat) {
				if (!get_state_current()->check_completion())				can_eat = true;
			} else {
				if (object->conditions().GetSatiety() < object->get_sd()->m_fMinSatiety) can_eat = true;
			}
		}

		if (can_eat)	state_id = eStateEat;
		else			state_id = eStateRest;
	}

	select_state(state_id); 

	// выполнить текущее состояние
	get_state_current()->execute();

	prev_substate = current_substate;
}
