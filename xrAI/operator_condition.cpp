////////////////////////////////////////////////////////////////////////////
//	Module 		: operator_condition.cpp
//	Created 	: 24.02.2004
//  Modified 	: 24.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Operator condition
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "operator_condition.h"

#define TEMPLATE_SPECIALIZATION template<\
	typename T2,\
	typename T3\
>

#define CAbstractOperatorCondition COperatorConditionAbstract<T2,T3>

TEMPLATE_SPECIALIZATION
CAbstractOperatorCondition::~COperatorConditionAbstract		()
{
}

#undef TEMPLATE_SPECIALIZATION
#undef CAbstractOperatorCondition
