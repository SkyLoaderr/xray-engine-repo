#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterRestAbstract CStateMonsterRest<_Object>

TEMPLATE_SPECIALIZATION
CStateMonsterRestAbstract::CStateMonsterRest(LPCSTR state_name, SSubStatePtr state_sleep, SSubStatePtr state_walk) : inherited(state_name)
{
	states[eRS_Sleep]			= state_sleep;
	states[eRS_WalkGraphPoint]	= state_walk;
}

TEMPLATE_SPECIALIZATION
CStateMonsterRestAbstract::~CStateMonsterRest()
{
}

TEMPLATE_SPECIALIZATION
void CStateMonsterRestAbstract::Load(LPCSTR section)
{
	add_state				(states[eRS_Sleep],			 eRS_Sleep,				1);
	add_state				(states[eRS_WalkGraphPoint], eRS_WalkGraphPoint,	0);

	add_transition			(eRS_Sleep,eRS_WalkGraphPoint,	1,1);

	inherited::Load			(section);
}

TEMPLATE_SPECIALIZATION
void CStateMonsterRestAbstract::reinit(_Object *object)
{
	inherited::reinit		(object);
	set_current_state		(eRS_Sleep);
	set_dest_state			(eRS_Sleep);
}

TEMPLATE_SPECIALIZATION
void CStateMonsterRestAbstract::initialize()
{
	inherited::initialize();
}

TEMPLATE_SPECIALIZATION
void CStateMonsterRestAbstract::execute()
{
	bool bNormalSatiety =	(m_object->GetSatiety() > m_object->_sd->m_fMinSatiety) && 
		(m_object->GetSatiety() < m_object->_sd->m_fMaxSatiety); 

	if (bNormalSatiety) {
		set_dest_state	(eRS_Sleep);
	} else {
		set_dest_state	(eRS_WalkGraphPoint);
	}

	inherited::execute();
}

TEMPLATE_SPECIALIZATION
void CStateMonsterRestAbstract::finalize()
{
	inherited::finalize();
}

