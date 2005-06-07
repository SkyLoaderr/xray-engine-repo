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

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDangerInDirectionTakeCover
//////////////////////////////////////////////////////////////////////////

CStalkerActionDangerInDirectionTakeCover::CStalkerActionDangerInDirectionTakeCover	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionDangerInDirectionTakeCover::initialize						()
{
	inherited::initialize	();
	object().movement().set_node_evaluator		(0);
	object().movement().set_path_evaluator		(0);
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

void CStalkerActionDangerInDirectionTakeCover::execute							()
{
	inherited::execute		();
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
	object().movement().set_node_evaluator		(0);
	object().movement().set_path_evaluator		(0);
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
	inherited::initialize	();
	object().movement().set_node_evaluator		(0);
	object().movement().set_path_evaluator		(0);
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

void CStalkerActionDangerInDirectionLookAround::execute							()
{
	inherited::execute		();
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
	object().movement().set_node_evaluator		(0);
	object().movement().set_path_evaluator		(0);
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
}

void CStalkerActionDangerInDirectionSearch::finalize							()
{
	inherited::finalize		();
}
