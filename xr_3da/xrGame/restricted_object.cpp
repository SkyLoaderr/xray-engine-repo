////////////////////////////////////////////////////////////////////////////
//	Module 		: restricted_object.cpp
//	Created 	: 18.08.2004
//  Modified 	: 23.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Restricted object
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "restricted_object.h"
#include "space_restriction_manager.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "level.h"
#include "ai_space.h"
#include "level_graph.h"
#include "space_restriction.h"
#include "space_restriction_bridge.h"

CRestrictedObject::~CRestrictedObject		()
{
}

BOOL CRestrictedObject::net_Spawn			(LPVOID data)
{
	if (!inherited::net_Spawn(data))
		return					(FALSE);

	CSE_Abstract				*abstract	= (CSE_Abstract*)(data);
	CSE_ALifeMonsterAbstract	*monster	= smart_cast<CSE_ALifeMonsterAbstract*>(abstract);
	VERIFY						(monster);
	m_applied					= false;
	m_removed					= true;
	
	Level().space_restriction_manager().restrict	(ID(),monster->m_out_space_restrictors,monster->m_in_space_restrictors);

	return						(TRUE);
}

u32	CRestrictedObject::accessible_nearest	(const Fvector &position, Fvector &result) const
{
	VERIFY						(!accessible(position));
	return						(Level().space_restriction_manager().accessible_nearest(ID(),position,result));
}

bool CRestrictedObject::accessible			(const Fvector &position, float radius) const
{
	return						(Level().space_restriction_manager().accessible(ID(),position,radius));
}

bool CRestrictedObject::accessible			(u32 level_vertex_id, float radius) const
{
	return						(Level().space_restriction_manager().accessible(ID(),level_vertex_id,radius));
}

void CRestrictedObject::add_restrictions	(ref_str out_restrictions, ref_str in_restrictions)
{
	Level().space_restriction_manager().add_restrictions	(ID(),out_restrictions,in_restrictions);
}

void CRestrictedObject::remove_restrictions	(ref_str out_restrictions, ref_str in_restrictions)
{
	Level().space_restriction_manager().remove_restrictions	(ID(),out_restrictions,in_restrictions);
}

void CRestrictedObject::add_border			(u32 start_vertex_id, float radius) const
{
	VERIFY						(!m_applied);
	VERIFY						(m_removed);
	m_removed					= false;
	if (accessible(start_vertex_id)) {
		m_applied				= true;
		Level().space_restriction_manager().add_border(ID(),start_vertex_id,radius);
	}
}

void CRestrictedObject::add_border			(const Fvector &start_position, const Fvector &dest_position) const
{
	VERIFY						(!m_applied);
	VERIFY						(m_removed);
	m_removed					= false;
	if (accessible(start_position)) {
		m_applied				= true;
		Level().space_restriction_manager().add_border(ID(),start_position,dest_position);
	}
}

void CRestrictedObject::add_border			(u32 start_vertex_id, u32 dest_vertex_id) const
{
	VERIFY						(!m_applied);
	VERIFY						(m_removed);
	m_removed					= false;
	if (accessible(start_vertex_id)) {
		m_applied				= true;
		Level().space_restriction_manager().add_border(ID(),start_vertex_id,dest_vertex_id);
	}
}

void CRestrictedObject::remove_border		() const
{
	VERIFY						(!m_removed);
	m_removed					= true;
	if (m_applied)
		Level().space_restriction_manager().remove_border(ID());
	m_applied					= false;
}

void CRestrictedObject::remove_all_restrictions	()
{
	Level().space_restriction_manager().restrict(ID(),"","");
}

ref_str	CRestrictedObject::in_restrictions	() const
{
	return						(Level().space_restriction_manager().in_restrictions(ID()));
}

ref_str CRestrictedObject::out_restrictions	() const
{
	return						(Level().space_restriction_manager().out_restrictions(ID()));
}
