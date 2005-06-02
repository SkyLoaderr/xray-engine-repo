#include "stdafx.h"
#include "boar.h"
#include "boar_state_manager.h"
#include "../states/monster_state_rest.h"
#include "../states/monster_state_attack.h"
#include "../states/monster_state_panic.h"
#include "../states/monster_state_eat.h"
#include "../states/monster_state_hear_int_sound.h"
#include "../states/monster_state_hear_danger_sound.h"
#include "../states/monster_state_hitted.h"
#include "../states/monster_state_controlled.h"


CStateManagerBoar::CStateManagerBoar(CAI_Boar *monster) : inherited(monster)
{
	add_state(eStateRest,				xr_new<CStateMonsterRest<CAI_Boar> >				(monster));
	add_state(eStatePanic,				xr_new<CStateMonsterPanic<CAI_Boar> >				(monster));
	add_state(eStateAttack,				xr_new<CStateMonsterAttack<CAI_Boar> >				(monster));
	add_state(eStateEat,				xr_new<CStateMonsterEat<CAI_Boar> >					(monster));
	add_state(eStateHearInterestingSound,	xr_new<CStateMonsterHearInterestingSound<CAI_Boar> >(monster));
	add_state(eStateHearDangerousSound,		xr_new<CStateMonsterHearDangerousSound<CAI_Boar> >	(monster));
	add_state(eStateHitted,				xr_new<CStateMonsterHitted<CAI_Boar> >				(monster));
	add_state(eStateControlled,			xr_new<CStateMonsterControlled<CAI_Boar> >			(monster));
}

void CStateManagerBoar::execute()
{
	u32 state_id = u32(-1);

	if (!object->is_under_control()) {
	
		const CEntityAlive* enemy	= object->EnemyMan.get_enemy();

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
			state_id = eStateHearInterestingSound;
		} else if (object->hear_dangerous_sound) {
			state_id = eStateHearDangerousSound;	
		} else {
			//if (can_eat())	state_id = eStateEat;
			//else			state_id = eStateRest;
			if (object->CorpseMan.get_corpse()) state_id = eStateEat;
			else state_id = eStateRest;
		}
	} else state_id = eStateControlled;
	
	select_state(state_id); 

	// ��������� ������� ���������
	get_state_current()->execute();

	prev_substate = current_substate;
}
