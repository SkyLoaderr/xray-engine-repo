////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_combat_actions.cpp
//	Created 	: 25.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker combat action classes
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_combat_actions.h"
#include "ai/stalker/ai_stalker.h"
#include "script_game_object.h"
#include "stalker_decision_space.h"
#include "inventory.h"
#include "cover_evaluators.h"
#include "cover_point.h"
#include "cover_manager.h"
#include "missile.h"
#include "stalker_movement_restriction.h"

using namespace StalkerDecisionSpace;

typedef CStalkerActionBase::_edge_value_type _edge_value_type;

//////////////////////////////////////////////////////////////////////////
// CStalkerActionCombatBase
//////////////////////////////////////////////////////////////////////////

CStalkerActionCombatBase::CStalkerActionCombatBase	(CCoverPoint **last_cover, CAI_Stalker *object, LPCSTR action_name) :
	inherited	(object,action_name)
{
	m_last_cover	= last_cover;
}

void CStalkerActionCombatBase::initialize			()
{
	inherited::initialize		();
	m_object->set_sound_mask	(u32(eStalkerSoundMaskNoHumming));
}

void CStalkerActionCombatBase::finalize				()
{
	inherited::finalize			();

	if (!m_object->g_Alive())
		return;

	m_object->set_sound_mask	(0);
}

void CStalkerActionCombatBase::last_cover			(CCoverPoint *last_cover)
{
	*m_last_cover				= last_cover;
	m_object->agent_manager().member(m_object).cover(last_cover);
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionGetItemToKill
//////////////////////////////////////////////////////////////////////////

CStalkerActionGetItemToKill::CStalkerActionGetItemToKill	(CCoverPoint **last_cover, CAI_Stalker *object, LPCSTR action_name) :
	inherited				(last_cover,object,action_name)
{
}

void CStalkerActionGetItemToKill::initialize	()
{
	inherited::initialize	();
	m_object->set_sound_mask(u32(eStalkerSoundMaskNoHumming));
	m_object->CSightManager::setup(CSightAction(m_object->m_best_found_item_to_kill,true));
}

void CStalkerActionGetItemToKill::finalize	()
{
	inherited::finalize		();
	m_object->CSightManager::clear();

	if (!m_object->g_Alive())
		return;

	m_object->set_sound_mask(0);
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
	m_object->set_path_type			(MovementManager::ePathTypeLevelPath);
	m_object->set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	m_object->set_body_state		(eBodyStateStand);
	m_object->set_movement_type		(eMovementTypeWalk);
	m_object->set_mental_state		(eMentalStateDanger);
	m_object->set_goal				(eObjectActionIdle);
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionMakeItemKilling
//////////////////////////////////////////////////////////////////////////

CStalkerActionMakeItemKilling::CStalkerActionMakeItemKilling	(CCoverPoint **last_cover, CAI_Stalker *object, LPCSTR action_name) :
	inherited				(last_cover,object,action_name)
{
}

void CStalkerActionMakeItemKilling::initialize	()
{
	inherited::initialize			();
	m_object->set_sound_mask		(u32(eStalkerSoundMaskNoHumming));
	m_object->CSightManager::clear	();
	m_object->CSightManager::add_action(eSightActionTypeWatchItem,xr_new<CSightControlAction>(1.f,3000,CSightAction(SightManager::eSightTypePathDirection)));
	m_object->CSightManager::add_action(eSightActionTypeWatchEnemy,xr_new<CSightControlAction>(1.f,3000,CSightAction(SightManager::eSightTypePosition,m_object->enemy()->Position(),false)));
}

void CStalkerActionMakeItemKilling::finalize	()
{
	inherited::finalize				();
	m_object->CSightManager::clear();

	if (!m_object->g_Alive())
		return;

	m_object->set_sound_mask		(0);
}

void CStalkerActionMakeItemKilling::execute	()
{
	inherited::execute				();

	if (!m_object->m_best_found_ammo)
		return;

	m_object->set_level_dest_vertex	(m_object->m_best_found_ammo->level_vertex_id());
	m_object->set_node_evaluator	(0);
	m_object->set_path_evaluator	(0);
	m_object->set_desired_position	(&m_object->m_best_found_ammo->Position());
	m_object->set_desired_direction	(0);
	m_object->set_path_type			(MovementManager::ePathTypeLevelPath);
	m_object->set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	m_object->set_body_state		(eBodyStateStand);
	m_object->set_movement_type		(eMovementTypeWalk);
	m_object->set_mental_state		(eMentalStateDanger);
	m_object->CSightManager::action	(eSightActionTypeWatchEnemy).set_vector3d(m_object->enemy()->Position());
	m_object->set_goal				(eObjectActionIdle);
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionRetreatFromEnemy
//////////////////////////////////////////////////////////////////////////

CStalkerActionRetreatFromEnemy::CStalkerActionRetreatFromEnemy	(CCoverPoint **last_cover, CAI_Stalker *object, LPCSTR action_name) :
	inherited				(last_cover,object,action_name)
{
}

void CStalkerActionRetreatFromEnemy::initialize	()
{
	inherited::initialize	();
}

void CStalkerActionRetreatFromEnemy::finalize	()
{
	inherited::finalize		();

	if (!m_object->g_Alive())
		return;

	m_object->set_sound_mask(0);
}

void CStalkerActionRetreatFromEnemy::execute		()
{
	inherited::execute		();

	if (!m_object->enemy())
		return;

	if (!m_object->enemy())
		return;

	CMemoryInfo						mem_object = m_object->memory(m_object->enemy());

	m_object->set_movement_type			(eMovementTypeRun);
	m_object->set_path_type				(MovementManager::ePathTypeLevelPath);
	m_object->set_detail_path_type		(DetailPathManager::eDetailPathTypeSmooth);
	m_object->set_body_state			(eBodyStateStand);
	m_object->set_mental_state			(eMentalStateDanger);

	m_object->m_ce_far->setup			(mem_object.m_object_params.m_position,0.f,300.f);
	CCoverPoint							*point = ai().cover_manager().best_cover(m_object->Position(),30.f,*m_object->m_ce_far,CMovementRestrictor(m_object,false));
	if (!point)
		point							= ai().cover_manager().best_cover(m_object->Position(),50.f,*m_object->m_ce_far,CMovementRestrictor(m_object,false));

	if (point) {
		m_object->set_level_dest_vertex			(point->level_vertex_id());
		m_object->set_desired_position			(&point->position());
		m_object->CObjectHandler::set_goal		(eObjectActionIdle);
		m_object->setup							(CSightAction(SightManager::eSightTypePathDirection,false));
	}
	else {
		if (m_object->visible_now(m_object->enemy())) {
			m_object->CObjectHandler::set_goal	(eObjectActionFire1,m_object->best_weapon());
			m_object->setup						(CSightAction(m_object->enemy(),true));
		}
		else {
			m_object->CObjectHandler::set_goal	(eObjectActionIdle);
			m_object->setup						(CSightAction(SightManager::eSightTypeCover,true));
		}
	}

	m_object->play	(m_object->enemy()->human_being() ? eStalkerSoundPanicHuman : eStalkerSoundPanicMonster,0,0,10000);
}

_edge_value_type CStalkerActionRetreatFromEnemy::weight	(const CSConditionState &condition0, const CSConditionState &condition1) const
{
	return								(_edge_value_type(100));
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionGetReadyToKill
//////////////////////////////////////////////////////////////////////////

CStalkerActionGetReadyToKill::CStalkerActionGetReadyToKill(CCoverPoint **last_cover, CAI_Stalker *object, LPCSTR action_name) :
	inherited(last_cover,object,action_name)
{
}

void CStalkerActionGetReadyToKill::initialize	()
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
	m_object->set_mental_state			(eMentalStateDanger);
	m_object->CSightManager::setup		(CSightAction(SightManager::eSightTypeCurrentDirection));
	m_object->CObjectHandler::set_goal	(eObjectActionIdle);
	m_storage->set_property				(eWorldPropertyInCover,false);
	m_storage->set_property				(eWorldPropertyLookedOut,false);
	m_storage->set_property				(eWorldPropertyPositionHolded,false);
	m_storage->set_property				(eWorldPropertyEnemyDetoured,false);
}

void CStalkerActionGetReadyToKill::finalize		()
{
	inherited::finalize					();
}

void CStalkerActionGetReadyToKill::execute		()
{
	inherited::execute					();

	if (!m_object->m_best_item_to_kill)
		return;

	if (!m_object->enemy())
		return;

	CMemoryInfo							mem_object = m_object->memory(m_object->enemy());
	Fvector								position = mem_object.m_object_params.m_position;
	m_object->m_ce_best->setup			(position,10.f,170.f,10.f);
	CCoverPoint							*point = ai().cover_manager().best_cover(m_object->Position(),10.f,*m_object->m_ce_best,CMovementRestrictor(m_object,true));
	if (!point) {
		m_object->m_ce_best->setup		(position,10.f,170.f,10.f);
		point							= ai().cover_manager().best_cover(m_object->Position(),30.f,*m_object->m_ce_best,CMovementRestrictor(m_object,true));
	}

	if (point) {
		m_object->set_level_dest_vertex	(point->level_vertex_id());
		m_object->set_desired_position	(&point->position());
		m_object->set_movement_type		(eMovementTypeRun);
	}
	else
		m_object->set_movement_type		(eMovementTypeStand);

	if (m_object->visible_now(m_object->enemy()))
		m_object->CSightManager::setup	(CSightAction(m_object->enemy(),true));
	else
		m_object->CSightManager::setup	(CSightAction(SightManager::eSightTypePosition,mem_object.m_object_params.m_position,true));

	m_object->CObjectHandler::set_goal	(eObjectActionAimReady1,m_object->best_weapon());
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionKillEnemy
//////////////////////////////////////////////////////////////////////////

CStalkerActionKillEnemy::CStalkerActionKillEnemy(CCoverPoint **last_cover, CAI_Stalker *object, LPCSTR action_name) :
	inherited(last_cover,object,action_name)
{
}

void CStalkerActionKillEnemy::initialize		()
{
	inherited::initialize				();
	m_object->set_node_evaluator		(0);
	m_object->set_path_evaluator		(0);
	m_object->set_desired_direction		(0);
	m_object->set_path_type				(MovementManager::ePathTypeLevelPath);
	m_object->set_detail_path_type		(DetailPathManager::eDetailPathTypeSmooth);
	m_object->set_level_dest_vertex		(m_object->level_vertex_id());
	m_object->set_desired_position		(&m_object->Position());
	m_object->set_body_state			(eBodyStateCrouch);
	m_object->set_movement_type			(eMovementTypeStand);
	m_object->set_mental_state			(eMentalStateDanger);
	m_storage->set_property				(eWorldPropertyLookedOut,false);
	m_storage->set_property				(eWorldPropertyPositionHolded,false);
	m_storage->set_property				(eWorldPropertyEnemyDetoured,false);
	if (m_object->enemy()->human_being())
		m_object->play					(eStalkerSoundAttack);
}

void CStalkerActionKillEnemy::finalize			()
{
	inherited::finalize					();
}

void CStalkerActionKillEnemy::execute			()
{
	inherited::execute					();
	
	m_object->CSightManager::setup		(CSightAction(m_object->enemy(),true));

	if (!m_object->can_kill_enemy()) {
		m_object->CObjectHandler::set_goal	(eObjectActionAimReady1,m_object->best_weapon());
		return;
	}

	u32									queue_interval, queue_size;
	float								distance = m_object->enemy()->Position().distance_to(m_object->Position());
	if (distance > 30.f) {
		queue_size						= 3;
		queue_interval					= 750;
	}
	else
		if (distance > 15.f) {
			queue_size					= 5;
			queue_interval				= 500;
		}
		else {
			queue_size					= 5;
			queue_interval				= 250;
		}

	m_object->CObjectHandler::set_goal	(eObjectActionFire1,m_object->best_weapon(),queue_size,queue_interval);
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionTakeCover
//////////////////////////////////////////////////////////////////////////

CStalkerActionTakeCover::CStalkerActionTakeCover(CCoverPoint **last_cover, CAI_Stalker *object, LPCSTR action_name) :
	inherited(last_cover,object,action_name)
{
}

void CStalkerActionTakeCover::initialize		()
{
	inherited::initialize				();
	m_object->set_node_evaluator		(0);
	m_object->set_path_evaluator		(0);
	m_object->set_desired_direction		(0);
	m_object->set_path_type				(MovementManager::ePathTypeLevelPath);
	m_object->set_detail_path_type		(DetailPathManager::eDetailPathTypeSmooth);
	m_object->set_body_state			(eBodyStateStand);
	m_object->set_movement_type			(eMovementTypeRun);
	m_object->set_mental_state			(eMentalStateDanger);
	m_storage->set_property				(eWorldPropertyLookedOut,false);
	m_storage->set_property				(eWorldPropertyPositionHolded,false);
	m_storage->set_property				(eWorldPropertyEnemyDetoured,false);

	if (m_object->enemy()->human_being()) {
		if (m_object->visible_now(m_object->enemy()) && m_object->agent_manager().group_behaviour())
//			if (::Random.randI(2))
				m_object->play				(eStalkerSoundBackup);
//			else
//				m_object->play				(eStalkerSoundAttack);
		else
			m_object->play					(eStalkerSoundAttack);
	}
}

void CStalkerActionTakeCover::finalize		()
{
	inherited::finalize					();
}

void CStalkerActionTakeCover::execute		()
{
	inherited::execute					();

	CMemoryInfo							mem_object = m_object->memory(m_object->enemy());

	if (!mem_object.m_object)
		return;

	Fvector								position = mem_object.m_object_params.m_position;
	m_object->m_ce_best->setup			(position,10.f,170.f,10.f);
	CCoverPoint							*point = ai().cover_manager().best_cover(m_object->Position(),10.f,*m_object->m_ce_best,CMovementRestrictor(m_object,true));
	if (!point) {
		m_object->m_ce_best->setup		(position,10.f,170.f,10.f);
		point							= ai().cover_manager().best_cover(m_object->Position(),30.f,*m_object->m_ce_best,CMovementRestrictor(m_object,true));
	}

	if (point) {
		last_cover						(point);
		m_object->set_level_dest_vertex	(point->level_vertex_id());
		m_object->set_desired_position	(&point->position());
	}
	else {
		m_object->set_level_dest_vertex	(m_object->level_vertex_id());
		m_object->set_desired_position	(&m_object->Position());
	}

	if (m_object->visible_now(m_object->enemy()) && m_object->can_kill_enemy())
		m_object->CObjectHandler::set_goal	(eObjectActionFire1,m_object->best_weapon());
	else
		m_object->CObjectHandler::set_goal	(eObjectActionAimReady1,m_object->best_weapon());

	if (m_object->path_completed())// && (m_object->enemy()->Position().distance_to_sqr(m_object->Position()) >= 10.f))
		m_storage->set_property			(eWorldPropertyInCover,true);

	m_object->CSightManager::setup		(CSightAction(SightManager::eSightTypePosition,mem_object.m_object_params.m_position,true));
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionLookOut
//////////////////////////////////////////////////////////////////////////

CStalkerActionLookOut::CStalkerActionLookOut(CCoverPoint **last_cover, CAI_Stalker *object, LPCSTR action_name) :
	inherited(last_cover,object,action_name)
{
}

void CStalkerActionLookOut::initialize		()
{
	inherited::initialize				();
	m_object->set_node_evaluator		(0);
	m_object->set_path_evaluator		(0);
	m_object->set_desired_direction		(0);
	m_object->set_path_type				(MovementManager::ePathTypeLevelPath);
	m_object->set_detail_path_type		(DetailPathManager::eDetailPathTypeSmooth);
	m_object->set_body_state			(eBodyStateCrouch);
	m_object->set_movement_type			(eMovementTypeWalk);
	m_object->set_mental_state			(eMentalStateDanger);
	m_object->CObjectHandler::set_goal	(eObjectActionAimReady1,m_object->best_weapon());
	set_inertia_time					(1000);
}

IC	float current_cover					(CAI_Stalker *m_object)
{
	Fvector								position, direction;
	m_object->g_fireParams				(0,position,direction);

	m_object->Center					(position);
	position.x							= m_object->Position().x;
	position.z							= m_object->Position().z;
	Collide::rq_result					ray_query_result;
	BOOL								result = Level().ObjectSpace.RayPick(
		position,
		direction,
		10.f,
		Collide::rqtStatic,
		ray_query_result
	);

	if (!result)
		return							(100.f);

	return								(ray_query_result.range);
}

void CStalkerActionLookOut::finalize		()
{
	inherited::finalize					();
}

void CStalkerActionLookOut::execute		()
{
	inherited::execute					();
	
	CMemoryInfo							mem_object = m_object->memory(m_object->enemy());

	if (!mem_object.m_object)
		return;

	m_object->CSightManager::setup		(CSightAction(SightManager::eSightTypePosition,mem_object.m_object_params.m_position,true));

	if (current_cover(m_object) >= 3.f) {
		m_object->set_level_dest_vertex	(m_object->level_vertex_id());
		m_object->set_desired_position	(&m_object->Position());
		m_storage->set_property			(eWorldPropertyLookedOut,true);
		return;
	}

	Fvector								desired_position = mem_object.m_object_params.m_position;
	u32									level_vertex_id = mem_object.m_object_params.m_level_vertex_id;

	if (!m_object->accessible(mem_object.m_object_params.m_position))
		level_vertex_id = m_object->accessible_nearest(mem_object.m_object_params.m_position,desired_position);

	m_object->set_level_dest_vertex		(level_vertex_id);
	m_object->set_desired_position		(&desired_position);
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionHoldPosition
//////////////////////////////////////////////////////////////////////////

CStalkerActionHoldPosition::CStalkerActionHoldPosition(CCoverPoint **last_cover, CAI_Stalker *object, LPCSTR action_name) :
	inherited(last_cover,object,action_name)
{
}

void CStalkerActionHoldPosition::initialize		()
{
	inherited::initialize				();
	m_object->set_node_evaluator		(0);
	m_object->set_path_evaluator		(0);
	m_object->set_desired_direction		(0);
	m_object->set_path_type				(MovementManager::ePathTypeLevelPath);
	m_object->set_detail_path_type		(DetailPathManager::eDetailPathTypeSmooth);
	m_object->set_level_dest_vertex		(m_object->level_vertex_id());
	m_object->set_desired_position		(&m_object->Position());
	m_object->set_body_state			(eBodyStateCrouch);
	m_object->set_movement_type			(eMovementTypeWalk);
	m_object->set_mental_state			(eMentalStateDanger);
	m_object->CObjectHandler::set_goal	(eObjectActionAimReady1,m_object->best_weapon());
	if (m_object->agent_manager().group_behaviour())
		m_object->play					(eStalkerSoundDetour,5000);
	set_inertia_time					(5000 + ::Random32.random(5000));
}

void CStalkerActionHoldPosition::finalize		()
{
	inherited::finalize					();
}

void CStalkerActionHoldPosition::execute		()
{
	inherited::execute					();
	
	CMemoryInfo							mem_object = m_object->memory(m_object->enemy());

	if (!mem_object.m_object)
		return;

	if (current_cover(m_object) < 3.f)
		m_storage->set_property			(eWorldPropertyLookedOut,false);

	m_object->CSightManager::setup		(CSightAction(SightManager::eSightTypePosition,mem_object.m_object_params.m_position,true));
	
	if (completed()) {
		m_storage->set_property			(eWorldPropertyPositionHolded,true);
		m_storage->set_property			(eWorldPropertyInCover,false);
	}
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDetourEnemy
//////////////////////////////////////////////////////////////////////////

CStalkerActionDetourEnemy::CStalkerActionDetourEnemy(CCoverPoint **last_cover, CAI_Stalker *object, LPCSTR action_name) :
	inherited(last_cover,object,action_name)
{
}

void CStalkerActionDetourEnemy::initialize		()
{
	inherited::initialize				();
	m_object->set_node_evaluator		(0);
	m_object->set_path_evaluator		(0);
	m_object->set_desired_direction		(0);
	m_object->set_path_type				(MovementManager::ePathTypeLevelPath);
	m_object->set_detail_path_type		(DetailPathManager::eDetailPathTypeSmooth);
	m_object->set_body_state			(eBodyStateStand);
	m_object->set_movement_type			(eMovementTypeRun);
	m_object->set_mental_state			(eMentalStateDanger);
	m_object->CObjectHandler::set_goal	(eObjectActionAimReady1,m_object->best_weapon());
	last_cover							(0);
}

void CStalkerActionDetourEnemy::finalize		()
{
	inherited::finalize					();
}

void CStalkerActionDetourEnemy::execute			()
{
	inherited::execute					();

	if (m_object->enemy()->human_being() && m_object->agent_manager().group_behaviour())
		m_object->play					(eStalkerSoundDetour,5000);
	
	CMemoryInfo							mem_object = m_object->memory(m_object->enemy());

	if (!mem_object.m_object)
		return;

	if (m_object->path_completed()) {
		Fvector								position = mem_object.m_object_params.m_position;
		
		m_object->m_ce_angle->setup			(position,10.f,170.f,mem_object.m_object_params.m_level_vertex_id);
		CCoverPoint							*point = ai().cover_manager().best_cover(m_object->Position(),10.f,*m_object->m_ce_angle,CMovementRestrictor(m_object,true));
		if (!point) {
			m_object->m_ce_angle->setup		(position,10.f,170.f,mem_object.m_object_params.m_level_vertex_id);
			point							= ai().cover_manager().best_cover(m_object->Position(),30.f,*m_object->m_ce_angle,CMovementRestrictor(m_object,true));
		}

		if (point) {
			m_object->set_level_dest_vertex	(point->level_vertex_id());
			m_object->set_desired_position	(&point->position());
		}
		else {
			m_object->set_level_dest_vertex	(m_object->level_vertex_id());
			m_object->set_desired_position	(&m_object->Position());
		}

		if (m_object->path_completed())
			m_storage->set_property			(eWorldPropertyEnemyDetoured,true);
	}

	m_object->CSightManager::setup	(CSightAction(SightManager::eSightTypePosition,mem_object.m_object_params.m_position,true));
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionSearchEnemy
//////////////////////////////////////////////////////////////////////////

CStalkerActionSearchEnemy::CStalkerActionSearchEnemy(CCoverPoint **last_cover, CAI_Stalker *object, LPCSTR action_name) :
	inherited(last_cover,object,action_name)
{
}

void CStalkerActionSearchEnemy::initialize		()
{
	inherited::initialize				();
	m_object->set_node_evaluator		(0);
	m_object->set_path_evaluator		(0);
	m_object->set_desired_direction		(0);
	m_object->set_path_type				(MovementManager::ePathTypeLevelPath);
	m_object->set_detail_path_type		(DetailPathManager::eDetailPathTypeSmooth);
	m_object->set_body_state			(eBodyStateStand);
	m_object->set_movement_type			(eMovementTypeWalk);
	m_object->set_mental_state			(eMentalStateDanger);
	m_object->CObjectHandler::set_goal	(eObjectActionAimReady1,m_object->best_weapon());
	last_cover							(0);
}

void CStalkerActionSearchEnemy::finalize		()
{
	inherited::finalize					();
}

void CStalkerActionSearchEnemy::execute			()
{
	inherited::execute					();
	
	m_object->play						(eStalkerSoundSearch,10000);
	CMemoryInfo							mem_object = m_object->memory(m_object->enemy());

	if (!mem_object.m_object)
		return;

	if (m_object->path_completed()) {
		m_object->m_ce_ambush->setup		(mem_object.m_object_params.m_position,mem_object.m_self_params.m_position,10.f);
		CCoverPoint							*point = ai().cover_manager().best_cover(mem_object.m_object_params.m_position,10.f,*m_object->m_ce_ambush,CMovementRestrictor(m_object,true));
		if (!point) {
			m_object->m_ce_ambush->setup	(mem_object.m_object_params.m_position,mem_object.m_self_params.m_position,10.f);
			point							= ai().cover_manager().best_cover(mem_object.m_object_params.m_position,30.f,*m_object->m_ce_ambush,CMovementRestrictor(m_object,true));
		}

		if (point) {
			m_object->set_level_dest_vertex	(point->level_vertex_id());
			m_object->set_desired_position	(&point->position());
		}
		else {
			m_object->set_level_dest_vertex	(m_object->level_vertex_id());
			m_object->set_desired_position	(&m_object->Position());
		}

//		if (m_object->path_completed())
//			m_object->CMemoryManager::enable(m_object->enemy(),false);
	}

	m_object->CSightManager::setup	(CSightAction(SightManager::eSightTypePosition,mem_object.m_object_params.m_position,true));
}
