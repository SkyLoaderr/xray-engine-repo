////////////////////////////////////////////////////////////////////////////
//	Module 		: space_restriction_bridge_inline.h
//	Created 	: 27.08.2004
//  Modified 	: 27.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Space restriction bridge inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CSpaceRestrictionBridge::CSpaceRestrictionBridge		(CSpaceRestrictionBase *object)
{
	VERIFY		(object);
	m_object	= object;
}

IC	CSpaceRestrictionBase &CSpaceRestrictionBridge::object	() const
{
	VERIFY		(m_object);
	return		(*m_object);
}