////////////////////////////////////////////////////////////////////////////
//	Module 		: restricted_object_inline.h
//	Created 	: 18.08.2004
//  Modified 	: 23.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Restricted object inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	RestrictedObject::CRestrictionContainer::CRestrictionContainer	()
{
	m_id_seed					= 0xffff;
}

IC	bool RestrictedObject::CRestrictionContainer::empty				() const
{
	return						(m_restrictions.empty());
}

IC	bool RestrictedObject::CRestrictionContainer::accessible	(const Fvector &position) const
{
	RESTRICTIONS::const_iterator	I = m_restrictions.begin();
	RESTRICTIONS::const_iterator	E = m_restrictions.end();
	for ( ; I != E; ++I)
		if ((*I).second->accessible(position))
			return					(true);
	return							(false);
}

IC	void RestrictedObject::CRestrictionContainer::add_restriction	(u32 restriction_id, const Fvector &position, float radius)
{
	RESTRICTIONS::const_iterator	I = m_restrictions.find(restriction_id);
	VERIFY							(I == m_restrictions.end());
	m_restrictions.insert			(std::make_pair(restriction_id,xr_new<RestrictedObject::CDynamicRestriction>(position,radius)));
}

IC	u32	 RestrictedObject::CRestrictionContainer::add_restriction	(const Fvector &position, float radius)
{
	RESTRICTIONS::const_iterator	I = m_restrictions.find(m_id_seed);
	VERIFY							(I == m_restrictions.end());
	m_restrictions.insert			(std::make_pair(m_id_seed,xr_new<RestrictedObject::CDynamicRestriction>(position,radius)));
	return							(m_id_seed++);
}

IC	void RestrictedObject::CRestrictionContainer::remove_restriction	(u32 restriction_id)
{
	RESTRICTIONS::iterator			I = m_restrictions.find(restriction_id);
	VERIFY							(I != m_restrictions.end());
	m_restrictions.erase			(I);
}

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

IC	RestrictedObject::CRestrictionContainer &CRestrictedObject::container(RestrictedObject::ERestrictionType type) const
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
