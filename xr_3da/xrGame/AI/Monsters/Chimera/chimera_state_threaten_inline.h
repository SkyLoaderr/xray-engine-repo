#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateChimeraThreatenAbstract CStateChimeraThreaten<_Object>

TEMPLATE_SPECIALIZATION
CStateChimeraThreatenAbstract::CStateChimeraThreaten(_Object *obj, state_ptr state_walk, state_ptr state_threaten) : inherited(obj)
{
	add_state(eStateWalk,		state_walk);
	add_state(eStateThreaten,	state_threaten);
}

TEMPLATE_SPECIALIZATION
CStateChimeraThreatenAbstract::~CStateChimeraThreaten()
{
}


TEMPLATE_SPECIALIZATION
void CStateChimeraThreatenAbstract::execute()
{
	
}

