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
IC	u8	CConditionStateAbstract::weight		(const CConditionState &condition) const
{
	u8						result = 0;
	xr_vector<COperatorCondition>::const_iterator	I = m_conditions.begin();
	xr_vector<COperatorCondition>::const_iterator	E = m_conditions.end();
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
	return					(false);
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
IC	void CConditionStateAbstract::clear	()
{
	m_conditions.clear			();
}

#undef TEMPLATE_SPECIALIZATION
#undef CConditionStateAbstract