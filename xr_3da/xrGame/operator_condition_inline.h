////////////////////////////////////////////////////////////////////////////
//	Module 		: operator_condition_inline.h
//	Created 	: 24.02.2004
//  Modified 	: 24.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Operator condition inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION template<\
	typename _condition_type,\
	typename _value_type\
>

#define CAbstractOperatorCondition COperatorConditionAbstract<_condition_type,_value_type>

TEMPLATE_SPECIALIZATION
IC	CAbstractOperatorCondition::COperatorConditionAbstract	(const _condition_type condition, const _value_type value) :
	m_condition			(condition),
	m_value				(value)
{
#ifdef USE_HASH
	u32					seed = rand32.seed();
	rand32.seed			(u32(condition));
	m_hash				= rand32.random(0xffffffff);
	rand32.seed			(u32(condition) + u32(value) + 1);
	m_hash				^= rand32.random(0xffffffff);
	rand32.seed			(seed);
#endif
}

TEMPLATE_SPECIALIZATION
CAbstractOperatorCondition::~COperatorConditionAbstract	()
{
}

TEMPLATE_SPECIALIZATION
IC	const _condition_type &CAbstractOperatorCondition::condition	() const
{
	return				(m_condition);
}

TEMPLATE_SPECIALIZATION
IC	const _value_type &CAbstractOperatorCondition::value			() const
{
	return				(m_value);
}

#ifdef USE_HASH
TEMPLATE_SPECIALIZATION
IC	const u32 &CAbstractOperatorCondition::hash_value	() const
{
	return				(m_hash);
}
#endif

TEMPLATE_SPECIALIZATION
IC	bool CAbstractOperatorCondition::operator<			(const COperatorCondition &_condition) const
{
	if (condition() < _condition.condition())
		return			(true);
	if (condition() > _condition.condition())
		return			(false);
	if (value() < _condition.value())
		return			(true);
	return				(false);
}

TEMPLATE_SPECIALIZATION
IC	bool CAbstractOperatorCondition::operator==			(const COperatorCondition &_condition) const
{
	if ((condition() == _condition.condition()) && (value() == _condition.value()))
		return			(true);
	return				(false);
}

#undef TEMPLATE_SPECIALIZATION
#undef CAbstractOperatorCondition
