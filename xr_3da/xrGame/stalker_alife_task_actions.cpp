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
	m_stop_weapon_handling_time		= Level().timeServer();

	if (m_object->inventory().ActiveItem() && m_object->best_weapon() && (m_object->inventory().ActiveItem()->ID() == m_object->best_weapon()->ID()))
		m_stop_weapon_handling_time	+= ::Random.randI(120000,180000);

	m_object->set_node_evaluator	(0);
	m_object->set_path_evaluator	(0);
	m_object->set_desired_position	(0);
	m_object->set_desired_direction	(0);
	m_object->set_path_type			(MovementManager::ePathTypeGamePath);
	m_object->set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	m_object->set_body_state		(eBodyStateStand);
	m_object->set_movement_type		(eMovementTypeWalk);
	m_object->set_mental_state		(eMentalStateFree);
	m_object->CSightManager::setup	(CSightAction(SightManager::eSightTypeCover,false,true));
}

void CStalkerActionSolveZonePuzzle::finalize	()
{
	inherited::finalize				();

	if (!m_object->g_Alive())
		return;

	m_object->set_sound_mask		(u32(eStalkerSoundMaskNoHumming));
	m_object->set_sound_mask		(0);
}

void CStalkerActionSolveZonePuzzle::execute		()
{
	inherited::execute				();
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
	m_object->set_node_evaluator		(0);
	m_object->set_path_evaluator		(0);
	m_object->set_desired_position		(0);
	m_object->set_desired_direction		(0);
	m_object->game_location_selector().set_selection_type		(eSelectionTypeMask);
	m_object->set_path_type				(MovementManager::ePathTypeGamePath);
	m_object->set_detail_path_type		(DetailPathManager::eDetailPathTypeSmooth);
	m_object->set_body_state			(eBodyStateStand);
	m_object->set_movement_type			(eMovementTypeWalk);
	m_object->set_mental_state			(eMentalStateFree);
	m_object->CSightManager::setup		(CSightAction(SightManager::eSightTypePathDirection));
	m_object->CObjectHandler::set_goal	(eObjectActionIdle);
}

void CStalkerActionReachTaskLocation::finalize	()
{
	inherited::finalize					();
	m_object->game_location_selector().set_selection_type		(eSelectionTypeRandomBranching);
}

void CStalkerActionReachTaskLocation::execute		()
{
	inherited::execute					();
	m_object->play						(eStalkerSoundHumming,60000,10000);
	m_object->set_game_dest_vertex		(m_object->current_alife_task().m_tGraphID);
	if (m_object->path_completed() || m_object->alife_task_completed())
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
	
	if (pSettings->line_exist(*m_object->cNameSect(),"time_to_search_for_artefacts"))
		m_inertia_time					= pSettings->r_u32(*m_object->cNameSect(),"time_to_search_for_artefacts");

	if (pSettings->line_exist(*m_object->cNameSect(),"distance_to_search_for_artefacts"))
		m_max_search_distance			= pSettings->r_float(*m_object->cNameSect(),"distance_to_search_for_artefacts");
}

void CStalkerActionAccomplishTask::initialize	()
{
	inherited::initialize				();
	m_object->set_node_evaluator		(0);
	m_object->set_path_evaluator		(0);
	m_object->set_desired_position		(0);
	m_object->set_desired_direction		(0);
	m_object->set_path_type				(MovementManager::ePathTypeLevelPath);
	m_object->set_detail_path_type		(DetailPathManager::eDetailPathTypeSmooth);
	m_object->set_body_state			(eBodyStateStand);
	m_object->set_movement_type			(eMovementTypeWalk);
	m_object->set_mental_state			(eMentalStateFree);
	m_object->CSightManager::setup		(CSightAction(SightManager::eSightTypeSearch,false,true));
	m_object->CObjectHandler::set_goal	(eObjectActionIdle);
	m_object->extrapolate_path			(true);
	m_object->set_sound_mask			(u32(eStalkerSoundMaskNoHumming));
}

void CStalkerActionAccomplishTask::finalize	()
{
	inherited::finalize					();
	m_object->extrapolate_path			(false);

	if (!m_object->g_Alive())
		return;

	m_object->set_sound_mask			(0);
}

void CStalkerActionAccomplishTask::execute		()
{
	inherited::execute					();

	if (m_object->alife_task_completed())
		m_storage->set_property			(eWorldPropertyTaskCompleted,true);

	if (!m_object->path_completed())
		return;
	
	Fvector position					= ai().game_graph().vertex(m_object->current_alife_task().m_tGraphID)->level_point();
	m_object->m_ce_random_game->setup	(m_object->current_alife_task().m_tGraphID,m_max_search_distance);
	CCoverPoint							*point = ai().cover_manager().best_cover(position,m_max_search_distance,*m_object->m_ce_random_game);

	if (point && !completed()) {
		m_object->set_level_dest_vertex	(point->level_vertex_id());
		m_object->set_desired_position	(&point->position());
	}
	else {
		if (!m_object->alife_task_completed()) {
			m_object->failed_to_complete_alife_task();
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
	m_object->set_node_evaluator		(0);
	m_object->set_path_evaluator		(0);
	m_object->set_desired_position		(0);
	m_object->set_desired_direction		(0);
	m_object->game_location_selector().set_selection_type		(eSelectionTypeMask);
	m_object->set_path_type				(MovementManager::ePathTypeGamePath);
	m_object->set_detail_path_type		(DetailPathManager::eDetailPathTypeSmooth);
	m_object->set_body_state			(eBodyStateStand);
	m_object->set_movement_type			(eMovementTypeWalk);
	m_object->set_mental_state			(eMentalStateFree);
	m_object->CSightManager::setup		(CSightAction(SightManager::eSightTypePathDirection));
	m_object->CObjectHandler::set_goal	(eObjectActionIdle);
}

void CStalkerActionReachCustomerLocation::finalize	()
{
	inherited::finalize					();
	m_object->game_location_selector().set_selection_type		(eSelectionTypeRandomBranching);
}

void CStalkerActionReachCustomerLocation::execute		()
{
	inherited::execute					();

	m_object->play						(eStalkerSoundHumming,60000,10000);

	CSE_ALifeDynamicObject				*customer = ai().alife().objects().object(m_object->current_alife_task().m_tCustomerID);
	if (m_object->game_vertex_id() != customer->m_tGraphID) {
		m_object->set_game_dest_vertex	(customer->m_tGraphID);
		return;
	}

	m_object->set_path_type				(MovementManager::ePathTypeLevelPath);

	Fvector								dest_position = customer->o_Position;
	u32									level_vertex_id = customer->m_tNodeID;

	if (!m_object->accessible(customer->o_Position))
		level_vertex_id					= m_object->accessible_nearest(customer->o_Position,dest_position);

	m_object->set_level_dest_vertex		(level_vertex_id);
	m_object->set_desired_position		(&dest_position);

	if (m_object->path_completed())
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
	m_object->set_node_evaluator		(0);
	m_object->set_path_evaluator		(0);
	m_object->set_desired_direction		(0);
	m_object->set_path_type				(MovementManager::ePathTypeLevelPath);
	m_object->set_detail_path_type		(DetailPathManager::eDetailPathTypeSmooth);
	m_object->set_level_dest_vertex		(m_object->level_vertex_id());
	m_object->set_desired_position		(&m_object->Position());
	m_object->set_body_state			(eBodyStateStand);
	m_object->set_movement_type			(eMovementTypeStand);
	m_object->set_mental_state			(eMentalStateFree);
	m_object->CSightManager::setup		(CSightAction(SightManager::eSightTypeCover,false,true));
	m_object->CObjectHandler::set_goal	(eObjectActionIdle);
	m_object->set_sound_mask			(u32(eStalkerSoundMaskNoHumming));
	m_trader							= smart_cast<CAI_Trader*>(Level().Objects.net_Find(m_object->current_alife_task().m_tCustomerID));
	VERIFY								(m_trader);
}

void CStalkerActionCommunicateWithCustomer::finalize	()
{
	inherited::finalize					();
	
	if (!m_object->g_Alive())
		return;

	m_object->set_sound_mask			(0);
}

void CStalkerActionCommunicateWithCustomer::execute		()
{
	inherited::execute					();
	
	if (m_trader->busy_now())
		return;

	m_object->communicate				(m_trader);

	m_storage->set_property				(eWorldPropertyReachedTaskLocation,		false);
	m_storage->set_property				(eWorldPropertyTaskCompleted,			false);
	m_storage->set_property				(eWorldPropertyReachedCustomerLocation,	false);
	m_storage->set_property				(eWorldPropertyCustomerSatisfied,		false);
}
