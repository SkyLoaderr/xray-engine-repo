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
	IReader						*chunk;
	Msg							("* Loading spawn registry...");
	R_ASSERT2					(file_stream.find_chunk(SPAWN_CHUNK_DATA),"Cannot find chunk SPAWN_CHUNK_DATA!");
	
	chunk						= file_stream.open_chunk(0);
	file_stream.r_stringZ		(m_spawn_name);
	chunk->close				();

	string256					file_name;
	IReader						*stream;
	R_ASSERT3					(FS.exist(file_name, "$game_spawn$", *m_spawn_name, ".spawn"),"Can't find file spawn file:",*m_spawn_name);
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
	
	chunk						= file_stream.open_chunk(1);
	load_updates				(*chunk);
	chunk->close				();

	FS.r_close					(stream);
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
	m_artefact_anomaly_map.clear();

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

	Msg							("%d spawn points are successfully loaded",m_spawns.vertex_count());
}

void CALifeSpawnRegistry::save_updates			(IWriter &stream)
{
	stream.w_u32				(0);
}

void CALifeSpawnRegistry::load_updates			(IReader &stream)
{
	stream.r_u32				();
}

void CALifeSpawnRegistry::build_spawn_anomalies	()
{
//	
//	{
//		// building map of sets : get all the zone types which can generate given artefact
//		CSE_ALifeAnomalousZone	*anomaly = smart_cast<CSE_ALifeAnomalousZone*>(E);
//		if (anomaly) {
//			ALife::EAnomalousZoneType	type = anomaly->m_tAnomalyType;
//			for (u16 i=0, n = anomaly->m_wItemCount; i<n; ++i) {
//				ALife::ITEM_SET_PAIR_IT	I = m_artefact_anomaly_map.find(anomaly->m_cppArtefactSections[i]);
//				if (m_artefact_anomaly_map.end() != I)
//					(*I).second.insert(type);
//				else {
//					m_artefact_anomaly_map.insert(mk_pair(anomaly->m_cppArtefactSections[i],ALife::U32_SET()));
//					I = m_artefact_anomaly_map.find(anomaly->m_cppArtefactSections[i]);
//					if ((*I).second.find(type) == (*I).second.end())
//						(*I).second.insert(type);
//				}
//			}
//		}
//	}
}
