////////////////////////////////////////////////////////////////////////////
//	Module 		: motivation_inline.h
//	Created 	: 22.03.2004
//  Modified 	: 22.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Motivation class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION template <typename _object_type>
#define CSMotivation			CMotivation<_object_type>

TEMPLATE_SPECIALIZATION
IC	CSMotivation::CMotivation		()
{
	init				();
}

TEMPLATE_SPECIALIZATION
CSMotivation::~CMotivation			()
{
}

TEMPLATE_SPECIALIZATION
IC	void CSMotivation::init			()
{
	m_object			= 0;
}

TEMPLATE_SPECIALIZATION
void CSMotivation::reinit			(_object_type *object)
{
	m_object			= object;
}

TEMPLATE_SPECIALIZATION
void CSMotivation::Load				(LPCSTR section)
{
}

TEMPLATE_SPECIALIZATION
void CSMotivation::reload			(LPCSTR section)
{
}

TEMPLATE_SPECIALIZATION
float CSMotivation::evaluate		(u32 sub_motivation_id)
{
	return				(0.f);
}

#undef TEMPLATE_SPECIALIZATION
#undef CSMotivation