////////////////////////////////////////////////////////////////////////////
//	Module 		: operator_condition.h
//	Created 	: 24.02.2004
//  Modified 	: 24.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Operator condition
////////////////////////////////////////////////////////////////////////////

#pragma once

template <
	typename T2,
	typename T3
>
class COperatorConditionAbstract {
protected:
	T2					m_condition;
	T3					m_value;

public:
	IC					COperatorConditionAbstract	(const T2 condition, const T3 value);
	virtual				~COperatorConditionAbstract	();
	IC		T2			condition					() const;
	IC		T3			value						() const;
};

#include "operator_condition_inline.h"
