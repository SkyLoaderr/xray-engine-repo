#include "stdafx.h"
#include "fracture.h"
#include "fracture_state_manager.h"

#include "../states/monster_state_rest.h"
#include "../states/monster_state_eat.h"
#include "../states/monster_state_attack.h"
#include "../states/monster_state_panic.h"
#include "../states/monster_state_hear_danger_sound.h"
#include "../states/monster_state_hitted.h"

#include "../ai_monster_debug.h"

CStateManagerFracture::CStateManagerFracture(CFracture *obj) : inherited(obj)
{
	add_state(eStateRest,				xr_new<CStateMonsterRest<CFracture> >				(obj));
	add_state(eStateAttack,				xr_new<CStateMonsterAttack<CFracture> >				(obj));
	add_state(eStateEat,				xr_new<CStateMonsterEat<CFracture> >				(obj));
	add_state(eStateDangerousSound,		xr_new<CStateMonsterHearDangerousSound<CFracture> >	(obj));
	add_state(eStatePanic,				xr_new<CStateMonsterPanic<CFracture> >				(obj));
	add_state(eStateHitted,				xr_new<CStateMonsterHitted<CFracture> >				(obj));
}

CStateManagerFracture::~CStateManagerFracture()
{
}

void CStateManagerFracture::initialize()
{
	inherited::initialize();
}

void CStateManagerFracture::execute()
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
	} else if (object->HitMemory.is_hit()) {
		state_id = eStateHitted;
	} else if (object->hear_interesting_sound || object->hear_dangerous_sound) {
		state_id = eStateDangerousSound;
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
		else {
			// Rest & Idle states here 
			state_id = eStateRest;
		}
	}

	// установить текущее состояние
	select_state(state_id); 

	// выполнить текущее состояние
	get_state_current()->execute();

	prev_substate = current_substate;
}

