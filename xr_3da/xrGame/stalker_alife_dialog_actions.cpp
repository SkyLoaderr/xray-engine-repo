////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_alife_dialog_actions.cpp
//	Created 	: 08.12.2004
//  Modified 	: 08.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker alife dialog action classes
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_alife_dialog_actions.h"
#include "ai/stalker/ai_stalker.h"
#include "inventory_item.h"
#include "script_game_object.h"
#include "inventory.h"
#include "movement_manager_space.h"
#include "detail_path_manager_space.h"
#include "stalker_decision_space.h"
#include "stalker_animation_manager.h"
#include "sight_manager.h"
#include "ai_object_location.h"

using namespace StalkerDecisionSpace;

//////////////////////////////////////////////////////////////////////////
// CStalkerActionGetReadyToDialog
//////////////////////////////////////////////////////////////////////////

CStalkerActionGetReadyToDialog::CStalkerActionGetReadyToDialog	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionGetReadyToDialog::initialize	()
{
	inherited::initialize			();
	m_object->set_node_evaluator	(0);
	m_object->set_path_evaluator	(0);
	m_object->set_desired_direction	(0);
	m_object->set_path_type			(MovementManager::ePathTypeLevelPath);
	m_object->set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	m_object->set_body_state		(eBodyStateStand);
	m_object->set_movement_type		(eMovementTypeWalk);
	m_object->set_mental_state		(eMentalStateDanger);
	m_object->set_level_dest_vertex	(m_object->ai_location().level_vertex_id());
	m_object->set_desired_position	(&m_object->Position());
	m_object->sight().setup			(SightManager::eSightTypeCurrentDirection);
	m_object->remove_active_sounds	(u32(eStalkerSoundMaskNoHumming));

	if (m_object->inventory().ActiveItem() && m_object->best_weapon() && (m_object->inventory().ActiveItem()->ID() == m_object->best_weapon()->ID()))
		m_object->CObjectHandler::set_goal	(eObjectActionStrapped,m_object->best_weapon());
	else
		m_object->CObjectHandler::set_goal	(eObjectActionIdle);
}

void CStalkerActionGetReadyToDialog::finalize	()
{
	inherited::finalize		();

	if (!m_object->g_Alive())
		return;

	m_object->set_sound_mask(0);
}

void CStalkerActionGetReadyToDialog::execute		()
{
	inherited::execute		();
	if (m_object->CObjectHandler::weapon_strapped())
		m_storage->set_property	(eWorldPropertyReadyToDialog,true);
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionHello
//////////////////////////////////////////////////////////////////////////

CStalkerActionHello::CStalkerActionHello	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionHello::initialize	()
{
	inherited::initialize			();
	m_object->set_node_evaluator	(0);
	m_object->set_path_evaluator	(0);
	m_object->set_desired_direction	(0);
	m_object->set_path_type			(MovementManager::ePathTypeLevelPath);
	m_object->set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	m_object->set_body_state		(eBodyStateStand);
	m_object->set_movement_type		(eMovementTypeWalk);
	m_object->set_mental_state		(eMentalStateDanger);
	m_object->set_level_dest_vertex	(m_object->ai_location().level_vertex_id());
	m_object->set_desired_position	(&m_object->Position());
	m_object->sight().setup			(SightManager::eSightTypeCurrentDirection);
	m_object->remove_active_sounds	(u32(eStalkerSoundMaskNoHumming));
	m_object->body_action			(eBodyActionHello);
	
	if (m_object->inventory().ActiveItem() && m_object->best_weapon() && (m_object->inventory().ActiveItem()->ID() == m_object->best_weapon()->ID()))
		m_object->CObjectHandler::set_goal	(eObjectActionStrapped,m_object->best_weapon());
	else
		m_object->CObjectHandler::set_goal	(eObjectActionIdle);

	CStalkerAnimationManager		&animation_manager = m_object->animation();
	animation_manager.setup_storage	(m_storage);
	animation_manager.property_id	(eWorldPropertyHelloCompleted);
	animation_manager.property_value(true);
}

void CStalkerActionHello::finalize	()
{
	inherited::finalize		();
	m_object->animation().setup_storage	(0);

	if (!m_object->g_Alive())
		return;

	m_object->set_sound_mask(0);
}

void CStalkerActionHello::execute		()
{
	inherited::execute		();
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDialog
//////////////////////////////////////////////////////////////////////////

CStalkerActionDialog::CStalkerActionDialog	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionDialog::initialize	()
{
	inherited::initialize			();
	m_object->set_node_evaluator	(0);
	m_object->set_path_evaluator	(0);
	m_object->set_desired_direction	(0);
	m_object->set_path_type			(MovementManager::ePathTypeLevelPath);
	m_object->set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	m_object->set_body_state		(eBodyStateStand);
	m_object->set_movement_type		(eMovementTypeWalk);
	m_object->set_mental_state		(eMentalStateDanger);
	m_object->set_level_dest_vertex	(m_object->ai_location().level_vertex_id());
	m_object->set_desired_position	(&m_object->Position());
	m_object->sight().setup			(SightManager::eSightTypeCurrentDirection);
	m_object->remove_active_sounds	(u32(eStalkerSoundMaskNoHumming));
	m_object->body_action			(eBodyActionHello);
//	
//	if (m_object->inventory().ActiveItem() && m_object->best_weapon() && (m_object->inventory().ActiveItem()->ID() == m_object->best_weapon()->ID()))
//		m_object->CObjectHandler::set_goal	(eObjectActionStrapped,m_object->best_weapon());
//	else
//		m_object->CObjectHandler::set_goal	(eObjectActionIdle);
}

void CStalkerActionDialog::finalize	()
{
	inherited::finalize		();

	if (!m_object->g_Alive())
		return;

	m_object->set_sound_mask(0);
}

void CStalkerActionDialog::execute		()
{
	inherited::execute		();
	m_object->CObjectHandler::set_goal	(eObjectActionFire1,m_object->inventory().m_slots[5].m_pIItem);
}
