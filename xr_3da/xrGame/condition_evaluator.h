////////////////////////////////////////////////////////////////////////////
//	Module 		: condition_evaluator.h
//	Created 	: 09.03.2004
//  Modified 	: 09.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Condition evaluator
////////////////////////////////////////////////////////////////////////////

#pragma once

template <typename _value_type>
interface IConditionEvaluator {
public:
	virtual _value_type evaluate				() const = 0;
};