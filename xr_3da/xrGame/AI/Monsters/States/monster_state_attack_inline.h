#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterAttackAbstract CStateMonsterAttack<_Object>

TEMPLATE_SPECIALIZATION
CStateMonsterAttackAbstract::CStateMonsterAttack(LPCSTR state_name, SSubStatePtr state_run, SSubStatePtr state_melee) : inherited(state_name)
{
	states[eStateRun]		= state_run;
	states[eStateMelee]		= state_melee;
}

TEMPLATE_SPECIALIZATION
CStateMonsterAttackAbstract::~CStateMonsterAttack()
{
}

TEMPLATE_SPECIALIZATION
void CStateMonsterAttackAbstract::Load(LPCSTR section)
{
	add_state				(states[eStateRun],		eStateRun,			1);
	add_state				(states[eStateMelee],	eStateMelee,		0);

	add_transition			(eStateRun,eStateMelee,1,1);

	inherited::Load			(section);
}

TEMPLATE_SPECIALIZATION
void CStateMonsterAttackAbstract::reinit(_Object *object)
{
	inherited::reinit		(object);
	set_current_state		(eStateRun);
	set_dest_state			(eStateRun);
}

TEMPLATE_SPECIALIZATION
void CStateMonsterAttackAbstract::initialize()
{
	inherited::initialize();
}

TEMPLATE_SPECIALIZATION
void CStateMonsterAttackAbstract::execute()
{
	
	float dist, m_fDistMin, m_fDistMax;
	dist = m_object->GetEnemyDistances(m_fDistMin, m_fDistMax);

	//const CEntity *enemy = m_object->m_tEnemy.obj;

	// определить тип атаки
	bool b_melee = false;
	if ((current_state_id() == eStateMelee) && (dist < m_fDistMax)) b_melee = true;
	else if (dist < m_fDistMin) b_melee = true;

	// установить целевое состояние
	if (b_melee)	set_dest_state(eStateMelee);
	else			set_dest_state(eStateRun);

	inherited::execute();
}

TEMPLATE_SPECIALIZATION
void CStateMonsterAttackAbstract::finalize()
{
	inherited::finalize();
}

