#include "stdafx.h"
#include "bloodsucker_state_manager.h"
#include "bloodsucker.h"
#include "../BaseMonster/base_monster_state.h"

CStateManagerBloodsucker::CStateManagerBloodsucker(CAI_Bloodsucker *monster) : m_object(monster), inherited(monster)
{
	add_state(eStateRest,					xr_new<CBaseMonsterRest>		(monster));
	add_state(eStateEat,					xr_new<CBaseMonsterEat>			(monster));
	add_state(eStateAttack,					xr_new<CBaseMonsterAttack>		(monster));
	add_state(eStatePanic,					xr_new<CBaseMonsterPanic>		(monster));
	add_state(eStateHearInterestingSound,	xr_new<CBaseMonsterExploreNDE>	(monster));
	add_state(eStateHitted,					xr_new<CBaseMonsterRunAway>		(monster));
}

void CStateManagerBloodsucker::update()
{
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
		state = eStateHearInterestingSound;	
	} else if (m_object->can_eat_now())	state = eStateEat;	
	else								state = eStateRest;

	set_state(state);

	m_object->CEnergyHolder::set_aggressive(get_state(state)->GetStateAggressiveness());
}
