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
	xr_vector<ALife::_SPAWN_ID>				m_spawn_roots;
	xr_vector<ALife::_SPAWN_ID>				m_temp0;
	xr_vector<ALife::_SPAWN_ID>				m_temp1;

protected:
			void							save_updates				(IWriter &stream);
			void							load_updates				(IReader &stream);
			void							build_spawn_anomalies		();
			void							build_root_spawns			();
			void							fill_redundant_spawns		(SPAWN_GRAPH::CVertex *vertex, xr_vector<ALife::_SPAWN_ID> &spawns, bool parent_redundant = false);
			void							fill_new_spawns_single		(SPAWN_GRAPH::CVertex *vertex, xr_vector<ALife::_SPAWN_ID> &spawns, ALife::_TIME_ID game_time, xr_vector<ALife::_SPAWN_ID> &objects);
			void							fill_new_spawns				(SPAWN_GRAPH::CVertex *vertex, xr_vector<ALife::_SPAWN_ID> &spawns, ALife::_TIME_ID game_time, xr_vector<ALife::_SPAWN_ID> &objects);
	IC		void							process_spawns				(xr_vector<ALife::_SPAWN_ID> &spawns);
	IC		bool							redundant					(CSE_Abstract &abstract);
	IC		bool							new_spawn					(CSE_Abstract &abstract);
	IC		bool							active_spawn				(CSE_Abstract &abstract) const;
	IC		bool							count_limit					(CSE_Abstract &abstract) const;
	IC		bool							time_limit					(CSE_Abstract &abstract, ALife::_TIME_ID game_time) const;
	IC		bool							spawned_item				(CSE_Abstract &abstract, xr_vector<ALife::_SPAWN_ID> &objects) const;
	IC		bool							spawned_item				(SPAWN_GRAPH::CVertex *vertex, xr_vector<ALife::_SPAWN_ID> &objects);
	IC		bool							object_existance_limit		(CSE_Abstract &abstract, xr_vector<ALife::_SPAWN_ID> &objects) const;
	IC		bool							can_spawn					(CSE_Abstract &abstract, ALife::_TIME_ID game_time, xr_vector<ALife::_SPAWN_ID> &objects) const;

public:
											CALifeSpawnRegistry			(LPCSTR section);
	virtual									~CALifeSpawnRegistry		();
	virtual void							load						(IReader &file_stream);
	virtual void							save						(IWriter &memory_stream);
			void							load						(IReader &file_stream, LPCSTR game_name);
			void							load						(LPCSTR spawn_name);
			void							fill_redundant_spawns		(xr_vector<ALife::_SPAWN_ID> &spawns);
			void							fill_new_spawns				(xr_vector<ALife::_SPAWN_ID> &spawns, ALife::_TIME_ID game_time, xr_vector<ALife::_SPAWN_ID> &objects);
	IC		const CALifeSpawnHeader			&header						() const;
	IC		const SPAWN_GRAPH				&spawns						() const;
	IC		void							assign_artefact_position	(CSE_ALifeAnomalousZone	*anomaly, CSE_ALifeDynamicObject *object) const;
	IC		const ALife::ITEM_SET_MAP		&artefact_anomaly_map		() const;
};

#include "alife_spawn_registry_inline.h"