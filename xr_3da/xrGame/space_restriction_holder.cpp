////////////////////////////////////////////////////////////////////////////
//	Module 		: space_restriction_holder.cpp
//	Created 	: 17.08.2004
//  Modified 	: 27.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Space restriction holder
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "space_restriction_holder.h"
#include "object_broker.h"
#include "space_restrictor.h"
#include "space_restriction_bridge.h"
#include "space_restriction_shape.h"
#include "space_restriction_composition.h"
#include "restriction_space.h"

const u32 time_to_delete = 30000;

CSpaceRestrictionHolder::~CSpaceRestrictionHolder			()
{
	delete_data				(m_restrictions);
}

SpaceRestrictionHolder::CBaseRestrictionPtr CSpaceRestrictionHolder::restriction	(ref_str space_restrictors)
{
	if (!xr_strlen(space_restrictors))
		return				(0);

	space_restrictors		= normalize_string(space_restrictors);
	
	RESTRICTIONS::const_iterator	I = m_restrictions.find(space_restrictors);
	if (I != m_restrictions.end())
		return				((*I).second);

	remove_unused			();

//	Msg						("INTRUSIVE : adding CSpaceRestrictionBase %s",*space_restrictors);
	CSpaceRestrictionBase	*composition = xr_new<CSpaceRestrictionComposition>(this,space_restrictors);
	CSpaceRestrictionBridge	*bridge = xr_new<CSpaceRestrictionBridge>(composition);
	m_restrictions.insert	(std::make_pair(space_restrictors,bridge));
	return					(bridge);
}

void CSpaceRestrictionHolder::register_restrictor				(CSpaceRestrictor *space_restrictor)
{
	ref_str					space_restrictors = space_restrictor->cName();
	CSpaceRestrictionShape	*shape = xr_new<CSpaceRestrictionShape>(space_restrictor);

//	Msg						("INTRUSIVE : adding CSpaceRestrictionBase %s",*space_restrictors);
	RESTRICTIONS::iterator	I = m_restrictions.find(space_restrictors);
	if (I == m_restrictions.end()) {
		CSpaceRestrictionBridge	*bridge = xr_new<CSpaceRestrictionBridge>(shape);
		m_restrictions.insert	(std::make_pair(space_restrictors,bridge));
		return;
	}

	(*I).second->change_implementation(shape);
}

IC	void CSpaceRestrictionHolder::remove_unused					()
{
	RESTRICTIONS::iterator	I = m_restrictions.begin(), J;
	RESTRICTIONS::iterator	E = m_restrictions.end();
	for ( ; I != E; ) {
		if (!(*I).second->shape() && !(*I).second->m_ref_count && (Device.dwTimeGlobal >= (*I).second->m_last_time_dec + time_to_delete)) {
			J				= I;
			++I;
//			Msg				("INTRUSIVE : removing CSpaceRestrictionBase %s",*(*J).second->name());
			xr_delete		((*J).second);
			m_restrictions.erase(J);
		}
		else
			++I;
	}
}
