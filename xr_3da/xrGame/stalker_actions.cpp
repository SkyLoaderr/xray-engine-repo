////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_actions.cpp
//	Created 	: 25.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker action classes
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_actions.h"
#include "ai/stalker/ai_stalker.h"
#include "motivation_action_manager_stalker.h"
#include "stalker_decision_space.h"

using namespace StalkerDecisionSpace;

typedef CStalkerActionBase::_edge_value_type _edge_value_type;

//////////////////////////////////////////////////////////////////////////
// CStalkerActionBase
//////////////////////////////////////////////////////////////////////////

CStalkerActionBase::CStalkerActionBase		(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDead
//////////////////////////////////////////////////////////////////////////

CStalkerActionDead::CStalkerActionDead	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionDead::execute		()
{
	inherited::execute		();
	m_object->CMovementManager::enable_movement(false);
	set_property			(eWorldPropertyDead,true);
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionFreeNoALife
//////////////////////////////////////////////////////////////////////////

CStalkerActionFreeNoALife::CStalkerActionFreeNoALife	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionFreeNoALife::execute		()
{
	inherited::execute		();
	m_object->play			(eStalkerSoundHumming,60000,10000);

	m_object->set_node_evaluator	(0);
	m_object->set_path_evaluator	(0);
	m_object->set_desired_position	(0);
	m_object->set_desired_direction	(0);
	m_object->set_path_type			(CMovementManager::ePathTypeGamePath);
	m_object->set_detail_path_type	(CMovementManager::eDetailPathTypeSmooth);
	m_object->set_body_state		(eBodyStateStand);
	m_object->set_movement_type		(eMovementTypeWalk);
	m_object->set_mental_state		(eMentalStateFree);

	m_object->CSightManager::setup				(SightManager::eSightTypePathDirection);
#ifdef OLD_OBJECT_HANDLER
	m_object->CObjectHandler::set_dest_state	(eObjectActionNoItems);
#else
	m_object->CObjectHandlerGOAP::set_goal		(eObjectActionIdle);
#endif
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionGatherItems
//////////////////////////////////////////////////////////////////////////

CStalkerActionGatherItems::CStalkerActionGatherItems	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionGatherItems::initialize	()
{
	inherited::initialize	();
	m_object->set_sound_mask(u32(eStalkerSoundMaskHumming));
}

void CStalkerActionGatherItems::finalize	()
{
	inherited::finalize		();
	m_object->set_sound_mask(0);
}

void CStalkerActionGatherItems::execute		()
{
	inherited::execute		();

	if (!m_object->item())
		return;

	m_object->set_level_dest_vertex	(m_object->item()->level_vertex_id());
	m_object->set_node_evaluator	(0);
	m_object->set_path_evaluator	(0);
	m_object->set_desired_position	(&m_object->item()->Position());
	m_object->set_desired_direction	(0);
	m_object->set_path_type			(CMovementManager::ePathTypeLevelPath);
	m_object->set_detail_path_type	(CMovementManager::eDetailPathTypeSmooth);
	m_object->set_body_state		(eBodyStateStand);
	m_object->set_movement_type		(eMovementTypeWalk);
	m_object->set_mental_state		(eMentalStateDanger);

	m_object->setup					(SightManager::eSightTypePosition,&m_object->item()->Position());
#ifdef OLD_OBJECT_HANDLER
	m_object->CObjectHandler::set_dest_state	(eObjectActionNoItems);
#else
	m_object->CObjectHandlerGOAP::set_goal		(eObjectActionIdle);
#endif
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionKillEnemy
//////////////////////////////////////////////////////////////////////////

CStalkerActionKillEnemy::CStalkerActionKillEnemy	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionKillEnemy::initialize	()
{
	inherited::initialize	();
	m_object->set_sound_mask(u32(eStalkerSoundMaskHumming));
}

void CStalkerActionKillEnemy::finalize	()
{
	inherited::finalize		();
	m_object->set_sound_mask(0);
}

void CStalkerActionKillEnemy::execute		()
{
	inherited::execute		();

	if (!m_object->enemy())
		return;

	CMemoryInfo									mem_object = m_object->memory(m_object->enemy());

	if (!mem_object.m_object)
		return;

	if (m_object->visible(m_object->enemy())) {
#ifdef OLD_OBJECT_HANDLER
		m_object->CObjectHandler::set_dest_state(eObjectActionFire1,m_object->best_weapon());
#else
		m_object->CObjectHandlerGOAP::set_goal	(eObjectActionFire1,m_object->best_weapon());
#endif
		Fvector						position;
		m_object->enemy()->Center	(position);
		m_object->setup				(SightManager::eSightTypeFirePosition,&position);
	}
	else {
		if (!dynamic_cast<CMissile*>(m_object->best_weapon()))
#ifdef OLD_OBJECT_HANDLER
			m_object->CObjectHandler::set_dest_state(eObjectActionAim1,m_object->best_weapon());
#else
			m_object->CObjectHandlerGOAP::set_goal	(eObjectActionAim1,m_object->best_weapon());
#endif
		
		Fvector			direction;
		direction.sub	(mem_object.m_object_params.m_position,m_object->Position());
		m_object->setup	(SightManager::eSightTypeDirection,&direction);
	}

	if (m_object->visible(m_object->enemy()) && (m_object->Position().distance_to(m_object->enemy()->Position()) < 10.f)) {
		m_object->set_path_type			(CMovementManager::ePathTypeLevelPath);
		m_object->set_detail_path_type	(CMovementManager::eDetailPathTypeSmooth);
		m_object->set_body_state		(eBodyStateStand);
		m_object->set_movement_type		(eMovementTypeStand);
		m_object->set_mental_state		(eMentalStateDanger);
		return;
	}

	m_object->set_level_dest_vertex	(mem_object.m_object_params.m_level_vertex_id);
	m_object->set_desired_position	(&mem_object.m_object_params.m_position);
	m_object->set_path_type			(CMovementManager::ePathTypeLevelPath);
	m_object->set_detail_path_type	(CMovementManager::eDetailPathTypeSmooth);
	m_object->set_body_state		(eBodyStateStand);
	m_object->set_movement_type		(eMovementTypeWalk);
	m_object->set_mental_state		(eMentalStateDanger);

	if (m_object->CMovementManager::path_completed())
		m_object->CMemoryManager::enable		(m_object->enemy(),false);
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionGetKillDistance
//////////////////////////////////////////////////////////////////////////

CStalkerActionGetKillDistance::CStalkerActionGetKillDistance	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionGetKillDistance::initialize	()
{
	inherited::initialize	();
	m_object->set_sound_mask(u32(eStalkerSoundMaskHumming));
}

void CStalkerActionGetKillDistance::finalize	()
{
	inherited::finalize		();
	m_object->set_sound_mask(0);
}

void CStalkerActionGetKillDistance::execute	()
{
	inherited::execute		();

	m_object->set_level_dest_vertex	(m_object->level_vertex_id());
	m_object->set_node_evaluator	(0);
	m_object->set_path_evaluator	(0);
	m_object->set_desired_position	(&m_object->Position());
	m_object->set_desired_direction	(0);
	m_object->set_path_type			(CMovementManager::ePathTypeLevelPath);
	m_object->set_detail_path_type	(CMovementManager::eDetailPathTypeSmooth);
	m_object->set_body_state		(eBodyStateStand);
	m_object->set_movement_type		(eMovementTypeWalk);
	m_object->set_mental_state		(eMentalStateDanger);

	m_object->CSightManager::setup				(SightManager::eSightTypeCurrentDirection);
#ifdef OLD_OBJECT_HANDLER
	m_object->CObjectHandler::set_dest_state	(eObjectActionNoItems);
#else
	m_object->CObjectHandlerGOAP::set_goal		(eObjectActionIdle);
#endif
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionGetReadyToKill
//////////////////////////////////////////////////////////////////////////

CStalkerActionGetReadyToKill::CStalkerActionGetReadyToKill	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionGetReadyToKill::initialize	()
{
	inherited::initialize	();
	m_object->set_sound_mask(u32(eStalkerSoundMaskHumming));
}

void CStalkerActionGetReadyToKill::finalize	()
{
	inherited::finalize		();
	m_object->set_sound_mask(0);
}

void CStalkerActionGetReadyToKill::execute	()
{
	inherited::execute		();

	if (!m_object->m_best_item_to_kill)
		return;

	m_object->set_level_dest_vertex	(m_object->level_vertex_id());
	m_object->set_node_evaluator	(0);
	m_object->set_path_evaluator	(0);
	m_object->set_desired_position	(&m_object->Position());
	m_object->set_desired_direction	(0);
	m_object->set_path_type			(CMovementManager::ePathTypeLevelPath);
	m_object->set_detail_path_type	(CMovementManager::eDetailPathTypeSmooth);
	m_object->set_body_state		(eBodyStateStand);
	m_object->set_movement_type		(eMovementTypeWalk);
	m_object->set_mental_state		(eMentalStateDanger);

	m_object->CSightManager::setup				(SightManager::eSightTypeCurrentDirection);
	if (!dynamic_cast<CMissile*>(m_object->best_weapon()))
#ifdef OLD_OBJECT_HANDLER
		m_object->CObjectHandler::set_dest_state	(eObjectActionNoItems);
#else
		m_object->CObjectHandlerGOAP::set_goal		(eObjectActionAimReady1,m_object->best_weapon());
#endif
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionGetEnemy
//////////////////////////////////////////////////////////////////////////

CStalkerActionGetEnemy::CStalkerActionGetEnemy	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionGetEnemy::initialize	()
{
	inherited::initialize	();
	m_object->set_sound_mask(u32(eStalkerSoundMaskHumming));
}

void CStalkerActionGetEnemy::finalize	()
{
	inherited::finalize		();
	m_object->set_sound_mask(0);
}

void CStalkerActionGetEnemy::execute	()
{
	inherited::execute		();

	m_object->set_level_dest_vertex	(m_object->level_vertex_id());
	m_object->set_node_evaluator	(0);
	m_object->set_path_evaluator	(0);
	m_object->set_desired_position	(&m_object->Position());
	m_object->set_desired_direction	(0);
	m_object->set_path_type			(CMovementManager::ePathTypeLevelPath);
	m_object->set_detail_path_type	(CMovementManager::eDetailPathTypeSmooth);
	m_object->set_body_state		(eBodyStateStand);
	m_object->set_movement_type		(eMovementTypeWalk);
	m_object->set_mental_state		(eMentalStateDanger);

	m_object->CSightManager::setup				(SightManager::eSightTypeCurrentDirection);
#ifdef OLD_OBJECT_HANDLER
	m_object->CObjectHandler::set_dest_state	(eObjectActionNoItems);
#else
	m_object->CObjectHandlerGOAP::set_goal		(eObjectActionIdle);
#endif
}

_edge_value_type CStalkerActionGetEnemy::weight	(const CSConditionState &condition0, const CSConditionState &condition1) const
{
	return					(_edge_value_type(100));
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionGetEnemySeen
//////////////////////////////////////////////////////////////////////////

CStalkerActionGetEnemySeen::CStalkerActionGetEnemySeen	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionGetEnemySeen::initialize	()
{
	inherited::initialize	();
	m_object->set_sound_mask(u32(eStalkerSoundMaskHumming));
}

void CStalkerActionGetEnemySeen::finalize	()
{
	inherited::finalize		();
	m_object->set_sound_mask(0);
}

void CStalkerActionGetEnemySeen::execute	()
{
	inherited::execute		();

	if (!m_object->m_best_item_to_kill)
		return;

	if (!m_object->enemy())
		return;

	
	
	CMemoryInfo									mem_object = m_object->memory(m_object->enemy());

	if (!mem_object.m_object)
		return;

	m_object->set_level_dest_vertex	(mem_object.m_object_params.m_level_vertex_id);
	m_object->set_desired_position	(&mem_object.m_object_params.m_position);
	m_object->set_path_type			(CMovementManager::ePathTypeLevelPath);
	m_object->set_detail_path_type	(CMovementManager::eDetailPathTypeSmooth);
	m_object->set_body_state		(eBodyStateStand);
	m_object->set_movement_type		(eMovementTypeWalk);
	m_object->set_mental_state		(eMentalStateDanger);

	Fvector							direction;
	direction.sub					(mem_object.m_object_params.m_position,m_object->Position());
	m_object->setup					(SightManager::eSightTypeDirection,&direction);

	if (!dynamic_cast<CMissile*>(m_object->best_weapon()))
#ifdef OLD_OBJECT_HANDLER
		m_object->CObjectHandler::set_dest_state	(eObjectActionNoItems);
#else
		m_object->CObjectHandlerGOAP::set_goal		(eObjectActionAimReady1,m_object->best_weapon());
#endif

	if (ai().level_graph().inside(mem_object.m_object_params.m_level_vertex_id,mem_object.m_object_params.m_position)) {
		if (m_object->Position().similar(mem_object.m_object_params.m_position,.5f))
			m_object->CMemoryManager::enable	(m_object->enemy(),false);
	}
	else
		if ((mem_object.m_object_params.m_level_vertex_id == m_object->level_vertex_id()) || 
			 (m_object->Position().distance_to_xz(ai().level_graph().vertex_position(mem_object.m_object_params.m_level_vertex_id)) < ai().level_graph().header().cell_size() * 1.5f))
				m_object->CMemoryManager::enable	(m_object->enemy(),false);
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionGetItemToKill
//////////////////////////////////////////////////////////////////////////

CStalkerActionGetItemToKill::CStalkerActionGetItemToKill	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionGetItemToKill::initialize	()
{
	inherited::initialize	();
	m_object->set_sound_mask(u32(eStalkerSoundMaskHumming));
	m_object->CSightManager::clear();
	m_object->CSightManager::add_action(eSightActionTypeWatchItem,xr_new<CSightControlAction>(1.f,3000,CSightAction(SightManager::eSightTypePathDirection)));//,m_object->m_best_found_item_to_kill->Position(),false)));
	m_object->CSightManager::add_action(eSightActionTypeWatchEnemy,xr_new<CSightControlAction>(1.f,3000,CSightAction(SightManager::eSightTypePosition,m_object->enemy()->Position(),false)));
}

void CStalkerActionGetItemToKill::finalize	()
{
	inherited::finalize		();
	m_object->set_sound_mask(0);
	m_object->CSightManager::clear();
}

void CStalkerActionGetItemToKill::execute	()
{
	inherited::execute		();

	if (!m_object->m_best_found_item_to_kill)
		return;

	m_object->set_level_dest_vertex	(m_object->m_best_found_item_to_kill->level_vertex_id());
	m_object->set_node_evaluator	(0);
	m_object->set_path_evaluator	(0);
	m_object->set_desired_position	(&m_object->m_best_found_item_to_kill->Position());
	m_object->set_desired_direction	(0);
	m_object->set_path_type			(CMovementManager::ePathTypeLevelPath);
	m_object->set_detail_path_type	(CMovementManager::eDetailPathTypeSmooth);
	m_object->set_body_state		(eBodyStateStand);
	m_object->set_movement_type		(eMovementTypeWalk);
	m_object->set_mental_state		(eMentalStateDanger);
//	m_object->setup					(SightManager::eSightTypePosition,&m_object->m_best_found_item_to_kill->Position());
//	m_object->setup					(SightManager::eSightTypePosition,&m_object->enemy()->Position(),"bip01_head");
//	m_object->setup					(SightManager::eSightTypeSearch);

//	m_object->CSightManager::action(eSightActionTypeWatchItem).set_vector3d(m_object->m_best_found_item_to_kill->Position());
	m_object->CSightManager::action(eSightActionTypeWatchEnemy).set_vector3d(m_object->enemy()->Position());
#ifdef OLD_OBJECT_HANDLER
	m_object->set_dest_state		(eObjectActionNoItems);
#else
	m_object->set_goal				(eObjectActionIdle);
#endif
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionFindItemToKill
//////////////////////////////////////////////////////////////////////////

CStalkerActionFindItemToKill::CStalkerActionFindItemToKill	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionFindItemToKill::initialize	()
{
	inherited::initialize	();
	m_object->set_sound_mask(u32(eStalkerSoundMaskHumming));
}

void CStalkerActionFindItemToKill::finalize	()
{
	inherited::finalize		();
	m_object->set_sound_mask(0);
}

void CStalkerActionFindItemToKill::execute	()
{
	inherited::execute		();

	m_object->set_level_dest_vertex	(m_object->level_vertex_id());
	m_object->set_node_evaluator	(0);
	m_object->set_path_evaluator	(0);
	m_object->set_desired_position	(&m_object->Position());
	m_object->set_desired_direction	(0);
	m_object->set_path_type			(CMovementManager::ePathTypeLevelPath);
	m_object->set_detail_path_type	(CMovementManager::eDetailPathTypeSmooth);
	m_object->set_body_state		(eBodyStateStand);
	m_object->set_movement_type		(eMovementTypeWalk);
	m_object->set_mental_state		(eMentalStateDanger);

	m_object->CSightManager::setup				(SightManager::eSightTypeCurrentDirection);
#ifdef OLD_OBJECT_HANDLER
	m_object->CObjectHandler::set_dest_state	(eObjectActionNoItems);
#else
	m_object->CObjectHandlerGOAP::set_goal		(eObjectActionIdle);
#endif
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionMakeItemKilling
//////////////////////////////////////////////////////////////////////////

CStalkerActionMakeItemKilling::CStalkerActionMakeItemKilling	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionMakeItemKilling::initialize	()
{
	inherited::initialize	();
	m_object->set_sound_mask(u32(eStalkerSoundMaskHumming));
	m_object->CSightManager::clear();
	m_object->CSightManager::add_action(eSightActionTypeWatchItem,xr_new<CSightControlAction>(1.f,3000,CSightAction(SightManager::eSightTypePathDirection)));//,m_object->m_best_found_ammo->Position(),false)));
	m_object->CSightManager::add_action(eSightActionTypeWatchEnemy,xr_new<CSightControlAction>(1.f,3000,CSightAction(SightManager::eSightTypePosition,m_object->enemy()->Position(),false)));
}

void CStalkerActionMakeItemKilling::finalize	()
{
	inherited::finalize		();
	m_object->set_sound_mask(0);
	m_object->CSightManager::clear();
}

void CStalkerActionMakeItemKilling::execute	()
{
	inherited::execute		();

	if (!m_object->m_best_found_ammo)
		return;

	m_object->set_level_dest_vertex	(m_object->m_best_found_ammo->level_vertex_id());
	m_object->set_node_evaluator	(0);
	m_object->set_path_evaluator	(0);
	m_object->set_desired_position	(&m_object->m_best_found_ammo->Position());
	m_object->set_desired_direction	(0);
	m_object->set_path_type			(CMovementManager::ePathTypeLevelPath);
	m_object->set_detail_path_type	(CMovementManager::eDetailPathTypeSmooth);
	m_object->set_body_state		(eBodyStateStand);
	m_object->set_movement_type		(eMovementTypeWalk);
	m_object->set_mental_state		(eMentalStateDanger);
//	m_object->setup					(SightManager::eSightTypePosition,&m_object->m_best_found_ammo->Position());
//	m_object->setup					(SightManager::eSightTypePosition,&m_object->enemy()->Position(),"bip01_head");
//	m_object->setup					(SightManager::eSightTypeSearch);

//	m_object->CSightManager::action(eSightActionTypeWatchItem).set_vector3d(m_object->m_best_found_ammo->Position());
	m_object->CSightManager::action(eSightActionTypeWatchEnemy).set_vector3d(m_object->enemy()->Position());
#ifdef OLD_OBJECT_HANDLER
	m_object->set_dest_state		(eObjectActionNoItems);
#else
	m_object->set_goal				(eObjectActionIdle);
#endif
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionFindAmmo
//////////////////////////////////////////////////////////////////////////

CStalkerActionFindAmmo::CStalkerActionFindAmmo	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionFindAmmo::initialize	()
{
	inherited::initialize	();
	m_object->set_sound_mask(u32(eStalkerSoundMaskHumming));
}

void CStalkerActionFindAmmo::finalize	()
{
	inherited::finalize		();
	m_object->set_sound_mask(0);
}

void CStalkerActionFindAmmo::execute	()
{
	inherited::execute		();

	m_object->set_level_dest_vertex	(m_object->level_vertex_id());
	m_object->set_node_evaluator	(0);
	m_object->set_path_evaluator	(0);
	m_object->set_desired_position	(&m_object->Position());
	m_object->set_desired_direction	(0);
	m_object->set_path_type			(CMovementManager::ePathTypeLevelPath);
	m_object->set_detail_path_type	(CMovementManager::eDetailPathTypeSmooth);
	m_object->set_body_state		(eBodyStateStand);
	m_object->set_movement_type		(eMovementTypeWalk);
	m_object->set_mental_state		(eMentalStateDanger);

	m_object->CSightManager::setup				(SightManager::eSightTypeCurrentDirection);
#ifdef OLD_OBJECT_HANDLER
	m_object->CObjectHandler::set_dest_state	(eObjectActionNoItems);
#else
	m_object->CObjectHandlerGOAP::set_goal		(eObjectActionIdle);
#endif
}

_edge_value_type CStalkerActionFindAmmo::weight	(const CSConditionState &condition0, const CSConditionState &condition1) const
{
	return					(_edge_value_type(100));
}
