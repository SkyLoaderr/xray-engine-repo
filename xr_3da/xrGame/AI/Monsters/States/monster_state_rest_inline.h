#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterRestAbstract CStateMonsterRest<_Object>

TEMPLATE_SPECIALIZATION
CStateMonsterRestAbstract::CStateMonsterRest(_Object *obj, state_ptr state_sleep, state_ptr state_walk) : inherited(obj)
{
	add_state	(eStateSleep,			state_sleep);
	add_state	(eStateWalkGraphPoint,	state_walk);
}

TEMPLATE_SPECIALIZATION
CStateMonsterRestAbstract::~CStateMonsterRest	()
{
}


TEMPLATE_SPECIALIZATION
void CStateMonsterRestAbstract::execute()
{
	bool bNormalSatiety =	(object->GetSatiety() > object->_sd->m_fMinSatiety) && 
		(object->GetSatiety() < object->_sd->m_fMaxSatiety); 

	if (bNormalSatiety) {
		select_state	(eStateSleep);
	} else {
		select_state	(eStateWalkGraphPoint);
	}

	get_state_current()->execute();
}

