////////////////////////////////////////////////////////////////////////////
//	Module 		: operator_abstract_inline.h
//	Created 	: 24.02.2004
//  Modified 	: 24.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Operator abstract inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION template<\
	typename T1,\
	typename T2,\
	typename T3\
>

#define CAbstractOperator COperatorAbstract<T1,T2,T3>

TEMPLATE_SPECIALIZATION
IC	CAbstractOperator::COperatorAbstract	(T1 *state, u32 priority, const xr_vector<COperatorCondition> &conditions, const xr_vector<COperatorEffect> &effects) :
	m_state(state),
	m_priority(priority),
	m_conditions(preconditions),
	m_effects(effects)
{
}

TEMPLATE_SPECIALIZATION
IC	T1	*CAbstractOperator::state			()
{
	return				(m_state);
}

TEMPLATE_SPECIALIZATION
IC	u32	CAbstractOperator::priority			() const
{
	return				(m_priority);
}

//TEMPLATE_SPECIALIZATION
//IC	const xr_vector<CAbstractOperator::COperatorEffect>	&CAbstractOperator::effects	() const
//{
//	return				(m_effects);
//}

#undef TEMPLATE_SPECIALIZATION
#undef CAbstractOperator
