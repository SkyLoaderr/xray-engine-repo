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
	m_spawn_name				= 0;
}

CALifeSpawnRegistry::~CALifeSpawnRegistry	()
{
	ALife::D_OBJECT_P_IT		I = m_spawns.begin();
	ALife::D_OBJECT_P_IT		E = m_spawns.end();
	for ( ; I != E; ++I)
		xr_delete				(*I);
	xr_free						(m_spawn_name);
}

void CALifeSpawnRegistry::save	(IWriter &memory_stream)
{
	Msg							("* Saving spawn extra info...");
	memory_stream.open_chunk	(SPAWN_CHUNK_DATA);
	memory_stream.w_stringZ		(m_spawn_name);
	memory_stream.close_chunk	();
}

void CALifeSpawnRegistry::load	(IReader &file_stream, LPCSTR game_name)
{
	Msg							("* Loading spawn registry...");
	R_ASSERT2					(file_stream.find_chunk(SPAWN_CHUNK_DATA),"Cannot find chunk SPAWN_CHUNK_DATA!");
	xr_free						(m_spawn_name);
	m_spawn_name				= (LPSTR)xr_malloc(256*sizeof(char));
	file_stream.r_stringZ		(m_spawn_name);

	string256					file_name;
	IReader						*stream;
	R_ASSERT3					(FS.exist(file_name, "$game_spawn$", m_spawn_name, ".spawn"),"Can't find file spawn file:",m_spawn_name);
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
}

void CALifeSpawnRegistry::load	(LPCSTR spawn_name)
{
	Msg							("* Loading spawn registry...");
	xr_free						(m_spawn_name);
	m_spawn_name				= xr_strdup(spawn_name);

	string256					file_name;
	IReader						*stream;
	R_ASSERT3					(FS.exist(file_name, "$game_spawn$", m_spawn_name, ".spawn"),"Can't find file spawn file:",m_spawn_name);
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
	m_header.load				(file_stream);
	m_spawns.resize				(header().count());
	m_artefact_anomaly_map.clear();
	ALife::D_OBJECT_P_IT		I = m_spawns.begin();
	ALife::D_OBJECT_P_IT		E = m_spawns.end();
	NET_Packet					tNetPacket;
	IReader						*S = 0;
	u16							ID;
	for (int id=0; I != E; ++I, ++id) {
		R_ASSERT2				(0!=(S = file_stream.open_chunk(id)),"Can't find entity chunk in the 'game.spawn'");
		// Spawn
		tNetPacket.B.count		= S->r_u16();
		S->r					(tNetPacket.B.data,tNetPacket.B.count);
		tNetPacket.r_begin		(ID);
		R_ASSERT2				(M_SPAWN == ID,"Invalid packet ID (!= M_SPAWN)!");

		string64				s_name;
		tNetPacket.r_stringZ	(s_name);
		if (psAI_Flags.test(aiALife)) {
			Msg					("Loading spawn point %s",s_name);
		}
		CSE_Abstract			*E = F_entity_Create(s_name);

		R_ASSERT3				(E,"Can't create entity.",s_name);
		E->Spawn_Read			(tNetPacket);
		// Update
		tNetPacket.B.count		= S->r_u16();
		S->r					(tNetPacket.B.data,tNetPacket.B.count);
		tNetPacket.r_begin		(ID);
		R_ASSERT2				(M_UPDATE == ID,"Invalid packet ID (!= M_UPDATE)!");
		E->UPDATE_Read			(tNetPacket);

		VERIFY					(smart_cast<CSE_ALifeObject*>(E));

		R_ASSERT2				((GAME_SINGLE == E->s_gameid) || (GAME_ANY == E->s_gameid),"Invalid game type!");
		R_ASSERT2				(0 != (*I = smart_cast<CSE_ALifeDynamicObject*>(E)),"Non-ALife object in the 'game.spawn'");
		R_ASSERT3				(!((*I)->used_ai_locations()) || ((*I)->m_tNodeID != u32(-1)),"Invalid vertex for object ",(*I)->s_name_replace);
		
		// building map of sets : get all the zone types which can generate given artefact
		CSE_ALifeAnomalousZone	*anomaly = smart_cast<CSE_ALifeAnomalousZone*>(E);
		if (anomaly) {
			ALife::EAnomalousZoneType	type = anomaly->m_tAnomalyType;
			for (u16 i=0, n = anomaly->m_wItemCount; i<n; ++i) {
				ALife::ITEM_SET_PAIR_IT	I = m_artefact_anomaly_map.find(anomaly->m_cppArtefactSections[i]);
				if (m_artefact_anomaly_map.end() != I)
					(*I).second.insert(type);
				else {
					m_artefact_anomaly_map.insert(mk_pair(anomaly->m_cppArtefactSections[i],ALife::U32_SET()));
					I = m_artefact_anomaly_map.find(anomaly->m_cppArtefactSections[i]);
					if ((*I).second.find(type) == (*I).second.end())
						(*I).second.insert(type);
				}
			}
		}

		if (psAI_Flags.test(aiALife)) {
			Msg					("Spawn point %s is loaded",E->s_name_replace);
		}
	}
	R_ASSERT2					(0!=(S = file_stream.open_chunk(id++)),"Can't find artefact spawn points chunk in the 'game.spawn'");
	load_data					(m_artefact_spawn_positions,file_stream);
	Msg							("%d spawn points are successfully loaded",id);
}
