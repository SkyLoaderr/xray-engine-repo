#include "stdafx.h"
#include "dog.h"
#include "dog_state_manager.h"
#include "../BaseMonster/base_monster_state.h"
#include "../ai_monster_utils.h"

CStateManagerDog::CStateManagerDog(CAI_Dog *monster) : m_object(monster), inherited(monster)
{
	add_state(eStateRest,					xr_new<CBaseMonsterRest>		(monster));
	add_state(eStateEat,					xr_new<CBaseMonsterEat>			(monster));
	add_state(eStateAttack,					xr_new<CBaseMonsterAttack>		(monster));
	add_state(eStatePanic,					xr_new<CBaseMonsterPanic>		(monster));
	add_state(eStateHearInterestingSound,	xr_new<CBaseMonsterExploreNDE>	(monster));
	add_state(eStateHearDangerousSound,		xr_new<CBaseMonsterRunAway>		(monster));
	add_state(eStateHitted,					xr_new<CBaseMonsterRunAway>		(monster));
	add_state(eStateControlled,				xr_new<CBaseMonsterControlled>	(monster));
}

void CStateManagerDog::update()
{
	if (m_object->is_under_control()) {
		set_state(eStateControlled);
		return;
	}

	EGlobalStates state = eStateUnknown;

	TTime last_hit_time = 0;
	if (m_object->HitMemory.is_hit()) last_hit_time = m_object->HitMemory.get_last_hit_time();

	if (m_object->EnemyMan.get_enemy()) {
		switch (m_object->EnemyMan.get_danger_type()) {
			case eVeryStrong:				state = eStatePanic; break;
			case eStrong:		
			case eNormal:
			case eWeak:						state = eStateAttack; break;
		}
	} else if (m_object->HitMemory.is_hit() && (last_hit_time + 10000 > m_object->m_current_update)) state = eStateHitted;
	else if (m_object->hear_dangerous_sound || m_object->hear_interesting_sound) {
		if (m_object->hear_dangerous_sound)			state = eStateHearDangerousSound;	
		if (m_object->hear_interesting_sound)		state = eStateHearInterestingSound;	
	} else if (m_object->can_eat_now())	state = eStateEat;	
	else								state = eStateRest;

	if (state == eStateAttack) {
		float yaw,pitch;
		Fvector().sub(m_object->EnemyMan.get_enemy_position(), m_object->Position()).getHP(yaw,pitch);

		yaw = angle_normalize(-yaw);

		if (angle_difference(yaw, m_object->m_body.current.yaw) > PI_DIV_2) {

			if (from_right(yaw, m_object->m_body.current.yaw)) m_object->MotionMan.SetSpecParams(ASP_ROTATION_RUN_RIGHT);
			else m_object->MotionMan.SetSpecParams(ASP_ROTATION_RUN_LEFT);
		}
	}

	set_state(state);
}
