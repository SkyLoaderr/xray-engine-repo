////////////////////////////////////////////////////////////////////////////
//	Module 		: operator_abstract.cpp
//	Created 	: 24.02.2004
//  Modified 	: 24.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Operator abstract
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "operator_abstract.h"

#define TEMPLATE_SPECIALIZATION template<\
	typename T1,\
	typename T2,\
	typename T3\
>

#define CAbstractOperator COperatorAbstract<T1,T2,T3>

TEMPLATE_SPECIALIZATION
CAbstractOperator::~COperatorAbstract	()
{
}

#undef TEMPLATE_SPECIALIZATION
#undef CAbstractOperator
