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
	u32											m_hash;

public:
	IC											CConditionState		();
	virtual										~CConditionState	();
	IC	const xr_vector<COperatorCondition>		&conditions			() const;
	IC	void									add_condition		(const COperatorCondition &condition);
	IC	void									add_condition		(typename xr_vector<COperatorCondition>::const_iterator &J, const COperatorCondition &condition);
	IC	u8										weight				(const CConditionState &condition) const;
	IC	bool									operator<			(const CConditionState &condition) const;
	IC	bool									includes			(const CConditionState &condition) const;
	IC	void									clear				();
	IC	CSConditionState						&operator-=			(const CConditionState &condition);
	IC	bool 									operator==			(const CConditionState &condition);
	IC											operator u32		() const;
	IC	u32										hash_value			() const;
};

#include "condition_state_inline.h"