#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateChimeraThreatenAbstract CStateChimeraThreaten<_Object>

TEMPLATE_SPECIALIZATION
CStateChimeraThreatenAbstract::CStateChimeraThreaten(LPCSTR state_name, SSubStatePtr state_walk, SSubStatePtr state_threaten) : inherited(state_name)
{
	states[eStateWalk]			= state_walk;
	states[eStateThreaten]		= state_threaten;
}

TEMPLATE_SPECIALIZATION
CStateChimeraThreatenAbstract::~CStateChimeraThreaten()
{
}

TEMPLATE_SPECIALIZATION
void CStateChimeraThreatenAbstract::Load(LPCSTR section)
{
	add_state				(states[eStateWalk],		eStateWalk,			1);
	add_state				(states[eStateThreaten],	eStateThreaten,		0);

	add_transition			(eStateWalk,eStateThreaten,1,1);

	inherited::Load			(section);
}

TEMPLATE_SPECIALIZATION
void CStateChimeraThreatenAbstract::reinit(_Object *object)
{
	inherited::reinit		(object);
	set_current_state		(eStateWalk);
	set_dest_state			(eStateThreaten);
}

TEMPLATE_SPECIALIZATION
void CStateChimeraThreatenAbstract::initialize()
{
	inherited::initialize();
}

TEMPLATE_SPECIALIZATION
void CStateChimeraThreatenAbstract::execute()
{

	float dist, m_fDistMin, m_fDistMax;
	dist = m_object->GetEnemyDistances(m_fDistMin, m_fDistMax);

	const CEntity *enemy = m_object->m_tEnemy.obj;

	// установить целевое состояние
	bool b_came = false;
	if ((current_state_id() == eStateThreaten) && (dist < m_fDistMax+2.f)) b_came = true;
	else if (dist < m_fDistMin+2.f) b_came = true;	
	
	if (!b_came) set_dest_state(eStateWalk);
	else 		set_dest_state(eStateThreaten);

	inherited::execute();
}

TEMPLATE_SPECIALIZATION
void CStateChimeraThreatenAbstract::finalize()
{
	inherited::finalize();
}

