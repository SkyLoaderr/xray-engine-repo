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
#include "movement_manager_space.h"
#include "detail_path_manager_space.h"
#include "memory_manager.h"
#include "enemy_manager.h"
#include "visual_memory_manager.h"
#include "sight_manager.h"
#include "ai_object_location.h"
#include "stalker_movement_manager.h"
#include "sound_player.h"
#include "motivation_action_manager_stalker.h"

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
	object().sound().remove_active_sounds	(u32(eStalkerSoundMaskNoHumming));
}

void CStalkerActionCombatBase::finalize				()
{
	inherited::finalize			();

	if (!object().g_Alive())
		return;

	object().sound().set_sound_mask	(0);
}

IC	void CStalkerActionCombatBase::last_cover			(CCoverPoint *last_cover)
{
	*m_last_cover				= last_cover;
	object().agent_manager().member(m_object).cover(last_cover);
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
	object().sound().remove_active_sounds(u32(eStalkerSoundMaskNoHumming));
	object().sight().setup	(CSightAction(object().m_best_found_item_to_kill ? &object().m_best_found_item_to_kill->object() : 0,true));
}

void CStalkerActionGetItemToKill::finalize	()
{
	inherited::finalize		();
	object().sight().clear();

	if (!object().g_Alive())
		return;

	object().sound().set_sound_mask(0);
}

void CStalkerActionGetItemToKill::execute	()
{
	inherited::execute		();

	if (!object().m_best_found_item_to_kill)
		return;

	object().movement().set_level_dest_vertex	(object().m_best_found_item_to_kill->object().ai_location().level_vertex_id());
	object().movement().set_node_evaluator		(0);
	object().movement().set_path_evaluator		(0);
	object().movement().set_desired_position	(&object().m_best_found_item_to_kill->object().Position());
	object().movement().set_desired_direction	(0);
	object().movement().set_path_type			(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_body_state			(object().movement().body_state() == eBodyStateCrouch ? eBodyStateCrouch : eBodyStateStand);
	object().movement().set_movement_type		(eMovementTypeWalk);
	object().movement().set_mental_state		(eMentalStateDanger);
	object().set_goal							(eObjectActionIdle);
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
	object().sound().remove_active_sounds	(u32(eStalkerSoundMaskNoHumming));
	object().sight().clear	();
	object().sight().add_action(eSightActionTypeWatchItem,xr_new<CSightControlAction>(1.f,3000,CSightAction(SightManager::eSightTypePathDirection)));
	object().sight().add_action(eSightActionTypeWatchEnemy,xr_new<CSightControlAction>(1.f,3000,CSightAction(SightManager::eSightTypePosition,object().memory().enemy().selected()->Position(),false)));
}

void CStalkerActionMakeItemKilling::finalize	()
{
	inherited::finalize				();
	object().sight().clear();

	if (!object().g_Alive())
		return;

	object().sound().set_sound_mask		(0);
}

void CStalkerActionMakeItemKilling::execute	()
{
	inherited::execute				();

	if (!object().m_best_found_ammo)
		return;

	object().movement().set_level_dest_vertex	(object().m_best_found_ammo->object().ai_location().level_vertex_id());
	object().movement().set_node_evaluator	(0);
	object().movement().set_path_evaluator	(0);
	object().movement().set_desired_position	(&object().m_best_found_ammo->object().Position());
	object().movement().set_desired_direction	(0);
	object().movement().set_path_type			(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_body_state		(eBodyStateStand);
	object().movement().set_movement_type		(eMovementTypeWalk);
	object().movement().set_mental_state		(eMentalStateDanger);
	object().sight().action	(eSightActionTypeWatchEnemy).set_vector3d(object().memory().enemy().selected()->Position());
	object().set_goal				(eObjectActionIdle);
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

	if (!object().g_Alive())
		return;

	object().sound().set_sound_mask(0);
}

void CStalkerActionRetreatFromEnemy::execute		()
{
	inherited::execute		();

	if (!object().memory().enemy().selected())
		return;

	if (!object().memory().enemy().selected())
		return;

	CMemoryInfo							mem_object = object().memory().memory(object().memory().enemy().selected());

	object().movement().set_movement_type			(eMovementTypeRun);
	object().movement().set_path_type				(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type		(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_body_state			(eBodyStateStand);
	object().movement().set_mental_state			(eMentalStatePanic);

	object().m_ce_far->setup			(mem_object.m_object_params.m_position,0.f,300.f);
	CCoverPoint							*point = ai().cover_manager().best_cover(object().Position(),30.f,*object().m_ce_far,CMovementRestrictor(m_object,false));
	if (!point) {
		object().m_ce_far->setup		(mem_object.m_object_params.m_position,0.f,300.f);
		point							= ai().cover_manager().best_cover(object().Position(),50.f,*object().m_ce_far,CMovementRestrictor(m_object,false));
	}

	if (point) {
		object().movement().set_level_dest_vertex			(point->level_vertex_id());
		object().movement().set_desired_position			(&point->position());
		object().CObjectHandler::set_goal		(eObjectActionIdle);
		object().sight().setup			(CSightAction(SightManager::eSightTypePathDirection,false));
	}
	else {
		if (object().memory().visual().visible_now(object().memory().enemy().selected())) {
			object().movement().set_mental_state			(eMentalStateDanger);
			object().CObjectHandler::set_goal	(eObjectActionFire1,object().best_weapon());
			object().sight().setup		(CSightAction(object().memory().enemy().selected(),true));
		}
		else {
			object().CObjectHandler::set_goal	(eObjectActionIdle);
			object().sight().setup		(CSightAction(SightManager::eSightTypeCover,true));
		}
	}

	object().sound().play	(object().memory().enemy().selected()->human_being() ? eStalkerSoundPanicHuman : eStalkerSoundPanicMonster,0,0,10000);
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
	object().movement().set_node_evaluator		(0);
	object().movement().set_path_evaluator		(0);
	object().movement().set_desired_direction		(0);
	object().movement().set_path_type				(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type		(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_nearest_accessible_position		();
	object().movement().set_body_state			(eBodyStateStand);
	object().movement().set_movement_type			(eMovementTypeStand);
	object().movement().set_mental_state			(eMentalStateDanger);
	object().sight().setup		(CSightAction(SightManager::eSightTypeCurrentDirection));
	object().CObjectHandler::set_goal	(eObjectActionIdle);
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

	if (!object().m_best_item_to_kill)
		return;

	if (!object().memory().enemy().selected())
		return;

	CMemoryInfo							mem_object = object().memory().memory(object().memory().enemy().selected());
	Fvector								position = mem_object.m_object_params.m_position;
	object().m_ce_best->setup			(position,10.f,170.f,10.f);
	CCoverPoint							*point = ai().cover_manager().best_cover(object().Position(),10.f,*object().m_ce_best,CMovementRestrictor(m_object,true));
	if (!point) {
		object().m_ce_best->setup		(position,10.f,170.f,10.f);
		point							= ai().cover_manager().best_cover(object().Position(),30.f,*object().m_ce_best,CMovementRestrictor(m_object,true));
	}

	if (point) {
		object().movement().set_level_dest_vertex	(point->level_vertex_id());
		object().movement().set_desired_position	(&point->position());
		object().movement().set_movement_type		(eMovementTypeRun);
		object().brain().affect_cover				(false);
	}
	else {
		object().brain().affect_cover				(true);
		object().movement().set_movement_type		(eMovementTypeStand);
	}

	if (object().memory().visual().visible_now(object().memory().enemy().selected()))
		object().sight().setup	(CSightAction(object().memory().enemy().selected(),true));
	else
		object().sight().setup	(CSightAction(SightManager::eSightTypePosition,mem_object.m_object_params.m_position,true));

	object().CObjectHandler::set_goal	(eObjectActionAimReady1,object().best_weapon());
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
	object().movement().set_node_evaluator		(0);
	object().movement().set_path_evaluator		(0);
	object().movement().set_desired_direction		(0);
	object().movement().set_path_type				(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type		(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_nearest_accessible_position		();
	object().movement().set_body_state			(eBodyStateCrouch);
	object().movement().set_movement_type			(eMovementTypeStand);
	object().movement().set_mental_state			(eMentalStateDanger);
	m_storage->set_property				(eWorldPropertyLookedOut,false);
	m_storage->set_property				(eWorldPropertyPositionHolded,false);
	m_storage->set_property				(eWorldPropertyEnemyDetoured,false);
	if (object().memory().enemy().selected()->human_being())
		object().sound().play					(eStalkerSoundAttack);
	object().brain().affect_cover		(true);
}

void CStalkerActionKillEnemy::finalize			()
{
	inherited::finalize					();
}

void CStalkerActionKillEnemy::execute			()
{
	inherited::execute					();
	
	object().sight().setup		(CSightAction(object().memory().enemy().selected(),true));

	if (!object().can_kill_enemy()) {
		object().CObjectHandler::set_goal	(eObjectActionAimReady1,object().best_weapon());
		return;
	}

	u32									queue_interval, queue_size;
	float								distance = object().memory().enemy().selected()->Position().distance_to(object().Position());
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

	object().CObjectHandler::set_goal	(eObjectActionFire1,object().best_weapon(),queue_size,queue_interval);
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
	object().movement().set_node_evaluator		(0);
	object().movement().set_path_evaluator		(0);
	object().movement().set_desired_direction		(0);
	object().movement().set_path_type				(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type		(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_body_state			(eBodyStateStand);
	object().movement().set_movement_type			(eMovementTypeRun);
	object().movement().set_mental_state			(eMentalStateDanger);
	m_storage->set_property				(eWorldPropertyLookedOut,false);
	m_storage->set_property				(eWorldPropertyPositionHolded,false);
	m_storage->set_property				(eWorldPropertyEnemyDetoured,false);

	if (object().memory().enemy().selected()->human_being()) {
		if (object().memory().visual().visible_now(object().memory().enemy().selected()) && object().agent_manager().group_behaviour())
//			if (::Random.randI(2))
				object().sound().play				(eStalkerSoundBackup);
//			else
//				object().sound().play				(eStalkerSoundAttack);
		else
			object().sound().play					(eStalkerSoundAttack);
	}
}

void CStalkerActionTakeCover::finalize		()
{
	inherited::finalize					();
}

void CStalkerActionTakeCover::execute		()
{
	inherited::execute					();

	CMemoryInfo							mem_object = object().memory().memory(object().memory().enemy().selected());

	if (!mem_object.m_object)
		return;

	Fvector								position = mem_object.m_object_params.m_position;
	object().m_ce_best->setup			(position,10.f,170.f,10.f);
	CCoverPoint							*point = ai().cover_manager().best_cover(object().Position(),10.f,*object().m_ce_best,CMovementRestrictor(m_object,true));
	if (!point) {
		object().m_ce_best->setup		(position,10.f,170.f,10.f);
		point							= ai().cover_manager().best_cover(object().Position(),30.f,*object().m_ce_best,CMovementRestrictor(m_object,true));
	}

	if (point) {
		last_cover						(point);
		object().movement().set_level_dest_vertex	(point->level_vertex_id());
		object().movement().set_desired_position	(&point->position());
	}
	else
		object().movement().set_nearest_accessible_position	();

	if (object().memory().visual().visible_now(object().memory().enemy().selected()) && object().can_kill_enemy())
		object().CObjectHandler::set_goal	(eObjectActionFire1,object().best_weapon());
	else
		object().CObjectHandler::set_goal	(eObjectActionAimReady1,object().best_weapon());

	if (object().movement().path_completed())// && (object().memory().enemy().selected()->Position().distance_to_sqr(object().Position()) >= 10.f))
		m_storage->set_property			(eWorldPropertyInCover,true);

	object().sight().setup		(CSightAction(SightManager::eSightTypePosition,mem_object.m_object_params.m_position,true));
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
	object().movement().set_node_evaluator		(0);
	object().movement().set_path_evaluator		(0);
	object().movement().set_desired_direction		(0);
	object().movement().set_path_type				(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type		(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_body_state			(eBodyStateCrouch);
	object().movement().set_movement_type			(eMovementTypeWalk);
	object().movement().set_mental_state			(eMentalStateDanger);
	object().CObjectHandler::set_goal	(eObjectActionAimReady1,object().best_weapon());
	set_inertia_time					(1000);
	object().brain().affect_cover		(true);
}

IC	float current_cover					(CAI_Stalker *object)
{
	Fvector								position, direction;
	object->g_fireParams				(0,position,direction);

	object->Center						(position);
	position.x							= object->Position().x;
	position.z							= object->Position().z;
	collide::rq_result					ray_query_result;
	BOOL								result = Level().ObjectSpace.RayPick(
		position,
		direction,
		10.f,
		collide::rqtStatic,
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
	
	CMemoryInfo							mem_object = object().memory().memory(object().memory().enemy().selected());

	if (!mem_object.m_object)
		return;

	object().sight().setup		(CSightAction(SightManager::eSightTypePosition,mem_object.m_object_params.m_position,true));

	if (current_cover(m_object) >= 3.f) {
		object().movement().set_nearest_accessible_position	();
		m_storage->set_property			(eWorldPropertyLookedOut,true);
		return;
	}

	object().movement().set_nearest_accessible_position(mem_object.m_object_params.m_position,mem_object.m_object_params.m_level_vertex_id);
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
	object().movement().set_node_evaluator		(0);
	object().movement().set_path_evaluator		(0);
	object().movement().set_desired_direction		(0);
	object().movement().set_path_type				(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type		(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_nearest_accessible_position		();
	object().movement().set_body_state			(eBodyStateCrouch);
	object().movement().set_movement_type			(eMovementTypeWalk);
	object().movement().set_mental_state			(eMentalStateDanger);
	object().CObjectHandler::set_goal	(eObjectActionAimReady1,object().best_weapon());
	if (object().agent_manager().group_behaviour())
		object().sound().play					(eStalkerSoundDetour,5000);
	set_inertia_time					(5000 + ::Random32.random(5000));
	object().brain().affect_cover		(true);
}

void CStalkerActionHoldPosition::finalize		()
{
	inherited::finalize					();
}

void CStalkerActionHoldPosition::execute		()
{
	inherited::execute					();
	
	CMemoryInfo							mem_object = object().memory().memory(object().memory().enemy().selected());

	if (!mem_object.m_object)
		return;

	if (current_cover(m_object) < 3.f)
		m_storage->set_property			(eWorldPropertyLookedOut,false);

	object().sight().setup		(CSightAction(SightManager::eSightTypePosition,mem_object.m_object_params.m_position,true));
	
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
	object().movement().set_node_evaluator		(0);
	object().movement().set_path_evaluator		(0);
	object().movement().set_desired_direction		(0);
	object().movement().set_path_type				(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type		(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_body_state			(eBodyStateStand);
	object().movement().set_movement_type			(eMovementTypeRun);
	object().movement().set_mental_state			(eMentalStateDanger);
	object().CObjectHandler::set_goal	(eObjectActionAimReady1,object().best_weapon());
	last_cover							(0);
}

void CStalkerActionDetourEnemy::finalize		()
{
	inherited::finalize					();
}

void CStalkerActionDetourEnemy::execute			()
{
	inherited::execute					();

	if (object().memory().enemy().selected()->human_being() && object().agent_manager().group_behaviour())
		object().sound().play					(eStalkerSoundDetour,5000);
	
	CMemoryInfo							mem_object = object().memory().memory(object().memory().enemy().selected());

	if (!mem_object.m_object)
		return;

	if (object().movement().path_completed()) {
		Fvector								position = mem_object.m_object_params.m_position;
		
		object().m_ce_angle->setup			(position,10.f,170.f,mem_object.m_object_params.m_level_vertex_id);
		CCoverPoint							*point = ai().cover_manager().best_cover(object().Position(),10.f,*object().m_ce_angle,CMovementRestrictor(m_object,true));
		if (!point) {
			object().m_ce_angle->setup		(position,10.f,170.f,mem_object.m_object_params.m_level_vertex_id);
			point							= ai().cover_manager().best_cover(object().Position(),30.f,*object().m_ce_angle,CMovementRestrictor(m_object,true));
		}

		if (point) {
			object().movement().set_level_dest_vertex	(point->level_vertex_id());
			object().movement().set_desired_position	(&point->position());
		}
		else
			object().movement().set_nearest_accessible_position	();

		if (object().movement().path_completed())
			m_storage->set_property			(eWorldPropertyEnemyDetoured,true);
	}

	object().sight().setup	(CSightAction(SightManager::eSightTypePosition,mem_object.m_object_params.m_position,true));
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
	object().movement().set_node_evaluator		(0);
	object().movement().set_path_evaluator		(0);
	object().movement().set_desired_direction		(0);
	object().movement().set_path_type				(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type		(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_body_state			(eBodyStateStand);
	object().movement().set_movement_type			(eMovementTypeWalk);
	object().movement().set_mental_state			(eMentalStateDanger);
	object().CObjectHandler::set_goal	(eObjectActionAimReady1,object().best_weapon());
	last_cover							(0);
}

void CStalkerActionSearchEnemy::finalize		()
{
	inherited::finalize					();
}

void CStalkerActionSearchEnemy::execute			()
{
	inherited::execute					();
	
	object().sound().play						(eStalkerSoundSearch,10000);
	CMemoryInfo							mem_object = object().memory().memory(object().memory().enemy().selected());

	if (!mem_object.m_object)
		return;

	if (object().movement().path_completed()) {
		object().m_ce_ambush->setup		(mem_object.m_object_params.m_position,mem_object.m_self_params.m_position,10.f);
		CCoverPoint							*point = ai().cover_manager().best_cover(mem_object.m_object_params.m_position,10.f,*object().m_ce_ambush,CMovementRestrictor(m_object,true));
		if (!point) {
			object().m_ce_ambush->setup	(mem_object.m_object_params.m_position,mem_object.m_self_params.m_position,10.f);
			point							= ai().cover_manager().best_cover(mem_object.m_object_params.m_position,30.f,*object().m_ce_ambush,CMovementRestrictor(m_object,true));
		}

		if (point) {
			object().movement().set_level_dest_vertex	(point->level_vertex_id());
			object().movement().set_desired_position	(&point->position());
		}
		else
			object().movement().set_nearest_accessible_position	();

//		if (object().movement().path_completed())
//			object().memory().enable(object().memory().enemy().selected(),false);
	}

	object().sight().setup	(CSightAction(SightManager::eSightTypePosition,mem_object.m_object_params.m_position,true));
}
