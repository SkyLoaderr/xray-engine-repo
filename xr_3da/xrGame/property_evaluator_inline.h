////////////////////////////////////////////////////////////////////////////
//	Module 		: property_evaluator_inline.h
//	Created 	: 12.03.2004
//  Modified 	: 12.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Property evaluator inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION template <typename _object_type>
#define CEvaluator				CPropertyEvaluator<_object_type>

TEMPLATE_SPECIALIZATION
IC	CEvaluator::CPropertyEvaluator	()
{
	init				();
}

TEMPLATE_SPECIALIZATION
IC	CEvaluator::~CPropertyEvaluator	()
{
}

TEMPLATE_SPECIALIZATION
IC	void CEvaluator::init			()
{
}

TEMPLATE_SPECIALIZATION
void CEvaluator::reinit				(_object_type *object)
{
	m_object			= object;
}

TEMPLATE_SPECIALIZATION
void CEvaluator::Load				(LPCSTR section)
{
}

TEMPLATE_SPECIALIZATION
void CEvaluator::reload				(LPCSTR section)
{
}

TEMPLATE_SPECIALIZATION
bool CEvaluator::evaluate			()
{
	return				(false);
}

#undef TEMPLATE_SPECIALIZATION
#undef CEvaluator