////////////////////////////////////////////////////////////////////////////
//	Module 		: restricted_object_inline.h
//	Created 	: 18.08.2004
//  Modified 	: 23.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Restricted object inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CRestrictedObject::CRestrictedObject	()
{
	m_in_restrictions				= 0;
	m_out_restrictions				= 0;
}

IC	void CRestrictedObject::clear				()
{
	xr_delete	(m_in_restrictions);
	xr_delete	(m_out_restrictions);
}

IC	CRestrictionContainer &CRestrictedObject::container(RestrictedObject::ERestrictionType type) const
{
	return		*(type == RestrictedObject::eRestrictionTypeIn ? m_in_restrictions : m_out_restrictions);
}

IC	void CRestrictedObject::add_restriction		(RestrictedObject::ERestrictionType type, CSpaceRestrictor *restrictor)
{
	container(type).add_restriction(restrictor);
}

IC	void CRestrictedObject::add_restriction		(RestrictedObject::ERestrictionType type, u32 restriction_id, const Fvector &position, float radius)
{
	container(type).add_restriction(restriction_id,position,radius);
}

IC	u32	 CRestrictedObject::add_restriction		(RestrictedObject::ERestrictionType type, const Fvector &position, float radius)
{
	return		(container(type).add_restriction(position,radius));
}

IC	void CRestrictedObject::remove_restriction	(RestrictedObject::ERestrictionType type, u32 restriction_id)
{
	container(type).remove_restriction(restriction_id);
}
