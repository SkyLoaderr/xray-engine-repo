#include "stdafx.h"
#include "dog.h"
#include "dog_state_manager.h"
#include "../ai_monster_utils.h"
#include "../../../level.h"
#include "../../../level_debug.h"
#include "../states/monster_state_rest.h"
#include "../states/monster_state_attack.h"
#include "../states/monster_state_panic.h"
#include "../states/monster_state_eat.h"
#include "../states/monster_state_hear_int_sound.h"
#include "../states/monster_state_hear_danger_sound.h"
#include "../states/monster_state_hitted.h"


CStateManagerDog::CStateManagerDog(CAI_Dog *monster) : inherited(monster)
{
	add_state(eStateRest,				xr_new<CStateMonsterRest<CAI_Dog> >					(monster));
	add_state(eStatePanic,				xr_new<CStateMonsterPanic<CAI_Dog> >				(monster));
	add_state(eStateAttack,				xr_new<CStateMonsterAttack<CAI_Dog> >				(monster));
	add_state(eStateEat,				xr_new<CStateMonsterEat<CAI_Dog> >					(monster));
	add_state(eStateInterestingSound,	xr_new<CStateMonsterHearInterestingSound<CAI_Dog> >	(monster));
	add_state(eStateDangerousSound,		xr_new<CStateMonsterHearDangerousSound<CAI_Dog> >	(monster));
	add_state(eStateHitted,				xr_new<CStateMonsterHitted<CAI_Dog> >				(monster));

	//add_state(eStateControlled,				xr_new<CBaseMonsterControlled>	(monster));
}

void CStateManagerDog::execute()
{
	//if (m_object->is_under_control()) {
	//	set_state(eStateControlled);
	//	return;
	//}

	u32 state_id = u32(-1);

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
		state_id = eStateInterestingSound;
	} else if (object->hear_dangerous_sound) {
		state_id = eStateHearDangerousSound;	
	} else {
		if (can_eat())	state_id = eStateEat;
		else			state_id = eStateRest;
	}

	select_state		(state_id); 

	// выполнить текущее состояние
	get_state_current()->execute();

	prev_substate		= current_substate;


	if (state_id == eStateAttack) {
		float				yaw,pitch;

		Fvector().sub		(object->EnemyMan.get_enemy_position(), object->Position()).getHP(yaw,pitch);
		yaw					= angle_normalize(-yaw);

		float angle_diff	= angle_difference(yaw, object->movement().m_body.current.yaw);

		if ((angle_diff > PI_DIV_3) && (angle_diff < 5 * PI_DIV_6)) {

			if (from_right(yaw, object->movement().m_body.current.yaw)) object->MotionMan.SetSpecParams(ASP_ROTATION_RUN_RIGHT);
			else object->MotionMan.SetSpecParams(ASP_ROTATION_RUN_LEFT);
		}
	}
}
