////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_anomaly_actions.cpp
//	Created 	: 25.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker anomaly action classes
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_anomaly_actions.h"
#include "ai/stalker/ai_stalker.h"
#include "script_game_object.h"
#include "stalker_decision_space.h"
#include "customzone.h"
#include "space_restriction_manager.h"
#include "space_restriction_bridge.h"
#include "space_restriction_base.h"
#include "inventory.h"

using namespace StalkerDecisionSpace;

//////////////////////////////////////////////////////////////////////////
// CStalkerActionGetOutOfAnomaly
//////////////////////////////////////////////////////////////////////////

CStalkerActionGetOutOfAnomaly::CStalkerActionGetOutOfAnomaly	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionGetOutOfAnomaly::initialize	()
{
	inherited::initialize				();

	m_object->set_sound_mask			(u32(eStalkerSoundMaskNoHumming));

	m_object->set_node_evaluator		(0);
	m_object->set_path_evaluator		(0);
	m_object->set_desired_direction		(0);
	m_object->set_path_type				(MovementManager::ePathTypeLevelPath);
	m_object->set_detail_path_type		(DetailPathManager::eDetailPathTypeSmooth);
	m_object->set_body_state			(eBodyStateStand);
	m_object->set_movement_type			(eMovementTypeWalk);
	m_object->set_mental_state			(eMentalStateDanger);
	m_object->CSightManager::setup		(SightManager::eSightTypeCurrentDirection);
	if (m_object->enemy())
		m_object->CObjectHandler::set_goal	(eObjectActionIdle,m_object->best_weapon());
	else
		m_object->CObjectHandler::set_goal	(eObjectActionIdle);
	set_property						(eWorldPropertyAnomaly,true);
}

void CStalkerActionGetOutOfAnomaly::finalize	()
{
	inherited::finalize					();

	if (!m_object->g_Alive())
		return;

	m_object->set_sound_mask			(0);
}

void CStalkerActionGetOutOfAnomaly::execute	()
{
	inherited::execute					();

	strcpy								(m_temp,"");
	xr_vector<CObject*>::const_iterator	I = m_object->feel_touch.begin();
	xr_vector<CObject*>::const_iterator	E = m_object->feel_touch.end();
	for ( ; I != E; ++I) {
		CCustomZone						*zone = smart_cast<CCustomZone*>(*I);
		if (zone)
			strcat						(m_temp,*zone->cName());
	}
	
	SpaceRestrictionHolder::CBaseRestrictionPtr	restriction = Level().space_restriction_manager().restriction(m_temp);
	if (restriction->inside(m_object->Position()))
		m_object->add_restrictions		("",m_temp);

	if (m_object->accessible(m_object->Position())) {
//		if (m_object->use_desired_position() && !m_object->accessible(m_object->desired_position()))
		return;
	}

	Fvector								dest_pos = m_object->Position();
	u32 dest_vertex_id					= m_object->accessible_nearest(m_object->Position(),dest_pos);
	m_object->set_level_dest_vertex		(dest_vertex_id);
	m_object->set_desired_position		(&dest_pos);
}

//////////////////////////////////////////////////////////////////////////
// CStalkerActionDetectAnomaly
//////////////////////////////////////////////////////////////////////////

CStalkerActionDetectAnomaly::CStalkerActionDetectAnomaly	(CAI_Stalker *object, LPCSTR action_name) :
	inherited				(object,action_name)
{
}

void CStalkerActionDetectAnomaly::initialize	()
{
	inherited::initialize			();
	m_object->set_sound_mask		(u32(eStalkerSoundMaskNoHumming));
	m_inertia_time					= 15000 + ::Random32.random(5000);
}

void CStalkerActionDetectAnomaly::finalize	()
{
	inherited::finalize				();

	if (!m_object->g_Alive())
		return;

	m_object->set_sound_mask		(0);
}

void CStalkerActionDetectAnomaly::execute	()
{
	inherited::execute				();

	if (completed() || m_object->enemy()) {
		set_property				(eWorldPropertyAnomaly,false);
		return;
	}
	
	m_object->CObjectHandler::set_goal	(eObjectActionFire1,m_object->inventory().m_slots[5].m_pIItem);
}
