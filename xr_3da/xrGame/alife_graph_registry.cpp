////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_graph_registry.cpp
//	Created 	: 15.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife graph registry
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "alife_graph_registry.h"

using namespace ALife;

CALifeGraphRegistry::CALifeGraphRegistry	(ref_str *server_command_line)
{
	VERIFY							(server_command_line);
	m_server_command_line			= server_command_line;
	for (int i=0; i<LOCATION_TYPE_COUNT; ++i) {
		{
			for (int j=0; j<LOCATION_COUNT; ++j)
				m_terrain[i][j].clear();
		}
		for (_GRAPH_ID j=0; j<(_GRAPH_ID)ai().game_graph().header().vertex_count(); ++j)
			m_terrain[i][ai().game_graph().vertex(j)->vertex_type()[i]].push_back(j);
	}

	m_objects.resize				(ai().game_graph().header().vertex_count());
	{
		GRAPH_REGISTRY::iterator	I = m_objects.begin();
		GRAPH_REGISTRY::iterator	E = m_objects.end();
		for ( ; I != E; ++I) {
			(*I).objects().clear	();
			(*I).events().clear		();
		}
	}

	m_level							= 0;
	m_process_time					= 0;
	m_actor							= 0;
}

CALifeGraphRegistry::~CALifeGraphRegistry	()
{
	xr_delete						(m_level);
}

void CALifeGraphRegistry::update			(CSE_ALifeDynamicObject *object)
{
	if (!object->m_bDirectControl)
		return;

	if (object->s_flags.is(M_SPAWN_OBJECT_ASPLAYER)) {
		m_actor						= dynamic_cast<CSE_ALifeCreatureActor*>(object);
		R_ASSERT2					(m_actor,"Invalid flag M_SPAWN_OBJECT_ASPLAYER for non-actor object!");
	}

	if (m_actor && !m_level)
		setup_current_level			();

	CSE_ALifeInventoryItem			*item = dynamic_cast<CSE_ALifeInventoryItem*>(object);
	if (!item || !item->attached())
		add							(object,object->m_tGraphID);
}

void CALifeGraphRegistry::setup_current_level	()
{
	m_level						= xr_new<CALifeLevelRegistry>(ai().game_graph().vertex(actor()->m_tGraphID)->level_id());
	level().set_process_time	(m_process_time);
	for (int i=0, n=ai().game_graph().header().vertex_count(); i<n; ++i)
		if (ai().game_graph().vertex(i)->level_id() == level().level_id()) {
			D_OBJECT_P_MAP::const_iterator	I = m_objects[i].objects().objects().begin();
			D_OBJECT_P_MAP::const_iterator	E = m_objects[i].objects().objects().end();
			for ( ; I != E; ++I)
				level().add		((*I).second);
		}

	{
		xr_vector<CSE_ALifeDynamicObject*>::const_iterator	I = m_temp.begin();
		xr_vector<CSE_ALifeDynamicObject*>::const_iterator	E = m_temp.end();
		for ( ; I != E; ++I)
			level().add			(*I);

		m_temp.clear			();
	}
	string512					S,S1;
	strcpy						(S,**m_server_command_line);
	LPSTR						temp = strchr(S,'/');
	R_ASSERT2					(temp,"Invalid server options!");
	xr_map<_LEVEL_ID,CGameGraph::SLevel>::const_iterator I = ai().game_graph().header().levels().find(ai().game_graph().vertex(actor()->m_tGraphID)->level_id());
	R_ASSERT2					(ai().game_graph().header().levels().end() != I,"Graph point level ID not found!");
	strconcat					(S1,(*I).second.name(),temp);
	*m_server_command_line		= S1;

	int							id = pApp->Level_ID((*I).second.name());
	VERIFY						(id >= 0);
	pApp->Level_Set				(id);
	ai().load					((*I).second.name());
}
