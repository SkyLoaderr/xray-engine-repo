////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_spawn_registry.h
//	Created 	: 15.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife spawn registry
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_spawn_registry_header.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "game_graph.h"
#include "graph_abstract.h"
#include "server_entity_wrapper.h"

class CServerEntityWrapper;

class CALifeSpawnRegistry : CRandom {
public:
	typedef CGameGraph::LEVEL_POINT_VECTOR										ARTEFACT_SPAWNS;
	typedef CGraphAbstract<CServerEntityWrapper*,float,ALife::_SPAWN_ID,u32>	SPAWN_GRAPH;

protected:
	CALifeSpawnHeader						m_header;
	SPAWN_GRAPH								m_spawns;
	ARTEFACT_SPAWNS							m_artefact_spawn_positions;
	ALife::ITEM_SET_MAP						m_artefact_anomaly_map;
	shared_str								m_spawn_name;

protected:
			void							save_updates				(IWriter &stream);
			void							load_updates				(IReader &stream);
			void							build_spawn_anomalies		();

public:
											CALifeSpawnRegistry			(LPCSTR section);
	virtual									~CALifeSpawnRegistry		();
	virtual void							load						(IReader &file_stream);
	virtual void							save						(IWriter &memory_stream);
			void							load						(IReader &file_stream, LPCSTR game_name);
			void							load						(LPCSTR spawn_name);
			void							fill_redundant_spawns		(xr_vector<ALife::_SPAWN_ID> &spawns);
			void							fill_new_spawns				(xr_vector<ALife::_SPAWN_ID> &spawns);
	IC		const CALifeSpawnHeader			&header						() const;
	IC		const SPAWN_GRAPH				&spawns						() const;
	IC		void							assign_artefact_position	(CSE_ALifeAnomalousZone	*anomaly, CSE_ALifeDynamicObject *object);
	IC		const ALife::ITEM_SET_MAP		&artefact_anomaly_map		() const;
};

#include "alife_spawn_registry_inline.h"