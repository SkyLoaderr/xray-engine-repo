////////////////////////////////////////////////////////////////////////////
//	Module 		: operator_condition_inline.h
//	Created 	: 24.02.2004
//  Modified 	: 24.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Operator condition inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION template<\
	typename T2,\
	typename T3\
>

#define CAbstractOperatorCondition COperatorConditionAbstract<T2,T3>

TEMPLATE_SPECIALIZATION
IC	CAbstractOperatorCondition::COperatorConditionAbstract	(const T2 condition, const T3 value) :
	m_condition(condition),
	m_value(value)
{
}

TEMPLATE_SPECIALIZATION
IC	T2	CAbstractOperatorCondition::condition				() const
{
	return				(m_condition);
}

TEMPLATE_SPECIALIZATION
IC	T3	CAbstractOperatorCondition::value					() const
{
	return				(m_value);
}

#undef TEMPLATE_SPECIALIZATION
#undef CAbstractOperatorCondition
