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
	inherited::initialize	();
	m_object->set_sound_mask(u32(eStalkerSoundMaskNoHumming));
}

void CStalkerActionGatherItems::finalize	()
{
	inherited::finalize		();

	if (!m_object->g_Alive())
		return;

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
	m_object->set_path_type			(MovementManager::ePathTypeLevelPath);
	m_object->set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	m_object->set_body_state		(eBodyStateStand);
	m_object->set_movement_type		(eMovementTypeWalk);
	m_object->set_mental_state		(eMentalStateDanger);

	m_object->setup					(SightManager::eSightTypePosition,&m_object->item()->Position());
	m_object->CObjectHandler::set_goal		(eObjectActionIdle);
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
#else
//	m_object->CObjectHandler::set_goal	(eObjectActionAimReady1,m_object->best_weapon());
	m_object->CSightManager::setup	(CSightAction(SightManager::eSightTypeCurrentDirection));
	m_object->set_node_evaluator	(0);
	m_object->set_path_evaluator	(0);
	m_object->set_desired_direction	(0);
	m_object->set_path_type			(MovementManager::ePathTypeLevelPath);
	m_object->set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	m_object->set_level_dest_vertex	(m_object->level_vertex_id());
	m_object->set_desired_position	(&m_object->Position());
//	Fvector							direction = Fvector().set(0.f,0.f,1.f);//Fvector().set(::Random.randF(1.f),0.f,::Random.randF(1.f));
//	direction.normalize_safe		();
//	m_object->set_desired_direction	(&direction);
//	m_object->set_desired_position	(0);
//	m_object->set_path_type			(MovementManager::ePathTypePatrolPath);
//	m_object->set_path				("way0000",PatrolPathManager::ePatrolStartTypeNearest,PatrolPathManager::ePatrolRouteTypeContinue,false);

//	CGameObject						*actor = smart_cast<CGameObject*>(Level().CurrentEntity());
//	m_object->set_desired_position	(&actor->Position());
//	m_object->set_level_dest_vertex	(actor->level_vertex_id());
//	m_object->set_path_type			(MovementManager::ePathTypeLevelPath);
//	Fvector							look_pos = Fvector().set(0.f,0.f,1.f);//actor->Position();
//	look_pos.y						+= .8f;
//	m_object->CSightManager::setup	(CSightAction(SightManager::eSightTypePosition,look_pos,true));

//	m_object->set_detail_path_type	(eDetailPathTypeSmooth);
	m_object->set_body_state		(eBodyStateStand);
	m_object->set_movement_type		(eMovementTypeStand);
	m_object->set_mental_state		(eMentalStateDanger);
//	m_object->CObjectHandler::set_goal	(eObjectActionUse,m_object->inventory().GetItemFromInventory("bread"));
//	smart_cast<CAttachableItem*>(m_object->inventory().GetItemFromInventory("hand_radio"))->enable(false);
//	CGameObject						*actor = smart_cast<CGameObject*>(Level().CurrentEntity());
//	m_object->CSightManager::setup	(CSightAction(actor,true));
	Fvector2						start_position  = Fvector2().set(-37.100006f,-42.700001f);
	Fvector2						finish_position = Fvector2().set(-36.750004f,-45.850002f);
	u32								start_vertex_id = 34013;
	ai().level_graph().check_position_in_direction_slow(start_vertex_id,start_position,finish_position);
#endif
}

void CStalkerActionNoALife::finalize	()
{
	inherited::finalize				();

	if (!m_object->g_Alive())
		return;

	m_object->set_sound_mask		(u32(eStalkerSoundMaskNoHumming));
	m_object->set_sound_mask		(0);
}

void CStalkerActionNoALife::execute		()
{
	inherited::execute				();
#ifndef STALKER_DEBUG_MODE
	m_object->play					(eStalkerSoundHumming,60000,10000);
	if (Level().timeServer() >= m_stop_weapon_handling_time)
		m_object->CObjectHandler::set_goal	(eObjectActionIdle);
	else
		m_object->CObjectHandler::set_goal	(eObjectActionIdle,m_object->best_weapon());
#else
//	CGameObject						*actor = smart_cast<CGameObject*>(Level().CurrentEntity());
//	Fvector							dest_position = actor->Position();
//	u32								dest_vertex = actor->level_vertex_id();
//	if (!ai().level_graph().inside(dest_vertex,dest_position))
//		dest_position				= ai().level_graph().vertex_position(dest_vertex);
//
//	if (m_object->accessible(dest_position)) {
//		m_object->set_desired_position	(&dest_position);
//		m_object->set_level_dest_vertex (dest_vertex);
//		return;
//	}
//	dest_vertex						= m_object->accessible_nearest(Fvector(dest_position),dest_position);
//	m_object->set_desired_position	(&dest_position);
//	m_object->set_level_dest_vertex (dest_vertex);
//	Fvector							look_pos = actor->Position();
//	look_pos.y						+= .8f;
//	m_object->CSightManager::setup	(CSightAction(SightManager::eSightTypePosition,look_pos,true));
//	m_object->play					(eStalkerSoundAttack,10000);

//	CWeaponMagazined					*weapon = smart_cast<CWeaponMagazined*>(m_object->best_weapon());
//	VERIFY								(weapon);
//	weapon->SetQueueSize				(3);
//	m_object->CObjectHandler::set_goal	(eObjectActionFire1,m_object->best_weapon(),5,2000);
	Fvector2						start_position, finish_position;
	start_position.set				(-123.55067,-4.5493350);
	finish_position.set				(-102.20000,-20.300003);
	ai().level_graph().check_position_in_direction_slow(8053,start_position,finish_position);
#endif
}

