////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_fire.cpp
//	Created 	: 25.02.2003
//  Modified 	: 25.02.2003
//	Author		: Dmitriy Iassenev
//	Description : Fire and enemy parameters for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"
#include "../../script_entity_action.h"
#include "../../inventory.h"
#include "../../ef_storage.h"
#include "../../stalker_decision_space.h"
#include "../../script_game_object.h"

float CAI_Stalker::GetWeaponAccuracy	() const
{
	float				base = PI/180.f;
	if (movement_type() == eMovementTypeWalk)
		if (body_state() == eBodyStateStand)
			return		(base*m_disp_walk_stand);
		else
			return		(base*m_disp_walk_crouch);
	else
		if (movement_type() == eMovementTypeRun)
			if (body_state() == eBodyStateStand)
				return	(base*m_disp_run_stand);
			else
				return	(base*m_disp_run_crouch);
		else
			if (body_state() == eBodyStateStand)
				return	(base*m_disp_stand_stand);
			else
				return	(base*m_disp_stand_crouch);
}

void CAI_Stalker::g_fireParams(const CHudItem* pHudItem, Fvector& P, Fvector& D)
{
	if (g_Alive()) {
		D.setHP			(-m_head.current.yaw,-m_head.current.pitch);
		Center			(P);
		P.mad			(D,.5f);
		P.y				+= body_state() == eBodyStateStand ? .75f : .15f;
	}
}

void CAI_Stalker::g_WeaponBones	(int &L, int &R1, int &R2)
{
	R1				= m_r_hand;
	R2				= m_r_finger2;
	if	(
			(IsLimping() && (mental_state() == eMentalStateFree)) || 
			(GetCurrentAction() && !GetCurrentAction()->m_tAnimationAction.m_bHandUsage) ||
			(!m_script_animations.empty() && m_script_animations.front().m_hand_usage)
		)
	{
			L		= R2;
	}
	else {
		L			= m_l_finger1;
	}
}

void CAI_Stalker::HitSignal(float amount, Fvector& vLocalDir, CObject* who, s16 element)
{
	if (g_Alive()) {
		// Play hit-ref_sound
		CEntityAlive		*entity_alive = smart_cast<CEntityAlive*>(who);
		if (!entity_alive || (tfGetRelationType(entity_alive) != ALife::eRelationTypeFriend))
			CSoundPlayer::play	(eStalkerSoundInjuring);
		else
			CSoundPlayer::play	(eStalkerSoundInjuringByFriend);
		Fvector				D;
		float				yaw, pitch;
		D.getHP				(yaw,pitch);
		CSkeletonAnimated	*tpKinematics = PSkeletonAnimated(Visual());
#pragma todo("Dima to Dima : forward-back bone impulse direction has been determined incorrectly!")
		CMotionDef			*tpMotionDef = m_tAnims.A[body_state()].m_tGlobal.A[0].A[iFloor(tpKinematics->LL_GetBoneInstance(element).get_param(1) + (angle_difference(m_body.current.yaw,-yaw) <= PI_DIV_2 ? 0 : 1))];
		float				power_factor = 3.f*amount/100.f; clamp(power_factor,0.f,1.f);
		tpKinematics->PlayFX(tpMotionDef,power_factor);
	}
	if (g_Alive())
		add_hit_object		(amount,vLocalDir,who,element);
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

	if (m_item_actuality) {
		if (m_best_item_to_kill) {
			if (m_best_item_to_kill->can_kill())
				return;
		}
	}

	// initialize parameters
	m_item_actuality							= true;
	ai().ef_storage().m_tpCurrentMember			= this;
	ai().ef_storage().m_tpCurrentALifeMember	= 0;
	ai().ef_storage().m_tpCurrentEnemy			= enemy() ? enemy() : this;
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
				ai().ef_storage().m_tpGameObject	= *I;
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
		xr_vector<const CGameObject*>::const_iterator	I = items().begin();
		xr_vector<const CGameObject*>::const_iterator	E = items().end();
		for ( ; I != E; ++I) {
			const CInventoryItem	*inventory_item = smart_cast<const CInventoryItem*>(*I);
			if (!inventory_item || !CItemManager::useful(inventory_item))
				continue;
			CInventoryItem			*item			= inventory_item->can_kill(&inventory());
			if (item) {
				ai().ef_storage().m_tpGameObject	= inventory_item;
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

				ai().ef_storage().m_tpGameObject	= item;
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
	xr_vector<const CGameObject*>::const_iterator	I = items().begin();
	xr_vector<const CGameObject*>::const_iterator	E = items().end();
	for ( ; I != E; ++I) {
		const CInventoryItem	*inventory_item = smart_cast<const CInventoryItem*>(*I);
		if (!inventory_item || !CItemManager::useful(inventory_item))
			continue;
		const CInventoryItem	*item = inventory_item->can_kill(items());
		if (item) {
			ai().ef_storage().m_tpGameObject	= inventory_item;
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
		(inventory().ActiveItem()->ID() == m_best_item_to_kill->ID()) &&
		m_best_item_to_kill->ready_to_kill()
	);
}

bool CAI_Stalker::kill_distance			()
{
	return					(true);
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
	Collide::rq_result		ray_query_result;
	Level().ObjectSpace.RayPick(position, direction, 50.f, Collide::rqtBoth, ray_query_result);
	
	if (!ray_query_result.O)
		return				(false);
	
	CEntityAlive			*entity_alive = smart_cast<CEntityAlive*>(ray_query_result.O);
	if (!entity_alive)
		return				(false);

	if (tfGetRelationType(entity_alive) != ALife::eRelationTypeEnemy)
		return				(true);

	return					(false);
}

bool CAI_Stalker::undetected_anomaly	()
{
	return					(inside_anomaly() || m_anomaly_planner->CScriptActionPlanner::m_storage.property(StalkerDecisionSpace::eWorldPropertyAnomaly));
}

#include "../../customzone.h"

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
