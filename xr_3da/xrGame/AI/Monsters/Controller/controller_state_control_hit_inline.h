#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateControllerControlHitAbstract CStateControlAttack<_Object>

TEMPLATE_SPECIALIZATION
CStateControllerControlHitAbstract::CStateControlAttack(_Object *obj) : inherited(obj)
{
}

TEMPLATE_SPECIALIZATION
CStateControllerControlHitAbstract::~CStateControlAttack()
{
}

TEMPLATE_SPECIALIZATION
void CStateControllerControlHitAbstract::execute()
{
}

TEMPLATE_SPECIALIZATION
bool CStateControllerControlHitAbstract::check_start_conditions()
{
	return true;
}

TEMPLATE_SPECIALIZATION
bool CStateControllerControlHitAbstract::check_completion()
{
	return false;
}
