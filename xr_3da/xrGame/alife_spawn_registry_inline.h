////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_spawn_registry_inline.h
//	Created 	: 15.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife spawn registry inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	const CALifeSpawnHeader &CALifeSpawnRegistry::header	() const
{
	return					(m_header);
}

IC	void CALifeSpawnRegistry::assign_artefact_position		(CSE_ALifeAnomalousZone	*anomaly, CSE_ALifeDynamicObject *object)
{
	object->m_tGraphID		= anomaly->m_tGraphID;
	u32						index = anomaly->m_dwStartIndex + randI(anomaly->m_wArtefactSpawnCount);
	object->o_Position		= m_artefact_spawn_positions[index].level_point();
	object->m_tNodeID		= m_artefact_spawn_positions[index].level_vertex_id();
	object->m_fDistance		= m_artefact_spawn_positions[index].distance();
#ifdef DEBUG
	if (psAI_Flags.test(aiALife)) {
		Msg					("[LSS] Zone %s[%f][%f][%f] %d: generated artefact position %s[%f][%f][%f]",anomaly->s_name_replace,VPUSH(anomaly->o_Position),anomaly->m_dwStartIndex,object->s_name_replace,VPUSH(object->o_Position));
	}
#endif
}

IC	const CALifeSpawnRegistry::SPAWN_GRAPH &CALifeSpawnRegistry::spawns	() const
{
	return					(m_spawns);
}

IC	const ALife::ITEM_SET_MAP &CALifeSpawnRegistry::artefact_anomaly_map	() const
{
	return					(m_artefact_anomaly_map);
}

IC	void CALifeSpawnRegistry::process_spawns		(xr_vector<ALife::_SPAWN_ID> &spawns)
{
	std::sort								(spawns.begin(),spawns.end());
	xr_vector<ALife::_SPAWN_ID>::iterator	I = unique(spawns.begin(),spawns.end());
	spawns.erase							(I,spawns.end());
}
