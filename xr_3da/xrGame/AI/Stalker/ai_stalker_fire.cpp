////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_fire.cpp
//	Created 	: 25.02.2003
//  Modified 	: 25.02.2003
//	Author		: Dmitriy Iassenev
//	Description : Fire and enemy parameters for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"
#include "ai_stalker_impl.h"
#include "../../script_entity_action.h"
#include "../../inventory.h"
#include "../../ef_storage.h"
#include "../../stalker_decision_space.h"
#include "../../script_game_object.h"
#include "../../customzone.h"
#include "../../../skeletonanimated.h"
#include "../../agent_manager.h"
#include "../../stalker_animation_manager.h"
#include "../../motivation_action_manager_stalker.h"
#include "../../ef_pattern.h"
#include "../../memory_manager.h"
#include "../../hit_memory_manager.h"
#include "../../enemy_manager.h"
#include "../../item_manager.h"
#include "../../stalker_movement_manager.h"
#include "../../entitycondition.h"
#include "../../sound_player.h"
#include "../../cover_point.h"
#include "../../agent_member_manager.h"
#include "../../agent_location_manager.h"
#include "../../danger_cover_location.h"

const float DANGER_DISTANCE = 5.f;
const u32	DANGER_INTERVAL = 30000;

float CAI_Stalker::GetWeaponAccuracy	() const
{
	float				base = PI/180.f;
	
	//влияние ранга на меткость
	base *= m_fRankDisperison;

	if (movement().movement_type() == eMovementTypeWalk)
		if (movement().body_state() == eBodyStateStand)
			return		(base*m_disp_walk_stand);
		else
			return		(base*m_disp_walk_crouch);
	else
		if (movement().movement_type() == eMovementTypeRun)
			if (movement().body_state() == eBodyStateStand)
				return	(base*m_disp_run_stand);
			else
				return	(base*m_disp_run_crouch);
		else
			if (movement().body_state() == eBodyStateStand)
				return	(base*m_disp_stand_stand);
			else
				return	(base*m_disp_stand_crouch);
}

void CAI_Stalker::g_fireParams(const CHudItem* pHudItem, Fvector& P, Fvector& D)
{
	if (g_Alive()) {
		D.setHP			(-movement().m_head.current.yaw,-movement().m_head.current.pitch);
		Center			(P);
		P.mad			(D,.5f);
		P.y				+= movement().body_state() == eBodyStateStand ? .75f : .15f;
	}
}

void CAI_Stalker::g_WeaponBones	(int &L, int &R1, int &R2)
{
	int				r_hand, r_finger2, l_finger1;
	CObjectHandler::weapon_bones(r_hand, r_finger2, l_finger1);
	R1				= r_hand;
	R2				= r_finger2;
	if	(
			(conditions().IsLimping() && (movement().mental_state() == eMentalStateFree)) || 
			(GetCurrentAction() && !GetCurrentAction()->m_tAnimationAction.m_bHandUsage) ||
			(!animation().script_animations().empty() && animation().script_animations().front().hand_usage())
		)
	{
			L		= R2;
	}
	else {
		L			= l_finger1;
	}
}

void CAI_Stalker::HitSignal(float amount, Fvector& vLocalDir, CObject* who, s16 element)
{
	if (g_Alive()) {
		CCoverPoint				*cover = agent_manager().member().member(this).cover();
		if (cover && who && (who->ID() != ID()) && !fis_zero(amount) && brain().affect_cover())
			agent_manager().location().add	(xr_new<CDangerCoverLocation>(cover,Device.dwTimeGlobal,DANGER_INTERVAL,DANGER_DISTANCE));

		// Play hit-ref_sound
		CEntityAlive		*entity_alive = smart_cast<CEntityAlive*>(who);
		if (!entity_alive || (tfGetRelationType(entity_alive) != ALife::eRelationTypeFriend))
			sound().play	(eStalkerSoundInjuring);
		else
			sound().play	(eStalkerSoundInjuringByFriend);
		Fvector				D;
		float				yaw, pitch;
		D.getHP				(yaw,pitch);
#pragma todo("Dima to Dima : forward-back bone impulse direction has been determined incorrectly!")
		float				power_factor = 3.f*amount/100.f;
		clamp				(power_factor,0.f,1.f);
		CSkeletonAnimated	*tpKinematics = smart_cast<CSkeletonAnimated*>(Visual());
		animation().play_fx	(power_factor,iFloor(tpKinematics->LL_GetBoneInstance(element).get_param(1) + (angle_difference(movement().m_body.current.yaw,-yaw) <= PI_DIV_2 ? 0 : 1)));
	}
	
	if (g_Alive())
		memory().hit().add	(amount,vLocalDir,who,element);
}

void CAI_Stalker::OnItemTake			(CInventoryItem *inventory_item)
{
	CObjectHandler::OnItemTake	(inventory_item);
	m_last_best_item_frame		= 0;
	m_item_actuality			= false;
}

void CAI_Stalker::OnItemDrop			(CInventoryItem *inventory_item)
{
	CObjectHandler::OnItemDrop	(inventory_item);
	m_last_best_item_frame		= 0;
	m_item_actuality			= false;
}

void CAI_Stalker::update_best_item_info	()
{
	// check if we already updated
	if (!frame_check(m_last_best_item_frame))
		return;

	ai().ef_storage().alife_evaluation(false);

	if (m_item_actuality) {
		if (m_best_item_to_kill) {
			if (m_best_item_to_kill->can_kill())
				return;
		}
	}

	// initialize parameters
	m_item_actuality							= true;
	ai().ef_storage().non_alife().member()		= this;
	ai().ef_storage().non_alife().enemy()		= memory().enemy().selected() ? memory().enemy().selected() : this;
	m_best_item_to_kill			= 0;
	m_best_ammo					= 0;
	m_best_found_item_to_kill	= 0;
	m_best_found_ammo			= 0;
	float						best_value = 0;

	// try to find the best item which can kill
	{
		PSPIItem					I = inventory().m_all.begin();
		PSPIItem					E = inventory().m_all.end();
		for ( ; I != E; ++I) {
			if ((*I)->can_kill()) {
				ai().ef_storage().non_alife().member_item()	= &(*I)->object();
				float value							= ai().ef_storage().m_pfWeaponEffectiveness->ffGetValue();
				if (value > best_value) {
					best_value			= value;
					m_best_item_to_kill = *I;
				}
			}
		}
	}

	// check if we found
	if (m_best_item_to_kill) {
		m_best_ammo				= m_best_item_to_kill;
		return;
	}

	// so we do not have such an item
	// check if we remember we saw item which can kill
	// or items which can make my item killing
	{
		xr_vector<const CGameObject*>::const_iterator	I = memory().item().objects().begin();
		xr_vector<const CGameObject*>::const_iterator	E = memory().item().objects().end();
		for ( ; I != E; ++I) {
			const CInventoryItem	*inventory_item = smart_cast<const CInventoryItem*>(*I);
			if (!inventory_item || !memory().item().useful(&inventory_item->object()))
				continue;
			CInventoryItem			*item			= inventory_item->can_kill(&inventory());
			if (item) {
				ai().ef_storage().non_alife().member_item()	= &inventory_item->object();
				float value							= ai().ef_storage().m_pfWeaponEffectiveness->ffGetValue();
				if (value > best_value) {
					best_value						= value;
					m_best_found_item_to_kill		= inventory_item;
					m_best_found_ammo				= 0;
					m_best_ammo						= item;
				}
			}
			else {
				item								= inventory_item->can_make_killing(&inventory());
				if (!item)
					continue;

				ai().ef_storage().non_alife().member_item()	= &item->object();
				float value							= ai().ef_storage().m_pfWeaponEffectiveness->ffGetValue();
				if (value > best_value) {
					best_value						= value;
					m_best_item_to_kill				= item;
					m_best_found_item_to_kill		= 0;
					m_best_found_ammo				= inventory_item;
				}
			}
		}
	}

	// check if we found such an item
	if (m_best_found_item_to_kill || m_best_found_ammo)
		return;

	// check if we remember we saw item to kill
	// and item which can make this item killing
	xr_vector<const CGameObject*>::const_iterator	I = memory().item().objects().begin();
	xr_vector<const CGameObject*>::const_iterator	E = memory().item().objects().end();
	for ( ; I != E; ++I) {
		const CInventoryItem	*inventory_item = smart_cast<const CInventoryItem*>(*I);
		if (!inventory_item || !memory().item().useful(&inventory_item->object()))
			continue;
		const CInventoryItem	*item = inventory_item->can_kill(memory().item().objects());
		if (item) {
			ai().ef_storage().non_alife().member_item()	= &inventory_item->object();
			float value							= ai().ef_storage().m_pfWeaponEffectiveness->ffGetValue();
			if (value > best_value) {
				best_value					= value;
				m_best_found_item_to_kill	= inventory_item;
				m_best_found_ammo			= item;
			}
		}
	}
}

bool CAI_Stalker::item_to_kill			()
{
	update_best_item_info	();
	return					(!!m_best_item_to_kill);
}

bool CAI_Stalker::item_can_kill			()
{
	update_best_item_info	();
	return					(!!m_best_ammo);
}

bool CAI_Stalker::remember_item_to_kill	()
{
	update_best_item_info	();
	return					(!!m_best_found_item_to_kill);
}

bool CAI_Stalker::remember_ammo			()
{
	update_best_item_info	();
	return					(!!m_best_found_ammo);
}

bool CAI_Stalker::ready_to_kill			()
{
	return					(
		m_best_item_to_kill && 
		inventory().ActiveItem() && 
		(inventory().ActiveItem()->object().ID() == m_best_item_to_kill->object().ID()) &&
		m_best_item_to_kill->ready_to_kill()
	);
}

bool CAI_Stalker::can_kill_member		()
{
	Fvector					position, direction;
	g_fireParams			(0,position,direction);
	
	if (can_kill_member(position,direction))
		return				(true);

	float					yaw, pitch, safety_fire_angle = PI_DIV_8;
	direction.getHP			(yaw,pitch);

	direction.setHP			(yaw - safety_fire_angle,pitch);
	if (can_kill_member(position,direction))
		return				(true);

	direction.setHP			(yaw + safety_fire_angle,pitch);
	if (can_kill_member(position,direction))
		return				(true);

	direction.setHP			(yaw,pitch - safety_fire_angle);
	if (can_kill_member(position,direction))
		return				(true);

	direction.setHP			(yaw,pitch + safety_fire_angle);
	if (can_kill_member(position,direction))
		return				(true);

	return					(false);
}

bool CAI_Stalker::can_kill_member		(const Fvector &position, const Fvector &direction) const
{
	collide::rq_result		ray_query_result;
	Level().ObjectSpace.RayPick(position, direction, 50.f, collide::rqtBoth, ray_query_result);
	
	if (!ray_query_result.O)
		return				(false);
	
	CEntityAlive			*entity_alive = smart_cast<CEntityAlive*>(ray_query_result.O);
	if (!entity_alive || (entity_alive->ID() == ID()))
		return				(false);

	if (tfGetRelationType(entity_alive) != ALife::eRelationTypeEnemy)
		return				(true);

	return					(false);
}

bool CAI_Stalker::can_kill_enemy		()
{
//	Fvector					position, direction;
//	g_fireParams			(0,position,direction);
//
//	collide::rq_result		ray_query_result;
//	Level().ObjectSpace.RayPick(position, direction, 50.f, collide::rqtBoth, ray_query_result);
//
//	if (!ray_query_result.O)
//		return				(false);
//
//	return					(ray_query_result.O->ID() == enemy()->ID());
	return					(angle_difference(movement().m_head.current.yaw,movement().m_head.target.yaw) < PI_DIV_8);
}

bool CAI_Stalker::undetected_anomaly	()
{
	return					(inside_anomaly() || brain().CMotivationActionManagerStalker::m_storage.property(StalkerDecisionSpace::eWorldPropertyAnomaly));
}

bool CAI_Stalker::inside_anomaly		()
{
	xr_vector<CObject*>::const_iterator	I = feel_touch.begin();
	xr_vector<CObject*>::const_iterator	E = feel_touch.end();
	for ( ; I != E; ++I) {
		CCustomZone			*zone = smart_cast<CCustomZone*>(*I);
		if (zone)
			return			(true);
	}
	return					(false);
}
