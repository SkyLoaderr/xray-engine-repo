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
	u32		count = xr_strlen(result) ? _GetItemCount(result) : 0;
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
	
	string4096					temp0;
	string4096					temp1;
	
	strcpy						(temp0,*monster->m_out_space_restrictors);
	strcpy						(temp1,*monster->m_in_space_restrictors);

	if (ai().get_alife()) {
		construct_string		(temp0,monster->m_dynamic_out_restrictions);
		construct_string		(temp1,monster->m_dynamic_in_restrictions);
	}

	Level().space_restriction_manager().restrict	(ID(),temp0,temp1);
	
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

IC	void CRestrictedObject::add_object_restriction(ALife::_OBJECT_ID id, const RestrictionSpace::ERestrictorTypes &restrictor_type)
{
	NET_Packet			net_packet;
	u_EventGen			(net_packet,GE_ADD_RESTRICTION,ID());
	net_packet.w		(&id,sizeof(id));
	net_packet.w		(&restrictor_type,sizeof(restrictor_type));
	Level().Send		(net_packet,net_flags(TRUE,TRUE));
}

IC	void CRestrictedObject::remove_object_restriction(ALife::_OBJECT_ID id, const RestrictionSpace::ERestrictorTypes &restrictor_type)
{
	NET_Packet			net_packet;
	u_EventGen			(net_packet,GE_REMOVE_RESTRICTION,ID());
	net_packet.w		(&id,sizeof(id));
	net_packet.w		(&restrictor_type,sizeof(restrictor_type));
	Level().Send		(net_packet,net_flags(TRUE,TRUE));
}

template <typename P, bool value>
IC	void CRestrictedObject::construct_restriction_string(LPSTR temp_restrictions, const xr_vector<ALife::_OBJECT_ID> &restrictions, shared_str current_restrictions, const P &p)
{
	u32							count = 0;
	strcpy						(temp_restrictions,"");
	xr_vector<ALife::_OBJECT_ID>::const_iterator	I = restrictions.begin();
	xr_vector<ALife::_OBJECT_ID>::const_iterator	E = restrictions.end();
	for ( ; I != E; ++I) {
		CObject					*object = Level().Objects.net_Find(*I);
		if (!object || !!strstr(*current_restrictions,*object->cName()) == value)
			continue;

		p						(this,object->ID());

		if (count)
			strcat(temp_restrictions,",");

		strcat(temp_restrictions,*object->cName());

		count++;
	}
}

template <bool add>
struct CRestrictionPredicate {
	RestrictionSpace::ERestrictorTypes m_restrictor_type;

    IC	CRestrictionPredicate	(RestrictionSpace::ERestrictorTypes restrictor_type)
	{
		m_restrictor_type = restrictor_type;
	}

	IC	void operator()			(CRestrictedObject *object, ALife::_OBJECT_ID id) const
	{
		if (add)
			object->add_object_restriction(id,m_restrictor_type);
		else
			object->remove_object_restriction(id,m_restrictor_type);
	}
};

void CRestrictedObject::add_restrictions	(const xr_vector<ALife::_OBJECT_ID> &out_restrictions, const xr_vector<ALife::_OBJECT_ID> &in_restrictions)
{
	START_PROFILE("AI/Restricted Object/Add Restrictions");

	string4096					temp_out_restrictions;
	string4096					temp_in_restrictions;
	
	construct_restriction_string<CRestrictionPredicate<true>,true>(temp_out_restrictions,out_restrictions,this->out_restrictions(),CRestrictionPredicate<true>(RestrictionSpace::eRestrictorTypeOut));
	construct_restriction_string<CRestrictionPredicate<true>,true>(temp_in_restrictions,in_restrictions,this->in_restrictions(),CRestrictionPredicate<true>(RestrictionSpace::eRestrictorTypeIn));

	Level().space_restriction_manager().add_restrictions	(ID(),temp_out_restrictions,temp_in_restrictions);
	
	STOP_PROFILE;
}

void CRestrictedObject::remove_restrictions	(const xr_vector<ALife::_OBJECT_ID> &out_restrictions, const xr_vector<ALife::_OBJECT_ID> &in_restrictions)
{
	START_PROFILE("AI/Restricted Object/Remove Restrictions");
	
	string4096					temp_out_restrictions;
	string4096					temp_in_restrictions;

	construct_restriction_string<CRestrictionPredicate<false>,false>(temp_out_restrictions,out_restrictions,this->out_restrictions(),CRestrictionPredicate<false>(RestrictionSpace::eRestrictorTypeOut));
	construct_restriction_string<CRestrictionPredicate<false>,false>(temp_in_restrictions,in_restrictions,this->in_restrictions(),CRestrictionPredicate<false>(RestrictionSpace::eRestrictorTypeIn));

	Level().space_restriction_manager().remove_restrictions	(ID(),temp_out_restrictions,temp_in_restrictions);
	
	STOP_PROFILE;
}
