////////////////////////////////////////////////////////////////////////////
//	Module 		: operator_effect.cpp
//	Created 	: 24.02.2004
//  Modified 	: 24.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Operator effect
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "operator_effect.h"

#define TEMPLATE_SPECIALIZATION template<\
	typename T2,\
	typename T3\
>

#define CAbstractOperatorEffect COperatorEffectAbstract<T2,T3>

TEMPLATE_SPECIALIZATION
CAbstractOperatorEffect::~COperatorEffectAbstract		()
{
}

#undef TEMPLATE_SPECIALIZATION
#undef CAbstractOperatorEffect
