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
protected:
	typedef CConditionState<T2,T3>				CSConditionState;
	typedef COperatorConditionAbstract<T2,T3>	COperatorCondition;

protected:
	xr_vector<COperatorCondition>				m_conditions;

public:
	IC											CConditionState		();
	IC											CConditionState		(const xr_vector<COperatorCondition> &conditions);
	virtual										~CConditionState	();
	IC	const xr_vector<COperatorCondition>		&conditions			() const;
	IC	void									add_condition		(const COperatorCondition &condition);
	IC	u8										weight				(const CConditionState &condition) const;
	IC	u8										weight				(const CConditionState &condition, const CConditionState &start) const;
	IC	bool									operator<			(const CConditionState &condition) const;
	IC	bool									includes			(const CConditionState &condition) const;
	IC	bool									includes			(const CConditionState &condition, const CConditionState &start) const;
	IC	void									clear				();
	IC	CSConditionState						&operator-=			(const CConditionState &condition);
};

#include "condition_state_inline.h"