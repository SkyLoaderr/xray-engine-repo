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
	typename _value_type,\
	typename _edge_value_type\
>

#define CAbstractOperator COperatorAbstract<_condition_type,_value_type,_edge_value_type>

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
IC	void CAbstractOperator::add_effect		(const COperatorCondition &effect)
{
	m_effects.push_back		(effect);
}

TEMPLATE_SPECIALIZATION
IC	bool CAbstractOperator::applicable		(const xr_vector<COperatorCondition> &condition, const xr_vector<COperatorCondition> &start, const xr_vector<COperatorCondition> &self_condition) const
{
	xr_vector<COperatorCondition>::const_iterator	i = self_condition.begin();
	xr_vector<COperatorCondition>::const_iterator	e = self_condition.end();
	xr_vector<COperatorCondition>::const_iterator	I = condition.begin();
	xr_vector<COperatorCondition>::const_iterator	E = condition.end();
	xr_vector<COperatorCondition>::const_iterator	J = start.begin();
	xr_vector<COperatorCondition>::const_iterator	EE = start.end();
	for ( ; (I != E) && (i != e); )
		if ((*I).condition() < (*i).condition())
			++I;
		else
			if ((*I).condition() > (*i).condition()) {
				while ((J != EE) && ((*J).condition() < (*i).condition()))
					++J;
				if ((J != EE) && ((*J).condition() == (*i).condition())) {
					if ((*J).value() != (*i).value())
						return	(false);
					++J;
				}
				++i;
			}
			else {
				if ((*I).value() != (*i).value())
					return	(false);
				++I;
				++i;
			}

	if (i == e)
		return				(true);

	for ( ; (J != EE) && (i != e); )
		if ((*J).condition() < (*i).condition())
			++J;
		else
			if ((*J).condition() > (*i).condition())
				++i;
			else {
				if ((*J).value() != (*i).value())
					return	(false);
				++J;
				++i;
			}
	return					(true);
}

TEMPLATE_SPECIALIZATION
IC	const typename CAbstractOperator::CSConditionState &CAbstractOperator::apply	(const CSConditionState &condition, const xr_vector<COperatorCondition> &self_condition, CSConditionState &result) const
{
	result.clear			();
	xr_vector<COperatorCondition>::const_iterator	i = self_condition.begin();
	xr_vector<COperatorCondition>::const_iterator	e = self_condition.end();
	xr_vector<COperatorCondition>::const_iterator	I = condition.conditions().begin();
	xr_vector<COperatorCondition>::const_iterator	E = condition.conditions().end();
	for ( ; (I != E) && (i != e); )
		if ((*I).condition() < (*i).condition()) {
			result.add_condition(*I);
			++I;
		}
		else
			if ((*I).condition() > (*i).condition()) {
				result.add_condition(*i);
				++i;
			}
			else {
				VERIFY			((*I).condition() == (*i).condition());
				result.add_condition(*i);
				++I;
				++i;
			}

	if (i == e) {
		for ( ; I != E; ++I)
			result.add_condition(*I);
	}
	else {
		for ( ; i != e; ++i)
			result.add_condition(*i);
	}

	return					(result);
}

TEMPLATE_SPECIALIZATION
IC	bool CAbstractOperator::apply_reverse	(const CSConditionState &condition, const xr_vector<COperatorCondition> &start, CSConditionState &result, const xr_vector<COperatorCondition> &self_condition) const
{
	result.clear			();
	bool					changed = false;
	xr_vector<COperatorCondition>::const_iterator	i = self_condition.begin();
	xr_vector<COperatorCondition>::const_iterator	e = self_condition.end();
	xr_vector<COperatorCondition>::const_iterator	I = condition.conditions().begin();
	xr_vector<COperatorCondition>::const_iterator	E = condition.conditions().end();
	xr_vector<COperatorCondition>::const_iterator	J = start.begin();
	xr_vector<COperatorCondition>::const_iterator	EE = start.end();
	for ( ; (I != E) && (i != e); )
		if ((*I).condition() < (*i).condition()) {
			while ((J != EE) && ((*J).condition() < (*I).condition()))
				++J;
			if ((J != EE) && ((*J).condition() == (*I).condition())) {
				VERIFY		((*J).value() == (*I).value());
				changed		= true;
				++J;
			}
			else
				result.add_condition(*I);
			++I;
		}
		else
			if ((*I).condition() > (*i).condition()) {
				result.add_condition(*i);
				++i;
			}
			else {
				if ((*I).value() != (*i).value())
					changed	= true;
				result.add_condition(*i);
				++I;
				++i;
			}

	if (I == E) {
		if (!changed)
			return			(false);
		for ( ; (i != e); ++i)
			result.add_condition(*i);
		return				(true);
	}

	for ( ; (J != EE) && (I != E); )
		if ((*J).condition() < (*I).condition())
			++J;
		else
			if ((*J).condition() > (*I).condition()) {
				result.add_condition(*I);
				++I;
			}
			else {
				VERIFY		((*J).value() == (*I).value());
				changed		= true;
				++J;
				++I;
			}

	if (!changed)
		return				(false);

	if ((J == EE) && (I != E))
		for ( ; (I != E); ++I)
			result.add_condition(*I);

	return					(true);
}

TEMPLATE_SPECIALIZATION
IC	_edge_value_type CAbstractOperator::weight		() const
{
	return					(_edge_value_type(1));
}

#undef TEMPLATE_SPECIALIZATION
#undef CAbstractOperator