////////////////////////////////////////////////////////////////////////////
//	Module 		: agent_location_manager.cpp
//	Created 	: 24.05.2004
//  Modified 	: 14.01.2005
//	Author		: Dmitriy Iassenev
//	Description : Agent location manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "agent_location_manager.h"
#include "agent_manager.h"
#include "agent_member_manager.h"
#include "agent_enemy_manager.h"
#include "ai/stalker/ai_stalker.h"
#include "cover_point.h"

struct CRemoveOldDangerCover {
	IC	bool	operator()	(const CAgentLocationManager::CDangerLocationPtr &location) const
	{
		return			(!location->useful());
	}
};

struct CDangerLocationPredicate {
	Fvector				m_position;

	IC			CDangerLocationPredicate	(const Fvector &position)
	{
		m_position		= position;
	}

	IC	bool	operator()	(const CAgentLocationManager::CDangerLocationPtr &location) const
	{
		return			(*location == m_position);
	}
};

IC	CAgentLocationManager::CDangerLocationPtr CAgentLocationManager::location	(const Fvector &position)
{
	LOCATIONS::iterator	I = std::find_if(m_danger_locations.begin(),m_danger_locations.end(),CDangerLocationPredicate(position));
	if (I != m_danger_locations.end())
		return			(*I);
	return				(0);
}

bool CAgentLocationManager::suitable	(CAI_Stalker *object, CCoverPoint *location, bool use_enemy_info) const
{
	CAgentMemberManager::const_iterator	I = this->object().member().members().begin();
	CAgentMemberManager::const_iterator	E = this->object().member().members().end();
	for ( ; I != E; ++I) {
		if ((*I)->object().ID() == object->ID())
			continue;

		if (!(*I)->cover())
			continue;

		if ((*I)->cover()->m_position.distance_to(location->position()) <= 5.f)
			if ((*I)->object().Position().distance_to(location->position()) <= object->Position().distance_to(location->position()))
				return				(false);
	}

	if (use_enemy_info) {
		CAgentEnemyManager::ENEMIES::const_iterator	I = this->object().enemy().enemies().begin();
		CAgentEnemyManager::ENEMIES::const_iterator	E = this->object().enemy().enemies().end();
		for ( ; I != E; ++I)
			if ((*I).m_enemy_position.distance_to_sqr(location->position()) < 100.f)
				return				(false);
	}
	return							(true);
}

void CAgentLocationManager::add	(CDangerLocationPtr location)
{
	CDangerLocationPtr				danger = this->location(location->position());
	if (!danger) {
		m_danger_locations.push_back(location);
		return;
	}

	danger->m_level_time			= location->m_level_time;
	
	if (danger->m_interval < location->m_interval)
		danger->m_interval			= location->m_interval;
	
	if (danger->m_radius < location->m_radius)
		danger->m_radius			= location->m_radius;
}

void CAgentLocationManager::remove_old_danger_covers	()
{
	LOCATIONS::iterator			I = remove_if(m_danger_locations.begin(),m_danger_locations.end(),CRemoveOldDangerCover());
	m_danger_locations.erase	(I,m_danger_locations.end());
}

float CAgentLocationManager::danger		(CCoverPoint *cover) const
{
	float						result = 1;
	LOCATIONS::const_iterator	I = m_danger_locations.begin();
	LOCATIONS::const_iterator	E = m_danger_locations.end();
	for ( ; I != E; ++I) {
		if (Device.dwTimeGlobal > (*I)->m_level_time + (*I)->m_interval)
			continue;

		float		distance = 1.f + (*I)->position().distance_to(cover->position());
		if (distance > (*I)->m_radius)
			continue;

		result		*= distance/(*I)->m_radius*float(Device.dwTimeGlobal - (*I)->m_level_time)/float((*I)->m_interval);
	}

	return			(result);
}

void CAgentLocationManager::update	()
{
	remove_old_danger_covers	();
}

void CAgentLocationManager::remove_links(CObject *object)
{
	LOCATIONS::iterator			I = remove_if(m_danger_locations.begin(),m_danger_locations.end(),CRemoveDangerObject(object));
	m_danger_locations.erase	(I,m_danger_locations.end());
}
