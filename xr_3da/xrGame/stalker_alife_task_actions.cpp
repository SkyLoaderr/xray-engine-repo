////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_alife_task_actions.cpp
//	Created 	: 25.10.2004
//  Modified 	: 25.10.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker alife task action classes
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_alife_task_actions.h"
#include "ai/stalker/ai_stalker.h"
#include "ai/trader/ai_trader.h"
#include "inventory_item.h"
#include "script_game_object.h"
#include "inventory.h"
#include "alife_task.h"
#include "alife_simulator.h"
#include "alife_object_registry.h"
#include "stalker_decision_space.h"
#include "cover_manager.h"
#include "cover_evaluators.h"
#include "cover_point.h"
#include "movement_manager_space.h"
#include "detail_path_manager_space.h"
#include "game_location_selector.h"
#include "sight_manager.h"
#include "ai_object_location.h"
#include "stalker_movement_manager.h"

using namespace StalkerDecisionSpace;

//////////////////////////////////////////////////////////////////////////
// CStalkerActionSolveZonePuzzle
//////////////////////////////////////////////////////////////////////////

CStalkerActionSolveZonePuzzle::CStalkerActionSolveZonePuzzle	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionSolveZonePuzzle::initialize	()
{
	inherited::initialize			();

	m_stop_weapon_handling_time		= Device.dwTimeGlobal;
	if (object().inventory().ActiveItem() && object().best_weapon() && (object().inventory().ActiveItem()->object().ID() == object().best_weapon()->object().ID()))
		m_stop_weapon_handling_time	+= ::Random32.random(30000) + 30000;

	object().movement().set_node_evaluator	(0);
	object().movement().set_path_evaluator	(0);
	object().movement().set_desired_position	(0);
	object().movement().set_desired_direction	(0);
	object().movement().set_path_type			(MovementManager::ePathTypeGamePath);
	object().movement().set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_body_state		(eBodyStateStand);
	object().movement().set_movement_type		(eMovementTypeWalk);
	object().movement().set_mental_state		(eMentalStateFree);
	object().sight().setup			(CSightAction(SightManager::eSightTypeCover,false,true));
}

void CStalkerActionSolveZonePuzzle::finalize	()
{
	inherited::finalize				();

	if (!object().g_Alive())
		return;

	object().sound().remove_active_sounds	(u32(eStalkerSoundMaskNoHumming));
}

void CStalkerActionSolveZonePuzzle::execute		()
{
	inherited::execute				();
	if (Device.dwTimeGlobal >= m_stop_weapon_handling_time)
		if (!object().best_weapon())
			object().CObjectHandler::set_goal	(eObjectActionIdle);
		else
			object().CObjectHandler::set_goal	(eObjectActionStrapped,object().best_weapon());
	else
		object().CObjectHandler::set_goal		(eObjectActionIdle,object().best_weapon());
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionReachTaskLocation
//////////////////////////////////////////////////////////////////////////

CStalkerActionReachTaskLocation::CStalkerActionReachTaskLocation	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionReachTaskLocation::initialize	()
{
	inherited::initialize				();
	object().movement().set_node_evaluator		(0);
	object().movement().set_path_evaluator		(0);
	object().movement().set_desired_position		(0);
	object().movement().set_desired_direction		(0);
	object().movement().game_selector().set_selection_type		(eSelectionTypeMask);
	object().movement().set_path_type				(MovementManager::ePathTypeGamePath);
	object().movement().set_detail_path_type		(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_body_state			(eBodyStateStand);
	object().movement().set_movement_type			(eMovementTypeWalk);
	object().movement().set_mental_state			(eMentalStateFree);
	object().sight().setup				(CSightAction(SightManager::eSightTypePathDirection));
	if (!object().best_weapon())
		object().CObjectHandler::set_goal	(eObjectActionIdle);
	else
		object().CObjectHandler::set_goal	(eObjectActionStrapped,object().best_weapon());
}

void CStalkerActionReachTaskLocation::finalize	()
{
	inherited::finalize					();
	object().movement().game_selector().set_selection_type		(eSelectionTypeRandomBranching);
}

void CStalkerActionReachTaskLocation::execute		()
{
	inherited::execute					();
	object().sound().play					(eStalkerSoundHumming,60000,10000);
	object().movement().set_game_dest_vertex		(object().current_alife_task().m_tGraphID);
	if (object().movement().path_completed() || object().alife_task_completed())
		m_storage->set_property			(eWorldPropertyReachedTaskLocation,true);
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionAccomplishTask
//////////////////////////////////////////////////////////////////////////

CStalkerActionAccomplishTask::CStalkerActionAccomplishTask	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionAccomplishTask::setup	(CAI_Stalker *object, CPropertyStorage *storage)
{
	inherited::setup					(object,storage);
	
	m_inertia_time						= 60000;
	m_max_search_distance				= 30.f;
	
	if (pSettings->line_exist(*object->cNameSect(),"time_to_search_for_artefacts"))
		m_inertia_time					= pSettings->r_u32(*object->cNameSect(),"time_to_search_for_artefacts");

	if (pSettings->line_exist(*object->cNameSect(),"distance_to_search_for_artefacts"))
		m_max_search_distance			= pSettings->r_float(*object->cNameSect(),"distance_to_search_for_artefacts");
}

void CStalkerActionAccomplishTask::initialize	()
{
	inherited::initialize						();
	object().movement().set_node_evaluator		(0);
	object().movement().set_path_evaluator		(0);
	object().movement().set_desired_position	(0);
	object().movement().set_desired_direction	(0);
	object().movement().set_path_type			(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_body_state			(eBodyStateStand);
	object().movement().set_movement_type		(eMovementTypeWalk);
	object().movement().set_mental_state		(eMentalStateFree);
	object().sight().setup						(CSightAction(SightManager::eSightTypeSearch,false,true));
	object().movement().extrapolate_path		(true);
	object().sound().remove_active_sounds		(u32(eStalkerSoundMaskNoHumming));
	if (!object().best_weapon())
		object().CObjectHandler::set_goal		(eObjectActionIdle);
	else
		object().CObjectHandler::set_goal		(eObjectActionStrapped,object().best_weapon());
}

void CStalkerActionAccomplishTask::finalize	()
{
	inherited::finalize						();
	object().movement().extrapolate_path	(false);

	if (!object().g_Alive())
		return;

	object().sound().set_sound_mask			(0);
}

void CStalkerActionAccomplishTask::execute		()
{
	inherited::execute					();

	if (object().alife_task_completed())
		m_storage->set_property			(eWorldPropertyTaskCompleted,true);

	if (!object().movement().path_completed())
		return;
	
	Fvector position					= ai().game_graph().vertex(object().current_alife_task().m_tGraphID)->level_point();
	object().m_ce_random_game->setup	(object().current_alife_task().m_tGraphID,m_max_search_distance);
	CCoverPoint							*point = ai().cover_manager().best_cover(position,m_max_search_distance,*object().m_ce_random_game);

	if (point && !completed()) {
		object().movement().set_level_dest_vertex	(point->level_vertex_id());
		object().movement().set_desired_position	(&point->position());
	}
	else {
		if (!object().alife_task_completed()) {
			object().failed_to_complete_alife_task();
			m_storage->set_property		(eWorldPropertyReachedTaskLocation,false);
		}
		else {
			m_storage->set_property		(eWorldPropertyTaskCompleted,true);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionReachCustomerLocation
//////////////////////////////////////////////////////////////////////////

CStalkerActionReachCustomerLocation::CStalkerActionReachCustomerLocation	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionReachCustomerLocation::initialize	()
{
	inherited::initialize				();
	object().movement().set_node_evaluator		(0);
	object().movement().set_path_evaluator		(0);
	object().movement().set_desired_position		(0);
	object().movement().set_desired_direction		(0);
	object().movement().game_selector().set_selection_type		(eSelectionTypeMask);
	object().movement().set_path_type				(MovementManager::ePathTypeGamePath);
	object().movement().set_detail_path_type		(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_body_state			(eBodyStateStand);
	object().movement().set_movement_type			(eMovementTypeWalk);
	object().movement().set_mental_state			(eMentalStateFree);
	object().sight().setup				(CSightAction(SightManager::eSightTypePathDirection));
	if (!object().best_weapon())
		object().CObjectHandler::set_goal	(eObjectActionIdle);
	else
		object().CObjectHandler::set_goal	(eObjectActionStrapped,object().best_weapon());
}

void CStalkerActionReachCustomerLocation::finalize	()
{
	inherited::finalize					();
	object().movement().game_selector().set_selection_type		(eSelectionTypeRandomBranching);
}

void CStalkerActionReachCustomerLocation::execute		()
{
	inherited::execute					();

	object().sound().play				(eStalkerSoundHumming,60000,10000);

	CSE_ALifeDynamicObject				*customer = ai().alife().objects().object(object().current_alife_task().m_tCustomerID);
	if (object().ai_location().game_vertex_id() != customer->m_tGraphID) {
		object().movement().set_game_dest_vertex	(customer->m_tGraphID);
		return;
	}

	object().movement().set_path_type				(MovementManager::ePathTypeLevelPath);

	object().movement().set_nearest_accessible_position(customer->o_Position,customer->m_tNodeID);

	if (object().movement().path_completed())
		m_storage->set_property			(eWorldPropertyReachedCustomerLocation,true);
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionCommunicateWithCustomer
//////////////////////////////////////////////////////////////////////////

CStalkerActionCommunicateWithCustomer::CStalkerActionCommunicateWithCustomer	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionCommunicateWithCustomer::initialize	()
{
	inherited::initialize				();
	object().movement().set_node_evaluator		(0);
	object().movement().set_path_evaluator		(0);
	object().movement().set_desired_direction		(0);
	object().movement().set_path_type				(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type		(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_nearest_accessible_position();
	object().movement().set_body_state			(eBodyStateStand);
	object().movement().set_movement_type			(eMovementTypeWalk);
	object().movement().set_mental_state			(eMentalStateFree);
	object().sight().setup				(CSightAction(SightManager::eSightTypeCover,false,true));
	object().CObjectHandler::set_goal	(eObjectActionIdle);
	object().sound().remove_active_sounds		(u32(eStalkerSoundMaskNoHumming));
	m_trader							= smart_cast<CAI_Trader*>(Level().Objects.net_Find(object().current_alife_task().m_tCustomerID));
	VERIFY								(m_trader);
}

void CStalkerActionCommunicateWithCustomer::finalize	()
{
	inherited::finalize					();
	
	if (!object().g_Alive())
		return;

	object().sound().set_sound_mask		(0);
}

void CStalkerActionCommunicateWithCustomer::execute		()
{
	inherited::execute					();
	
	if (m_trader->busy_now())
		return;

	object().communicate				(m_trader);

	m_storage->set_property				(eWorldPropertyReachedTaskLocation,		false);
	m_storage->set_property				(eWorldPropertyTaskCompleted,			false);
	m_storage->set_property				(eWorldPropertyReachedCustomerLocation,	false);
	m_storage->set_property				(eWorldPropertyCustomerSatisfied,		false);
}
