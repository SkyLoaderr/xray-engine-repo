////////////////////////////////////////////////////////////////////////////
//	Module 		: space_restriction_bridge.cpp
//	Created 	: 27.08.2004
//  Modified 	: 27.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Space restriction bridge
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "space_restriction_bridge.h"
#include "space_restriction_base.h"

CSpaceRestrictionBridge::~CSpaceRestrictionBridge		()
{
	xr_delete			(m_object);
}

void CSpaceRestrictionBridge::change_implementation		(CSpaceRestrictionBase *object)
{
	xr_delete			(m_object);
	m_object			= object;
}

const xr_vector<u32> &CSpaceRestrictionBridge::border	(bool out_restriction) const
{
	return				(object().border(out_restriction));
}

bool CSpaceRestrictionBridge::initialized				() const
{
	return				(object().initialized());
}

void CSpaceRestrictionBridge::initialize				()
{
	object().initialize	();
}

ref_str CSpaceRestrictionBridge::name					() const
{
	return				(object().name());
}

u32	CSpaceRestrictionBridge::accessible_nearest			(const Fvector &position, Fvector &result, bool out_restriction)
{
	return				(object().accessible_nearest(position,result,out_restriction));
}

bool CSpaceRestrictionBridge::shape						() const
{
	return				(object().shape());
}

bool CSpaceRestrictionBridge::default_restrictor		() const
{
	return				(object().default_restrictor());
}

bool CSpaceRestrictionBridge::inside					(const Fvector &position, float radius)
{
	return		(object().inside(position,radius));
}

bool CSpaceRestrictionBridge::inside					(u32 level_vertex_id, bool out, float radius)
{
	return		(object().inside(level_vertex_id,out,radius));
}
