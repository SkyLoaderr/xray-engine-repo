#include "stdafx.h"
#include "pseudodog.h"
#include "pseudodog_state_manager.h"
#include "../BaseMonster/base_monster_state.h"
#include "pseudodog_state_growling.h"
#include "../../../actor.h"
#include "../../stalker/ai_stalker.h"


CStateManagerPseudodog::CStateManagerPseudodog(CAI_PseudoDog *monster) : m_object(monster), inherited(monster)
{
	add_state(eStateRest,					xr_new<CBaseMonsterRest>		(monster));
	add_state(eStateEat,					xr_new<CBaseMonsterEat>			(monster));
	add_state(eStateAttack,					xr_new<CBaseMonsterAttack>		(monster));
	add_state(eStatePanic,					xr_new<CBaseMonsterPanic>		(monster));
	add_state(eStateHearInterestingSound,	xr_new<CBaseMonsterExploreNDE>	(monster));
	add_state(eStateHearDangerousSound,		xr_new<CBaseMonsterRunAway>		(monster));
	add_state(eStateHitted,					xr_new<CBaseMonsterRunAway>		(monster));
	add_state(eStateControlled,				xr_new<CBaseMonsterControlled>	(monster));
	add_state(eStateCustom,					xr_new<CPseudodogGrowling>		(monster));
	
}

#define MIN_ANGRY_TIME		10000
#define MAX_GROWLING_TIME	20000

void CStateManagerPseudodog::update()
{
	EGlobalStates state = eStateUnknown;
	
	bool prev_angry = m_object->m_bAngry;
	m_object->m_bAngry = false;

	if (m_object->HitMemory.is_hit())																				m_object->m_bAngry = true;
	if (m_object->SoundMemory.is_loud_sound(m_object->m_anger_loud_threshold) || m_object->hear_dangerous_sound)	m_object->m_bAngry = true;
	if (m_object->CEntityCondition::GetSatiety() < m_object->m_anger_hunger_threshold)								m_object->m_bAngry = true;

	// если слишком долго рычит - включать универсальную схему
	if ((m_object->time_growling !=0) && (m_object->time_growling + MAX_GROWLING_TIME < m_object->m_current_update))			m_object->m_bAngry = true;

	// если на этом кадре стал злым, сохранить время когда стал злым
	if ((prev_angry == false) && m_object->m_bAngry) m_object->m_time_became_angry = m_object->m_current_update;
	if (!m_object->m_bAngry && (m_object->m_time_became_angry + MIN_ANGRY_TIME > m_object->m_current_update))		m_object->m_bAngry = true;

	
	const CEntityAlive	*enemy		= m_object->EnemyMan.get_enemy();
	const CAI_Stalker	*pStalker	= smart_cast<const CAI_Stalker *>(enemy);
	const CActor		*pActor		= smart_cast<const CActor *>(enemy);

	TTime last_hit_time = 0;
	if (m_object->HitMemory.is_hit()) last_hit_time = m_object->HitMemory.get_last_hit_time();

	if (!m_object->m_bAngry && (pActor || pStalker)) {
		if (m_object->EnemyMan.get_danger_type() == eVeryStrong) state = eStatePanic; 
		else state = eStateCustom;
	} else {
		// если злой или враг - монстр
		if (enemy) {
			switch (m_object->EnemyMan.get_danger_type()) {
				case eVeryStrong:	state = eStatePanic;  break;
				case eStrong:		
				case eNormal:
				case eWeak:			state = eStateAttack;  break;
			}
		} else if (m_object->HitMemory.is_hit() && (last_hit_time + 10000 > m_object->m_current_update)) state = eStateHitted;
		else if (m_object->hear_dangerous_sound || m_object->hear_interesting_sound) {
			if (m_object->hear_dangerous_sound)			state = eStateHearInterestingSound;	
			if (m_object->hear_interesting_sound)		state = eStateHearInterestingSound;	
		} else if (m_object->can_eat_now())	state = eStateEat;	
		else								state = eStateRest;
	}

	if ((m_object->time_growling == 0) && (state == eStateCustom)) m_object->time_growling = m_object->m_current_update;
	if ((state != eStateCustom)) m_object->time_growling = 0;

	set_state(state);
}

