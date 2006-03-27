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
#include "stalker_planner.h"
#include "agent_member_manager.h"
#include "agent_location_manager.h"
#include "danger_cover_location.h"
#include "ai/stalker/ai_stalker_space.h"
#include "weapon.h"
#include "danger_manager.h"
#include "detail_path_manager.h"

#define DISABLE_COVER_BEFORE_DETOUR

const float TEMP_DANGER_DISTANCE = 5.f;
const u32	TEMP_DANGER_INTERVAL = 120000;

const float	CLOSE_MOVE_DISTANCE	 = 10.f;

using namespace StalkerSpace;
using namespace StalkerDecisionSpace;

typedef CStalkerActionBase::_edge_value_type _edge_value_type;

#ifdef _DEBUG
//#	define SILENT_COMBAT
#endif

//////////////////////////////////////////////////////////////////////////
// CStalkerActionCombatBase
//////////////////////////////////////////////////////////////////////////

CStalkerActionCombatBase::CStalkerActionCombatBase	(CAI_Stalker *object, LPCSTR action_name) :
	inherited	(object,action_name)
{
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

void CStalkerActionCombatBase::select_queue_params	(const float &distance, u32 &min_queue_size, u32 &max_queue_size, u32 &min_queue_interval, u32 &max_queue_interval) const
{
	if (distance > 30.f) {
		min_queue_size					= object().min_queue_size_far();
		max_queue_size					= object().max_queue_size_far();
		min_queue_interval				= object().min_queue_interval_far();
		max_queue_interval				= object().max_queue_interval_far();
	}
	else
		if (distance > 15.f) {
			min_queue_size				= object().min_queue_size_medium();
			max_queue_size				= object().max_queue_size_medium();
			min_queue_interval			= object().min_queue_interval_medium();
			max_queue_interval			= object().max_queue_interval_medium();
		}
		else {
			min_queue_size				= object().min_queue_size_close();
			max_queue_size				= object().max_queue_size_close();
			min_queue_interval			= object().min_queue_interval_close();
			max_queue_interval			= object().max_queue_interval_close();
		}
}

bool CStalkerActionCombatBase::fire_make_sense		() const
{
	return					(object().fire_make_sense());
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

	object().movement().set_mental_state		(eMentalStateDanger);
	object().movement().set_level_dest_vertex	(object().m_best_found_item_to_kill->object().ai_location().level_vertex_id());
	object().movement().set_desired_position	(&object().m_best_found_item_to_kill->object().Position());
	object().movement().set_desired_direction	(0);
	object().movement().set_path_type			(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_body_state			(object().movement().body_state() == eBodyStateCrouch ? eBodyStateCrouch : eBodyStateStand);
	object().movement().set_movement_type		(eMovementTypeWalk);
	object().set_goal							(eObjectActionIdle);
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

CStalkerActionRetreatFromEnemy::CStalkerActionRetreatFromEnemy	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
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
	CCoverPoint							*point = ai().cover_manager().best_cover(object().Position(),30.f,*object().m_ce_far,CStalkerMovementRestrictor(m_object,true));
	if (!point) {
		object().m_ce_far->setup		(mem_object.m_object_params.m_position,0.f,300.f);
		point							= ai().cover_manager().best_cover(object().Position(),50.f,*object().m_ce_far,CStalkerMovementRestrictor(m_object,true));
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
			object().sight().setup				(CSightAction(object().memory().enemy().selected(),true,true));
		}
		else {
			object().CObjectHandler::set_goal	(eObjectActionIdle);
			object().sight().setup		(CSightAction(SightManager::eSightTypeCover,true));
		}
	}

#ifndef SILENT_COMBAT
	object().sound().play					(object().memory().enemy().selected()->human_being() ? eStalkerSoundPanicHuman : eStalkerSoundPanicMonster,0,0,10000);
#endif
}

_edge_value_type CStalkerActionRetreatFromEnemy::weight	(const CSConditionState &condition0, const CSConditionState &condition1) const
{
	return								(_edge_value_type(100));
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionGetReadyToKill
//////////////////////////////////////////////////////////////////////////

CStalkerActionGetReadyToKill::CStalkerActionGetReadyToKill(CAI_Stalker *object, LPCSTR action_name) :
	inherited(object,action_name)
{
}

void CStalkerActionGetReadyToKill::initialize	()
{
	inherited::initialize								();

	m_body_state										= object().movement().body_state();
	m_movement_type										= Random.randI(2) ? eMovementTypeRun : eMovementTypeWalk;

	object().movement().set_desired_direction			(0);
	object().movement().set_path_type					(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type			(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_nearest_accessible_position	();
	object().movement().set_body_state					(m_body_state);
	object().movement().set_movement_type				(eMovementTypeRun);
	object().movement().set_mental_state				(eMentalStateDanger);
	object().sight().setup								(CSightAction(SightManager::eSightTypeCurrentDirection));
	object().CObjectHandler::set_goal					(eObjectActionIdle);
	m_storage->set_property								(eWorldPropertyInCover,false);
	m_storage->set_property								(eWorldPropertyLookedOut,false);
	m_storage->set_property								(eWorldPropertyPositionHolded,false);
	m_storage->set_property								(eWorldPropertyEnemyDetoured,false);
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

	if (object().movement().detail().distance_to_target() > CLOSE_MOVE_DISTANCE)
		object().movement().set_body_state		(eBodyStateStand);
	else
		object().movement().set_movement_type	(m_movement_type);

	CMemoryInfo							mem_object = object().memory().memory(object().memory().enemy().selected());
	Fvector								position = mem_object.m_object_params.m_position;
	object().m_ce_best->setup			(position,10.f,170.f,10.f);
	CCoverPoint							*point = ai().cover_manager().best_cover(object().Position(),10.f,*object().m_ce_best,CStalkerMovementRestrictor(m_object,true));
	if (!point) {
		object().m_ce_best->setup		(position,10.f,170.f,10.f);
		point							= ai().cover_manager().best_cover(object().Position(),30.f,*object().m_ce_best,CStalkerMovementRestrictor(m_object,true));
	}

	if (point) {
		object().movement().set_level_dest_vertex	(point->level_vertex_id());
		object().movement().set_desired_position	(&point->position());
		object().movement().set_movement_type		(eMovementTypeRun);
		if (object().movement().path_completed() || object().Position().distance_to(point->position()) < 1.f) {
//			object().movement().set_body_state		(eBodyStateCrouch);
			object().brain().affect_cover			(true);
		}
		else {
//			object().movement().set_body_state		(eBodyStateStand);
			object().brain().affect_cover			(false);
		}
	}
	else {
		object().brain().affect_cover				(true);
		object().movement().set_movement_type		(eMovementTypeStand);
//		object().movement().set_body_state			(eBodyStateCrouch);
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

CStalkerActionKillEnemy::CStalkerActionKillEnemy(CAI_Stalker *object, LPCSTR action_name) :
	inherited(object,action_name)
{
}

void CStalkerActionKillEnemy::initialize		()
{
	inherited::initialize				();
	object().movement().set_desired_direction	(0);
	object().movement().set_path_type			(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_nearest_accessible_position		();
	object().movement().set_mental_state		(eMentalStateDanger);
	object().movement().set_body_state			(m_storage->property(eWorldPropertyUseCrouchToLookOut) ? eBodyStateCrouch : eBodyStateStand);
	object().movement().set_movement_type		(eMovementTypeStand);
	m_storage->set_property						(eWorldPropertyLookedOut,false);
	m_storage->set_property						(eWorldPropertyPositionHolded,false);
	m_storage->set_property						(eWorldPropertyEnemyDetoured,false);
#ifndef SILENT_COMBAT
	if (object().memory().enemy().selected()->human_being())
		if (object().agent_manager().member().can_cry_noninfo_phrase())
			object().sound().play				(eStalkerSoundAttack,0,0,6000,4000);
#endif
	object().brain().affect_cover		(true);
}

void CStalkerActionKillEnemy::finalize			()
{
	inherited::finalize					();
}

void CStalkerActionKillEnemy::execute			()
{
	inherited::execute					();
	
	object().sight().setup		(CSightAction(object().memory().enemy().selected(),true,true));

//. add fire here
//	if (!object().can_kill_enemy()) {
//		object().CObjectHandler::set_goal	(eObjectActionAimReady1,object().best_weapon());
//		return;
//	}

	u32									min_queue_size, max_queue_size, min_queue_interval, max_queue_interval;
	float								distance = object().memory().enemy().selected()->Position().distance_to(object().Position());
	select_queue_params					(distance,min_queue_size, max_queue_size, min_queue_interval, max_queue_interval);
	object().CObjectHandler::set_goal	(eObjectActionFire1,object().best_weapon(),min_queue_size, max_queue_size, min_queue_interval, max_queue_interval);
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionTakeCover
//////////////////////////////////////////////////////////////////////////

CStalkerActionTakeCover::CStalkerActionTakeCover(CAI_Stalker *object, LPCSTR action_name) :
	inherited(object,action_name)
{
}

void CStalkerActionTakeCover::initialize		()
{
	inherited::initialize						();

	m_body_state								= object().movement().body_state();
	m_movement_type								= Random.randI(2) ? eMovementTypeRun : eMovementTypeWalk;

	object().movement().set_desired_direction	(0);
	object().movement().set_path_type			(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_body_state			(m_body_state);
	object().movement().set_movement_type		(eMovementTypeRun);
	object().movement().set_mental_state		(eMentalStateDanger);
	m_storage->set_property						(eWorldPropertyLookedOut,false);
	m_storage->set_property						(eWorldPropertyPositionHolded,false);
	m_storage->set_property						(eWorldPropertyEnemyDetoured,false);

#ifndef SILENT_COMBAT
	if (object().memory().enemy().selected()->human_being()) {
		if (object().agent_manager().member().can_cry_noninfo_phrase()) {
			if (object().memory().visual().visible_now(object().memory().enemy().selected()) && object().agent_manager().member().group_behaviour())
				object().sound().play			(eStalkerSoundBackup,0,0,6000,4000);
//			else
//				object().sound().play			(eStalkerSoundAttack,0,0,6000,4000);
		}
	}
#endif
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

	if (object().movement().detail().distance_to_target() > CLOSE_MOVE_DISTANCE)
		object().movement().set_body_state		(eBodyStateStand);
	else
		object().movement().set_movement_type	(m_movement_type);

	Fvector								position = mem_object.m_object_params.m_position;
	object().m_ce_best->setup			(position,10.f,170.f,10.f);
	CCoverPoint							*point = ai().cover_manager().best_cover(object().Position(),10.f,*object().m_ce_best,CStalkerMovementRestrictor(m_object,true));
	if (!point) {
		object().m_ce_best->setup		(position,10.f,170.f,10.f);
		point							= ai().cover_manager().best_cover(object().Position(),30.f,*object().m_ce_best,CStalkerMovementRestrictor(m_object,true));
	}

	if (point) {
		object().movement().set_level_dest_vertex	(point->level_vertex_id());
		object().movement().set_desired_position	(&point->position());
		if (object().movement().path_completed() && object().Position().distance_to(point->position()) < 1.f)
			object().brain().affect_cover			(true);
		else
			object().brain().affect_cover			(false);
	}
	else {
		object().movement().set_nearest_accessible_position	();
		object().brain().affect_cover				(true);
	}

//.	Add fire here
//	if (object().memory().visual().visible_now(object().memory().enemy().selected()) && object().can_kill_enemy())
//	if (object().memory().visual().visible_now(object().memory().enemy().selected()))
	if (fire_make_sense()) {
		u32									min_queue_size, max_queue_size, min_queue_interval, max_queue_interval;
		float								distance = object().memory().enemy().selected()->Position().distance_to(object().Position());
		select_queue_params					(distance,min_queue_size, max_queue_size, min_queue_interval, max_queue_interval);
		object().CObjectHandler::set_goal	(eObjectActionFire1,object().best_weapon(),min_queue_size, max_queue_size, min_queue_interval, max_queue_interval);
	}
	else
		object().CObjectHandler::set_goal	(eObjectActionAimReady1,object().best_weapon());

	if (object().movement().path_completed())// && (object().memory().enemy().selected()->Position().distance_to_sqr(object().Position()) >= 10.f))
		m_storage->set_property			(eWorldPropertyInCover,true);

	if (object().memory().visual().visible_now(object().memory().enemy().selected()))
		object().sight().setup		(CSightAction(object().memory().enemy().selected(),true,true));
	else
		object().sight().setup		(CSightAction(SightManager::eSightTypePosition,mem_object.m_object_params.m_position,true));
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionLookOut
//////////////////////////////////////////////////////////////////////////

CStalkerActionLookOut::CStalkerActionLookOut(CAI_Stalker *object, LPCSTR action_name) :
	inherited(object,action_name)
{
	m_crouch_look_out_random.seed				(u32(CPU::QPC() & 0xffffffff));
}

void CStalkerActionLookOut::initialize		()
{
	inherited::initialize						();
	
	m_storage->set_property						(eWorldPropertyUseCrouchToLookOut,	!!m_crouch_look_out_random.random(2));

	object().movement().set_desired_direction	(0);
	object().movement().set_path_type			(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_mental_state		(eMentalStateDanger);

	object().movement().set_body_state			(m_storage->property(eWorldPropertyUseCrouchToLookOut) ? eBodyStateCrouch : eBodyStateStand);
	object().movement().set_movement_type		(eMovementTypeWalk);
	object().CObjectHandler::set_goal			(eObjectActionAimReady1,object().best_weapon());
	set_inertia_time							(1000);
	object().brain().affect_cover				(true);
}

IC	float current_cover					(CAI_Stalker *object)
{
	Fvector								position, direction;
	position							= object->eye_matrix.c;
	direction							= object->eye_matrix.k;
	collide::rq_result					ray_query_result;
	BOOL								result = Level().ObjectSpace.RayPick(
		position,
		direction,
		10.f,
		collide::rqtStatic,
		ray_query_result,
		NULL
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

	object().sight().setup				(CSightAction(SightManager::eSightTypePosition,mem_object.m_object_params.m_position,true));

	if (current_cover(m_object) >= 3.f) {
		object().movement().set_nearest_accessible_position	();
		m_storage->set_property			(eWorldPropertyLookedOut,true);
		return;
	}

	Fvector								position = mem_object.m_object_params.m_position;
	object().m_ce_close->setup			(position,10.f,170.f,10.f);
	CCoverPoint							*point = ai().cover_manager().best_cover(object().Position(),10.f,*object().m_ce_close,CStalkerMovementRestrictor(m_object,true,false));
	if (!point || (object().movement().desired_position().similar(object().Position()) && object().movement().path_completed())) {
		object().m_ce_close->setup		(position,10.f,170.f,10.f);
		point							= ai().cover_manager().best_cover(object().Position(),30.f,*object().m_ce_close,CStalkerMovementRestrictor(m_object,true,false));
	}

	if (point) {
		object().movement().set_level_dest_vertex	(point->level_vertex_id());
		object().movement().set_desired_position	(&point->position());
	}

	if ((object().movement().desired_position().similar(object().Position()) && object().movement().path_completed())) {
		m_storage->set_property			(eWorldPropertyLookedOut,true);
		object().movement().set_nearest_accessible_position	();
	}
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionHoldPosition
//////////////////////////////////////////////////////////////////////////

CStalkerActionHoldPosition::CStalkerActionHoldPosition(CAI_Stalker *object, LPCSTR action_name) :
	inherited(object,action_name)
{
}

void CStalkerActionHoldPosition::initialize		()
{
	inherited::initialize				();
	object().movement().set_desired_direction	(0);
	object().movement().set_path_type			(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_nearest_accessible_position		();
	object().movement().set_mental_state		(eMentalStateDanger);
	object().movement().set_body_state			(m_storage->property(eWorldPropertyUseCrouchToLookOut) ? eBodyStateCrouch : eBodyStateStand);
	object().movement().set_movement_type		(eMovementTypeStand);
	object().CObjectHandler::set_goal			(eObjectActionAimReady1,object().best_weapon());
	set_inertia_time							(5000 + ::Random32.random(5000));
	object().brain().affect_cover				(true);
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
	
	if (completed() && object().agent_manager().member().can_detour()) {
		m_storage->set_property			(eWorldPropertyPositionHolded,true);
		m_storage->set_property			(eWorldPropertyInCover,false);
	}

	if (object().agent_manager().member().cover_detouring() && fire_make_sense()) {
		u32									min_queue_size, max_queue_size, min_queue_interval, max_queue_interval;
		float								distance = object().memory().enemy().selected()->Position().distance_to(object().Position());
		select_queue_params					(distance,min_queue_size, max_queue_size, min_queue_interval, max_queue_interval);
		object().CObjectHandler::set_goal	(eObjectActionFire1,object().best_weapon(),min_queue_size, max_queue_size, min_queue_interval, max_queue_interval);
	}
	else
		object().CObjectHandler::set_goal	(eObjectActionAimReady1,object().best_weapon());
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDetourEnemy
//////////////////////////////////////////////////////////////////////////

CStalkerActionDetourEnemy::CStalkerActionDetourEnemy(CAI_Stalker *object, LPCSTR action_name) :
	inherited(object,action_name)
{
}

void CStalkerActionDetourEnemy::initialize		()
{
	inherited::initialize						();
	object().agent_manager().member().member	(&object()).detour	(true);
	object().movement().set_desired_direction	(0);
	object().movement().set_path_type			(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_body_state			(eBodyStateStand);
	object().movement().set_movement_type		(eMovementTypeRun);
	object().movement().set_mental_state		(eMentalStateDanger);
	object().CObjectHandler::set_goal			(eObjectActionAimReady1,object().best_weapon());

#ifdef DISABLE_COVER_BEFORE_DETOUR
	if (/**(Random.randF(1.f) < .8f) && /**/object().agent_manager().member().member(m_object).cover())	
		object().agent_manager().location().add	(
			xr_new<CDangerCoverLocation>(
				object().agent_manager().member().member(m_object).cover(),
				Device.dwTimeGlobal,
				TEMP_DANGER_INTERVAL,
				TEMP_DANGER_DISTANCE
				,object().agent_manager().member().mask(&object())
			)
		);
#endif

	object().agent_manager().member().member(m_object).cover(0);

//#ifndef SILENT_COMBAT
	if (object().memory().enemy().selected()->human_being() && object().agent_manager().member().group_behaviour())
		object().sound().play			(eStalkerSoundNeedBackup);
//#endif
}

void CStalkerActionDetourEnemy::finalize		()
{
	inherited::finalize					();
	if (object().g_Alive())
		object().agent_manager().member().member(&object()).detour	(false);
}

void CStalkerActionDetourEnemy::execute			()
{
	inherited::execute					();

	CMemoryInfo							mem_object = object().memory().memory(object().memory().enemy().selected());

	if (!mem_object.m_object)
		return;

	if (object().movement().path_completed()) {
		Fvector								position = mem_object.m_object_params.m_position;
		
		object().m_ce_angle->setup			(position,10.f,object().ffGetRange(),mem_object.m_object_params.m_level_vertex_id);
		CCoverPoint							*point = ai().cover_manager().best_cover(object().Position(),10.f,*object().m_ce_angle,CStalkerMovementRestrictor(m_object,true));
		if (!point) {
			object().m_ce_angle->setup		(position,10.f,object().ffGetRange(),mem_object.m_object_params.m_level_vertex_id);
			point							= ai().cover_manager().best_cover(object().Position(),30.f,*object().m_ce_angle,CStalkerMovementRestrictor(m_object,true));
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

CStalkerActionSearchEnemy::CStalkerActionSearchEnemy(CAI_Stalker *object, LPCSTR action_name) :
	inherited(object,action_name)
{
}

void CStalkerActionSearchEnemy::initialize		()
{
	inherited::initialize				();
	object().movement().set_desired_direction		(0);
	object().movement().set_path_type				(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type		(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_body_state				(eBodyStateStand);
	object().movement().set_movement_type			(eMovementTypeWalk);
	object().movement().set_mental_state			(eMentalStateDanger);
	object().CObjectHandler::set_goal				(eObjectActionAimReady1,object().best_weapon());
	object().agent_manager().member().member(m_object).cover(0);
}

void CStalkerActionSearchEnemy::finalize		()
{
	inherited::finalize					();
}

void CStalkerActionSearchEnemy::execute			()
{
	inherited::execute					();
	
#ifndef SILENT_COMBAT
	if (object().agent_manager().member().can_cry_noninfo_phrase())
		object().sound().play			(eStalkerSoundSearch,10000,5000);
#endif

	CMemoryInfo							mem_object = object().memory().memory(object().memory().enemy().selected());

	if (!mem_object.m_object)
		return;

	if (object().movement().path_completed()) {
		object().m_ce_ambush->setup		(mem_object.m_object_params.m_position,mem_object.m_self_params.m_position,10.f);
		CCoverPoint						*point = ai().cover_manager().best_cover(mem_object.m_object_params.m_position,10.f,*object().m_ce_ambush,CStalkerMovementRestrictor(m_object,true));
		if (!point) {
			object().m_ce_ambush->setup	(mem_object.m_object_params.m_position,mem_object.m_self_params.m_position,10.f);
			point						= ai().cover_manager().best_cover(mem_object.m_object_params.m_position,30.f,*object().m_ce_ambush,CStalkerMovementRestrictor(m_object,true));
		}

		if (point) {
			object().movement().set_level_dest_vertex	(point->level_vertex_id());
			object().movement().set_desired_position	(&point->position());
		}
		else
			object().movement().set_nearest_accessible_position	();

		if (object().movement().path_completed() && completed())
			object().memory().enable(object().memory().enemy().selected(),false);
	}

	object().sight().setup	(CSightAction(SightManager::eSightTypePosition,mem_object.m_object_params.m_position,true));
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionPostCombatWait
//////////////////////////////////////////////////////////////////////////

CStalkerActionPostCombatWait::CStalkerActionPostCombatWait	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object, action_name)
{
}

void CStalkerActionPostCombatWait::initialize		()
{
	inherited::initialize	();
	
	object().movement().set_movement_type	(eMovementTypeStand);
	
	if (object().inventory().ActiveItem() && object().best_weapon() && (object().inventory().ActiveItem()->object().ID() == object().best_weapon()->object().ID()))
		object().CObjectHandler::set_goal	(eObjectActionAimReady1,object().best_weapon());

	if (object().memory().enemy().last_enemy() && object().memory().visual().visible_now(object().memory().enemy().last_enemy()))
		object().sight().setup	(CSightAction(object().memory().enemy().last_enemy(),true,true));
}

void CStalkerActionPostCombatWait::execute			()
{
	inherited::execute		();
}

void CStalkerActionPostCombatWait::finalize			()
{
	inherited::finalize		();
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionGetDistance
//////////////////////////////////////////////////////////////////////////

CStalkerActionGetDistance::CStalkerActionGetDistance	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object, action_name)
{
}

void CStalkerActionGetDistance::initialize				()
{
	inherited::initialize	();
	object().movement().set_desired_direction	(0);
	object().movement().set_path_type			(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_body_state			(eBodyStateStand);
	object().movement().set_movement_type		(eMovementTypeRun);
	object().movement().set_mental_state		(eMentalStateDanger);
}

void CStalkerActionGetDistance::execute					()
{
	inherited::execute		();

	if (!object().memory().enemy().selected())
		return;

	CMemoryInfo							mem_object = object().memory().memory(object().memory().enemy().selected());

	if (!mem_object.m_object)
		return;

	Fvector								position = mem_object.m_object_params.m_position;

	if (object().inventory().ActiveItem() && object().best_weapon() && (object().inventory().ActiveItem()->object().ID() == object().best_weapon()->object().ID()))
		object().CObjectHandler::set_goal(eObjectActionAimReady1,object().best_weapon());

	object().sight().setup				(CSightAction(SightManager::eSightTypePosition,position,true));

	if (!object().movement().path_completed() && (Device.dwTimeGlobal >= (mem_object.m_level_time + 1000)))
		return;
	
	object().m_ce_best_by_time->setup	(position,10.f,object().ffGetRange(),10.f);
	CCoverPoint							*point = ai().cover_manager().best_cover(object().Position(),10.f,*object().m_ce_best_by_time,CStalkerMovementRestrictor(m_object,true));
	if (!point) {
		object().m_ce_best_by_time->setup(position,10.f,object().ffGetRange(),10.f);
		point							= ai().cover_manager().best_cover(object().Position(),30.f,*object().m_ce_best_by_time,CStalkerMovementRestrictor(m_object,true));
	}

	if (point) {
		object().movement().set_level_dest_vertex	(point->level_vertex_id());
		object().movement().set_desired_position	(&point->position());
	}
	else
		object().movement().set_nearest_accessible_position	();
}

void CStalkerActionGetDistance::finalize				()
{
	inherited::finalize		();
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionHideFromGrenade
//////////////////////////////////////////////////////////////////////////

CStalkerActionHideFromGrenade::CStalkerActionHideFromGrenade	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object, action_name)
{
}

void CStalkerActionHideFromGrenade::initialize				()
{
	inherited::initialize	();

	m_storage->set_property						(eWorldPropertyUseSuddenness,false);

	object().movement().set_desired_direction	(0);
	object().movement().set_path_type			(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_body_state			(eBodyStateStand);
	object().movement().set_movement_type		(eMovementTypeRun);
	object().movement().set_mental_state		(eMentalStateDanger);
	object().m_ce_best->invalidate				();
}

void CStalkerActionHideFromGrenade::execute					()
{
	inherited::execute		();

	if (!object().memory().danger().selected())
		return;

	Fvector								position = object().memory().danger().selected()->position();
	object().m_ce_best->setup			(position,10.f,170.f,10.f);
	CCoverPoint							*point = ai().cover_manager().best_cover(object().Position(),15.f,*object().m_ce_best,CStalkerMovementRestrictor(m_object,false));
	if (!point) {
		object().m_ce_best->setup		(position,10.f,170.f,10.f);
		point							= ai().cover_manager().best_cover(object().Position(),30.f,*object().m_ce_best,CStalkerMovementRestrictor(m_object,false));
	}
	
	if (point) {
		object().movement().set_level_dest_vertex	(point->level_vertex_id());
		object().movement().set_desired_position	(&point->position());
		object().movement().set_movement_type		(eMovementTypeRun);
	}
	else {
		object().movement().set_movement_type		(eMovementTypeStand);
		object().movement().set_body_state			(eBodyStateCrouch);
	}

	if (!object().memory().enemy().selected())
		object().sight().setup			(CSightAction(SightManager::eSightTypePathDirection,true,true));
	else {
		CMemoryInfo						mem_object = object().memory().memory(object().memory().enemy().selected());

		if (!mem_object.m_object) {// || (!m_object->memory().visual().visible_now(object().memory().enemy().selected()) && !object().movement().path_completed())) {
			object().sight().setup			(CSightAction(SightManager::eSightTypePathDirection,true,true));
		}
		else {
			if (!m_object->memory().visual().visible_now(object().memory().enemy().selected())) {
				object().sight().setup				(CSightAction(SightManager::eSightTypePosition,mem_object.m_object_params.m_position,true));
				object().CObjectHandler::set_goal	(eObjectActionAimReady1,object().best_weapon());
			}
			else {
				object().sight().setup				(CSightAction(object().memory().enemy().selected(),true,true));
				object().CObjectHandler::set_goal	(eObjectActionFire1,object().best_weapon());
			}
		}
	}
	
	if (object().movement().path_completed())
		object().movement().set_body_state			(eBodyStateCrouch);

	/**
	EMentalState				temp;
	{
		if (!object().inventory().ActiveItem()) {
			object().CObjectHandler::set_goal			(eObjectActionIdle);
			temp										= eMentalStatePanic;
		}
		else {
			CWeapon					*weapon = smart_cast<CWeapon*>(&object().inventory().ActiveItem()->object());
			if (weapon && weapon->can_be_strapped() && object().best_weapon() && (object().best_weapon()->object().ID() == weapon->ID())) {
				object().CObjectHandler::set_goal			(eObjectActionStrapped,object().inventory().ActiveItem());
				if (weapon->strapped_mode())
					temp									= eMentalStatePanic;
				else
					temp									= eMentalStateDanger;
			}
			else {
				object().CObjectHandler::set_goal			(eObjectActionIdle);
				temp										= eMentalStateDanger;
			}
		}
	}

	if (!object().movement().path_completed()) {
		object().movement().set_body_state			(eBodyStateStand);
		if (object().movement().distance_to_destination_greater(2.f)) {
			object().movement().set_mental_state	(temp);
			object().sight().setup					(CSightAction(SightManager::eSightTypePathDirection,true,true));
		}
		else {
			object().CObjectHandler::set_goal		(eObjectActionAimReady1,m_object->best_weapon());
			object().movement().set_mental_state	(eMentalStateDanger);
			if (object().memory().enemy().selected()) {
				CMemoryInfo							mem_object = object().memory().memory(object().memory().enemy().selected());
				object().sight().setup				(CSightAction(SightManager::eSightTypePosition,mem_object.m_object_params.m_position,true));
			}
			else {
				object().sight().setup				(CSightAction(SightManager::eSightTypeCover,true,true));
			}
		}
	}
	/**/
}

void CStalkerActionHideFromGrenade::finalize				()
{
	inherited::finalize		();
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionSuddenAttack
//////////////////////////////////////////////////////////////////////////

CStalkerActionSuddenAttack::CStalkerActionSuddenAttack		(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object, action_name)
{
}

void CStalkerActionSuddenAttack::initialize					()
{
	inherited::initialize	();

	object().movement().set_desired_direction	(0);
	object().movement().set_path_type			(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_mental_state		(eMentalStateDanger);
	object().CObjectHandler::set_goal			(eObjectActionAimReady1,object().best_weapon());
}

void CStalkerActionSuddenAttack::finalize					()
{
	inherited::finalize		();
}

void CStalkerActionSuddenAttack::execute					()
{
	inherited::execute		();

	if (!object().memory().enemy().selected())
		return;

	CMemoryInfo							mem_object = object().memory().memory(object().memory().enemy().selected());

	if (!mem_object.m_object)
		return;

	bool								visible_now = object().memory().visual().visible_now(object().memory().enemy().selected());
	if (visible_now)
		object().sight().setup			(CSightAction(object().memory().enemy().selected(),true));
	else
		object().sight().setup			(CSightAction(SightManager::eSightTypePosition,mem_object.m_object_params.m_position,true));

	if (object().movement().accessible(mem_object.m_object_params.m_level_vertex_id))
		object().movement().set_level_dest_vertex	(mem_object.m_object_params.m_level_vertex_id);
	else
		object().movement().set_nearest_accessible_position	(ai().level_graph().vertex_position(mem_object.m_object_params.m_level_vertex_id),mem_object.m_object_params.m_level_vertex_id);

	float								distance = object().Position().distance_to(mem_object.m_object_params.m_position);
	if (distance >= 15.f) {
		object().movement().set_body_state				(eBodyStateStand);
		object().movement().set_movement_type			(eMovementTypeRun);
	}
	else {
		if (distance >= 8.f) {
			object().movement().set_body_state			(eBodyStateStand);
			object().movement().set_movement_type		(eMovementTypeWalk);
		}
		else {
			if (distance >= 6.f) {
				object().movement().set_body_state		(eBodyStateCrouch);
				object().movement().set_movement_type	(eMovementTypeRun);
			}
			else {
				if ((distance >= 4.f) || !visible_now) {
					object().movement().set_body_state		(eBodyStateCrouch);
					object().movement().set_movement_type	(eMovementTypeRun);
				}
				else {
					object().movement().set_body_state		(eBodyStateCrouch);
					object().movement().set_movement_type	(eMovementTypeStand);

					u32									min_queue_size, max_queue_size, min_queue_interval, max_queue_interval;
					float								distance = object().memory().enemy().selected()->Position().distance_to(object().Position());
					select_queue_params					(distance,min_queue_size, max_queue_size, min_queue_interval, max_queue_interval);
					object().CObjectHandler::set_goal	(eObjectActionFire1,object().best_weapon(),min_queue_size, max_queue_size, min_queue_interval, max_queue_interval);
				}
			}
		}
	}

	CVisualMemoryManager	*visual_memory_manager = object().memory().enemy().selected()->visual_memory();
	VERIFY					(visual_memory_manager);
	if (!visual_memory_manager->visible_now(&object()))
		return;

	m_storage->set_property	(eWorldPropertyUseSuddenness,	false);
}
