////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_alife_actions.cpp
//	Created 	: 25.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker alife action classes
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_alife_actions.h"
#include "ai/stalker/ai_stalker.h"
#include "inventory_item.h"
#include "script_game_object.h"
#include "inventory.h"
#include "weaponmagazined.h"
#include "movement_manager_space.h"
#include "detail_path_manager_space.h"
#include "memory_manager.h"
#include "item_manager.h"
#include "sight_manager.h"
#include "ai_object_location.h"
#include "stalker_movement_manager.h"
#include "sound_player.h"

#ifdef _DEBUG
//#	define STALKER_DEBUG_MODE
#endif

#ifdef STALKER_DEBUG_MODE
#	include "attachable_item.h"
#endif

//////////////////////////////////////////////////////////////////////////
// CStalkerActionGatherItems
//////////////////////////////////////////////////////////////////////////

CStalkerActionGatherItems::CStalkerActionGatherItems	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionGatherItems::initialize	()
{
	inherited::initialize			();
	object().movement().set_node_evaluator	(0);
	object().movement().set_path_evaluator	(0);
	object().movement().set_desired_direction	(0);
	object().movement().set_path_type			(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_body_state		(eBodyStateStand);
	object().movement().set_movement_type		(eMovementTypeWalk);
	object().movement().set_mental_state		(eMentalStateDanger);
	object().sound().remove_active_sounds	(u32(eStalkerSoundMaskNoHumming));
	if (!object().inventory().ActiveItem())
		object().CObjectHandler::set_goal	(eObjectActionIdle);
	else
		object().CObjectHandler::set_goal	(eObjectActionIdle,object().inventory().ActiveItem());
}

void CStalkerActionGatherItems::finalize	()
{
	inherited::finalize		();

	if (!object().g_Alive())
		return;

	object().sound().set_sound_mask(0);
}

void CStalkerActionGatherItems::execute		()
{
	inherited::execute		();

	if (!object().memory().item().selected())
		return;

	object().movement().set_level_dest_vertex	(object().memory().item().selected()->ai_location().level_vertex_id());
	object().movement().set_desired_position	(&object().memory().item().selected()->Position());
	object().sight().setup			(SightManager::eSightTypePosition,&object().memory().item().selected()->Position());
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionNoALife
//////////////////////////////////////////////////////////////////////////

CStalkerActionNoALife::CStalkerActionNoALife	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionNoALife::initialize	()
{
	inherited::initialize			();
#ifndef STALKER_DEBUG_MODE
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
	
	m_stop_weapon_handling_time		= Level().timeServer();
	if (object().inventory().ActiveItem() && object().best_weapon() && (object().inventory().ActiveItem()->object().ID() == object().best_weapon()->object().ID()))
		m_stop_weapon_handling_time	+= ::Random32.random(30000) + 30000;
#else
//	object().CObjectHandler::set_goal	(eObjectActionAimReady1,object().best_weapon());
	object().sight().setup			(CSightAction(SightManager::eSightTypeCurrentDirection));
	object().movement().set_node_evaluator	(0);
	object().movement().set_path_evaluator	(0);
	object().movement().set_desired_direction	(0);
	object().movement().set_path_type			(MovementManager::ePathTypeLevelPath);
	object().movement().set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	object().movement().set_level_dest_vertex	(object().ai_location().level_vertex_id());
	object().movement().set_desired_position	(&object().Position());
//	Fvector							direction = Fvector().set(0.f,0.f,1.f);//Fvector().set(::Random.randF(1.f),0.f,::Random.randF(1.f));
//	direction.normalize_safe		();
//	object().movement().set_desired_direction	(&direction);
//	object().movement().set_desired_position	(0);
//	object().movement().set_path_type			(MovementManager::ePathTypePatrolPath);
//	object().set_path				("way0000",PatrolPathManager::ePatrolStartTypeNearest,PatrolPathManager::ePatrolRouteTypeContinue,false);

//	CGameObject						*actor = smart_cast<CGameObject*>(Level().CurrentEntity());
//	object().movement().set_desired_position	(&actor->Position());
//	object().movement().set_level_dest_vertex	(actor->ai_location().level_vertex_id());
//	object().movement().set_path_type			(MovementManager::ePathTypeLevelPath);
//	Fvector							look_pos = Fvector().set(0.f,0.f,1.f);//actor->Position();
//	look_pos.y						+= .8f;
//	object().sight().setup			(CSightAction(SightManager::eSightTypePosition,look_pos,true));

//	object().movement().set_detail_path_type	(eDetailPathTypeSmooth);
	object().movement().set_body_state		(eBodyStateStand);
	object().movement().set_movement_type		(eMovementTypeStand);
	object().movement().set_mental_state		(eMentalStateFree);

//	object().CObjectHandler::set_goal	(eObjectActionUse,object().inventory().GetItemFromInventory("bread"));
//	smart_cast<CAttachableItem*>(object().inventory().GetItemFromInventory("hand_radio"))->enable(false);
//	CGameObject						*actor = smart_cast<CGameObject*>(Level().CurrentEntity());
//	object().sight().setup			(CSightAction(actor,true));
//	Fvector2						start_position  = Fvector2().set(-37.100006f,-42.700001f);
//	Fvector2						finish_position = Fvector2().set(-36.750004f,-45.850002f);
//	u32								start_vertex_id = 34013;
//	ai().level_graph().check_position_in_direction_slow(start_vertex_id,start_position,finish_position);
#endif
}

void CStalkerActionNoALife::finalize	()
{
	inherited::finalize				();

	if (!object().g_Alive())
		return;

	object().sound().remove_active_sounds	(u32(eStalkerSoundMaskNoHumming));
}

IC	u32 get_value(u32 diff)
{
	return	((diff / 5000) % 6);
}

void CStalkerActionNoALife::execute		()
{
	inherited::execute				();
#ifndef STALKER_DEBUG_MODE
	object().sound().play			(eStalkerSoundHumming,60000,10000);
	if (Level().timeServer() >= m_stop_weapon_handling_time)
		if (!object().best_weapon())
			object().CObjectHandler::set_goal	(eObjectActionIdle);
		else
			object().CObjectHandler::set_goal	(eObjectActionStrapped,object().best_weapon());
	else
		object().CObjectHandler::set_goal		(eObjectActionIdle,object().best_weapon());
#else
//	CGameObject						*actor = smart_cast<CGameObject*>(Level().CurrentEntity());
//	Fvector							dest_position = actor->Position();
//	u32								dest_vertex = actor->ai_location().level_vertex_id();
//	if (!ai().level_graph().inside(dest_vertex,dest_position))d
//		dest_position				= ai().level_graph().vertex_position(dest_vertex);
//
//	if (object().movement().restrictions().accessible(dest_position)) {
//		object().movement().set_desired_position	(&dest_position);
//		object().movement().set_level_dest_vertex (dest_vertex);
//		return;
//	}
//	dest_vertex						= object().accessible_nearest(Fvector(dest_position),dest_position);
//	object().movement().set_desired_position	(&dest_position);
//	object().movement().set_level_dest_vertex (dest_vertex);
//	Fvector							look_pos = actor->Position();
//	look_pos.y						+= .8f;
//	object().sight().setup			(CSightAction(SightManager::eSightTypePosition,look_pos,true));
//	object().play					(eStalkerSoundAttack,10000);

//	CWeaponMagazined					*weapon = smart_cast<CWeaponMagazined*>(object().best_weapon());
//	VERIFY								(weapon);
//	weapon->SetQueueSize				(3);
//	object().CObjectHandler::set_goal	(eObjectActionFire1,object().best_weapon(),5,2000);
//	Fvector2						start_position, finish_position;
//	start_position.set				(-123.55067,-4.5493350);
//	finish_position.set				(-102.20000,-20.300003);
//	ai().level_graph().check_position_in_direction_slow(8053,start_position,finish_position);
//	object().CObjectHandler::set_goal	(eObjectActionStrapped,object().best_weapon());
//	return;

	u32		time_diff = Device.dwTimeGlobal - m_start_level_time;
	const u32 diff = 300;
	u32		iter = get_value(time_diff);
	switch (iter) {
		case 0 : {
			if (get_value(time_diff-diff) != 0)
				Msg	("IDLE");
			object().CObjectHandler::set_goal	(eObjectActionIdle,object().best_weapon());
			break;
		}
		case 1 : {
			if (get_value(time_diff-diff) != 1)
				Msg	("STRAPPED");
			object().CObjectHandler::set_goal	(eObjectActionStrapped,object().best_weapon());
			break;
		}
		case 2 : {
			if (get_value(time_diff-diff) != 2)
				Msg	("AIM");
			object().CObjectHandler::set_goal	(eObjectActionFire1,object().inventory().m_slots[5].m_pIItem);
			break;
		}
		case 3 : {
			if (get_value(time_diff-diff) != 3)
				Msg	("STRAPPED");
			object().CObjectHandler::set_goal	(eObjectActionStrapped,object().best_weapon());
			break;
		}
		case 4 : {
			if (get_value(time_diff-diff) != 4)
				Msg	("AIM");
			object().CObjectHandler::set_goal	(eObjectActionFire1,object().inventory().m_slots[5].m_pIItem);
			break;
		}
		case 5 : {
			if (get_value(time_diff-diff) != 5)
				Msg	("STRAPPED");
			object().CObjectHandler::set_goal	(eObjectActionStrapped,object().best_weapon());
			break;
		}
	}
#endif
}
