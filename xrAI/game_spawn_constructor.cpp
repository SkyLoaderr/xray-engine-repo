////////////////////////////////////////////////////////////////////////////
//	Module 		: game_spawn_constructor.cpp
//	Created 	: 16.10.2004
//  Modified 	: 16.10.2004
//	Author		: Dmitriy Iassenev
//	Description : Game spawn constructor
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "game_spawn_constructor.h"
#include "object_broker.h"
#include "level_spawn_constructor.h"
#include "xrServer_Objects_ALife_All.h"
#include "xrai.h"
#include "server_entity_wrapper.h"

#define NO_MULTITHREADING

CGameSpawnConstructor::CGameSpawnConstructor	(LPCSTR name, LPCSTR output)
{
	load_spawns		(name);
	process_spawns	();
	save_spawn		(name,output);
}

CGameSpawnConstructor::~CGameSpawnConstructor	()
{
	delete_data		(m_level_spawns);
	delete_data		(m_spawn_graph);
	xr_delete		(m_game_graph);
	xr_delete		(m_game_info);
}

IC	shared_str CGameSpawnConstructor::actor_level_name()
{
	string256						temp;
	CSE_ALifeCreatureActor			*actor = 0;
	CLevelSpawnConstructor			*level = 0;
	
	LEVEL_SPAWN_STORAGE::iterator	I = m_level_spawns.begin();
	LEVEL_SPAWN_STORAGE::iterator	E = m_level_spawns.end();
	for ( ; I != E; ++I) {
		if (!(*I)->actor())
			continue;

		Msg							("Actor is on the level %s",game_graph().header().level(game_graph().vertex((*I)->actor()->m_tGraphID)->level_id()).name());
		VERIFY2						(!actor,"There must be the SINGLE level with ACTOR!");
		actor						= (*I)->actor();
		level						= *I;
	}

	if (actor)
		return						(strconcat(temp,level->level().name(),".spawn"));

	R_ASSERT2						(false,"There is no actor!");
	return							("game.spawn");
}

extern void read_levels			(CInifile *ini, xr_set<CLevelInfo> &m_levels);

void CGameSpawnConstructor::load_spawns	(LPCSTR name)
{
	m_spawn_id							= 0;

	// init spawn graph
	m_spawn_graph						= xr_new<SPAWN_GRAPH>();
	
	// init ini file
	m_game_info							= xr_new<CInifile>(INI_FILE);
	R_ASSERT							(m_game_info->section_exist("levels"));

	// init game graph
	string256							game_graph_name;
	FS.update_path						(game_graph_name,"$game_data$","game.graph");
	m_game_graph						= xr_new<CGameGraph>(game_graph_name);

	// fill level info
	read_levels							(&game_info(),m_levels);

	// load levels
	CGameGraph::SLevel					level;
	string256							J;
	LEVEL_INFO_STORAGE::const_iterator	I = m_levels.begin();
	LEVEL_INFO_STORAGE::const_iterator	E = m_levels.end();
	for ( ; I != E; ++I) {
		if (xr_strlen(name)) {
			u32							N = _GetItemCount(name);
			bool						found = false;
			for (u32 i=0; i<N; ++i)
				if (!xr_strcmp(_GetItem(name,i,J),(*I).name)) {
					found				= true;
					break;
				}
			if (!found)
				continue;
		}
		level.tOffset					= (*I).offset;
		level.caLevelName				= (*I).name;
		level.tLevelID					= (*I).id;
		Msg								("%9s %2d %s","level",level.id(),(*I).name);
		m_level_spawns.push_back		(xr_new<CLevelSpawnConstructor>(level,this));
	}

	R_ASSERT2							(!m_level_spawns.empty(),"There are no m_levels in the section 'm_levels' in the 'game.ltx' to build 'game.spawn' from!");
}

void CGameSpawnConstructor::process_spawns	()
{
	LEVEL_SPAWN_STORAGE::iterator		I = m_level_spawns.begin();
	LEVEL_SPAWN_STORAGE::iterator		E = m_level_spawns.end();
	for ( ; I != E; ++I)
#ifdef NO_MULTITHREADING
		(*I)->Execute					();
#else
		m_thread_manager.start			(*I);
	m_thread_manager.wait				();
#endif

	I									= m_level_spawns.begin();
	for ( ; I != E; ++I)
		(*I)->update					();

	verify_level_changers				();
	verify_spawns						();
}

void CGameSpawnConstructor::build_root_spawns		()
{
	m_temp0.clear		();
	m_temp1.clear		();

	{
		SPAWN_GRAPH::const_vertex_iterator	I = m_spawn_graph->vertices().begin();
		SPAWN_GRAPH::const_vertex_iterator	E = m_spawn_graph->vertices().end();
		for ( ; I != E; ++I)
			m_temp0.push_back				((*I)->vertex_id());
	}

	{
		SPAWN_GRAPH::const_vertex_iterator	I = m_spawn_graph->vertices().begin();
		SPAWN_GRAPH::const_vertex_iterator	E = m_spawn_graph->vertices().end();
		for ( ; I != E; ++I) {
			SPAWN_GRAPH::const_iterator	i = (*I)->edges().begin();
			SPAWN_GRAPH::const_iterator	e = (*I)->edges().end();
			for ( ; i != e; ++i)
				m_temp1.push_back			((*i).vertex_id());
		}
	}

	process_spawns							(m_temp0);
	process_spawns							(m_temp1);

	m_spawn_roots.resize					(m_temp0.size() + m_temp1.size());
	xr_vector<ALife::_SPAWN_ID>::iterator	I = set_difference(
		m_temp0.begin(),
		m_temp0.end(),
		m_temp1.begin(),
		m_temp1.end(),
		m_spawn_roots.begin()
		);

	m_spawn_roots.erase						(I,m_spawn_roots.end());
}

void CGameSpawnConstructor::verify_spawns			(ALife::_SPAWN_ID spawn_id)
{
	xr_vector<ALife::_SPAWN_ID>::iterator	J = std::find(m_temp0.begin(),m_temp0.end(),spawn_id);
	R_ASSERT3								(J == m_temp0.end(),"RECURSIVE Spawn group chain found in spawn",m_spawn_graph->vertex(spawn_id)->data()->object().s_name_replace);
	m_temp0.push_back						(spawn_id);

	SPAWN_GRAPH::CVertex					*vertex = m_spawn_graph->vertex(spawn_id);
	SPAWN_GRAPH::const_iterator				I = vertex->edges().begin();
	SPAWN_GRAPH::const_iterator				E = vertex->edges().end();
	for ( ; I != E; ++I)
		verify_spawns						((*I).vertex_id());
}

void CGameSpawnConstructor::verify_spawns			()
{
	build_root_spawns						();
	
	xr_vector<ALife::_SPAWN_ID>::iterator	I = m_spawn_roots.begin();
	xr_vector<ALife::_SPAWN_ID>::iterator	E = m_spawn_roots.end();
	for ( ; I != E; ++I) {
		m_temp0.clear						();
		verify_spawns						(*I);
	}
}

void CGameSpawnConstructor::verify_level_changers	()
{
	if (m_level_changers.empty())
		return;

	Msg										("List of the level changers which are invalid for some reasons");
	LEVEL_CHANGER_STORAGE::const_iterator	I = m_level_changers.begin();
	LEVEL_CHANGER_STORAGE::const_iterator	E = m_level_changers.end();
	for ( ; I != E; ++I)
		Msg									("%s",(*I)->s_name_replace);

	VERIFY2									(m_level_changers.empty(),"Some of the level changers setup incorrectly");
}

void CGameSpawnConstructor::save_spawn		(LPCSTR name, LPCSTR output)
{
	CMemoryWriter					stream;

	m_spawn_header.m_version		= XRAI_CURRENT_VERSION;
	m_spawn_header.m_level_count	= (u32)m_level_spawns.size();
	m_spawn_header.m_spawn_count	= spawn_graph().vertex_count();
	
	stream.open_chunk				(0);
	stream.w						(&m_spawn_header,sizeof(m_spawn_header));
	stream.close_chunk				();
	
	stream.open_chunk				(1);
	save_data						(spawn_graph(),stream);
	stream.close_chunk				();

	stream.open_chunk				(2);
	save_data						(m_level_points,stream);
	stream.close_chunk				();

	stream.save_to					(*spawn_name(output));
}

shared_str CGameSpawnConstructor::spawn_name	(LPCSTR output)
{
	string256					file_name;
	if (!output)
		FS.update_path			(file_name,"$game_spawn$",*actor_level_name());
	else {
		actor_level_name		();
		string256				out;
		strconcat				(out,output,".spawn");
		FS.update_path			(file_name,"$game_spawn$",out);
	}
	return						(file_name);
}

void CGameSpawnConstructor::add_story_object	(ALife::_STORY_ID id, CSE_ALifeDynamicObject *object, LPCSTR level_name)
{
	if (id == INVALID_STORY_ID)
		return;

	ALife::STORY_P_PAIR_IT		I = m_story_objects.find(id);
	if (I != m_story_objects.end()) {
		Msg						("Object %s, story id %d",object->s_name_replace,object->m_story_id);
		Msg						("Object %s, story id %d",(*I).second->s_name_replace,(*I).second->m_story_id);
		VERIFY3					(I == m_story_objects.end(),"There are several objects which has the same unique story ID, level ",level_name);
	}
	
	m_story_objects.insert		(std::make_pair(id,object));
}

void CGameSpawnConstructor::add_object				(CSE_Abstract *object)
{
	m_critical_section.Enter	();
	object->m_tSpawnID			= spawn_id();
	spawn_graph().add_vertex	(xr_new<CServerEntityWrapper>(object),object->m_tSpawnID);
	m_critical_section.Leave	();
}
