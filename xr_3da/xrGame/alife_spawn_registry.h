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

class CALifeSpawnRegistry : CRandom {
public:
	typedef CGameGraph::LEVEL_POINT_VECTOR	ARTEFACT_SPAWNS;

protected:
	CALifeSpawnHeader						m_header;
	ALife::D_OBJECT_P_VECTOR				m_spawns;
	ARTEFACT_SPAWNS							m_artefact_spawn_positions;
	ALife::ITEM_SET_MAP						m_artefact_anomaly_map;
	LPSTR									m_spawn_name;

public:
											CALifeSpawnRegistry			(LPCSTR section);
	virtual									~CALifeSpawnRegistry		();
	virtual void							load						(IReader &file_stream);
	virtual void							save						(IWriter &memory_stream);
			void							load						(IReader &file_stream, LPCSTR game_name);
			void							load						(LPCSTR spawn_name);
	IC		const CALifeSpawnHeader			&header						() const;
	IC		const ALife::D_OBJECT_P_VECTOR	&spawns						() const;
	IC		void							assign_artefact_position	(CSE_ALifeAnomalousZone	*anomaly, CSE_ALifeDynamicObject *object);
	IC		const ALife::ITEM_SET_MAP		&artefact_anomaly_map		() const;
	IC		bool							valid_spawn_id				(ALife::_SPAWN_ID spawn_id) const;
};

#include "alife_spawn_registry_inline.h"