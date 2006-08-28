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
const u32 MAX_INTERVAL	= 1200;

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

	if (object().agent_manager().enemy().wounded_processed(enemy)) {
		object().movement().set_movement_type	(eMovementTypeStand);
		return;
	}
	else
		object().movement().set_movement_type	(eMovementTypeWalk);

	if (object().movement().accessible(mem_object.m_object_params.m_level_vertex_id))
		object().movement().set_level_dest_vertex			(mem_object.m_object_params.m_level_vertex_id);
	else
		object().movement().set_nearest_accessible_position	(ai().level_graph().vertex_position(mem_object.m_object_params.m_level_vertex_id),mem_object.m_object_params.m_level_vertex_id);
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
	object().movement().set_body_state			(eBodyStateCrouch);
	object().movement().set_movement_type		(eMovementTypeStand);
	object().sound().play						(eStalkerSoundKillWounded);
	object().CObjectHandler::set_goal			(eObjectActionAimReady1,weapon_to_kill(&object()),MIN_QUEUE,MAX_QUEUE,MIN_INTERVAL,MAX_INTERVAL);

	const CEntityAlive							*enemy = object().memory().enemy().selected();
	VERIFY										(enemy);

//	object().agent_manager().enemy().wounded_processed(enemy,true);
	if (object().agent_manager().enemy().wounded_processor(enemy) == ALife::_OBJECT_ID(-1))
		object().agent_manager().enemy().wounded_processor	(enemy,object().ID());
}

void CStalkerActionPrepareWounded::finalize					()
{
	inherited::finalize		();
}

void CStalkerActionPrepareWounded::execute					()
{
	inherited::execute		();

	if (!object().memory().enemy().selected())
		return;

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

	object().movement().set_desired_direction	(0);
	object().movement().set_path_type			(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_mental_state		(eMentalStateDanger);
	object().movement().set_body_state			(eBodyStateCrouch);
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
