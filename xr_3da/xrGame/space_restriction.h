////////////////////////////////////////////////////////////////////////////
//	Module 		: space_restriction.h
//	Created 	: 17.08.2004
//  Modified 	: 17.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Space restriction
////////////////////////////////////////////////////////////////////////////

#pragma once

class CSpaceRestriction {
protected:
	CCF_Shape		*m_shape;
	bool			m_complex_shape;

public:
					CSpaceRestriction	(ref_str space_restrictors);
	virtual			~CSpaceRestriction	();
			bool	inside				(const Fvector &position, float radius = EPS_L) const;
};

#include "space_restriction_inline.h"