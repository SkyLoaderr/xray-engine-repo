////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_spawn_registry.cpp
//	Created 	: 15.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife spawn registry
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "alife_spawn_registry.h"
#include "object_broker.h"
#include "game_base.h"

CALifeSpawnRegistry::CALifeSpawnRegistry	(LPCSTR section)
{
	m_spawn_name				= "";
	seed						(u32(CPU::GetCycleCount() & 0xffffffff));
}

CALifeSpawnRegistry::~CALifeSpawnRegistry	()
{
}

void CALifeSpawnRegistry::save	(IWriter &memory_stream)
{
	Msg							("* Saving spawn extra info...");
	memory_stream.open_chunk	(SPAWN_CHUNK_DATA);
	
	memory_stream.open_chunk	(0);
	memory_stream.w_stringZ		(m_spawn_name);
	memory_stream.close_chunk	();
	
	memory_stream.open_chunk	(1);
	save_updates				(memory_stream);
	memory_stream.close_chunk	();

	memory_stream.close_chunk	();
}

void CALifeSpawnRegistry::load	(IReader &file_stream, LPCSTR game_name)
{
	IReader						*chunk, *chunk0;
	Msg							("* Loading spawn registry...");
	R_ASSERT2					(file_stream.find_chunk(SPAWN_CHUNK_DATA),"Cannot find chunk SPAWN_CHUNK_DATA!");
	chunk0						= file_stream.open_chunk(SPAWN_CHUNK_DATA);
	
	chunk						= chunk0->open_chunk(0);
	chunk0->r_stringZ			(m_spawn_name);
	chunk->close				();

	string256					file_name;
	IReader						*stream;
	bool						file_exists = !!FS.exist(file_name, "$game_spawn$", *m_spawn_name, ".spawn");
	R_ASSERT3					(file_exists,"Can't find file spawn file:",*m_spawn_name);
	int							spawn_age = FS.get_file_age(file_name);
	
	R_ASSERT					(FS.exist(game_name));
	int							game_age = FS.get_file_age(game_name);
	R_ASSERT3					(game_age >= spawn_age,"Delete saved game ",game_name);
	
	string256					graph_file_name;
	FS.update_path				(graph_file_name,"$game_data$",GRAPH_NAME);
	int							graph_age = FS.get_file_age(graph_file_name);
	VERIFY3						(spawn_age >= graph_age,"Rebuild spawn file ",file_name);

	stream						= FS.r_open(file_name);
	load						(*stream);
	FS.r_close					(stream);
	
	chunk						= chunk0->open_chunk(1);
	load_updates				(*chunk);
	chunk->close				();
}

void CALifeSpawnRegistry::load	(LPCSTR spawn_name)
{
	Msg							("* Loading spawn registry...");
	m_spawn_name				= spawn_name;

	string256					file_name;
	IReader						*stream;
	R_ASSERT3					(FS.exist(file_name, "$game_spawn$", *m_spawn_name, ".spawn"),"Can't find file spawn file:",*m_spawn_name);
	int							spawn_age = FS.get_file_age(file_name);
	
	string256					graph_file_name;
	FS.update_path				(graph_file_name,"$game_data$",GRAPH_NAME);
	int							graph_age = FS.get_file_age(graph_file_name);
	VERIFY3						(spawn_age >= graph_age,"Rebuild spawn file ",file_name);

	stream						= FS.r_open(file_name);
	load						(*stream);
	FS.r_close					(stream);
}

void CALifeSpawnRegistry::load	(IReader &file_stream)
{
	IReader						*chunk;
	chunk						= file_stream.open_chunk(0);
	m_header.load				(*chunk);
	chunk->close				();

	chunk						= file_stream.open_chunk(1);
	m_spawns.load				(*chunk);
	chunk->close				();

	chunk						= file_stream.open_chunk(2);
	load_data					(m_artefact_spawn_positions,*chunk);
	chunk->close				();

	build_spawn_anomalies		();
	
	build_root_spawns			();

	Msg							("%d spawn points are successfully loaded",m_spawns.vertex_count());
}

void CALifeSpawnRegistry::save_updates			(IWriter &stream)
{
	SPAWN_GRAPH::vertex_iterator	I = m_spawns.vertices().begin();
	SPAWN_GRAPH::vertex_iterator	E = m_spawns.vertices().end();
	for ( ; I != E; ++I) {
		if ((*I)->edges().empty())
			continue;
		stream.open_chunk			((*I)->vertex_id());
		(*I)->data()->save_update	(stream);
		stream.close_chunk			();
	}
}

void CALifeSpawnRegistry::load_updates			(IReader &stream)
{
	SPAWN_GRAPH::vertex_iterator	I = m_spawns.vertices().begin();
	SPAWN_GRAPH::vertex_iterator	E = m_spawns.vertices().end();
	for ( ; I != E; ++I) {
		if ((*I)->edges().empty())
			continue;
		IReader						*chunk = stream.open_chunk((*I)->vertex_id());
		(*I)->data()->load_update	(*chunk);
		chunk->close				();
	}
}

void CALifeSpawnRegistry::build_spawn_anomalies	()
{
	// building map of sets : get all the zone types which can generate given artefact
	m_artefact_anomaly_map.clear	();

	SPAWN_GRAPH::vertex_iterator	I = m_spawns.vertices().begin();
	SPAWN_GRAPH::vertex_iterator	E = m_spawns.vertices().end();
	for ( ; I != E; ++I) {
		CSE_ALifeAnomalousZone				*anomaly = smart_cast<CSE_ALifeAnomalousZone*>(&(*I)->data()->object());
		if (anomaly) {
			ALife::EAnomalousZoneType		type = anomaly->m_tAnomalyType;
			for (u16 i=0, n = anomaly->m_wItemCount; i<n; ++i) {
				ALife::ITEM_SET_PAIR_IT		I = m_artefact_anomaly_map.find(anomaly->m_cppArtefactSections[i]);
				if (m_artefact_anomaly_map.end() != I)
					(*I).second.insert		(type);
				else {
					m_artefact_anomaly_map.insert(mk_pair(anomaly->m_cppArtefactSections[i],ALife::U32_SET()));
					I = m_artefact_anomaly_map.find(anomaly->m_cppArtefactSections[i]);
					if ((*I).second.find(type) == (*I).second.end())
						(*I).second.insert	(type);
				}
			}
		}
	}
}

void CALifeSpawnRegistry::build_root_spawns		()
{
	m_temp0.clear		();
	m_temp1.clear		();

	{
		SPAWN_GRAPH::const_vertex_iterator	I = m_spawns.vertices().begin();
		SPAWN_GRAPH::const_vertex_iterator	E = m_spawns.vertices().end();
		for ( ; I != E; ++I)
			m_temp0.push_back				((*I)->vertex_id());
	}

	{
		SPAWN_GRAPH::const_vertex_iterator	I = m_spawns.vertices().begin();
		SPAWN_GRAPH::const_vertex_iterator	E = m_spawns.vertices().end();
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
