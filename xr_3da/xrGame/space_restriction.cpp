////////////////////////////////////////////////////////////////////////////
//	Module 		: space_restriction.cpp
//	Created 	: 17.08.2004
//  Modified 	: 17.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Space restriction
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "space_restriction.h"

extern bool CCF_Shape_inside(const CCF_Shape *self, const Fvector &position, float radius);

CSpaceRestriction::CSpaceRestriction	(ref_str space_restrictors)
{
	m_complex_shape		= false;
}

CSpaceRestriction::~CSpaceRestriction	()
{
	if (m_complex_shape)
		xr_delete		(m_shape);
}

bool CSpaceRestriction::inside			(const Fvector &position, float radius) const
{
	return				(CCF_Shape_inside(m_shape,position,radius));
}
