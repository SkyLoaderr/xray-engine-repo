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

#ifdef INTENSIVE_MEMORY_USAGE
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
#else
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

#endif

#ifdef INTENSIVE_MEMORY_USAGE
TEMPLATE_SPECIALIZATION
IC	bool CAbstractOperator::apply	(const CSConditionState &condition, CSConditionState &result) const
{
	result.clear			();
#ifdef USE_AFFECT
	bool					changed = false;
#endif
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
#ifdef USE_AFFECT
				changed		= true;
#endif
				++i;
			}
			else {
#ifdef USE_AFFECT
				if ((*I).value() != (*i).value())
					changed	= true;
#endif
				result.add_condition(*i);
				++I;
				++i;
			}

#ifdef USE_AFFECT
	if (i == e) {
		if (!changed)
			return			(false);
	}
	else {
		I					= i;
		E					= e;
	}
#else
	if (I == E) {
		I					= i;
		E					= e;
	}
#endif
	
	for ( ; (I != E); ++I)
		result.add_condition(*I);

	return					(true);
}
#else
TEMPLATE_SPECIALIZATION
IC	bool CAbstractOperator::apply	(const CSConditionState &condition, const xr_vector<COperatorCondition> &start, CSConditionState &result, const xr_vector<COperatorCondition> &self_condition) const
{
	result.clear			();
#ifdef USE_AFFECT
	bool					changed = false;
#endif
	xr_vector<COperatorCondition>::const_iterator	i = self_condition.begin();
	xr_vector<COperatorCondition>::const_iterator	e = self_condition.end();
	xr_vector<COperatorCondition>::const_iterator	I = condition.conditions().begin();
	xr_vector<COperatorCondition>::const_iterator	E = condition.conditions().end();
	xr_vector<COperatorCondition>::const_iterator	J = start.begin();
	xr_vector<COperatorCondition>::const_iterator	EE = start.end();
	for ( ; (I != E) && (i != e); )
		if ((*I).condition() < (*i).condition()) {
			result.add_condition(*I);
			++I;
		}
		else
			if ((*I).condition() > (*i).condition()) {
				while ((J != EE) && ((*J).condition() < (*i).condition()))
					++J;
				if ((J != EE) && ((*J).condition() == (*i).condition())) {
					if ((*J).value() != (*i).value()) {
						result.add_condition(*i);
#ifdef USE_AFFECT
						changed	= true;
#endif
					}
					++J;
				}
				else {
					result.add_condition(*i);
#ifdef USE_AFFECT
					changed	= true;
#endif
				}
				++i;
			}
			else {
				if ((*I).value() == (*i).value())
					result.add_condition(*I);
				else {
#ifdef USE_AFFECT
					changed	= true;
#endif
					while ((J != EE) && ((*J).condition() < (*i).condition()))
						++J;
					if ((J != EE) && ((*J).condition() == (*i).condition())) {
						if ((*J).value() != (*i).value())
							result.add_condition(*i);
						++J;
					}
					else
						result.add_condition(*i);
				}
				++I;
				++i;
			}

	if (i == e) {
#ifdef USE_AFFECT
		if (!changed)
			return			(false);
#endif
		for ( ; (I != E); ++I)
			result.add_condition(*I);
		return				(true);
	}

	for ( ; (J != EE) && (i != e); )
		if ((*J).condition() < (*i).condition())
			++J;
		else
			if ((*J).condition() > (*i).condition()) {
				result.add_condition(*i);
#ifdef USE_AFFECT
				changed		= true;
#endif
				++i;
			}
			else {
				if ((*J).value() != (*i).value()) {
					result.add_condition(*i);
#ifdef USE_AFFECT
					changed	= true;
#endif
				}
				++J;
				++i;
			}

	if ((J == EE) && (i != e))
#ifdef USE_AFFECT
		changed = true;
#endif
		for ( ; (i != e); ++i)
			result.add_condition(*i);

#ifdef USE_AFFECT
	return					(changed);
#else
	return					(true);
#endif
}

TEMPLATE_SPECIALIZATION
IC	bool CAbstractOperator::apply_reverse	(const CSConditionState &condition, const xr_vector<COperatorCondition> &start, CSConditionState &result, const xr_vector<COperatorCondition> &self_condition) const
{
	result.clear			();
#ifdef USE_AFFECT
	bool					changed = false;
#endif
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
#ifdef USE_AFFECT
				changed		= true;
#endif
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
#ifdef USE_AFFECT
				if ((*I).value() != (*i).value())
					changed	= true;
#endif
				result.add_condition(*i);
				++I;
				++i;
			}

	if (I == E) {
#ifdef USE_AFFECT
		if (!changed)
			return			(false);
#endif
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
#ifdef USE_AFFECT
				changed		= true;
#endif
				++J;
				++I;
			}

#ifdef USE_AFFECT
	if (!changed)
		return				(false);
#endif

	if ((J == EE) && (I != E))
		for ( ; (I != E); ++I)
			result.add_condition(*I);

	return					(true);
}
#endif
#undef TEMPLATE_SPECIALIZATION
#undef CAbstractOperator