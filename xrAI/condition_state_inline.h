////////////////////////////////////////////////////////////////////////////
//	Module 		: condition_state_inline.h
//	Created 	: 26.02.2004
//  Modified 	: 26.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Condition state inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION template<\
	typename T2,\
	typename T3\
>

#define CConditionStateAbstract CConditionState<T2,T3>

TEMPLATE_SPECIALIZATION
IC	CConditionStateAbstract::COperatorAbstract	(const xr_vector<COperatorCondition> &conditions) :
	m_conditions(conditions)
{
}

//TEMPLATE_SPECIALIZATION
//IC	const xr_vector<CConditionStateAbstract::COperatorCondition> &CConditionStateAbstract::conditions	() const
//{
//	return				(m_conditions);
//}

#undef TEMPLATE_SPECIALIZATION
#undef CConditionStateAbstract
