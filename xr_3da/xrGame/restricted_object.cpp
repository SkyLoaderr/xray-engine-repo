////////////////////////////////////////////////////////////////////////////
//	Module 		: restricted_object.cpp
//	Created 	: 18.08.2004
//  Modified 	: 23.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Restricted object
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "restricted_object.h"
#include "space_restriction.h"
#include "level.h"
#include "space_restrictor_manager.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "space_restrictor.h"
#include "object_broker.h"

bool RestrictedObject::CShapeRestriction::accessible	(const Fvector &position) const
{
	VERIFY							(m_restrictor);
	return							(!m_restrictor->inside(position));
}

CRestrictedObject::~CRestrictedObject	()
{
	clear						();
}

BOOL CRestrictedObject::net_Spawn		(LPVOID data)
{
	if (!inherited::net_Spawn(data))
		return					(FALSE);

	CSE_Abstract				*abstract	= (CSE_Abstract*)(data);
	CSE_ALifeMonsterAbstract	*monster	= dynamic_cast<CSE_ALifeMonsterAbstract*>(abstract);
	VERIFY						(monster);
	
	m_in_restrictions			= xr_new<CRestrictionContainer>();
	m_out_restrictions			= xr_new<CRestrictionContainer>();

	Level().space_restrictor_manager().restrict		(ID(),monster->m_space_restrictors);

	return						(TRUE);
}

void CRestrictedObject::net_Destroy		()
{
	inherited::net_Destroy		();
	clear						();
}

void CRestrictedObject::add_border		() const
{
	if (accessible(Position()))
		Level().space_restrictor_manager().add_border	(ID());
}

void CRestrictedObject::remove_border	() const
{
	if (accessible(Position()))
		Level().space_restrictor_manager().remove_border(ID());
}

bool CRestrictedObject::accessible		(const Fvector &position) const
{
	return		(
		Level().space_restrictor_manager().accessible(ID(),position) &&
		!container(RestrictedObject::eRestrictionTypeIn).accessible(position) &&
		(
			container(RestrictedObject::eRestrictionTypeOut).empty() || 
			container(RestrictedObject::eRestrictionTypeOut).accessible(position)
		)
	);
}

bool CRestrictedObject::accessible		(u32 vertex_id) const
{
	Fvector		position = ai().level_graph().vertex_position(vertex_id);
	float		offset = ai().level_graph().header().cell_size()*.5f;
	return		(
		accessible(Fvector().set(position.x + offset,position.y,position.z + offset)) || 
		accessible(Fvector().set(position.x + offset,position.y,position.z - offset)) || 
		accessible(Fvector().set(position.x - offset,position.y,position.z + offset)) || 
		accessible(Fvector().set(position.x - offset,position.y,position.z - offset)) ||
		accessible(Fvector().set(position.x,position.y,position.z))
	);
}

u32	CRestrictedObject::accessible_nearest	(const Fvector &position, Fvector &result) const
{
#pragma todo("Dima to Dima : _Warning : this place can be optimized in case of a slowdown")
	VERIFY	(!accessible(position));
	const xr_vector<u32> &border = Level().space_restrictor_manager().restriction(ID())->border();

	float	min_dist_sqr = flt_max;
	u32		selected = u32(-1);
	xr_vector<u32>::const_iterator	I = border.begin();
	xr_vector<u32>::const_iterator	E = border.end();
	for ( ; I != E; ++I) {
		float	distance_sqr = ai().level_graph().vertex_position(*I).distance_to_sqr(position);
		if (distance_sqr < min_dist_sqr) {
			min_dist_sqr	= distance_sqr;
			selected		= *I;
		}
	}
	VERIFY	(ai().level_graph().valid_vertex_id(selected));
	{
		min_dist_sqr = flt_max;
		u32	new_selected = u32(-1);
		CLevelGraph::const_iterator	I, E;
		ai().level_graph().begin(selected,I,E);
		for ( ; I != E; ++I) {
			u32	current = ai().level_graph().value(selected,I);
			if (!ai().level_graph().valid_vertex_id(current))
				continue;
			if (!accessible(current))
				continue;
			float	distance_sqr = ai().level_graph().vertex_position(current).distance_to_sqr(position);
			if (distance_sqr < min_dist_sqr) {
				min_dist_sqr = distance_sqr;
				new_selected = current;
			}
		}
		selected	= new_selected;
	}
	VERIFY	(ai().level_graph().valid_vertex_id(selected));
	{
		Fvector		center = ai().level_graph().vertex_position(selected);
		float		offset = ai().level_graph().header().cell_size()*.5f;
		bool		found = false;
		min_dist_sqr = flt_max;
		for (u32 i=0; i<5; ++i) {
			Fvector	current;
			switch (i) {
				case 0 : current.set(center.x + offset,center.y,center.z + offset); break;
				case 1 : current.set(center.x + offset,center.y,center.z - offset); break;
				case 2 : current.set(center.x - offset,center.y,center.z + offset); break;
				case 3 : current.set(center.x - offset,center.y,center.z - offset); break;
				case 4 : current.set(center.x,center.y,center.z); break;
				default : NODEFAULT;
			}
			if (i < 4)
				center.y = ai().level_graph().vertex_plane_y(selected,center.x,center.z);
			if (!accessible(current))
				continue;
			float	distance_sqr = current.distance_to(position);
			if (distance_sqr < min_dist_sqr) {
				min_dist_sqr = distance_sqr;
				result = current;
				found = true;
			}
		}
		VERIFY	(found);
	}
	VERIFY	(ai().level_graph().valid_vertex_id(selected));
	return	(selected);
}
