////////////////////////////////////////////////////////////////////////////
//	Module 		: condition_state.h
//	Created 	: 26.02.2004
//  Modified 	: 26.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Condition state
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "operator_condition.h"

template <
	typename T2,
	typename T3
>
class CConditionState {
private:
	typedef COperatorConditionAbstract<T2,T3>	COperatorCondition;

protected:
	xr_vector<COperatorCondition>				m_conditions;

public:
	IC											CConditionState		(const xr_vector<COperatorCondition> &conditions);
	virtual										~CConditionState	();
	IC		const xr_vector<COperatorCondition>	&conditions			() const;
};

#include "condition_state_inline.h"