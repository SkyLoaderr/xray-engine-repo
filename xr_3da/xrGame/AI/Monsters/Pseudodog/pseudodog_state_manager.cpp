#include "stdafx.h"
#include "pseudodog.h"
#include "../../../level.h"
#include "../../../level_debug.h"
#include "pseudodog_state_manager.h"
#include "pseudodog_state_growling.h"
#include "../../../actor.h"
#include "../../stalker/ai_stalker.h"
#include "../states/monster_state_rest.h"
#include "../states/monster_state_attack.h"
#include "../states/monster_state_panic.h"
#include "../states/monster_state_eat.h"
#include "../states/monster_state_hear_int_sound.h"
#include "../states/monster_state_hear_danger_sound.h"
#include "../states/monster_state_hitted.h"


CStateManagerPseudodog::CStateManagerPseudodog(CAI_PseudoDog *monster) : inherited(monster)
{
	add_state(eStateRest,				xr_new<CStateMonsterRest<CAI_PseudoDog> >				(monster));
	add_state(eStatePanic,				xr_new<CStateMonsterPanic<CAI_PseudoDog> >				(monster));
	add_state(eStateAttack,				xr_new<CStateMonsterAttack<CAI_PseudoDog> >				(monster));
	add_state(eStateEat,				xr_new<CStateMonsterEat<CAI_PseudoDog> >				(monster));
	add_state(eStateInterestingSound,	xr_new<CStateMonsterHearInterestingSound<CAI_PseudoDog> >(monster));
	add_state(eStateDangerousSound,		xr_new<CStateMonsterHearDangerousSound<CAI_PseudoDog> >	(monster));
	add_state(eStateHitted,				xr_new<CStateMonsterHitted<CAI_PseudoDog> >				(monster));

	//add_state(eStateControlled,				xr_new<CBaseMonsterControlled>	(monster));
	//add_state(eStateCustom,					xr_new<CPseudodogGrowling>		(monster));
}

#define MIN_ANGRY_TIME		10000
#define MAX_GROWLING_TIME	20000

void CStateManagerPseudodog::execute()
{
	//EGlobalStates state = eStateUnknown;
	//
	//bool prev_angry = m_object->m_bAngry;
	//m_object->m_bAngry = false;

	//if (m_object->HitMemory.is_hit())																				m_object->m_bAngry = true;
	//if (m_object->SoundMemory.is_loud_sound(m_object->m_anger_loud_threshold) || m_object->hear_dangerous_sound)	m_object->m_bAngry = true;
	//if (m_object->CEntityCondition::GetSatiety() < m_object->m_anger_hunger_threshold)								m_object->m_bAngry = true;

	//// если слишком долго рычит - включать универсальную схему
	//if ((m_object->time_growling !=0) && (m_object->time_growling + MAX_GROWLING_TIME < m_object->m_current_update))			m_object->m_bAngry = true;

	//// если на этом кадре стал злым, сохранить время когда стал злым
	//if ((prev_angry == false) && m_object->m_bAngry) m_object->m_time_became_angry = m_object->m_current_update;
	//if (!m_object->m_bAngry && (m_object->m_time_became_angry + MIN_ANGRY_TIME > m_object->m_current_update))		m_object->m_bAngry = true;

	//
	//const CEntityAlive	*enemy		= m_object->EnemyMan.get_enemy();
	//const CAI_Stalker	*pStalker	= smart_cast<const CAI_Stalker *>(enemy);
	//const CActor		*pActor		= smart_cast<const CActor *>(enemy);

	//TTime last_hit_time = 0;
	//if (m_object->HitMemory.is_hit()) last_hit_time = m_object->HitMemory.get_last_hit_time();

	//if (!m_object->m_bAngry && (pActor || pStalker)) {
	//	if (m_object->EnemyMan.get_danger_type() == eVeryStrong) state = eStatePanic; 
	//	else state = eStateCustom;
	//} else {
	//	// если злой или враг - монстр
	//	if (enemy) {
	//		switch (m_object->EnemyMan.get_danger_type()) {
	//			case eVeryStrong:	state = eStatePanic;  break;
	//			case eStrong:		
	//			case eNormal:
	//			case eWeak:			state = eStateAttack;  break;
	//		}
	//	} else if (m_object->HitMemory.is_hit() && (last_hit_time + 10000 > m_object->m_current_update)) state = eStateHitted;
	//	else if (m_object->hear_dangerous_sound || m_object->hear_interesting_sound) {
	//		if (m_object->hear_dangerous_sound)			state = eStateHearInterestingSound;	
	//		if (m_object->hear_interesting_sound)		state = eStateHearInterestingSound;	
	//	} else if (m_object->can_eat_now())	state = eStateEat;	
	//	else								state = eStateRest;
	//}

	//if ((m_object->time_growling == 0) && (state == eStateCustom)) m_object->time_growling = m_object->m_current_update;
	//if ((state != eStateCustom)) m_object->time_growling = 0;

	//set_state(state);

	//execute();


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

