////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_spawn_registry_spawn.cpp
//	Created 	: 19.10.2004
//  Modified 	: 19.10.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife spawn registry spawn routines
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "alife_spawn_registry.h"

IC	bool CALifeSpawnRegistry::active_spawn		(CSE_Abstract &abstract) const
{
	return		(!!abstract.m_spawn_flags.is(CSE_Abstract::flSpawnActive));
}

IC	bool CALifeSpawnRegistry::count_limit		(CSE_Abstract &abstract) const
{
	return		(
		!abstract.m_spawn_flags.is(CSE_Abstract::flSpawnInfiniteCount) &&
		(abstract.m_max_spawn_count <= abstract.m_spawn_count)
	);
}

IC	bool CALifeSpawnRegistry::time_limit		(CSE_Abstract &abstract, ALife::_TIME_ID game_time) const
{
	return		(
		!abstract.m_spawn_flags.is(CSE_Abstract::flSpawnOnSurgeOnly) &&
		(
			(game_time < 
				abstract.m_last_spawn_time + 
				abstract.m_min_spawn_interval
			) ||
			(game_time < 
				abstract.m_last_spawn_time + 
				abstract.m_min_spawn_interval +
				::Random32.random(
					u32(abstract.m_max_spawn_interval - 
					abstract.m_min_spawn_interval)
				)
			)
		)
	);
}

IC	bool CALifeSpawnRegistry::spawned_item				(CSE_Abstract &abstract, xr_vector<ALife::_SPAWN_ID> &objects) const
{
	xr_vector<ALife::_SPAWN_ID>::iterator	I = std::lower_bound(objects.begin(),objects.end(),abstract.m_tSpawnID);
	return									((I != objects.end()) && (*I == abstract.m_tSpawnID));
}

IC	bool CALifeSpawnRegistry::object_existance_limit	(CSE_Abstract &abstract, xr_vector<ALife::_SPAWN_ID> &objects) const
{
	return		(
		!!abstract.m_spawn_flags.is(CSE_Abstract::flSpawnIfDestroyedOnly) &&
		spawned_item(abstract,objects)
	);
}

IC	bool CALifeSpawnRegistry::can_spawn			(CSE_Abstract &abstract, ALife::_TIME_ID game_time, xr_vector<ALife::_SPAWN_ID> &objects) const
{
	return		(
		active_spawn(abstract) &&
		!count_limit(abstract) &&
		!time_limit(abstract,game_time) &&
		!object_existance_limit(abstract,objects)
	);
}

void CALifeSpawnRegistry::fill_new_spawns		(SPAWN_GRAPH::CVertex *vertex, xr_vector<ALife::_SPAWN_ID> &spawns, ALife::_TIME_ID game_time, xr_vector<ALife::_SPAWN_ID> &objects)
{
	VERIFY									(vertex);
	if (!can_spawn(vertex->data()->object(),game_time,objects))
		return;

	if (vertex->edges().empty()) {
		spawns.push_back					(vertex->data()->object().m_tSpawnID);
		return;
	}
	
	SPAWN_GRAPH::const_iterator				I = vertex->edges().begin();
	SPAWN_GRAPH::const_iterator				E = vertex->edges().end();
	for ( ; I != E; ++I)
		fill_new_spawns						(m_spawns.vertex((*I).vertex_id()),spawns,game_time,objects);
}

void CALifeSpawnRegistry::fill_new_spawns		(xr_vector<ALife::_SPAWN_ID> &spawns, ALife::_TIME_ID game_time, xr_vector<ALife::_SPAWN_ID> &objects)
{
	process_spawns							(objects);

	xr_vector<ALife::_SPAWN_ID>::iterator	I = m_spawn_roots.begin();
	xr_vector<ALife::_SPAWN_ID>::iterator	E = m_spawn_roots.end();
	for ( ; I != E; ++I)
		fill_new_spawns						(m_spawns.vertex(*I),spawns,game_time,objects);

	process_spawns							(spawns);
}
