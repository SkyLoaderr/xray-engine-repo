////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_danger_in_direction_actions.cpp
//	Created 	: 31.05.2005
//  Modified 	: 31.05.2005
//	Author		: Dmitriy Iassenev
//	Description : Stalker danger in direction actions classes
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_danger_in_direction_actions.h"
#include "ai/stalker/ai_stalker.h"
#include "script_game_object.h"
#include "stalker_movement_manager.h"
#include "sight_manager.h"
#include "object_handler.h"
#include "movement_manager_space.h"
#include "detail_path_manager_space.h"
#include "stalker_decision_space.h"
#include "memory_manager.h"
#include "danger_manager.h"
#include "agent_manager.h"
#include "agent_member_manager.h"
#include "agent_location_manager.h"
#include "cover_point.h"
#include "danger_cover_location.h"

using namespace StalkerDecisionSpace;

const float DANGER_DISTANCE = 5.f;
const u32	DANGER_INTERVAL = 120000;

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDangerInDirectionTakeCover
//////////////////////////////////////////////////////////////////////////

CStalkerActionDangerInDirectionTakeCover::CStalkerActionDangerInDirectionTakeCover	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionDangerInDirectionTakeCover::initialize						()
{
	
	set_property			(eWorldPropertyCoverReached,false);
	set_property			(eWorldPropertyLookedAround,false);

	object().movement().set_desired_direction	(0);
	object().movement().set_path_type			(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_mental_state		(eMentalStateDanger);

	m_direction_sight		= !!::Random.randI(2);
}

void CStalkerActionDangerInDirectionTakeCover::execute							()
{
	inherited::execute		();

	if (!object().memory().danger().selected())
		return;

	CCoverPoint				*point = object().agent_manager().member().member(&object()).cover();
	if (point) {
		object().movement().set_level_dest_vertex	(point->level_vertex_id());
		object().movement().set_desired_position	(&point->position());
	}	
	else
		object().movement().set_nearest_accessible_position	();

	object().CObjectHandler::set_goal	(eObjectActionAimReady1,object().best_weapon());

	if (!object().movement().path_completed()) {
		object().movement().set_body_state		(eBodyStateStand);
		object().movement().set_movement_type	(eMovementTypeRun);
		object().sight().setup					(CSightAction(SightManager::eSightTypePosition,object().memory().danger().selected()->position(),true));
//		if (!m_direction_sight || !object().movement().distance_to_destination_greater(2.f))
//			object().sight().setup				(CSightAction(SightManager::eSightTypeCover,true,true));
//		else
//			object().sight().setup				(CSightAction(SightManager::eSightTypePathDirection,true,true));
		return;
	}

	set_property								(eWorldPropertyCoverReached,true);
}

void CStalkerActionDangerInDirectionTakeCover::finalize							()
{
	inherited::finalize		();
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDangerInDirectionLookOut
//////////////////////////////////////////////////////////////////////////

CStalkerActionDangerInDirectionLookOut::CStalkerActionDangerInDirectionLookOut	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionDangerInDirectionLookOut::initialize							()
{
	inherited::initialize	();
	object().movement().set_desired_direction	(0);
	object().movement().set_path_type			(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_nearest_accessible_position();
	object().movement().set_body_state			(eBodyStateStand);
	object().movement().set_movement_type		(eMovementTypeStand);
	object().movement().set_mental_state		(eMentalStateDanger);
	object().sight().setup						(SightManager::eSightTypeCurrentDirection);
	object().CObjectHandler::set_goal			(eObjectActionIdle);
}

void CStalkerActionDangerInDirectionLookOut::execute							()
{
	inherited::execute		();
}

void CStalkerActionDangerInDirectionLookOut::finalize							()
{
	inherited::finalize		();
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDangerInDirectionLookAround
//////////////////////////////////////////////////////////////////////////

CStalkerActionDangerInDirectionLookAround::CStalkerActionDangerInDirectionLookAround	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionDangerInDirectionLookAround::initialize						()
{
	set_inertia_time							(15000);

	inherited::initialize						();

	object().movement().set_desired_direction	(0);
	object().movement().set_path_type			(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_movement_type		(eMovementTypeStand);
	object().movement().set_mental_state		(eMentalStateDanger);
	object().movement().set_body_state			(eBodyStateCrouch);
}

void CStalkerActionDangerInDirectionLookAround::execute							()
{
	inherited::execute		();

	if (!object().memory().danger().selected())
		return;

	if (fsimilar(object().movement().body_orientation().target.yaw,object().movement().body_orientation().current.yaw))
		object().sight().setup					(CSightAction(SightManager::eSightTypeCoverLookOver,true));
	else
		object().sight().setup					(CSightAction(SightManager::eSightTypeCover,true));

	if (completed())
		set_property							(eWorldPropertyLookedAround,true);
}

void CStalkerActionDangerInDirectionLookAround::finalize							()
{
	inherited::finalize		();
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDangerInDirectionSearch
//////////////////////////////////////////////////////////////////////////

CStalkerActionDangerInDirectionSearch::CStalkerActionDangerInDirectionSearch	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionDangerInDirectionSearch::initialize						()
{
	inherited::initialize	();
	object().movement().set_desired_direction	(0);
	object().movement().set_path_type			(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_nearest_accessible_position();
	object().movement().set_body_state			(eBodyStateStand);
	object().movement().set_movement_type		(eMovementTypeStand);
	object().movement().set_mental_state		(eMentalStateDanger);
	object().sight().setup						(SightManager::eSightTypeCurrentDirection);
	object().CObjectHandler::set_goal			(eObjectActionIdle);
}

void CStalkerActionDangerInDirectionSearch::execute							()
{
	inherited::execute		();

	if (!object().agent_manager().member().member(&object()).cover())
		return;

	object().agent_manager().location().add		(
		xr_new<CDangerCoverLocation>(
			object().agent_manager().member().member(&object()).cover(),
			Device.dwTimeGlobal,
			DANGER_INTERVAL,
			DANGER_DISTANCE
		)
	);
}

void CStalkerActionDangerInDirectionSearch::finalize							()
{
	inherited::finalize		();
}
