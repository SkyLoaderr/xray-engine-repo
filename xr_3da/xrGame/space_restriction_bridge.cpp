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
#include "ai_space.h"
#include "level_graph.h"

CSpaceRestrictionBridge::~CSpaceRestrictionBridge		()
{
	xr_delete			(m_object);
}

void CSpaceRestrictionBridge::change_implementation		(CSpaceRestrictionBase *object)
{
	xr_delete			(m_object);
	m_object			= object;
}

const xr_vector<u32> &CSpaceRestrictionBridge::border	() const
{
	return				(object().border());
}

bool CSpaceRestrictionBridge::initialized				() const
{
	return				(object().initialized());
}

void CSpaceRestrictionBridge::initialize				()
{
	object().initialize	();
}

shared_str CSpaceRestrictionBridge::name				() const
{
	return				(object().name());
}

u32	CSpaceRestrictionBridge::accessible_nearest			(const Fvector &position, Fvector &result, bool out_restriction)
{
	return				(accessible_nearest(m_object,position,result,out_restriction));
}

bool CSpaceRestrictionBridge::shape						() const
{
	return				(object().shape());
}

bool CSpaceRestrictionBridge::default_restrictor		() const
{
	return				(object().default_restrictor());
}

bool CSpaceRestrictionBridge::inside					(u32 level_vertex_id, bool partially_inside)
{
	return		(object().inside(level_vertex_id,partially_inside));
}

bool CSpaceRestrictionBridge::inside					(u32 level_vertex_id, bool partially_inside, float radius)
{
	return		(object().inside(level_vertex_id,partially_inside,radius));
}

bool CSpaceRestrictionBridge::inside					(const Fvector &position)
{
	return		(object().inside(position));
}

bool CSpaceRestrictionBridge::inside					(const Fvector &position, float radius)
{
	return		(object().inside(position,radius));
}

bool CSpaceRestrictionBridge::on_border					(const Fvector &position) const
{
	xr_vector<u32>::const_iterator	I = object().border().begin();
	xr_vector<u32>::const_iterator	E = object().border().end();
	for ( ; I != E; ++I)
		if (ai().level_graph().inside(*I,position)) {
			if (_abs(ai().level_graph().vertex_plane_y(*I) - position.y) < 2.f)
				return	(true);
		}
	return				(false);
}
