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

CAI_Space *g_ai_space = 0;

CAI_Space::CAI_Space				()
{
	m_ef_storage			= xr_new<CEF_Storage>();
	m_game_graph			= xr_new<CGameGraph>();
	m_graph_engine			= xr_new<CGraphEngine>(game_graph().header().vertex_count());
	m_level_graph			= 0;
	m_cross_table			= 0;
	m_alife_simulator		= 0;


	string256				l_caLogFileName;
	strconcat               (l_caLogFileName,Core.ApplicationName,"_",Core.UserName,"_lua.log");
	FS.update_path          (l_caLogFileName,"$logs$",l_caLogFileName);
	m_lua_output			= FS.w_open(l_caLogFileName);
}

CAI_Space::~CAI_Space				()
{
	unload					();
	xr_delete				(m_ef_storage);
	xr_delete				(m_game_graph);
	FS.w_close				(m_lua_output);
}

void CAI_Space::load				(LPCSTR level_name)
{
	unload					();
	m_level_graph			= xr_new<CLevelGraph>();
	m_cross_table			= xr_new<CGameLevelCrossTable>();
	m_graph_engine			= xr_new<CGraphEngine>(
		_max(game_graph().header().vertex_count(),level_graph().header().vertex_count())
	);
	
	CGameGraph::LEVEL_MAP::const_iterator	I = game_graph().header().levels().begin();
	CGameGraph::LEVEL_MAP::const_iterator	E = game_graph().header().levels().begin();
	for ( ; I != E; ++I)
		if (!strcmp((*I).second.name(),level_name))
			break;

#ifdef DEBUG
	if (I == game_graph().header().levels().end()){
		Msg					("! !!!!!!!!There is no graph for the level %s",level_name);
		return;
	}
	validate				((*I).second.id());
#endif

	level_graph().set_level_id((*I).second.id());
}

void CAI_Space::unload				()
{
	xr_delete				(m_graph_engine);
	xr_delete				(m_level_graph);
	xr_delete				(m_cross_table);
}

#ifdef DEBUG
void CAI_Space::validate			(const u32 level_id) const
{
	for (_GRAPH_ID i=0, n = game_graph().header().vertex_count(); i<n; ++i)
		if ((level_id == game_graph().vertex(i)->level_id()) && 
			(cross_table().vertex(game_graph().vertex(i)->level_vertex_id()).game_vertex_id() != i)) {
			Msg				("! Graph doesn't correspond to the cross table");
			R_ASSERT2		(false,"Graph doesn't correspond to the cross table");
		}
	Msg						("* Graph corresponds to the cross table");
}
#endif