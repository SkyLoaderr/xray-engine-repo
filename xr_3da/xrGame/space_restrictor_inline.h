////////////////////////////////////////////////////////////////////////////
//	Module 		: space_restrictor_inline.h
//	Created 	: 17.08.2004
//  Modified 	: 17.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Space restrictor inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CSpaceRestrictor::CSpaceRestrictor		()
{
}

IC	bool CSpaceRestrictor::actual			() const
{
	return							(m_actuality);
}

IC	void CSpaceRestrictor::actual			(bool value) const
{
	m_actuality						= value;
}
