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
IC	CConditionStateAbstract::CConditionState	()
{
}

TEMPLATE_SPECIALIZATION
IC	CConditionStateAbstract::CConditionState	(const xr_vector<COperatorCondition> &conditions)
{
	m_conditions			= conditions;
}

TEMPLATE_SPECIALIZATION
CConditionStateAbstract::~CConditionState	()
{
}

TEMPLATE_SPECIALIZATION
IC	const xr_vector<typename CConditionStateAbstract::COperatorCondition> &CConditionStateAbstract::conditions	() const
{
	return					(m_conditions);
}

TEMPLATE_SPECIALIZATION
IC	void CConditionStateAbstract::add_condition	(const COperatorCondition &condition)
{
	m_conditions.push_back	(condition);
}

TEMPLATE_SPECIALIZATION
IC	void CConditionStateAbstract::clear	()
{
	m_conditions.clear			();
}

TEMPLATE_SPECIALIZATION
IC	u8	CConditionStateAbstract::weight		(const CConditionState &condition) const
{
	u8						result = 0;
	xr_vector<COperatorCondition>::const_iterator	I = conditions().begin();
	xr_vector<COperatorCondition>::const_iterator	E = conditions().end();
	xr_vector<COperatorCondition>::const_iterator	i = condition.conditions().begin();
	xr_vector<COperatorCondition>::const_iterator	e = condition.conditions().end();
	for ( ; (I != E) && (i != e); )
		if ((*I).condition() < (*i).condition())
			++I;
		else
			if ((*I).condition() > (*i).condition())
				++i;
			else {
				if ((*I).value() != (*i).value())
					++result;
				++I;
				++i;
			}
	return					(result);
}

TEMPLATE_SPECIALIZATION
IC	u8	CConditionStateAbstract::weight		(const CConditionState &condition, const CConditionState &start) const
{
	u8						result = 0;
	xr_vector<COperatorCondition>::const_iterator	I = conditions().begin();
	xr_vector<COperatorCondition>::const_iterator	E = conditions().end();
	xr_vector<COperatorCondition>::const_iterator	i = condition.conditions().begin();
	xr_vector<COperatorCondition>::const_iterator	e = condition.conditions().end();
	xr_vector<COperatorCondition>::const_iterator	J = start.conditions().begin();
	xr_vector<COperatorCondition>::const_iterator	EE = start.conditions().end();
	for ( ; (I != E) && (i != e); )
		if ((*I).condition() < (*i).condition())
			++I;
		else
			if ((*I).condition() > (*i).condition()) {
				while ((J != EE) && ((*J).condition() < (*i).condition()))
					++J;
				if ((J != EE) && ((*J).condition() == (*i).condition())) {
					if ((*J).value() != (*i).value())
						++result;
					++J;
				}
				++i;
			}
			else {
				if ((*I).value() != (*i).value())
					++result;
				++I;
				++i;
			}
	return					(result);
}

TEMPLATE_SPECIALIZATION
IC	bool CConditionStateAbstract::operator<	(const CConditionState &condition) const
{
	xr_vector<COperatorCondition>::const_iterator	I = conditions().begin();
	xr_vector<COperatorCondition>::const_iterator	E = conditions().end();
	xr_vector<COperatorCondition>::const_iterator	i = condition.conditions().begin();
	xr_vector<COperatorCondition>::const_iterator	e = condition.conditions().end();
	for ( ; (I != E) && (i != e); ++I, ++i)
		if (*I < *i)
			return			(true);
		else
			if (*i < *I)
				return		(false);
	if (I == E)
		if (i == e)
			return			(false);
		else
			return			(true);
	else
		return				(false);
}

TEMPLATE_SPECIALIZATION
IC	typename CConditionStateAbstract::CSConditionState &CConditionStateAbstract::operator-=(const CConditionState &condition)
{
	xr_vector<COperatorCondition>		temp;
	xr_vector<COperatorCondition>::const_iterator	I = conditions().begin();
	xr_vector<COperatorCondition>::const_iterator	E = conditions().end();
	xr_vector<COperatorCondition>::const_iterator	i = condition.conditions().begin();
	xr_vector<COperatorCondition>::const_iterator	e = condition.conditions().end();
	for ( ; (I != E) && (i != e); )
		if ((*I).condition() < (*i).condition())
			++I;
		else
			if ((*I).condition() > (*i).condition())
				++i;
			else {
				if ((*I).value() != (*i).value())
					temp.push_back	(*I);
				++I;
				++i;
			}
	m_conditions				= temp;
	return						(*this);
}

TEMPLATE_SPECIALIZATION
IC	bool CConditionStateAbstract::includes(const CConditionState &condition) const
{
	xr_vector<COperatorCondition>::const_iterator	I = conditions().begin();
	xr_vector<COperatorCondition>::const_iterator	E = conditions().end();
	xr_vector<COperatorCondition>::const_iterator	i = condition.conditions().begin();
	xr_vector<COperatorCondition>::const_iterator	e = condition.conditions().end();
	for ( ; (I != E) && (i != e); )
		if ((*I).condition() < (*i).condition())
			++I;
		else
			if ((*I).condition() > (*i).condition())
				return			(false);
			else
				if ((*I).value() != (*i).value())
					return		(false);
				else {
					++I;
					++i;
				}
	return						(i == e);
}

TEMPLATE_SPECIALIZATION
IC	bool CConditionStateAbstract::includes(const CConditionState &condition, const CConditionState &start) const
{
	xr_vector<COperatorCondition>::const_iterator	I = start.conditions().begin();
	xr_vector<COperatorCondition>::const_iterator	E = start.conditions().end();
	xr_vector<COperatorCondition>::const_iterator	i = condition.conditions().begin();
	xr_vector<COperatorCondition>::const_iterator	e = condition.conditions().end();
	xr_vector<COperatorCondition>::const_iterator	J = conditions().begin();
	xr_vector<COperatorCondition>::const_iterator	EE = conditions().end();
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
				else
					return		(false);
			}
			else {
				while ((J != EE) && ((*J).condition() < (*i).condition()))
					++J;
				if ((J != EE) && ((*J).condition() == (*i).condition())) {
					if ((*J).value() != (*i).value())
						return	(false);
					++J;
				}
				else
					if ((*I).value() != (*i).value())
						return	(false);
				++I;
				++i;
			}
	if (i == e)
		return					(true);

	for ( ; (i != e) && (J != EE); )
		if ((*J).condition() < (*i).condition())
			++I;
		else
			if ((*J).condition() > (*i).condition())
				return			(false);
			else
				if ((*J).value() != (*i).value())
					return		(false);
				else {
					++J;
					++i;
				}
	return						(i == e);
}

#undef TEMPLATE_SPECIALIZATION
#undef CConditionStateAbstract