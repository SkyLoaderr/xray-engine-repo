////////////////////////////////////////////////////////////////////////////
//	Module 		: condition_state.cpp
//	Created 	: 26.02.2004
//  Modified 	: 26.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Condition state
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "condition_state.h"

#define TEMPLATE_SPECIALIZATION template<\
	typename T2,\
	typename T3\
>

#define CConditionStateAbstract CConditionState<T2,T3>

TEMPLATE_SPECIALIZATION
CConditionStateAbstract::~CConditionState	()
{
}

#undef TEMPLATE_SPECIALIZATION
#undef CConditionStateAbstract
