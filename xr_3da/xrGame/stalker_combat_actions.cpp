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
#include "agent_manager.h"
#include "cover_evaluators.h"
#include "cover_point.h"
#include "cover_manager.h"
#include "missile.h"

using namespace StalkerDecisionSpace;

#ifdef DEBUG
extern EStalkerBehaviour	g_stalker_behaviour;
#endif

typedef CStalkerActionBase::_edge_value_type _edge_value_type;

struct CMovementRestrictor {
	CAI_Stalker			*m_object;
	const CAgentManager	*m_agent_manager;

	IC					CMovementRestrictor	(CAI_Stalker *object)
	{
		m_object		= object;
		m_agent_manager	= &object->agent_manager();
	}

	IC		bool		operator()			(CCoverPoint *cover) const
	{
		return			(m_agent_manager->suitable_location(m_object,cover));
	}
};

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
	m_object->set_sound_mask(u32(eStalkerSoundMaskNoHumming));
}

void CStalkerActionGetKillDistance::finalize	()
{
	inherited::finalize		();

	if (!m_object->g_Alive())
		return;

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
	m_object->set_path_type			(MovementManager::ePathTypeLevelPath);
	m_object->set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	m_object->set_body_state		(eBodyStateStand);
	m_object->set_movement_type		(eMovementTypeWalk);
	m_object->set_mental_state		(eMentalStateDanger);

	m_object->CSightManager::setup				(SightManager::eSightTypeCurrentDirection);
	m_object->CObjectHandler::set_goal		(eObjectActionIdle);
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
	m_object->set_sound_mask(u32(eStalkerSoundMaskNoHumming));
}

void CStalkerActionGetEnemySeen::finalize	()
{
	inherited::finalize		();
	if (m_object->enemy() && m_object->visible_now(m_object->enemy()))
		m_storage->set_property		(eWorldPropertyEnemyAimed,true);

	if (!m_object->g_Alive())
		return;

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

	if (m_object->accessible(mem_object.m_object_params.m_position)) {
		m_object->set_level_dest_vertex	(mem_object.m_object_params.m_level_vertex_id);
		m_object->set_desired_position	(&mem_object.m_object_params.m_position);
	}
	else {
		Fvector desired_position;
		u32	level_vertex_id = m_object->accessible_nearest(mem_object.m_object_params.m_position,desired_position);
		m_object->set_level_dest_vertex	(level_vertex_id);
		m_object->set_desired_position	(&desired_position);
	}
	m_object->set_path_type			(MovementManager::ePathTypeLevelPath);
	m_object->set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	m_object->set_body_state		(eBodyStateStand);
	m_object->set_movement_type		(eMovementTypeWalk);
	m_object->set_mental_state		(eMentalStateDanger);

	m_object->setup					(CSightAction(SightManager::eSightTypePosition,mem_object.m_object_params.m_position,true));

	if (!smart_cast<CMissile*>(m_object->best_weapon()))
		m_object->CObjectHandler::set_goal		(eObjectActionAimReady1,m_object->best_weapon());

	if (ai().level_graph().inside(mem_object.m_object_params.m_level_vertex_id,mem_object.m_object_params.m_position)) {
		if (m_object->Position().distance_to_xz(mem_object.m_object_params.m_position) <=.5f)
			m_object->CMemoryManager::enable	(m_object->enemy(),false);
	}
	else
		if ((mem_object.m_object_params.m_level_vertex_id == m_object->level_vertex_id()) || 
			 (m_object->Position().distance_to_xz(ai().level_graph().vertex_position(mem_object.m_object_params.m_level_vertex_id)) < ai().level_graph().header().cell_size() * 1.5f))
				m_object->CMemoryManager::enable	(m_object->enemy(),false);
}

_edge_value_type CStalkerActionGetEnemySeen::weight	(const CSConditionState &condition0, const CSConditionState &condition1) const
{
#ifdef DEBUG
	if ((g_stalker_behaviour == eStalkerBehaviourVeryAggressive) || (g_stalker_behaviour == eStalkerBehaviourAggressive))
		return				(_edge_value_type(1));
	else
#endif
		return				(_edge_value_type(100));
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
	m_object->set_sound_mask(u32(eStalkerSoundMaskNoHumming));
//	m_object->CSightManager::clear();
//	m_object->CSightManager::add_action(eSightActionTypeWatchItem,xr_new<CSightControlAction>(1.f,3000,CSightAction(SightManager::eSightTypePathDirection)));//,m_object->m_best_found_item_to_kill->Position(),false)));
//	m_object->CSightManager::add_action(eSightActionTypeWatchEnemy,xr_new<CSightControlAction>(1.f,3000,CSightAction(SightManager::eSightTypePosition,m_object->enemy()->Position(),false)));
	m_object->CSightManager::setup(CSightAction(m_object->m_best_found_item_to_kill,true));//,m_object->m_best_found_item_to_kill->Position(),false)));
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
//	m_object->setup					(SightManager::eSightTypePosition,&m_object->m_best_found_item_to_kill->Position());
//	m_object->setup					(SightManager::eSightTypePosition,&m_object->enemy()->Position(),"bip01_head");
//	m_object->setup					(SightManager::eSightTypeSearch);

//	m_object->CSightManager::action(eSightActionTypeWatchItem).set_vector3d(m_object->m_best_found_item_to_kill->Position());
//	m_object->CSightManager::action(eSightActionTypeWatchEnemy).set_vector3d(m_object->enemy()->Position());
	m_object->set_goal				(eObjectActionIdle);
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
	m_object->set_sound_mask(u32(eStalkerSoundMaskNoHumming));
}

void CStalkerActionFindItemToKill::finalize	()
{
	inherited::finalize		();

	if (!m_object->g_Alive())
		return;

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
	m_object->set_path_type			(MovementManager::ePathTypeLevelPath);
	m_object->set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	m_object->set_body_state		(eBodyStateStand);
	m_object->set_movement_type		(eMovementTypeWalk);
	m_object->set_mental_state		(eMentalStateDanger);
	m_object->setup					(SightManager::eSightTypeCurrentDirection);
	m_object->set_goal				(eObjectActionIdle);
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
	m_object->set_sound_mask(u32(eStalkerSoundMaskNoHumming));
	m_object->CSightManager::clear();
	m_object->CSightManager::add_action(eSightActionTypeWatchItem,xr_new<CSightControlAction>(1.f,3000,CSightAction(SightManager::eSightTypePathDirection)));//,m_object->m_best_found_ammo->Position(),false)));
	m_object->CSightManager::add_action(eSightActionTypeWatchEnemy,xr_new<CSightControlAction>(1.f,3000,CSightAction(SightManager::eSightTypePosition,m_object->enemy()->Position(),false)));
}

void CStalkerActionMakeItemKilling::finalize	()
{
	inherited::finalize		();
	m_object->CSightManager::clear();

	if (!m_object->g_Alive())
		return;

	m_object->set_sound_mask(0);
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
	m_object->set_path_type			(MovementManager::ePathTypeLevelPath);
	m_object->set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	m_object->set_body_state		(eBodyStateStand);
	m_object->set_movement_type		(eMovementTypeWalk);
	m_object->set_mental_state		(eMentalStateDanger);
//	m_object->setup					(SightManager::eSightTypePosition,&m_object->m_best_found_ammo->Position());
//	m_object->setup					(SightManager::eSightTypePosition,&m_object->enemy()->Position(),"bip01_head");
//	m_object->setup					(SightManager::eSightTypeSearch);

//	m_object->CSightManager::action(eSightActionTypeWatchItem).set_vector3d(m_object->m_best_found_ammo->Position());
	m_object->CSightManager::action(eSightActionTypeWatchEnemy).set_vector3d(m_object->enemy()->Position());
	m_object->set_goal				(eObjectActionIdle);
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionFindAmmo
//////////////////////////////////////////////////////////////////////////

CStalkerActionFindAmmo::CStalkerActionFindAmmo	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
	m_weight				= _edge_value_type(100);
}

void CStalkerActionFindAmmo::initialize	()
{
	inherited::initialize	();
	m_object->set_sound_mask(u32(eStalkerSoundMaskNoHumming));
}

void CStalkerActionFindAmmo::finalize	()
{
	inherited::finalize		();

	if (!m_object->g_Alive())
		return;

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
	m_object->set_path_type			(MovementManager::ePathTypeLevelPath);
	m_object->set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	m_object->set_body_state		(eBodyStateStand);
	m_object->set_movement_type		(eMovementTypeWalk);
	m_object->set_mental_state		(eMentalStateDanger);

	m_object->CSightManager::setup				(SightManager::eSightTypeCurrentDirection);
	m_object->CObjectHandler::set_goal		(eObjectActionIdle);
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionGetReadyToKillVeryAggressive
//////////////////////////////////////////////////////////////////////////

CStalkerActionGetReadyToKillVeryAggressive::CStalkerActionGetReadyToKillVeryAggressive	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionGetReadyToKillVeryAggressive::initialize	()
{
	inherited::initialize	();
	m_object->set_sound_mask(u32(eStalkerSoundMaskNoHumming));
}

void CStalkerActionGetReadyToKillVeryAggressive::finalize	()
{
	inherited::finalize		();

	if (!m_object->g_Alive())
		return;

	m_object->set_sound_mask(0);
}

void CStalkerActionGetReadyToKillVeryAggressive::execute	()
{
	inherited::execute		();

	if (!m_object->m_best_item_to_kill)
		return;

	if (m_object->enemy() && m_object->Position().distance_to(m_object->enemy()->Position()) >= 10.f) {
		CMemoryInfo						mem_object = m_object->memory(m_object->enemy());
		if (m_object->accessible(mem_object.m_object_params.m_position)) {
			m_object->set_level_dest_vertex	(mem_object.m_object_params.m_level_vertex_id);
			m_object->set_desired_position	(&mem_object.m_object_params.m_position);
		}
		else {
			Fvector desired_position;
			u32	level_vertex_id = m_object->accessible_nearest(mem_object.m_object_params.m_position,desired_position);
			m_object->set_level_dest_vertex	(level_vertex_id);
			m_object->set_desired_position	(&desired_position);
		}
		m_object->set_movement_type		(eMovementTypeWalk);
	}
	else
		m_object->set_movement_type		(eMovementTypeStand);

	m_object->set_path_type			(MovementManager::ePathTypeLevelPath);
	m_object->set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	m_object->set_body_state		(eBodyStateStand);
	m_object->set_mental_state		(eMentalStateDanger);

	if (m_object->enemy())
		m_object->CSightManager::setup	(CSightAction(m_object->enemy(),true));
	else
		m_object->CSightManager::setup	(CSightAction(SightManager::eSightTypeCurrentDirection,true));

	if (!smart_cast<CMissile*>(m_object->best_weapon()))
		m_object->CObjectHandler::set_goal		(eObjectActionAimReady1,m_object->best_weapon());
}

_edge_value_type CStalkerActionGetReadyToKillVeryAggressive::weight	(const CSConditionState &condition0, const CSConditionState &condition1) const
{
#ifdef DEBUG
	if (g_stalker_behaviour == eStalkerBehaviourVeryAggressive)
		return				(_edge_value_type(1));
	else
#endif
		return				(_edge_value_type(100));
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionKillEnemyVeryAggressive
//////////////////////////////////////////////////////////////////////////

CStalkerActionKillEnemyVeryAggressive::CStalkerActionKillEnemyVeryAggressive	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionKillEnemyVeryAggressive::initialize	()
{
	inherited::initialize	();
	m_object->set_sound_mask(u32(eStalkerSoundMaskNoHumming));
}

void CStalkerActionKillEnemyVeryAggressive::finalize	()
{
	inherited::finalize		();

	if (!m_object->g_Alive())
		return;

	m_object->set_sound_mask(0);
}

void CStalkerActionKillEnemyVeryAggressive::execute		()
{
	inherited::execute		();

	if (!m_object->enemy())
		return;

	CMemoryInfo						mem_object = m_object->memory(m_object->enemy());

	if (!mem_object.m_object)
		return;

	if (!m_object->visible_now(m_object->enemy()))
		return;

	Fvector							position;
	m_object->enemy()->Center		(position);

	if (m_object->Position().distance_to(m_object->enemy()->Position()) >= 10.f) {
		if (m_object->accessible(mem_object.m_object_params.m_position)) {
			m_object->set_level_dest_vertex	(mem_object.m_object_params.m_level_vertex_id);
			m_object->set_desired_position	(&mem_object.m_object_params.m_position);
		}
		else {
			Fvector desired_position;
			u32	level_vertex_id = m_object->accessible_nearest(mem_object.m_object_params.m_position,desired_position);
			m_object->set_level_dest_vertex	(level_vertex_id);
			m_object->set_desired_position	(&desired_position);
		}
		m_object->set_movement_type		(eMovementTypeWalk);
	}
	else
		m_object->set_movement_type		(eMovementTypeStand);

	m_object->set_path_type			(MovementManager::ePathTypeLevelPath);
	m_object->set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	m_object->set_body_state		(eBodyStateStand);
	m_object->set_mental_state		(eMentalStateDanger);

	m_object->setup					(SightManager::eSightTypeFirePosition,&position);
	m_object->CObjectHandler::set_goal	(eObjectActionFire1,m_object->best_weapon());
}

_edge_value_type CStalkerActionKillEnemyVeryAggressive::weight	(const CSConditionState &condition0, const CSConditionState &condition1) const
{
#ifdef DEBUG
	if (g_stalker_behaviour == eStalkerBehaviourVeryAggressive)
		return				(_edge_value_type(1));
	else
#endif
		return				(_edge_value_type(100));
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionGetReadyToKillAggressive
//////////////////////////////////////////////////////////////////////////

CStalkerActionGetReadyToKillAggressive::CStalkerActionGetReadyToKillAggressive	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionGetReadyToKillAggressive::initialize	()
{
	set_inertia_time		(1000);
	inherited::initialize	();
	m_object->set_sound_mask(u32(eStalkerSoundMaskNoHumming));
}

void CStalkerActionGetReadyToKillAggressive::finalize	()
{
	inherited::finalize		();

	if (!m_object->g_Alive())
		return;

	m_object->set_sound_mask(0);
}

void CStalkerActionGetReadyToKillAggressive::execute	()
{
	inherited::execute		();

	if (!m_object->m_best_item_to_kill)
		return;

	CMemoryInfo						mem_object;
	
	if (m_object->enemy())
		mem_object					= m_object->memory(m_object->enemy());

	if (m_object->enemy() && (m_object->visible_now(mem_object.m_object) || !completed())) {
		Fvector						position = mem_object.m_object_params.m_position;
		m_object->m_ce_close->setup	(position,10.f,170.f,10.f);
		CCoverPoint					*point = ai().cover_manager().best_cover(m_object->Position(),10.f,*m_object->m_ce_close,CMovementRestrictor(m_object));
		if (!point)
			point					= ai().cover_manager().best_cover(m_object->Position(),30.f,*m_object->m_ce_close,CMovementRestrictor(m_object));

		if (point) {
			m_object->set_level_dest_vertex	(point->level_vertex_id());
			m_object->set_desired_position	(&point->position());
			m_object->set_movement_type		(eMovementTypeRun);
		}
		else
			m_object->set_movement_type		(eMovementTypeStand);

		m_object->CSightManager::setup		(SightManager::eSightTypeFirePosition,&position);
	}
	else {
		m_object->set_movement_type	(eMovementTypeStand);
		CMemoryInfo							mem_object = m_object->memory(m_object->enemy());
		if (mem_object.m_object)
			m_object->CSightManager::setup	(SightManager::eSightTypeCurrentDirection);
		else
			m_object->CSightManager::setup	(SightManager::eSightTypeCover);
	}

	m_object->set_desired_direction	(0);
	m_object->set_path_type			(MovementManager::ePathTypeLevelPath);
	m_object->set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	m_object->set_body_state		(eBodyStateStand);
	m_object->set_mental_state		(eMentalStateDanger);

	if (!smart_cast<CMissile*>(m_object->best_weapon()))
		m_object->CObjectHandler::set_goal		(eObjectActionAimReady1,m_object->best_weapon());
}

_edge_value_type CStalkerActionGetReadyToKillAggressive::weight	(const CSConditionState &condition0, const CSConditionState &condition1) const
{
#ifdef DEBUG
	if (g_stalker_behaviour == eStalkerBehaviourAggressive)
		return				(_edge_value_type(1));
	else
#endif
		return				(_edge_value_type(100));
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionKillEnemyAggressive
//////////////////////////////////////////////////////////////////////////

CStalkerActionKillEnemyAggressive::CStalkerActionKillEnemyAggressive	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionKillEnemyAggressive::initialize	()
{
	inherited::initialize	();
	m_object->set_sound_mask(u32(eStalkerSoundMaskNoHumming));

	float					distance = m_object->Position().distance_to(m_object->enemy()->Position());
	if (distance >= 30.f) {
        set_inertia_time	(500);
		m_fire_crouch		= true;
	}
	else
		if (distance <= 5.f) {
			set_inertia_time(1500);
			m_fire_crouch	= false;
		}
		else {
			set_inertia_time(iFloor(float(1500 - 500)*(30.f - distance)/(30.f - 5.f)) + 500);
			m_fire_crouch	= ::Random.randF(40.f) < (distance - 5.f);
		}

	m_fire_crouch = distance > 10.f ? true : false;
}

void CStalkerActionKillEnemyAggressive::finalize	()
{
	inherited::finalize		();
	VERIFY					(m_storage);
	m_storage->set_property	(eWorldPropertyEnemyAimed,false);

	if (!m_object->g_Alive())
		return;

	m_object->set_sound_mask(0);
}

void CStalkerActionKillEnemyAggressive::execute		()
{
	inherited::execute		();

	if (!m_object->enemy())
		return;

	CMemoryInfo						mem_object = m_object->memory(m_object->enemy());

	if (!mem_object.m_object)
		return;

	if (!m_object->visible_now(m_object->enemy()))
		return;

	if (completed()) {
		VERIFY					(m_storage);
		m_storage->set_property	(eWorldPropertyEnemyAimed,false);
		return;
	}

	Fvector							position;
	m_object->enemy()->Center		(position);

	m_object->set_movement_type		(eMovementTypeStand);
	m_object->set_path_type			(MovementManager::ePathTypeLevelPath);
	m_object->set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	m_object->set_body_state		(m_fire_crouch ? eBodyStateCrouch : eBodyStateStand);
	m_object->set_mental_state		(eMentalStateDanger);

	m_object->setup					(SightManager::eSightTypeFirePosition,&position);
	m_object->CObjectHandler::set_goal	(eObjectActionFire1,m_object->best_weapon());
}

_edge_value_type CStalkerActionKillEnemyAggressive::weight	(const CSConditionState &condition0, const CSConditionState &condition1) const
{
#ifdef DEBUG
	if (g_stalker_behaviour == eStalkerBehaviourAggressive)
		return				(_edge_value_type(1));
	else
#endif
		return				(_edge_value_type(100));
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionAimEnemy
//////////////////////////////////////////////////////////////////////////

CStalkerActionAimEnemy::CStalkerActionAimEnemy	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionAimEnemy::initialize	()
{
	inherited::initialize	();
	m_object->set_sound_mask(u32(eStalkerSoundMaskNoHumming));
	float					distance = m_object->Position().distance_to(m_object->enemy()->Position());
	if (m_object->enemy() && m_object->enemy()->human_being() && !m_storage->property(eWorldPropertyFireEnough)) {
		CMemoryInfo			mem_object = m_object->memory(m_object->enemy());
		if (mem_object.m_object && 
			m_object->visible_now(m_object->enemy()) && 
			(Level().timeServer() >= mem_object.m_last_level_time + 1000)) {
			if (m_object->visible_now(m_object->enemy()))
				m_object->play	(eStalkerSoundAttack);
			else
				m_object->play	(eStalkerSoundAlarm);
		}
	}
	if (distance >= 50.f) {
		set_inertia_time	(1500);
		m_run				= true;
	}
	else
		if (distance <= 10.f) {
			set_inertia_time(500);
			m_run			= false;
		}
		else {
			set_inertia_time(iFloor(float(1500 - 500)*(distance - 10.f)/(50.f - 10.f)) + 500);
			m_run			= ::Random.randF(40.f) > (distance - 10.f);
		}
}

void CStalkerActionAimEnemy::finalize	()
{
	inherited::finalize		();

	if (!m_object->g_Alive())
		return;

	m_object->set_sound_mask(0);
}

void CStalkerActionAimEnemy::execute		()
{
	inherited::execute				();

	if (completed()) {
		VERIFY						(m_storage);
		m_storage->set_property		(eWorldPropertyEnemyAimed,true);
		return;
	}

	if (!m_object->enemy())
		return;

	CMemoryInfo						mem_object = m_object->memory(m_object->enemy());

	if (!mem_object.m_object)
		return;

	if (!m_object->visible_now(m_object->enemy()))
		return;

	Fvector							position;
	m_object->enemy()->Center		(position);
//	float							distance = m_object->Position().distance_to(m_object->enemy()->Position());

//	if (m_object->Position().distance_to(m_object->enemy()->Position()) > 5.f) {
//		m_object->set_level_dest_vertex	(m_object->enemy()->level_vertex_id());
//		m_object->set_desired_position	(&m_object->enemy()->Position());
//		m_object->set_movement_type		(eMovementTypeStand);
//	}
//	else
		m_object->set_movement_type		(eMovementTypeStand);

//	m_object->set_path_type			(MovementManager::ePathTypeLevelPath);
//	m_object->set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
//	m_object->set_body_state		(distance > 10.f ? eBodyStateCrouch : eBodyStateStand);
	m_object->set_mental_state		(eMentalStateDanger);

	m_object->setup					(SightManager::eSightTypeFirePosition,&position);
	m_object->CObjectHandler::set_goal	(eObjectActionAim1,m_object->best_weapon());
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionGetReadyToKillAvoid
//////////////////////////////////////////////////////////////////////////

CStalkerActionGetReadyToKillAvoid::CStalkerActionGetReadyToKillAvoid	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionGetReadyToKillAvoid::initialize	()
{
	inherited::initialize	();
	m_object->set_sound_mask(u32(eStalkerSoundMaskNoHumming));
}

void CStalkerActionGetReadyToKillAvoid::finalize	()
{
	inherited::finalize		();

	if (!m_object->g_Alive())
		return;

	m_object->set_sound_mask(0);
}

void CStalkerActionGetReadyToKillAvoid::execute	()
{
	inherited::execute		();

	if (!m_object->m_best_item_to_kill)
		return;

	if (m_object->enemy()) {
		CMemoryInfo					mem_object = m_object->memory(m_object->enemy());
		Fvector						position = mem_object.m_object_params.m_position;
		m_object->m_ce_far->setup	(position,10.f,170.f);
		CCoverPoint					*point = ai().cover_manager().best_cover(m_object->Position(),30.f,*m_object->m_ce_far,CMovementRestrictor(m_object));
		if (!point)
			point					= ai().cover_manager().best_cover(m_object->Position(),50.f,*m_object->m_ce_far,CMovementRestrictor(m_object));

		if (point) {
			m_object->set_level_dest_vertex	(point->level_vertex_id());
			m_object->set_desired_position	(&point->position());
			m_object->set_movement_type		(eMovementTypeWalk);
		}
		else
			m_object->set_movement_type	(eMovementTypeStand);
	}
	else
		m_object->set_movement_type	(eMovementTypeStand);

//	m_object->set_level_dest_vertex	(m_object->level_vertex_id());
//	m_object->set_node_evaluator	(0);
//	m_object->set_path_evaluator	(0);
//	m_object->set_desired_position	(&m_object->Position());
	m_object->set_desired_direction	(0);
	m_object->set_path_type			(MovementManager::ePathTypeLevelPath);
	m_object->set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	m_object->set_body_state		(eBodyStateStand);
//	m_object->set_movement_type		(eMovementTypeWalk);
	m_object->set_mental_state		(eMentalStateDanger);

	m_object->CSightManager::setup				(SightManager::eSightTypeCurrentDirection);
	if (!smart_cast<CMissile*>(m_object->best_weapon()))
		m_object->CObjectHandler::set_goal		(eObjectActionAimReady1,m_object->best_weapon());
}

_edge_value_type CStalkerActionGetReadyToKillAvoid::weight	(const CSConditionState &condition0, const CSConditionState &condition1) const
{
#ifdef DEBUG
	if (g_stalker_behaviour == eStalkerBehaviourAvoiding)
		return				(_edge_value_type(1));
	else
#endif
		return				(_edge_value_type(100));
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionKillEnemyAvoid
//////////////////////////////////////////////////////////////////////////

CStalkerActionKillEnemyAvoid::CStalkerActionKillEnemyAvoid	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionKillEnemyAvoid::initialize	()
{
	inherited::initialize	();
	m_object->set_sound_mask(u32(eStalkerSoundMaskNoHumming));
}

void CStalkerActionKillEnemyAvoid::finalize	()
{
	inherited::finalize		();

	if (!m_object->g_Alive())
		return;

	m_object->set_sound_mask(0);
}

void CStalkerActionKillEnemyAvoid::execute		()
{
	inherited::execute		();

	if (!m_object->enemy())
		return;

	CMemoryInfo						mem_object = m_object->memory(m_object->enemy());

	if (!mem_object.m_object)
		return;

	if (!m_object->visible_now(m_object->enemy()))
		return;

	Fvector							position;
	m_object->enemy()->Center		(position);

//	if (m_object->Position().distance_to(m_object->enemy()->Position()) >= 10.f) {
//		m_object->set_level_dest_vertex	(mem_object.m_object_params.m_level_vertex_id);
//		m_object->set_desired_position	(&mem_object.m_object_params.m_position);
//		m_object->set_movement_type		(eMovementTypeWalk);
//	}
//	else
//		m_object->set_movement_type		(eMovementTypeStand);

	m_object->set_movement_type		(eMovementTypeStand);

	m_object->set_path_type			(MovementManager::ePathTypeLevelPath);
	m_object->set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	m_object->set_body_state		(eBodyStateStand);
	m_object->set_mental_state		(eMentalStateDanger);

	m_object->setup					(SightManager::eSightTypeFirePosition,&position);
	m_object->CObjectHandler::set_goal	(eObjectActionFire1,m_object->best_weapon());
}

_edge_value_type CStalkerActionKillEnemyAvoid::weight	(const CSConditionState &condition0, const CSConditionState &condition1) const
{
#ifdef DEBUG
	if (g_stalker_behaviour == eStalkerBehaviourAvoiding)
		return				(_edge_value_type(1));
	else
#endif
		return				(_edge_value_type(100));
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
	m_object->set_sound_mask(u32(eStalkerSoundMaskNoHumming));
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

	if (!m_object->visible_now(m_object->enemy()))
		return;

	Fvector							position;
	m_object->enemy()->Center		(position);

//	if (m_object->Position().distance_to(m_object->enemy()->Position()) >= 10.f) {
//		m_object->set_level_dest_vertex	(mem_object.m_object_params.m_level_vertex_id);
//		m_object->set_desired_position	(&mem_object.m_object_params.m_position);
//		m_object->set_movement_type		(eMovementTypeWalk);
//	}
//	else
//		m_object->set_movement_type		(eMovementTypeStand);

	m_object->set_movement_type		(eMovementTypeStand);

	m_object->set_path_type			(MovementManager::ePathTypeLevelPath);
	m_object->set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	m_object->set_body_state		(eBodyStateStand);
	m_object->set_mental_state		(eMentalStateDanger);

	m_object->setup					(SightManager::eSightTypeFirePosition,&position);
	m_object->CObjectHandler::set_goal	(eObjectActionFire1,m_object->best_weapon());
}

_edge_value_type CStalkerActionRetreatFromEnemy::weight	(const CSConditionState &condition0, const CSConditionState &condition1) const
{
#ifdef DEBUG
	if (g_stalker_behaviour == eStalkerBehaviourRetreat)
		return				(_edge_value_type(1));
	else
#endif
		return				(_edge_value_type(100));
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionCamp
//////////////////////////////////////////////////////////////////////////

CStalkerActionCamp::CStalkerActionCamp	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
	set_inertia_time		(30000);
}

void CStalkerActionCamp::initialize	()
{
	inherited::initialize	();
	m_object->set_sound_mask(u32(eStalkerSoundMaskNoHumming));
}

void CStalkerActionCamp::finalize	()
{
	inherited::finalize		();

	if (!m_object->g_Alive())
		return;

	m_object->set_sound_mask(0);
}

void CStalkerActionCamp::execute	()
{
	inherited::execute		();

	if (!m_object->enemy())
		return;

	CMemoryInfo						mem_object = m_object->memory(m_object->enemy());

	if (!mem_object.m_object)
		return;

	if (completed()) {
//		m_object->enable			(m_object->enemy(),false);
//		return;
	}

//	Fvector							position = mem_object.m_object_params.m_position;
	m_object->m_ce_best->setup	(mem_object.m_self_params.m_position,0.f,30.f);
	CCoverPoint					*point = ai().cover_manager().best_cover(mem_object.m_self_params.m_position,10.f,*m_object->m_ce_best,CMovementRestrictor(m_object));
	if (!point)
		point					= ai().cover_manager().best_cover(mem_object.m_self_params.m_position,30.f,*m_object->m_ce_best,CMovementRestrictor(m_object));

	if (point) {
		m_object->set_level_dest_vertex	(point->level_vertex_id());
		m_object->set_desired_position	(&point->position());
		m_object->set_movement_type		(eMovementTypeRun);
		m_object->set_body_state		(eBodyStateStand);
		m_object->setup					(CSightAction(SightManager::eSightTypeCover,true,false));//,&position);
	}
	else {
		m_object->set_movement_type		(eMovementTypeStand);
		m_object->set_body_state		(eBodyStateCrouch);
		m_object->setup					(CSightAction(SightManager::eSightTypeCover,true,false));//,&position);
	}

	m_object->set_path_type				(MovementManager::ePathTypeLevelPath);
	m_object->set_detail_path_type		(DetailPathManager::eDetailPathTypeSmooth);
	m_object->set_mental_state			(eMentalStateDanger);

	m_object->CObjectHandler::set_goal	(eObjectActionAim1,m_object->best_weapon());
}

_edge_value_type CStalkerActionCamp::weight	(const CSConditionState &condition0, const CSConditionState &condition1) const
{
#ifdef DEBUG
	if ((g_stalker_behaviour == eStalkerBehaviourVeryAggressive) || (g_stalker_behaviour == eStalkerBehaviourAggressive))
		return				(_edge_value_type(100));
	else
#endif
		return				(_edge_value_type(100));
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionGetReadyToKillModerate
//////////////////////////////////////////////////////////////////////////

CStalkerActionGetReadyToKillModerate::CStalkerActionGetReadyToKillModerate	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionGetReadyToKillModerate::initialize	()
{
	inherited::initialize	();
	m_object->set_sound_mask(u32(eStalkerSoundMaskNoHumming));
//	m_storage->set_property	(eWorldPropertyEnemyAimed,true);
	m_storage->set_property	(eWorldPropertySafeToKill,true);
	m_storage->set_property	(eWorldPropertyFireEnough,false);
}

void CStalkerActionGetReadyToKillModerate::finalize	()
{
	inherited::finalize		();

	if (!m_object->g_Alive())
		return;

	m_object->set_sound_mask(0);
}

void CStalkerActionGetReadyToKillModerate::execute	()
{
	inherited::execute		();

	if (!m_object->m_best_item_to_kill)
		return;

	if (m_object->enemy()) {
		CMemoryInfo					mem_object = m_object->memory(m_object->enemy());
		Fvector						position = mem_object.m_object_params.m_position;
		m_object->m_ce_best->setup	(position,10.f,170.f,10.f);
		CCoverPoint					*point = ai().cover_manager().best_cover(m_object->Position(),10.f,*m_object->m_ce_best,CMovementRestrictor(m_object));
		if (!point)
			point					= ai().cover_manager().best_cover(m_object->Position(),30.f,*m_object->m_ce_best,CMovementRestrictor(m_object));

		if (point) {
			m_object->set_level_dest_vertex	(point->level_vertex_id());
			m_object->set_desired_position	(&point->position());
			m_object->set_movement_type		(eMovementTypeRun);
		}
		else
			m_object->set_movement_type	(eMovementTypeStand);
	}
	else
		m_object->set_movement_type	(eMovementTypeStand);

	m_object->set_desired_direction	(0);
	m_object->set_path_type			(MovementManager::ePathTypeLevelPath);
	m_object->set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	m_object->set_body_state		(eBodyStateStand);
	m_object->set_mental_state		(eMentalStateDanger);

	if (m_object->enemy() && m_object->visible_now(m_object->enemy()))
		m_object->CSightManager::setup		(CSightAction(m_object->enemy(),true));
	else
		m_object->CSightManager::setup		(CSightAction(SightManager::eSightTypePosition,m_object->memory(m_object->enemy()).m_object_params.m_position,true));

	if (!smart_cast<CMissile*>(m_object->best_weapon()))
		m_object->CObjectHandler::set_goal	(eObjectActionAimReady1,m_object->best_weapon());
}

_edge_value_type CStalkerActionGetReadyToKillModerate::weight	(const CSConditionState &condition0, const CSConditionState &condition1) const
{
#ifdef DEBUG
	if (g_stalker_behaviour == eStalkerBehaviourModerate)
#endif
		return				(_edge_value_type(1));
#ifdef DEBUG
	else
		return				(_edge_value_type(100));
#endif
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionKillEnemyModerate
//////////////////////////////////////////////////////////////////////////

CStalkerActionKillEnemyModerate::CStalkerActionKillEnemyModerate	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionKillEnemyModerate::initialize	()
{
	inherited::initialize	();
	m_object->set_sound_mask(u32(eStalkerSoundMaskNoHumming));

	float					distance = m_object->Position().distance_to(m_object->enemy()->Position());
	if (distance >= 30.f) {
		set_inertia_time	(500);
		m_fire_crouch		= true;
	}
	else
		if (distance <= 5.f) {
			set_inertia_time(1500);
			m_fire_crouch	= false;
		}
		else {
			set_inertia_time(iFloor(float(1500 - 500)*(30.f - distance)/(30.f - 5.f)) + 500);
			m_fire_crouch	= ::Random.randF(40.f) < (distance - 5.f);
		}

		m_fire_crouch = distance > 10.f ? true : false;
}

void CStalkerActionKillEnemyModerate::finalize	()
{
	inherited::finalize		();
	VERIFY					(m_storage);
//	if ((Level().timeServer() >= m_start_level_time + 500) && !::Random.randI(0,3))
	if (!::Random.randI(0,2)) {
		m_storage->set_property	(eWorldPropertyFireEnough,true);
//		m_storage->set_property	(eWorldPropertyEnemyAimed,true);
	}

	if (!m_object->g_Alive())
		return;

	m_object->set_sound_mask(0);
}

void CStalkerActionKillEnemyModerate::execute		()
{
	inherited::execute		();

	VERIFY							(m_object->enemy());
	if (!m_object->enemy())
		return;

	CMemoryInfo						mem_object = m_object->memory(m_object->enemy());

	VERIFY							(mem_object.m_object);
	if (!mem_object.m_object)
		return;

	VERIFY							(m_object->visible_now(m_object->enemy()));
	if (!m_object->visible_now(m_object->enemy()))
		return;

	if (completed()) {
		VERIFY						(m_storage);
		m_storage->set_property		(eWorldPropertyEnemyAimed,false);
		return;
	}

//	if (Level().timeServer() >= m_start_level_time + 500 + ::Random.randI(100,200))
//		m_storage->set_property	(eWorldPropertyLookOut,true);

	Fvector							position;
	m_object->enemy()->Center		(position);
	position.set					(m_object->enemy()->Position().x,position.y,m_object->enemy()->Position().z);

	m_object->set_movement_type		(eMovementTypeStand);
//	m_object->set_path_type			(MovementManager::ePathTypeLevelPath);
//	m_object->set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
//	m_object->set_body_state		(m_fire_crouch ? eBodyStateCrouch : eBodyStateStand);
	m_object->set_mental_state		(eMentalStateDanger);

	m_object->setup					(SightManager::eSightTypeFirePosition,&position);
	m_object->CObjectHandler::set_goal	(eObjectActionFire1,m_object->best_weapon());
}

_edge_value_type CStalkerActionKillEnemyModerate::weight	(const CSConditionState &condition0, const CSConditionState &condition1) const
{
#ifdef DEBUG
	if (g_stalker_behaviour == eStalkerBehaviourModerate)
#endif
		return				(_edge_value_type(1));
#ifdef DEBUG
	else
		return				(_edge_value_type(100));
#endif
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionGetEnemySeenModerate
//////////////////////////////////////////////////////////////////////////

CStalkerActionGetEnemySeenModerate::CStalkerActionGetEnemySeenModerate	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionGetEnemySeenModerate::initialize	()
{
	inherited::initialize	();
	m_object->set_sound_mask(u32(eStalkerSoundMaskNoHumming));
	m_storage->set_property	(eWorldPropertyFireEnough,false);
	m_storage->set_property	(eWorldPropertySafeToKill,true);
//	m_storage->set_property	(eWorldPropertyEnemyAimed,true);
}

void CStalkerActionGetEnemySeenModerate::finalize	()
{
	inherited::finalize		();
//	if (m_object->enemy() && m_object->enemy()->human_being()) {
//		CMemoryInfo			mem_object = m_object->memory(m_object->enemy());
//		if (mem_object.m_object && 
//			m_object->visible_now(m_object->enemy()) && 
//			(Level().timeServer() >= mem_object.m_last_level_time + 20000)) {// && !m_object->visible_now(m_object->enemy())) {
//			m_object->play	(eStalkerSoundAlarm);
//		}
//	}
	m_start_standing_time	= m_start_level_time;

	if (!m_object->g_Alive())
		return;

	m_object->set_sound_mask(eStalkerSoundMaskSurrender);
	m_object->set_sound_mask(0);
}

void CStalkerActionGetEnemySeenModerate::execute	()
{
	inherited::execute			();

	VERIFY						(m_object->enemy());
	if (!m_object->enemy())
		return;

	if (m_object->enemy()->human_being())
		m_object->play			(eStalkerSoundSurrender,20000,10000);

	CMemoryInfo									mem_object = m_object->memory(m_object->enemy());

	VERIFY						(mem_object.m_object);
	if (!mem_object.m_object)
		return;

	VERIFY						(!m_object->visible_now(m_object->enemy()));

	if (Level().timeServer() >= m_start_level_time + 7000) {
		m_storage->set_property	(eWorldPropertySafeToKill,false);
		m_storage->set_property	(eWorldPropertyEnemyAimed,false);
	}

	if (m_object->enemy()) {
		if (Level().timeServer() >= m_start_standing_time + 10000) {
			if (m_object->accessible(mem_object.m_object_params.m_position)) {
				m_object->set_level_dest_vertex	(mem_object.m_object_params.m_level_vertex_id);
				m_object->set_desired_position	(&mem_object.m_object_params.m_position);
			}
			else {
				Fvector desired_position;
				u32	level_vertex_id = m_object->accessible_nearest(mem_object.m_object_params.m_position,desired_position);
				m_object->set_level_dest_vertex	(level_vertex_id);
				m_object->set_desired_position	(&desired_position);
			}
			m_object->set_movement_type		(eMovementTypeWalk);
			if (m_object->Position().distance_to_xz(mem_object.m_object_params.m_position) <= .1f)
				m_object->CMemoryManager::enable(m_object->enemy(),false);
		}
		else {
			Fvector						position = mem_object.m_object_params.m_position;
			m_object->m_ce_angle->setup	(position,10.f,170.f,mem_object.m_object_params.m_level_vertex_id);
			CCoverPoint					*point = ai().cover_manager().best_cover(m_object->Position(),30.f,*m_object->m_ce_angle,CMovementRestrictor(m_object));
			if (!point)
				point					= ai().cover_manager().best_cover(m_object->Position(),50.f,*m_object->m_ce_angle,CMovementRestrictor(m_object));

			if (point) {
				m_object->set_level_dest_vertex	(point->level_vertex_id());
				m_object->set_desired_position	(&point->position());
				//m_object->set_movement_type		(eMovementTypeRun);
				if (!m_object->Position().similar(point->position(),.1f))
					m_start_standing_time	= Level().timeServer();
			}
			else
				m_object->set_movement_type	(eMovementTypeStand);
		}
	}
	else
		m_object->set_movement_type	(eMovementTypeStand);

	m_object->set_path_type			(MovementManager::ePathTypeLevelPath);
	m_object->set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	if (m_object->Position().distance_to(mem_object.m_self_params.m_position) >= 2.f) {
		m_object->set_body_state	(eBodyStateStand);
		m_object->set_movement_type		(eMovementTypeWalk);
	}
	else {
		m_object->set_body_state	(eBodyStateCrouch);
		m_object->set_movement_type	(eMovementTypeRun);
	}
	m_object->set_mental_state		(eMentalStateDanger);

	m_object->setup					(CSightAction(SightManager::eSightTypePosition,mem_object.m_object_params.m_position,true));

	if (!smart_cast<CMissile*>(m_object->best_weapon()))
		m_object->CObjectHandler::set_goal		(eObjectActionAimReady1,m_object->best_weapon());

	if (ai().level_graph().inside(mem_object.m_object_params.m_level_vertex_id,mem_object.m_object_params.m_position)) {
		if (m_object->Position().distance_to_xz(mem_object.m_object_params.m_position) <=.5f)
			m_object->CMemoryManager::enable	(m_object->enemy(),false);
	}
	else
		if ((mem_object.m_object_params.m_level_vertex_id == m_object->level_vertex_id()) || 
			 (m_object->Position().distance_to_xz(ai().level_graph().vertex_position(mem_object.m_object_params.m_level_vertex_id)) < ai().level_graph().header().cell_size() * 1.5f))
				m_object->CMemoryManager::enable	(m_object->enemy(),false);
}

_edge_value_type CStalkerActionGetEnemySeenModerate::weight	(const CSConditionState &condition0, const CSConditionState &condition1) const
{
#ifdef DEBUG
	if ((g_stalker_behaviour == eStalkerBehaviourVeryAggressive) || (g_stalker_behaviour == eStalkerBehaviourAggressive))
		return				(_edge_value_type(100));
	else
#endif
		return				(_edge_value_type(1));
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionKillEnemyLostModerate
//////////////////////////////////////////////////////////////////////////

CStalkerActionKillEnemyLostModerate::CStalkerActionKillEnemyLostModerate	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionKillEnemyLostModerate::initialize	()
{
	inherited::initialize	();
	m_object->set_sound_mask(u32(eStalkerSoundMaskNoHumming));
	if (m_object->enemy() && m_object->enemy()->human_being() && !m_storage->property(eWorldPropertyFireEnough)) {
		CMemoryInfo			mem_object = m_object->memory(m_object->enemy());
		if (mem_object.m_object && 
			m_object->visible_now(m_object->enemy()) && 
			(Level().timeServer() >= mem_object.m_last_level_time + 1000)) {
				if (m_object->visible_now(m_object->enemy()))
					m_object->play	(eStalkerSoundAttack);
				else
					m_object->play	(eStalkerSoundAlarm);
			}
	}
}

void CStalkerActionKillEnemyLostModerate::finalize	()
{
	inherited::finalize		();

	if (!m_object->g_Alive())
		return;

	m_object->set_sound_mask(0);
	VERIFY					(m_storage);
}

void CStalkerActionKillEnemyLostModerate::execute		()
{
	inherited::execute		();

	if (!m_object->enemy())
		return;

	CMemoryInfo						mem_object = m_object->memory(m_object->enemy());

	if (!mem_object.m_object)
		return;

	if (!m_object->visible_now(m_object->enemy()))
		return;

	m_object->setup					(CSightAction(m_object->enemy(),true));

	Fvector							position = mem_object.m_object_params.m_position;
	m_object->m_ce_best->setup		(position,10.f,170.f,10.f);
	CCoverPoint						*point = ai().cover_manager().best_cover(m_object->Position(),10.f,*m_object->m_ce_best,CMovementRestrictor(m_object));
	if (!point)
		point						= ai().cover_manager().best_cover(m_object->Position(),30.f,*m_object->m_ce_best,CMovementRestrictor(m_object));

	if (point) {
		m_object->set_level_dest_vertex	(point->level_vertex_id());
		m_object->set_desired_position	(&point->position());
		m_object->set_movement_type		(eMovementTypeRun);
	}
	else
		m_object->set_movement_type	(eMovementTypeStand);

	m_object->set_desired_direction	(0);
	m_object->set_movement_type		(eMovementTypeRun);
	m_object->set_path_type			(MovementManager::ePathTypeLevelPath);
	m_object->set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	m_object->set_body_state		(eBodyStateStand);
	m_object->set_mental_state		(eMentalStateDanger);

	m_object->CObjectHandler::set_goal	(eObjectActionFire1,m_object->best_weapon());
}

_edge_value_type CStalkerActionKillEnemyLostModerate::weight	(const CSConditionState &condition0, const CSConditionState &condition1) const
{
#ifdef DEBUG
	if (g_stalker_behaviour == eStalkerBehaviourModerate)
#endif
		return				(_edge_value_type(1));
#ifdef DEBUG
	else
		return				(_edge_value_type(100));
#endif
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionTakeCover
//////////////////////////////////////////////////////////////////////////

CStalkerActionTakeCover::CStalkerActionTakeCover	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionTakeCover::initialize()
{
	inherited::initialize	();
	m_object->set_sound_mask(u32(eStalkerSoundMaskNoHumming));
//	m_storage->set_property	(eWorldPropertyEnemyAimed,true);
}

void CStalkerActionTakeCover::finalize	()
{
	inherited::finalize		();
	m_storage->set_property	(eWorldPropertyFireEnough,false);

	if (!m_object->g_Alive())
		return;

	m_object->set_sound_mask(0);
}

void CStalkerActionTakeCover::execute	()
{
	inherited::execute		();

	if (!m_object->enemy())
		return;

	CMemoryInfo						mem_object = m_object->memory(m_object->enemy());

	if (!mem_object.m_object)
		return;

	m_object->setup					(CSightAction(m_object->enemy(),true));

	Fvector position				= mem_object.m_object_params.m_position;
	m_object->m_ce_best->setup		(position,10.f,170.f,10.f);
	CCoverPoint						*point = ai().cover_manager().best_cover(m_object->Position(),10.f,*m_object->m_ce_best,CMovementRestrictor(m_object));
	if (!point)
		point						= ai().cover_manager().best_cover(m_object->Position(),30.f,*m_object->m_ce_best,CMovementRestrictor(m_object));

	if (point) {
		m_object->set_level_dest_vertex	(point->level_vertex_id());
		m_object->set_desired_position	(&point->position());
		if (point->position().similar(m_object->Position(),.1f))
			m_storage->set_property	(eWorldPropertyFireEnough,false);
	}

	m_object->set_desired_direction	(0);
	m_object->set_path_type			(MovementManager::ePathTypeLevelPath);
	m_object->set_detail_path_type	(DetailPathManager::eDetailPathTypeSmooth);
	m_object->set_mental_state		(eMentalStateDanger);
	m_object->set_body_state		(eBodyStateStand);
	if (!point || !point->position().similar(m_object->Position(),1.5f))
		m_object->set_movement_type	(eMovementTypeRun);
	else
		m_object->set_movement_type	(eMovementTypeWalk);

	if (!m_object->ready_to_kill())
		m_object->CObjectHandler::set_goal	(eObjectActionAimReady1,m_object->best_weapon());
	else
		m_object->CObjectHandler::set_goal	(eObjectActionFire1,m_object->best_weapon());
}

_edge_value_type CStalkerActionTakeCover::weight	(const CSConditionState &condition0, const CSConditionState &condition1) const
{
#ifdef DEBUG
	if (g_stalker_behaviour == eStalkerBehaviourModerate)
#endif
		return				(_edge_value_type(1));
#ifdef DEBUG
	else
		return				(_edge_value_type(100));
#endif
}

