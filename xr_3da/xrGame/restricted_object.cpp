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
#include "space_restriction_base.h"
#include "profiler.h"
#include "alife_simulator.h"
#include "alife_object_registry.h"

CRestrictedObject::~CRestrictedObject		()
{
}

IC	void construct_string					(LPSTR result, const xr_vector<ALife::_OBJECT_ID> &restrictions)
{
	strcpy	(result,"");
	u32		count = 0;
	xr_vector<ALife::_OBJECT_ID>::const_iterator	I = restrictions.begin();
	xr_vector<ALife::_OBJECT_ID>::const_iterator	E = restrictions.end();
	for ( ; I != E; ++I) {
		CSE_ALifeDynamicObject	*object = ai().alife().objects().object(*I);
		if (ai().game_graph().vertex(object->m_tGraphID)->level_id() != ai().level_graph().level_id())
			continue;

		if (count)
			strcat(result,",");
		strcat(result,object->name_replace());
		++count;
	}
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
	
	if (!ai().get_alife())
		return					(TRUE);

	string4096					temp0;
	string4096					temp1;
	
	construct_string			(temp0,monster->m_dynamic_out_restrictions);
	construct_string			(temp1,monster->m_dynamic_in_restrictions);

	Level().space_restriction_manager().add_restrictions(ID(),temp0,temp1);

	return						(TRUE);
}

u32	CRestrictedObject::accessible_nearest	(const Fvector &position, Fvector &result) const
{
	START_PROFILE("AI/Restricted Object/Accessible Nearest");
	VERIFY						(!accessible(position));
	return						(Level().space_restriction_manager().accessible_nearest(ID(),position,result));
	STOP_PROFILE;
}

bool CRestrictedObject::accessible			(const Fvector &position) const
{
	START_PROFILE("AI/Restricted Object/Accessible");
	return						(accessible(position,EPS_L));
	STOP_PROFILE;
}

bool CRestrictedObject::accessible			(const Fvector &position, float radius) const
{
	START_PROFILE("AI/Restricted Object/Accessible");
	return						(Level().space_restriction_manager().accessible(ID(),position,radius));
	STOP_PROFILE;
}

bool CRestrictedObject::accessible			(u32 level_vertex_id) const
{
	START_PROFILE("AI/Restricted Object/Accessible");
	return						(accessible(level_vertex_id,EPS_L));
	STOP_PROFILE;
}

bool CRestrictedObject::accessible			(u32 level_vertex_id, float radius) const
{
	START_PROFILE("AI/Restricted Object/Accessible");
	return						(Level().space_restriction_manager().accessible(ID(),level_vertex_id,radius));
	STOP_PROFILE;
}

void CRestrictedObject::add_border			(u32 start_vertex_id, float radius) const
{
	START_PROFILE("AI/Restricted Object/Add Border");	
	
	VERIFY						(!m_applied);
	VERIFY						(m_removed);
	m_removed					= false;
	if (accessible(start_vertex_id)) {
		m_applied				= true;
		Level().space_restriction_manager().add_border(ID(),start_vertex_id,radius);
	}
	
	STOP_PROFILE;
}

void CRestrictedObject::add_border			(const Fvector &start_position, const Fvector &dest_position) const
{
	START_PROFILE("AI/Restricted Object/Add Border");	

	VERIFY						(!m_applied);
	VERIFY						(m_removed);
	m_removed					= false;
	if (accessible(start_position)) {
		m_applied				= true;
		Level().space_restriction_manager().add_border(ID(),start_position,dest_position);
	}

	STOP_PROFILE;
}

void CRestrictedObject::add_border			(u32 start_vertex_id, u32 dest_vertex_id) const
{
	START_PROFILE("AI/Restricted Object/Add Border");	
	VERIFY						(!m_applied);
	VERIFY						(m_removed);
	m_removed					= false;
	if (accessible(start_vertex_id)) {
		m_applied				= true;
		Level().space_restriction_manager().add_border(ID(),start_vertex_id,dest_vertex_id);
	}
	STOP_PROFILE;
}

void CRestrictedObject::remove_border		() const
{
	START_PROFILE("AI/Restricted Object/Remove Border");
	
	VERIFY						(!m_removed);
	m_removed					= true;
	if (m_applied)
		Level().space_restriction_manager().remove_border(ID());
	m_applied					= false;
	
	STOP_PROFILE;
}

void CRestrictedObject::remove_all_restrictions	()
{
	START_PROFILE("AI/Restricted Object/Remove Restrictions");
	Level().space_restriction_manager().restrict(ID(),"","");
	STOP_PROFILE;
}

shared_str	CRestrictedObject::in_restrictions	() const
{
	return						(Level().space_restriction_manager().in_restrictions(ID()));
}

shared_str CRestrictedObject::out_restrictions	() const
{
	return						(Level().space_restriction_manager().out_restrictions(ID()));
}

template <bool value>
IC	void CRestrictedObject::construct_restriction_string(LPSTR temp_restrictions, const xr_vector<ALife::_OBJECT_ID> &restrictions, shared_str current_restrictions)
{
	u32							count = 0;
	strcpy						(temp_restrictions,"");
	xr_vector<ALife::_OBJECT_ID>::const_iterator	I = restrictions.begin();
	xr_vector<ALife::_OBJECT_ID>::const_iterator	E = restrictions.end();
	for ( ; I != E; ++I) {
		CObject					*object = Level().Objects.net_Find(*I);
		if (!object || !!strstr(*current_restrictions,*object->cName()) == value)
			continue;

		if (count)
			strcat(temp_restrictions,",");

		strcat(temp_restrictions,*object->cName());

		count++;
	}
}

void CRestrictedObject::add_restrictions	(const xr_vector<ALife::_OBJECT_ID> &out_restrictions, const xr_vector<ALife::_OBJECT_ID> &in_restrictions)
{
	START_PROFILE("AI/Restricted Object/Add Restrictions");

	string4096					temp_out_restrictions;
	string4096					temp_in_restrictions;
	
	construct_restriction_string<true>(temp_out_restrictions,out_restrictions,this->out_restrictions());
	construct_restriction_string<true>(temp_in_restrictions,in_restrictions,this->in_restrictions());

	Level().space_restriction_manager().add_restrictions	(ID(),temp_out_restrictions,temp_in_restrictions);
	
	STOP_PROFILE;
}

void CRestrictedObject::remove_restrictions	(const xr_vector<ALife::_OBJECT_ID> &out_restrictions, const xr_vector<ALife::_OBJECT_ID> &in_restrictions)
{
	START_PROFILE("AI/Restricted Object/Remove Restrictions");
	
	string4096					temp_out_restrictions;
	string4096					temp_in_restrictions;

	construct_restriction_string<false>(temp_out_restrictions,out_restrictions,this->out_restrictions());
	construct_restriction_string<false>(temp_in_restrictions,in_restrictions,this->in_restrictions());

	Level().space_restriction_manager().remove_restrictions	(ID(),temp_out_restrictions,temp_in_restrictions);
	
	STOP_PROFILE;
}

