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
#include "ai_script_lua_extension.h"

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

	m_lua_virtual_machine	= 0;
	m_lua_virtual_machine	= lua_open();
	if (!m_lua_virtual_machine) {
		Msg					("! ERROR : Cannot initialize script virtual machine!");
		return;
	}
	// initialize lua standard library functions 
	luaopen_base			(m_lua_virtual_machine); 
	luaopen_table			(m_lua_virtual_machine);
	luaopen_string			(m_lua_virtual_machine);
	luaopen_math			(m_lua_virtual_machine);
#ifdef DEBUG
	luaopen_debug			(m_lua_virtual_machine);
#endif
	lua_settop				(m_lua_virtual_machine,0);
	lua_setgcthreshold		(m_lua_virtual_machine,64);

	Script::vfExportToLua	(m_lua_virtual_machine);
}

CAI_Space::~CAI_Space				()
{
	unload					();
	xr_delete				(m_ef_storage);
	xr_delete				(m_game_graph);
	FS.w_close				(m_lua_output);
	if (m_lua_virtual_machine)
		lua_close			(m_lua_virtual_machine);
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
	CGameGraph::LEVEL_MAP::const_iterator	E = game_graph().header().levels().end();
	for ( ; I != E; ++I)
		if (!xr_strcmp((*I).second.name(),level_name))
			break;

#ifdef DEBUG
	if (I == game_graph().header().levels().end()){
		VERIFY2				(!get_alife(),"There is no graph for the current level!");
		Msg					("! There is no graph for the level %s",level_name);
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
	lua_settop				(m_lua_virtual_machine,0);
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
	Msg						("* Graph corresponds to the cross table");
}
#endif