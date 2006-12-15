////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_kill_wounded_actions.cpp
//	Created 	: 25.05.2006
//  Modified 	: 25.05.2006
//	Author		: Dmitriy Iassenev
//	Description : Stalker kill wounded action classes
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_kill_wounded_actions.h"
#include "ai/stalker/ai_stalker.h"
#include "inventory.h"
#include "weaponmagazined.h"
#include "stalker_movement_manager.h"
#include "movement_manager_space.h"
#include "detail_path_manager_space.h"
#include "memory_manager.h"
#include "enemy_manager.h"
#include "sight_manager.h"
#include "memory_space.h"
#include "level_graph.h"
#include "visual_memory_manager.h"
#include "script_game_object.h"
#include "restricted_object.h"
#include "sound_player.h"
#include "ai/stalker/ai_stalker_space.h"
#include "stalker_decision_space.h"
#include "agent_manager.h"
#include "agent_enemy_manager.h"

const u32 MIN_QUEUE		= 0;
const u32 MAX_QUEUE		= 1;
const u32 MIN_INTERVAL	= 1000;
const u32 MAX_INTERVAL	= 1500;

using namespace StalkerSpace;
using namespace StalkerDecisionSpace;

CInventoryItem *weapon_to_kill(const CAI_Stalker *object)
{
	if (!object->inventory().m_slots[1].m_pIItem)
		return			(object->best_weapon());

	CWeaponMagazined	*temp = smart_cast<CWeaponMagazined*>(object->inventory().m_slots[1].m_pIItem);
	if (!temp)
		return			(object->best_weapon());

	if (!temp->can_kill())
		return			(object->best_weapon());

	return				(temp);
}

bool should_process	(CAI_Stalker &object, const CEntityAlive *enemy)
{
	if (object.agent_manager().enemy().wounded_processed(enemy))
		return			(false);

	ALife::_OBJECT_ID	processor_id = object.agent_manager().enemy().wounded_processor(enemy);
	if ((processor_id != ALife::_OBJECT_ID(-1)) && (processor_id != object.ID()))
		return			(false);

	return				(true);
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionReachWounded
//////////////////////////////////////////////////////////////////////////

CStalkerActionReachWounded::CStalkerActionReachWounded		(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object, action_name)
{
}

void CStalkerActionReachWounded::initialize					()
{
	inherited::initialize	();

	m_storage->set_property						(eWorldPropertyWoundedEnemyPrepared,false);
	m_storage->set_property						(eWorldPropertyWoundedEnemyAimed,false);
	object().movement().set_desired_direction	(0);
	object().movement().set_path_type			(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_body_state			(eBodyStateStand);
	object().movement().set_mental_state		(eMentalStateFree);
	object().sight().setup						(CSightAction(SightManager::eSightTypePathDirection,false));
	object().movement().set_movement_type		(eMovementTypeWalk);
	object().set_goal							(eObjectActionIdle,weapon_to_kill(&object()),MIN_QUEUE,MAX_QUEUE,MIN_INTERVAL,MAX_INTERVAL);
}

void CStalkerActionReachWounded::finalize					()
{
	inherited::finalize		();
//	object().movement().set_desired_position	(0);
}

void CStalkerActionReachWounded::execute					()
{
	inherited::execute		();

	if (!object().memory().enemy().selected())
		return;

	const CEntityAlive						*enemy = object().memory().enemy().selected();
	CMemoryInfo								mem_object = object().memory().memory(enemy);

	if (!mem_object.m_object)
		return;


	if (object().movement().accessible(mem_object.m_object_params.m_level_vertex_id))
		object().movement().set_level_dest_vertex			(mem_object.m_object_params.m_level_vertex_id);
	else
		object().movement().set_nearest_accessible_position	(ai().level_graph().vertex_position(mem_object.m_object_params.m_level_vertex_id),mem_object.m_object_params.m_level_vertex_id);

	if (should_process(object(),enemy)) {
		object().movement().set_movement_type	(eMovementTypeWalk);
		return;
	}

	if (object().agent_manager().enemy().wounded_processed(enemy)) {
		object().movement().set_movement_type	(eMovementTypeStand);
		return;
	}

	ALife::_OBJECT_ID					processor_id = object().agent_manager().enemy().wounded_processor(enemy);
	if (!((processor_id != ALife::_OBJECT_ID(-1)) && (processor_id != object().ID()))) {
		object().movement().set_movement_type	(eMovementTypeStand);
		return;
	}

	CObject								*processor = Level().Objects.net_Find(processor_id);
	VERIFY								(processor);
	if (processor->Position().distance_to(object().Position()) < 2.f) {
		object().movement().set_movement_type	(eMovementTypeStand);
		return;
	}

	object().movement().set_movement_type		(eMovementTypeWalk);
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionAimWounded
//////////////////////////////////////////////////////////////////////////

CStalkerActionAimWounded::CStalkerActionAimWounded	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object, action_name)
{
}

void CStalkerActionAimWounded::initialize				()
{
	inherited::initialize	();

	object().movement().set_desired_direction	(0);
	object().movement().set_path_type			(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_mental_state		(eMentalStateDanger);
	object().movement().set_body_state			(eBodyStateStand);
	object().movement().set_movement_type		(eMovementTypeStand);
	object().CObjectHandler::set_goal			(eObjectActionAimReady1,weapon_to_kill(&object()),MIN_QUEUE,MAX_QUEUE,MIN_INTERVAL,MAX_INTERVAL);

	const CEntityAlive							*enemy = object().memory().enemy().selected();
	object().sight().setup						(CSightAction(enemy,true));
	object().agent_manager().enemy().wounded_processed(enemy,true);

//	m_speed									= object().movement().m_head.speed;
//	object().movement().danger_head_speed	(PI_DIV_4);
}

void CStalkerActionAimWounded::execute					()
{
	inherited::execute		();

	if (first_time())
		return;

	if (!completed())
		return;

	if (!should_process(object(),object().memory().enemy().selected()))
		return;

	const SBoneRotation		&head = object().movement().m_head;
	if (!fsimilar(head.current.yaw,head.target.yaw))
		return;

	if (!fsimilar(head.current.pitch,head.target.pitch))
		return;

	m_storage->set_property	(eWorldPropertyWoundedEnemyAimed,true);
}

void CStalkerActionAimWounded::finalize					()
{
	inherited::finalize		();

//	object().movement().danger_head_speed	(m_speed);
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionPrepareWounded
//////////////////////////////////////////////////////////////////////////

CStalkerActionPrepareWounded::CStalkerActionPrepareWounded	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object, action_name)
{
}

void CStalkerActionPrepareWounded::initialize				()
{
	inherited::initialize	();

	object().movement().set_desired_direction	(0);
	object().movement().set_path_type			(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_mental_state		(eMentalStateDanger);
	object().movement().set_body_state			(eBodyStateStand);
	object().movement().set_movement_type		(eMovementTypeStand);
	object().sound().play						(eStalkerSoundKillWounded);
	object().CObjectHandler::set_goal			(eObjectActionAimReady1,weapon_to_kill(&object()),MIN_QUEUE,MAX_QUEUE,MIN_INTERVAL,MAX_INTERVAL);
}

void CStalkerActionPrepareWounded::finalize					()
{
	inherited::finalize		();

	object().sound().set_sound_mask				(0);
}

void CStalkerActionPrepareWounded::execute					()
{
	inherited::execute		();

	if (!object().memory().enemy().selected())
		return;

	if (!should_process(object(),object().memory().enemy().selected())) {
		object().sound().set_sound_mask	((u32)eStalkerSoundMaskKillWounded);
		return;
	}

	const CEntityAlive		*enemy = object().memory().enemy().selected();

	if (object().agent_manager().enemy().wounded_processor(enemy) != object().ID())
		return;

	VERIFY					(object().memory().visual().visible_now(enemy));
	object().sight().setup	(CSightAction(enemy,true));

	if (!object().sound().active_sound_count(true))
		m_storage->set_property					(eWorldPropertyWoundedEnemyPrepared,true);
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionKillWounded
//////////////////////////////////////////////////////////////////////////

CStalkerActionKillWounded::CStalkerActionKillWounded		(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object, action_name)
{
}

void CStalkerActionKillWounded::initialize					()
{
	inherited::initialize	();

	m_storage->set_property						(eWorldPropertyPausedAfterKill,true);
	object().movement().set_desired_direction	(0);
	object().movement().set_path_type			(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_mental_state		(eMentalStateDanger);
	object().movement().set_body_state			(eBodyStateStand);
	object().movement().set_movement_type		(eMovementTypeStand);
}

void CStalkerActionKillWounded::finalize					()
{
	inherited::finalize		();
}

void CStalkerActionKillWounded::execute					()
{
	inherited::execute		();

	if (!object().memory().enemy().selected())
		return;

	const CEntityAlive		*enemy = object().memory().enemy().selected();
	VERIFY					(object().memory().visual().visible_now(enemy));
	object().sight().setup	(CSightAction(enemy,true));
	object().set_goal		(eObjectActionFire1,weapon_to_kill(&object()),MIN_QUEUE,MAX_QUEUE,MIN_INTERVAL,MAX_INTERVAL);
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionPauseAfterKill
//////////////////////////////////////////////////////////////////////////

CStalkerActionPauseAfterKill::CStalkerActionPauseAfterKill	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object, action_name)
{
}

void CStalkerActionPauseAfterKill::initialize	()
{
	inherited::initialize	();

	object().movement().set_desired_direction	(0);
	object().movement().set_path_type			(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_mental_state		(eMentalStateDanger);
	object().movement().set_body_state			(eBodyStateStand);
	object().movement().set_movement_type		(eMovementTypeStand);
	object().CObjectHandler::set_goal			(eObjectActionAimReady1,weapon_to_kill(&object()),MIN_QUEUE,MAX_QUEUE,MIN_INTERVAL,MAX_INTERVAL);
	object().sight().setup						(CSightAction(SightManager::eSightTypeCurrentDirection,true));
}

void CStalkerActionPauseAfterKill::execute		()
{
	inherited::execute		();

	if (!completed())
		return;

	m_storage->set_property	(eWorldPropertyPausedAfterKill,false);
}
