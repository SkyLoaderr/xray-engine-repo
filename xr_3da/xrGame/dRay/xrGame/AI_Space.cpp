////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_space.h
//	Created 	: 12.11.2003
//  Modified 	: 12.11.2003
//	Author		: Dmitriy Iassenev
//	Description : AI space class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "game_graph.h"
#include "game_level_cross_table.h"
#include "level_graph.h"
#include "graph_engine.h"
#include "ef_storage.h"
#include "ai_space.h"
#include "cover_manager.h"
#include "cover_point.h"
#include "script_engine.h"

CAI_Space *g_ai_space = 0;

CAI_Space::CAI_Space				()
{
	m_ef_storage			= xr_new<CEF_Storage>();
	m_game_graph			= xr_new<CGameGraph>();
	m_graph_engine			= xr_new<CGraphEngine>(game_graph().header().vertex_count());
	m_cover_manager			= xr_new<CCoverManager>();
	m_script_engine			= xr_new<CScriptEngine>();
	m_level_graph			= 0;
	m_cross_table			= 0;
	m_alife_simulator		= 0;
}

CAI_Space::~CAI_Space				()
{
	unload					();
	xr_delete				(m_ef_storage);
	xr_delete				(m_game_graph);
	xr_delete				(m_script_engine);
}

void CAI_Space::load				(LPCSTR level_name)
{
	unload					();
	m_level_graph			= xr_new<CLevelGraph>();
	m_cross_table			= xr_new<CGameLevelCrossTable>();
	m_graph_engine			= xr_new<CGraphEngine>(
		_max(
			game_graph().header().vertex_count(),
			level_graph().header().vertex_count()
		)
	);
	
	CGameGraph::LEVEL_MAP::const_iterator	I = game_graph().header().levels().begin();
	CGameGraph::LEVEL_MAP::const_iterator	E = game_graph().header().levels().end();
	for ( ; I != E; ++I)
		if (!xr_strcmp((*I).second.name(),level_name))
			break;

	VERIFY2					((I != game_graph().header().levels().end()),"There is no graph for the current level or current level is not included into the game graph!");
	
#ifdef DEBUG
	validate				((*I).second.id());
#endif

	level_graph().set_level_id((*I).second.id());

	xr_vector<CCoverPoint*>	nearest;
	if (m_cover_manager->get_covers()) {
		m_cover_manager->covers().nearest(Fvector().set(0.f,0.f,0.f),100000.f,nearest);
		{
			xr_vector<CCoverPoint*>::iterator	I = nearest.begin();
			xr_vector<CCoverPoint*>::iterator	E = nearest.end();
			for ( ; I != E; ++I)
				xr_delete		(*I);
		}
		m_cover_manager->clear	();
	}
	m_cover_manager->compute_static_cover();
}

void CAI_Space::unload				()
{
	script_engine().unload	();
	xr_delete				(m_graph_engine);
	xr_delete				(m_level_graph);
	xr_delete				(m_cross_table);
}

#ifdef DEBUG
void CAI_Space::validate			(const u32 level_id) const
{
	VERIFY					(level_graph().header().vertex_count() == cross_table().header().level_vertex_count());
	for (ALife::_GRAPH_ID i=0, n = game_graph().header().vertex_count(); i<n; ++i)
		if ((level_id == game_graph().vertex(i)->level_id()) && 
			(!level_graph().valid_vertex_id(game_graph().vertex(i)->level_vertex_id()) ||
			(cross_table().vertex(game_graph().vertex(i)->level_vertex_id()).game_vertex_id() != i) ||
			!level_graph().inside(game_graph().vertex(i)->level_vertex_id(),game_graph().vertex(i)->level_point()))) {
			Msg				("! Graph doesn't correspond to the cross table");
			R_ASSERT2		(false,"Graph doesn't correspond to the cross table");
		}
//	Msg						("* Graph corresponds to the cross table");
}
#endif