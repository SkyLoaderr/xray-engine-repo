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

const u32 time_to_delete = 300000;

CSpaceRestrictionHolder::~CSpaceRestrictionHolder			()
{
	delete_data				(m_restrictions);
}

SpaceRestrictionHolder::CBaseRestrictionPtr CSpaceRestrictionHolder::restriction	(shared_str space_restrictors)
{
	if (!xr_strlen(space_restrictors))
		return				(0);

	space_restrictors		= normalize_string(space_restrictors);
	
	RESTRICTIONS::const_iterator	I = m_restrictions.find(space_restrictors);
	if (I != m_restrictions.end())
		return				((*I).second);

	collect_garbage			();

	CSpaceRestrictionBase	*composition = xr_new<CSpaceRestrictionComposition>(this,space_restrictors);
	CSpaceRestrictionBridge	*bridge = xr_new<CSpaceRestrictionBridge>(composition);
	m_restrictions.insert	(std::make_pair(space_restrictors,bridge));
	return					(bridge);
}

void CSpaceRestrictionHolder::register_restrictor				(CSpaceRestrictor *space_restrictor, const RestrictionSpace::ERestrictorTypes &restrictor_type)
{
	shared_str					space_restrictors = space_restrictor->cName();
	if (restrictor_type != RestrictionSpace::eDefaultRestrictorTypeNone) {
		shared_str				*temp = 0, temp1;
		if (restrictor_type == RestrictionSpace::eDefaultRestrictorTypeOut)
			temp			= &m_default_out_restrictions;
		else
			if (restrictor_type == RestrictionSpace::eDefaultRestrictorTypeIn)
				temp		= &m_default_in_restrictions;
			else
				NODEFAULT;
		temp1				= *temp;
		
		if (xr_strlen(*temp) && xr_strlen(space_restrictors))
			strconcat		(m_temp_string,**temp,",",*space_restrictors);
		else
			strconcat		(m_temp_string,**temp,*space_restrictors);

		*temp				= normalize_string(m_temp_string);
		
		if (xr_strcmp(*temp,temp1))
			on_default_restrictions_changed	(restrictor_type,temp1,*temp);
	}
	
	CSpaceRestrictionShape	*shape = xr_new<CSpaceRestrictionShape>(space_restrictor,restrictor_type != RestrictionSpace::eDefaultRestrictorTypeNone);
	RESTRICTIONS::iterator	I = m_restrictions.find(space_restrictors);
	if (I == m_restrictions.end()) {
		CSpaceRestrictionBridge	*bridge = xr_new<CSpaceRestrictionBridge>(shape);
		m_restrictions.insert	(std::make_pair(space_restrictors,bridge));
		return;
	}

	(*I).second->change_implementation(shape);
}

IC	void CSpaceRestrictionHolder::collect_garbage			()
{
	RESTRICTIONS::iterator	I = m_restrictions.begin(), J;
	RESTRICTIONS::iterator	E = m_restrictions.end();
	for ( ; I != E; ) {
		if (!(*I).second->shape() && !(*I).second->m_ref_count && (Device.dwTimeGlobal >= (*I).second->m_last_time_dec + time_to_delete)) {
			J				= I;
			++I;
			xr_delete		((*J).second);
			m_restrictions.erase(J);
		}
		else
			++I;
	}
}
