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

#ifdef _DEBUG
//#	define STALKER_DEBUG_MODE
#endif

//////////////////////////////////////////////////////////////////////////
// CStalkerActionFreeNoALife
//////////////////////////////////////////////////////////////////////////

CStalkerActionFreeNoALife::CStalkerActionFreeNoALife	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionFreeNoALife::initialize	()
{
	inherited::initialize			();
	m_stop_weapon_handling_time		= Level().timeServer();

	if (m_object->inventory().ActiveItem() && m_object->best_weapon() && (m_object->inventory().ActiveItem()->ID() == m_object->best_weapon()->ID()))
		m_stop_weapon_handling_time	+= ::Random.randI(120000,180000);

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
	m_object->CSightManager::setup	(CSightAction(SightManager::eSightTypeCover,false,true));
	m_object->set_node_evaluator	(0);
	m_object->set_path_evaluator	(0);
	m_object->set_desired_direction	(0);
//	m_object->set_desired_position	(0);

	m_object->CObjectHandler::set_goal	(eObjectActionIdle);

//	Fvector							direction = Fvector().set(0.f,0.f,1.f);//Fvector().set(::Random.randF(1.f),0.f,::Random.randF(1.f));
//	direction.normalize_safe		();
//	m_object->set_desired_direction	(&direction);
//	m_object->set_desired_position	(0);
	m_object->set_path_type			(MovementManager::ePathTypePatrolPath);
	m_object->set_path				("way0000",PatrolPathManager::ePatrolStartTypeNearest,PatrolPathManager::ePatrolRouteTypeContinue,false);

//	CGameObject						*actor = smart_cast<CGameObject*>(Level().CurrentEntity());
//	m_object->set_desired_position	(&actor->Position());
//	m_object->set_level_dest_vertex	(actor->level_vertex_id());
//	m_object->set_path_type			(MovementManager::ePathTypeLevelPath);
//	Fvector							look_pos = Fvector().set(0.f,0.f,1.f);//actor->Position();
//	look_pos.y						+= .8f;
//	m_object->CSightManager::setup	(CSightAction(SightManager::eSightTypePosition,look_pos,true));

	m_object->set_detail_path_type	(eDetailPathTypeSmooth);
	m_object->set_body_state		(eBodyStateStand);
	m_object->set_movement_type		(eMovementTypeWalk);
	m_object->set_mental_state		(eMentalStateDanger);
//	m_object->CObjectHandler::set_goal	(eObjectActionUse,m_object->inventory().GetItemFromInventory("bread"));
//	smart_cast<CAttachableItem*>(m_object->inventory().GetItemFromInventory("hand_radio"))->enable(false);
//	CGameObject						*actor = smart_cast<CGameObject*>(Level().CurrentEntity());
//	m_object->CSightManager::setup	(CSightAction(actor,true));
#endif
}

void CStalkerActionFreeNoALife::finalize	()
{
	inherited::finalize				();

	if (!m_object->g_Alive())
		return;

	m_object->set_sound_mask		(u32(eStalkerSoundMaskNoHumming));
	m_object->set_sound_mask		(0);
}

void CStalkerActionFreeNoALife::execute		()
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

