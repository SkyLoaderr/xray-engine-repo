////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_torso_animation.cpp
//	Created 	: 19.11.2004
//  Modified 	: 19.11.2004
//	Author		: Dmitriy Iassenev
//	Description : Torso animations for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_animation_manager.h"
#include "ai/stalker/ai_stalker.h"
#include "inventory.h"
#include "weapon.h"
#include "missile.h"
#include "clsid_game.h"
#include "object_handler_space.h"
#include "object_handler_planner.h"
#include "stalker_movement_manager.h"
#include "entitycondition.h"

bool CStalkerAnimationManager::strapped				() const
{
	VERIFY					(m_weapon);
	return					(object().CObjectHandler::weapon_strapped(m_weapon));
}

void CStalkerAnimationManager::torso_play_callback	(CBlend *blend)
{
	CAI_Stalker				*object = (CAI_Stalker*)blend->CallbackParam;
	VERIFY					(object);
	if (object->animation().setup_storage()) {
		object->animation().setup_storage()->set_property(object->animation().property_id(),object->animation().property_value());
		return;
	}
	object->animation().torso().make_inactual();
}

void CStalkerAnimationManager::fill_object_info		()
{
	VERIFY					(object().inventory().ActiveItem());
	m_weapon				= smart_cast<CWeapon*>	(object().inventory().ActiveItem());
	m_missile				= smart_cast<CMissile*>	(object().inventory().ActiveItem());
}

MotionID CStalkerAnimationManager::no_object_animation(const EBodyState &body_state) const
{
	if (eMentalStateFree == object().movement().mental_state()) {
		R_ASSERT2(eBodyStateStand == object().movement().body_state(),"Cannot run !free! animation when body state is not stand!");
		if ((eMovementTypeStand == object().movement().movement_type()) || fis_zero(object().movement().speed(object().m_PhysicMovementControl)))
			return			(m_part_animations.A[body_state].m_torso.A[0].A[9].A[1]);
		else
			return			(m_part_animations.A[body_state].m_torso.A[0].A[object().conditions().IsLimping() ? 9 : (7 + object().movement().movement_type())].A[object().conditions().IsLimping() ? 0 : 1]);
	}
	else
		if ((eMovementTypeStand == object().movement().movement_type()) || fis_zero(object().movement().speed(object().m_PhysicMovementControl)))
			return			(m_part_animations.A[body_state].m_torso.A[0].A[6].A[0]);
		else
			return			(m_part_animations.A[body_state].m_torso.A[0].A[6].A[1]);
}

MotionID CStalkerAnimationManager::unknown_object_animation(u32 slot, const EBodyState &body_state) const
{
	switch (object().CObjectHandler::planner().current_action_state_id()) {
		case ObjectHandlerSpace::eWorldOperatorAim1 :
		case ObjectHandlerSpace::eWorldOperatorAim2 :
		case ObjectHandlerSpace::eWorldOperatorAimingReady1 :
		case ObjectHandlerSpace::eWorldOperatorAimingReady2 :
		case ObjectHandlerSpace::eWorldOperatorFire1 :
		case ObjectHandlerSpace::eWorldOperatorFire2 :
		case ObjectHandlerSpace::eWorldOperatorQueueWait1 :
		case ObjectHandlerSpace::eWorldOperatorQueueWait2 : {
			if ((body_state == eBodyStateStand) && !fis_zero(object().movement().speed(object().m_PhysicMovementControl)))
				return m_part_animations.A[body_state].m_torso.A[slot].A[(body_state == eBodyStateStandDamaged) ? 9 : 6].A[1];
			else
				return m_part_animations.A[body_state].m_torso.A[slot].A[(body_state == eBodyStateStandDamaged) ? 9 : 6].A[0];
		}
#if 0
		case ObjectHandlerSpace::eWorldOperatorStrapping :
			return m_part_animations.A[body_state].m_torso.A[slot].A[11].A[0];
		case ObjectHandlerSpace::eWorldOperatorUnstrapping :
			return m_part_animations.A[body_state].m_torso.A[slot].A[12].A[0];
		case ObjectHandlerSpace::eWorldOperatorStrapping2Idle :
			return m_part_animations.A[body_state].m_torso.A[slot].A[11].A[1];
		case ObjectHandlerSpace::eWorldOperatorUnstrapping2Idle :
			return m_part_animations.A[body_state].m_torso.A[slot].A[12].A[1];
#else
		case ObjectHandlerSpace::eWorldOperatorStrapping :
			return m_part_animations.A[eBodyStateStand].m_torso.A[slot].A[11].A[0];
		case ObjectHandlerSpace::eWorldOperatorUnstrapping :
			return m_part_animations.A[eBodyStateStand].m_torso.A[slot].A[12].A[0];
		case ObjectHandlerSpace::eWorldOperatorStrapping2Idle :
			return m_part_animations.A[eBodyStateStand].m_torso.A[slot].A[11].A[1];
		case ObjectHandlerSpace::eWorldOperatorUnstrapping2Idle :
			return m_part_animations.A[eBodyStateStand].m_torso.A[slot].A[12].A[1];
#endif
		default : {
			if (eMentalStateFree == object().movement().mental_state()) {
				//. hack
				R_ASSERT2	(eBodyStateStand == object().movement().body_state(),"Cannot run !free! animation when body state is not stand!");
				if ((eMovementTypeStand == object().movement().movement_type()) || fis_zero(object().movement().speed(object().m_PhysicMovementControl)))
					return m_part_animations.A[body_state].m_torso.A[slot].A[9].A[1];
				else
					return m_part_animations.A[body_state].m_torso.A[slot].A[object().conditions().IsLimping() ? 9 : (7 + object().movement().movement_type())].A[1];
			}
			else {
				if (fis_zero(object().movement().speed(object().m_PhysicMovementControl))) {
					return m_part_animations.A[body_state].m_torso.A[slot].A[object().conditions().IsLimping() ? 9 : 6].A[0];
				}
				switch (object().movement().movement_type()) {
					case eMovementTypeStand :
						return m_part_animations.A[body_state].m_torso.A[slot].A[object().conditions().IsLimping() ? 9 : 6].A[0];
					case eMovementTypeWalk :
						if (object().movement().body_state() == eBodyStateStand)
							return m_part_animations.A[body_state].m_torso.A[slot].A[7].A[0];
						else
							return m_part_animations.A[body_state].m_torso.A[slot].A[6].A[0];
					case eMovementTypeRun :
						if (object().movement().body_state() == eBodyStateStand)
							return m_part_animations.A[body_state].m_torso.A[slot].A[object().conditions().IsLimping() ? 7 : 8].A[0];
						else
							return m_part_animations.A[body_state].m_torso.A[slot].A[6].A[0];
					default : NODEFAULT;
				}
			}
		}
	}
	return					(MotionID());
}

u32 CStalkerAnimationManager::object_slot	() const
{
	return			(m_weapon ? m_weapon->animation_slot() : (m_missile ? m_missile->animation_slot() : 0));
}

MotionID CStalkerAnimationManager::weapon_animation	(u32 slot, const EBodyState &body_state) const
{
	switch (m_weapon->STATE) {
		case CWeapon::eReload :
			return m_part_animations.A[body_state].m_torso.A[slot].A[4].A[0];
		case CWeapon::eShowing :
			return m_part_animations.A[body_state].m_torso.A[slot].A[0].A[0];
		case CWeapon::eHiding :
			return m_part_animations.A[body_state].m_torso.A[slot].A[3].A[0];
		case CWeapon::eFire:
			if ((body_state == eBodyStateStand) && !fis_zero(object().movement().speed(object().m_PhysicMovementControl)))
				return m_part_animations.A[body_state].m_torso.A[slot].A[1].A[1];
			else
				return m_part_animations.A[body_state].m_torso.A[slot].A[1].A[0];
		case CWeapon::eFire2 :
			if ((body_state == eBodyStateStand) && !fis_zero(object().movement().speed(object().m_PhysicMovementControl)))
				return m_part_animations.A[body_state].m_torso.A[slot].A[1].A[1];
			else
				return m_part_animations.A[body_state].m_torso.A[slot].A[1].A[0];
	}

	return			(unknown_object_animation(slot,body_state));
}

MotionID CStalkerAnimationManager::missile_animation	(u32 slot, const EBodyState &body_state) const
{
	VERIFY			(m_missile);
	if (body_state == eBodyStateCrouch)
		slot		= 0;

	switch (m_missile->State()) {
		case MS_SHOWING	 :
			return  (m_part_animations.A[body_state].m_torso.A[slot].A[0].A[0]);
		case MS_HIDING	 :
			return  (m_part_animations.A[body_state].m_torso.A[slot].A[3].A[0]);
		case MS_THREATEN :
			return  (m_part_animations.A[body_state].m_torso.A[slot].A[1].A[0]);
		case MS_READY	 :
			return  (m_part_animations.A[body_state].m_torso.A[slot].A[1].A[1]);
		case MS_THROW	 :
			return  (m_part_animations.A[body_state].m_torso.A[slot].A[1].A[2]);
		case MS_END		 :
			return  (m_part_animations.A[body_state].m_torso.A[slot].A[1].A[2]);
		case MS_PLAYING	 :
			return  (m_part_animations.A[body_state].m_torso.A[slot].A[1].A[2]);
		case MS_IDLE	 :
		case MS_HIDDEN	 :
		case MS_EMPTY	 :
		default			 :
			return  (m_part_animations.A[body_state].m_torso.A[slot].A[6].A[0]);
	}
}

MotionID CStalkerAnimationManager::assign_torso_animation	()
{
	if (!object().inventory().ActiveItem())
		return		(no_object_animation(body_state()));

	fill_object_info();

	if (m_weapon)
		if (!strapped())
			return	(weapon_animation(object_slot(),body_state()));
		else
			return	(no_object_animation(body_state()));

	if (m_missile)
		return		(missile_animation(object_slot(),body_state()));

	return			(unknown_object_animation(object_slot(),body_state()));
}
