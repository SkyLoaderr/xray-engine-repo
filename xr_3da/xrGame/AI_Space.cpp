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
#include "level_navigation_graph.h"
#include "graph_engine.h"
#include "ef_storage.h"
#include "ai_space.h"
#include "cover_manager.h"
#include "cover_point.h"
#include "script_engine.h"
#include "movement_coordinator.h"

CAI_Space *g_ai_space = 0;

CAI_Space::CAI_Space				()
{
	m_ef_storage			= xr_new<CEF_Storage>();
	m_game_graph			= xr_new<CGameGraph>();
	m_graph_engine			= xr_new<CGraphEngine>(game_graph().header().vertex_count());
	m_cover_manager			= xr_new<CCoverManager>();
	m_script_engine			= xr_new<CScriptEngine>();
	m_movement_coordinator	= xr_new<CMovementCoordinator>();
	m_level_graph			= 0;
	m_cross_table			= 0;
	m_alife_simulator		= 0;

	extern string4096		g_ca_stdout;
	setvbuf					(stderr,g_ca_stdout,_IOFBF,sizeof(g_ca_stdout));
}

CAI_Space::~CAI_Space				()
{
	unload					();
	xr_delete				(m_ef_storage);
	xr_delete				(m_game_graph);
	try {
		xr_delete			(m_script_engine);
	}
	catch(...) {
	}
	xr_delete				(m_cover_manager);
	xr_delete				(m_graph_engine);
	xr_delete				(m_movement_coordinator);
}

void CAI_Space::load				(LPCSTR level_name)
{
	unload					(true);

#ifdef DEBUG
	Memory.mem_compact		();
	u32						mem_usage = Memory.mem_usage();
	u64						start = CPU::GetCycleCount();
#endif

	m_level_graph			= xr_new<CLevelNavigationGraph>();
	m_cross_table			= xr_new<CGameLevelCrossTable>();
	R_ASSERT2				(cross_table().header().level_guid() == level_graph().header().guid(), "cross_table doesn't correspond to the AI-map");
	R_ASSERT2				(cross_table().header().game_guid() == game_graph().header().guid(), "graph doesn't correspond to the cross table");
	m_graph_engine			= xr_new<CGraphEngine>(
		_max(
			game_graph().header().vertex_count(),
			level_graph().header().vertex_count()
		)
	);
	
	const CGameGraph::SLevel &current_level = game_graph().header().level(level_name);
	R_ASSERT2				(current_level.guid() == level_graph().header().guid(), "graph doesn't correspond to the AI-map");
	
#ifdef DEBUG
	if (!xr_strcmp(current_level.name(),level_name))
		validate			(current_level.id());
#endif

	level_graph().level_id	(current_level.id());
	game_graph().set_current_level(current_level.id());

	m_cover_manager->compute_static_cover	();

#ifdef DEBUG
	u64						finish = CPU::GetCycleCount();
	Msg						("* Loading ai space is successfully completed (%.3fs, %7.3f Mb)",float(finish - start)*CPU::cycles2seconds,float(Memory.mem_usage() - mem_usage)/1048576.0);
#endif
}

void CAI_Space::unload				(bool reload)
{
	script_engine().unload	();
	xr_delete				(m_graph_engine);
	xr_delete				(m_level_graph);
	xr_delete				(m_cross_table);
	if (!reload)
		m_graph_engine		= xr_new<CGraphEngine>(game_graph().header().vertex_count());
}

#ifdef DEBUG
void CAI_Space::validate			(const u32 level_id) const
{
	VERIFY					(level_graph().header().vertex_count() == cross_table().header().level_vertex_count());
	for (GameGraph::_GRAPH_ID i=0, n = game_graph().header().vertex_count(); i<n; ++i)
		if ((level_id == game_graph().vertex(i)->level_id()) && 
			(!level_graph().valid_vertex_id(game_graph().vertex(i)->level_vertex_id()) ||
			(cross_table().vertex(game_graph().vertex(i)->level_vertex_id()).game_vertex_id() != i) ||
			!level_graph().inside(game_graph().vertex(i)->level_vertex_id(),game_graph().vertex(i)->level_point()))) {
			Msg				("! Graph doesn't correspond to the cross table");
			R_ASSERT2		(false,"Graph doesn't correspond to the cross table");
		}

//	Msg						("death graph point id : %d",cross_table().vertex(455236).game_vertex_id());

	for (u32 i=0, n=game_graph().header().vertex_count(); i<n; ++i) {
		if (level_id != game_graph().vertex(i)->level_id())
			continue;

		CGameGraph::const_spawn_iterator	I, E;
		game_graph().begin_spawn			(i,I,E);
//		Msg									("vertex [%d] has %d death points",i,game_graph().vertex(i)->death_point_count());
		for ( ; I != E; ++I) {
			VERIFY							(cross_table().vertex((*I).level_vertex_id()).game_vertex_id() == i);
		}
	}
	

//	Msg						("* Graph corresponds to the cross table");
}
#endif