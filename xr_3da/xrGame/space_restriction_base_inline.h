////////////////////////////////////////////////////////////////////////////
//	Module 		: space_restriction_base_inline.h
//	Created 	: 17.08.2004
//  Modified 	: 27.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Space restriction base inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CSpaceRestrictionBase::CSpaceRestrictionBase		()
{
	m_initialized				= false;
}

IC	const xr_vector<u32> &CSpaceRestrictionBase::border	()
{
	if (!initialized())
		initialize				();
	VERIFY						(initialized());
	VERIFY3						(!m_border.empty(),"Space restrictor has no border!",*name());
	return						(m_border);
}

IC	bool CSpaceRestrictionBase::initialized				() const
{
	return						(m_initialized);
}
