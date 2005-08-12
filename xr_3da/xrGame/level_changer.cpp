////////////////////////////////////////////////////////////////////////////
//	Module 		: level_changer.cpp
//	Created 	: 10.07.2003
//  Modified 	: 10.07.2003
//	Author		: Dmitriy Iassenev
//	Description : Level change object
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "level_changer.h"
#include "hit.h"
#include "actor.h"
#include "xrserver_objects_alife.h"
#include "level.h"
#include "ai_object_location.h"
#include "ai_space.h"
#include "level_navigation_graph.h"
#include "game_level_cross_table.h"

xr_vector<CLevelChanger*>	g_lchangers;

CLevelChanger::~CLevelChanger	()
{
}

void CLevelChanger::Center		(Fvector& C) const
{
	XFORM().transform_tiny		(C,CFORM()->getSphere().P);
}

float CLevelChanger::Radius		() const
{
	return CFORM()->getRadius	();
}

void CLevelChanger::net_Destroy	() 
{
	inherited ::net_Destroy	();
	xr_vector<CLevelChanger*>::iterator it = std::find(g_lchangers.begin(), g_lchangers.end(), this);
	if(it != g_lchangers.end())
		g_lchangers.erase(it);
}

BOOL CLevelChanger::net_Spawn	(CSE_Abstract* DC) 
{
	CCF_Shape *l_pShape			= xr_new<CCF_Shape>(this);
	collidable.model			= l_pShape;
	
	CSE_Abstract				*l_tpAbstract = (CSE_Abstract*)(DC);
	CSE_ALifeLevelChanger		*l_tpALifeLevelChanger = smart_cast<CSE_ALifeLevelChanger*>(l_tpAbstract);
	R_ASSERT					(l_tpALifeLevelChanger);

	m_game_vertex_id			= l_tpALifeLevelChanger->m_tNextGraphID;
	m_level_vertex_id			= l_tpALifeLevelChanger->m_dwNextNodeID;
	m_position					= l_tpALifeLevelChanger->m_tNextPosition;
	m_angles					= l_tpALifeLevelChanger->m_tAngles;

	if (ai().get_level_graph()) {
		//. this information should be computed in xrAI
		ai_location().level_vertex	(ai().level_graph().vertex(u32(-1),Position()));
		ai_location().game_vertex	(ai().cross_table().vertex(ai_location().level_vertex_id()).game_vertex_id());
	}

	feel_touch.clear			();
	
	for (u32 i=0; i < l_tpALifeLevelChanger->shapes.size(); ++i) {
		CSE_Shape::shape_def	&S = l_tpALifeLevelChanger->shapes[i];
		switch (S.type) {
			case 0 : {
				l_pShape->add_sphere(S.data.sphere);
				break;
			}
			case 1 : {
				l_pShape->add_box(S.data.box);
				break;
			}
		}
	}

	BOOL						bOk = inherited::net_Spawn(DC);
	if (bOk) {
		l_pShape->ComputeBounds	();
		Fvector					P;
		XFORM().transform_tiny	(P,CFORM()->getSphere().P);
		setEnabled				(true);
	}
	g_lchangers.push_back		(this);
	return						(bOk);
}

void CLevelChanger::shedule_Update(u32 dt)
{
	inherited::shedule_Update	(dt);

	const Fsphere				&s = CFORM()->getSphere();
	Fvector						P;
	XFORM().transform_tiny		(P,s.P);
	feel_touch_update			(P,s.R);
}

void CLevelChanger::feel_touch_new	(CObject *tpObject)
{
	CActor						*l_tpActor = smart_cast<CActor*>(tpObject);
	if (l_tpActor) {
		NET_Packet				net_packet;
		net_packet.w_begin		(M_CHANGE_LEVEL);
		net_packet.w			(&m_game_vertex_id,sizeof(m_game_vertex_id));
		net_packet.w			(&m_level_vertex_id,sizeof(m_level_vertex_id));
		net_packet.w_vec3		(m_position);
		net_packet.w_vec3		(m_angles);
		Level().Send			(net_packet,net_flags(TRUE));
	}
}

BOOL CLevelChanger::feel_touch_contact	(CObject *object)
{
	return						((CCF_Shape*)CFORM())->Contact(object);
}
