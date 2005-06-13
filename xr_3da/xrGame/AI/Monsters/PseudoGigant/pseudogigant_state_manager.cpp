#include "stdafx.h"
#include "pseudo_gigant.h"
#include "pseudogigant_state_manager.h" 
#include "../states/monster_state_rest.h"
#include "../states/monster_state_attack.h"
#include "../states/monster_state_panic.h"
#include "../states/monster_state_eat.h"
#include "../states/monster_state_hear_int_sound.h"
#include "../states/monster_state_hear_danger_sound.h"
#include "../states/monster_state_hitted.h"
#include "../../../entitycondition.h"
#include "../states/monster_state_controlled.h"

CStateManagerGigant::CStateManagerGigant(CPseudoGigant *monster) : inherited(monster)
{
	add_state(eStateRest,					xr_new<CStateMonsterRest<CPseudoGigant> >				(monster));
	add_state(eStatePanic,					xr_new<CStateMonsterPanic<CPseudoGigant> >				(monster));
	add_state(eStateAttack,					xr_new<CStateMonsterAttack<CPseudoGigant> >				(monster));
	add_state(eStateEat,					xr_new<CStateMonsterEat<CPseudoGigant> >				(monster));
	add_state(eStateHearInterestingSound,	xr_new<CStateMonsterHearInterestingSound<CPseudoGigant> >(monster));
	add_state(eStateHearDangerousSound,		xr_new<CStateMonsterHearDangerousSound<CPseudoGigant> >	(monster));
	add_state(eStateHitted,					xr_new<CStateMonsterHitted<CPseudoGigant> >				(monster));
	add_state(eStateControlled,				xr_new<CStateMonsterControlled<CPseudoGigant> >			(monster));
}

void CStateManagerGigant::execute()
{
	u32 state_id = u32(-1);

	if (!object->is_under_control()) {
		const CEntityAlive* enemy	= object->EnemyMan.get_enemy();
		const CEntityAlive* corpse	= object->CorpseMan.get_corpse();

		if (enemy) {
			switch (object->EnemyMan.get_danger_type()) {
			case eStrong:	state_id = eStatePanic; break;
			case eWeak:		state_id = eStateAttack; break;
			}

		} else if (object->HitMemory.is_hit()) {
			state_id = eStateHitted;
		} else if (object->hear_interesting_sound) {
			state_id = eStateHearInterestingSound;
		} else if (object->hear_dangerous_sound) {
			state_id = eStateHearDangerousSound;	
		} else {
			bool can_eat = false;
			if (corpse) {
				if (prev_substate == eStateEat) {
					if (!get_state_current()->check_completion())				can_eat = true;
				} else {
					if (object->conditions().GetSatiety() < object->db().m_fMinSatiety) can_eat = true;
				}
			}

			if (can_eat)	state_id = eStateEat;
			else			state_id = eStateRest;
		}
	} else state_id = eStateControlled;

	select_state(state_id); 

	// выполнить текущее состояние
	get_state_current()->execute();

	prev_substate = current_substate;
}
