#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterAttackAbstract CStateMonsterAttack<_Object>

TEMPLATE_SPECIALIZATION
CStateMonsterAttackAbstract::CStateMonsterAttack(_Object *obj, state_ptr state_run, state_ptr state_melee) : inherited(obj)
{
	add_state	(eStateRun,		state_run);
	add_state	(eStateMelee,	state_melee);
}

TEMPLATE_SPECIALIZATION
CStateMonsterAttackAbstract::~CStateMonsterAttack()
{
}

TEMPLATE_SPECIALIZATION
void CStateMonsterAttackAbstract::execute()
{
	float dist, m_fDistMin, m_fDistMax;
	dist = object->GetEnemyDistances(m_fDistMin, m_fDistMax);

	// определить тип атаки
	bool b_melee = false; 
	if ((prev_substate == eStateMelee) && (dist < m_fDistMax)) b_melee = true;
	else if (dist < m_fDistMin) b_melee = true;

	// установить целевое состояние
	if (b_melee)	select_state(eStateMelee);
	else			select_state(eStateRun);

	get_state_current()->execute();
	
	prev_substate = current_substate;
}
