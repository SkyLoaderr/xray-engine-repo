#pragma once

#include "monster_state_attack_melee.h"
#include "monster_state_attack_run.h"
#include "monster_state_attack_run_attack.h"
#include "state_hide_from_point.h"
#include "monster_state_find_enemy.h"
#include "monster_state_steal.h"

#include "../ai_monster_squad.h"
#include "../ai_monster_squad_manager.h"

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterAttackAbstract CStateMonsterAttack<_Object>

TEMPLATE_SPECIALIZATION
CStateMonsterAttackAbstract::CStateMonsterAttack(_Object *obj) : inherited(obj)
{
	add_state	(eStateRun,			xr_new<CStateMonsterAttackRun<_Object> >		(obj));
	add_state	(eStateMelee,		xr_new<CStateMonsterAttackMelee<_Object> >		(obj));
	add_state	(eStateRunAttack,	xr_new<CStateMonsterAttackRunAttack<_Object> >	(obj));
	add_state	(eStateRunAway,		xr_new<CStateMonsterHideFromPoint<_Object> >	(obj));
	add_state	(eStateFindEnemy,	xr_new<CStateMonsterFindEnemy<_Object> >		(obj));	
	add_state	(eStateSteal,		xr_new<CStateMonsterSteal<_Object> >			(obj));	
}

TEMPLATE_SPECIALIZATION
CStateMonsterAttackAbstract::CStateMonsterAttack(_Object *obj, state_ptr state_run, state_ptr state_melee) : inherited(obj)
{
	add_state	(eStateRun,			state_run);
	add_state	(eStateMelee,		state_melee);
	add_state	(eStateRunAttack,	xr_new<CStateMonsterAttackRunAttack<_Object> >	(obj));
	add_state	(eStateRunAway,		xr_new<CStateMonsterHideFromPoint<_Object> >	(obj));
	add_state	(eStateFindEnemy,	xr_new<CStateMonsterFindEnemy<_Object> >		(obj));	
	add_state	(eStateSteal,		xr_new<CStateMonsterSteal<_Object> >			(obj));	
}

TEMPLATE_SPECIALIZATION
CStateMonsterAttackAbstract::~CStateMonsterAttack()
{
}

TEMPLATE_SPECIALIZATION
void CStateMonsterAttackAbstract::initialize()
{
	inherited::initialize				();
	object->MeleeChecker.init_attack	();
	
	m_time_next_run_away				= 0;
}

#define FIND_ENEMY_DELAY	6000


TEMPLATE_SPECIALIZATION
void CStateMonsterAttackAbstract::execute()
{
	bool selected = false;
	
	if (check_steal_state()) {
		select_state	(eStateSteal);
		selected		= true;
	} else if (check_find_enemy_state()) {
		select_state	(eStateFindEnemy);
		selected		= true;
	} else if (check_run_away_state()) {
		select_state	(eStateRunAway);
		selected		= true;
	} else if (check_run_attack_state()) {
		select_state	(eStateRunAttack);
		selected		= true;
	}
	
	if (!selected) {
		// определить тип атаки
		bool b_melee = false; 
		if ((prev_substate == eStateMelee) && !object->MeleeChecker.should_stop_melee(object->EnemyMan.get_enemy())) b_melee = true;
		else if (object->MeleeChecker.can_start_melee(object->EnemyMan.get_enemy())) b_melee = true;

		// установить целевое состояние
		if (b_melee)	select_state(eStateMelee);
		else			select_state(eStateRun);
	}

	get_state_current()->execute();
	
	prev_substate = current_substate;


	// Notify squad	
	CMonsterSquad *squad	= monster_squad().get_squad(object);
	if (squad) {
		SMemberGoal			goal;
		
		goal.type			= MG_AttackEnemy;
		goal.entity			= const_cast<CEntityAlive*>(object->EnemyMan.get_enemy());
		
		squad->UpdateGoal	(object, goal);
	}
	//////////////////////////////////////////////////////////////////////////
}

TEMPLATE_SPECIALIZATION
bool CStateMonsterAttackAbstract::check_steal_state()
{
	if (prev_substate == u32(-1)) {
		if (get_state(eStateSteal)->check_start_conditions())	return true;
	} else if (prev_substate == eStateSteal) {
		if (!get_state(eStateSteal)->check_completion())		return true;
	}
	return false;
}

TEMPLATE_SPECIALIZATION
bool CStateMonsterAttackAbstract::check_find_enemy_state()
{
	// check state find enemy
	if (object->EnemyMan.get_enemy_time_last_seen() + FIND_ENEMY_DELAY < Device.dwTimeGlobal) return true;
	return false;
}

TEMPLATE_SPECIALIZATION
bool CStateMonsterAttackAbstract::check_run_away_state()
{
	if (prev_substate == eStateRunAway) {
		if (!get_state(eStateRunAway)->check_completion()) return true;
		else m_time_next_run_away = Device.dwTimeGlobal + 10000;
	} else if (object->Morale.is_despondent() && (m_time_next_run_away < Device.dwTimeGlobal)) {
		return true;
	}

	return false;
}

TEMPLATE_SPECIALIZATION
bool CStateMonsterAttackAbstract::check_run_attack_state()
{
	if (!object->ability_run_attack()) return false;

	if (prev_substate == eStateRun) {
		if (get_state(eStateRunAttack)->check_start_conditions())	return true;			
	} else if (prev_substate == eStateRunAttack) {
		if (!get_state(eStateRunAttack)->check_completion())		return true;
	}

	return false;
}



TEMPLATE_SPECIALIZATION
void CStateMonsterAttackAbstract::setup_substates()
{
	state_ptr state = get_state_current();

	if (current_substate == eStateRunAway) {
		
		SStateHideFromPoint		data;
		data.point				= object->EnemyMan.get_enemy_position();
		data.accelerated		= true;
		data.braking			= false;
		data.accel_type			= eAT_Aggressive;
		data.distance			= 20.f;
		data.action.action		= ACT_RUN;
		data.action.sound_type	= MonsterSpace::eMonsterSoundAttack;
		data.action.sound_delay = object->get_sd()->m_dwAttackSndDelay;
		data.action.time_out	= 5000;

		state->fill_data_with(&data, sizeof(SStateHideFromPoint));

#ifdef DEBUG
		if (psAI_Flags.test(aiMonsterDebug)) {
			DBG().object_info(object,object).remove_item	(u32(0));
			DBG().object_info(object,object).add_item		("Attack :: Run Away", D3DCOLOR_XRGB(255,0,0), 0);
		}
#endif
		return;
	}
}

#undef TEMPLATE_SPECIALIZATION
#undef CStateMonsterAttackAbstract
