////////////////////////////////////////////////////////////////////////////
//	Module 		: space_restriction_inline.h
//	Created 	: 17.08.2004
//  Modified 	: 17.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Space restriction inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	const xr_vector<u32> &CSpaceRestriction::border	()
{
	if (!m_initialized)
		initialize	();
	return			(m_border);
}

IC	ref_str	CSpaceRestriction::space_restrictors	() const
{
	return			(m_space_restrictors);
}

IC	Fvector	CSpaceRestriction::position				(const CCF_Shape::shape_def &data) const
{
	switch (data.type) {
		case 0	: return(data.data.sphere.P);
		case 1	: return(data.data.box.c);
		default : NODEFAULT;
	}
#ifdef DEBUG
	return	(Fvector().set(0.f,0.f,0.f));
#endif
}

IC	float CSpaceRestriction::radius					(const CCF_Shape::shape_def &data) const
{
	switch (data.type) {
		case 0	: return(data.data.sphere.R);
		case 1	: return(Fbox().set(Fvector().set(-.5f,-.5f,-.5f),Fvector().set(.5f,.5f,.5f)).xform(data.data.box).getradius());
		default : NODEFAULT;
	}
#ifdef DEBUG
	return	(0.f);
#endif
}
