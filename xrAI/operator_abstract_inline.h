////////////////////////////////////////////////////////////////////////////
//	Module 		: operator_abstract_inline.h
//	Created 	: 24.02.2004
//  Modified 	: 24.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Operator abstract inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION template<\
	typename _condition_type,\
	typename _value_type\
>

#define CAbstractOperator COperatorAbstract<_condition_type,_value_type>

TEMPLATE_SPECIALIZATION
IC	CAbstractOperator::COperatorAbstract	()
{
}

TEMPLATE_SPECIALIZATION
IC	CAbstractOperator::COperatorAbstract	(u32 priority, const xr_vector<COperatorCondition> &conditions, const xr_vector<COperatorCondition> &effects) :
	m_priority			(priority)
{
	m_conditions		= conditions;
	m_effects			= effects;
}

TEMPLATE_SPECIALIZATION
CAbstractOperator::~COperatorAbstract	()
{
}

TEMPLATE_SPECIALIZATION
IC	u32	CAbstractOperator::priority			() const
{
	return				(m_priority);
}

TEMPLATE_SPECIALIZATION
IC	const xr_vector<typename CAbstractOperator::COperatorCondition>	&CAbstractOperator::effects	() const
{
	return				(m_effects);
}

TEMPLATE_SPECIALIZATION
IC	void CAbstractOperator::add_condition	(const COperatorCondition &condition)
{
	m_conditions.push_back	(condition);
}

TEMPLATE_SPECIALIZATION
IC	void CAbstractOperator::add_effect		(const COperatorCondition &effect)
{
	m_effects.push_back		(effect);
}

TEMPLATE_SPECIALIZATION
IC	bool CAbstractOperator::applicable		(const CSConditionState &condition) const
{
	xr_vector<COperatorCondition>::const_iterator	i = conditions().begin();
	xr_vector<COperatorCondition>::const_iterator	e = conditions().end();
	xr_vector<COperatorCondition>::const_iterator	I = condition.conditions().begin();
	xr_vector<COperatorCondition>::const_iterator	E = condition.conditions().end();
	for ( ; (I != E) && (i != e); )
		if ((*I).condition() < (*i).condition())
			++I;
		else
			if ((*I).condition() > (*i).condition())
				++i;
			else {
				if ((*I).value() != (*i).value())
					return	(false);
				++I;
				++i;
			}
	return					(true);
}

TEMPLATE_SPECIALIZATION
IC	const typename CAbstractOperator::CSConditionState &CAbstractOperator::apply	(const CSConditionState &condition, CSConditionState &result) const
{
	result.clear			();
	xr_vector<COperatorCondition>::const_iterator	i = effects().begin();
	xr_vector<COperatorCondition>::const_iterator	e = effects().end();
	xr_vector<COperatorCondition>::const_iterator	I = condition.conditions().begin();
	xr_vector<COperatorCondition>::const_iterator	E = condition.conditions().end();
	for ( ; (I != E) && (i != e); )
		if ((*I).condition() < (*i).condition()) {
			result.add_condition	(*I);
			++I;
		}
		else
			if ((*I).condition() > (*i).condition()) {
				result.add_condition(*i);
				++i;
			}
			else {
				result.add_condition(*i);
				++I;
				++i;
			}

	if (I == E) {
		I					= i;
		E					= e;
	}
	
	for ( ; (I != E); ++I)
		result.add_condition(*I);

	return					(result);
}

TEMPLATE_SPECIALIZATION
IC	void CAbstractOperator::initialize	()
{
}

TEMPLATE_SPECIALIZATION
IC	void CAbstractOperator::execute		()
{
}

TEMPLATE_SPECIALIZATION
IC	void CAbstractOperator::finalize	()
{
}

TEMPLATE_SPECIALIZATION
IC	bool CAbstractOperator::completed	() const
{
	return					(true);
}

#undef TEMPLATE_SPECIALIZATION
#undef CAbstractOperator