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

IC	const xr_vector<u32> &CSpaceRestrictionBase::border	(bool out_restriction)
{
	if (!initialized())
		initialize				();
	VERIFY						(initialized());
	return						(out_restriction ? m_out_border : m_in_border);
}

IC	void CSpaceRestrictionBase::process_borders			()
{
	process_borders				(m_out_border);
	process_borders				(m_in_border);
}

IC	void CSpaceRestrictionBase::process_borders			(xr_vector<u32> &border)
{
	std::sort					(border.begin(),border.end());
	xr_vector<u32>::iterator	I = unique(border.begin(),border.end());
	border.erase				(I,border.end());
}

IC	bool CSpaceRestrictionBase::initialized				() const
{
	return						(m_initialized);
}
