////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_animation_manager.cpp
//	Created 	: 25.02.2003
//  Modified 	: 19.11.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker animation manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_animation_manager.h"
#include "ai/stalker/ai_stalker.h"
#include "../skeletonanimated.h"
#include "stalker_movement_manager.h"
#include "entitycondition.h"

#include "inventory.h"
#include "missile.h"

void CStalkerAnimationManager::reinit				()
{
	m_animation_switch_interval	= 500;
	m_direction_start_time		= 0;
	m_movement_direction		= eMovementDirectionForward;
	m_desirable_direction		= eMovementDirectionForward;
	
	m_script_animations.clear	();
	
	m_legs.step_dependence		(true);
	m_global.step_dependence	(true);
	m_script.step_dependence	(true);
	
	m_global.global_animation	(true);
	m_script.global_animation	(true);
	
	m_storage					= 0;
}

void CStalkerAnimationManager::reload				(CAI_Stalker *_object)
{
	m_object					= _object;
	m_visual					= object().Visual();

	m_skeleton_animated			= smart_cast<CSkeletonAnimated*>(m_visual);
	VERIFY						(m_skeleton_animated);

	m_part_animations.Load		(m_skeleton_animated,"");
	m_head_animations.Load		(m_skeleton_animated,"");
	m_global_animations.Load	(m_skeleton_animated,"item_");
	
	if (!object().g_Alive())
		return;

	assign_bone_callbacks		();

#ifdef DEBUG
	global().set_dbg_info		(*object().cName(),"Global");
	head().set_dbg_info			(*object().cName(),"Head  ");
	torso().set_dbg_info		(*object().cName(),"Torso ");
	legs().set_dbg_info			(*object().cName(),"Legs  ");
	script().set_dbg_info		(*object().cName(),"Script");
#endif
};

CStalkerAnimationManager::EBodyState CStalkerAnimationManager::body_state() const
{
	return						(((eBodyStateStand == object().movement().body_state()) && object().conditions().IsLimping()) ? eBodyStateStandDamaged : object().movement().body_state());
}

void CStalkerAnimationManager::play_fx(float power_factor, int fx_index)
{
	m_skeleton_animated->PlayFX	(m_part_animations.A[object().movement().body_state()].m_global.A[0].A[fx_index].animation(),power_factor);
}

void CStalkerAnimationManager::update						()
{
	if (!object().g_Alive())
		return;

	if (!script_animations().empty()) {
		global().reset		();
		head().reset		();
		torso().reset		();
		legs().reset		();
		script().animation	(assign_script_animation());
		script().play		(m_skeleton_animated,script_play_callback,&object());
		return;
	}
	
	script().reset			();

	const CAnimationPair	*global_animation = assign_global_animation();
	if (global_animation) {
		head().reset		();
		torso().reset		();
		legs().reset		();
		global().animation	(global_animation);
		global().play		(m_skeleton_animated,global_play_callback,&object());
		return;
	}

	global().reset			();

	head().animation		(assign_head_animation());
	head().play				(m_skeleton_animated,head_play_callback,&object());

	torso().animation		(assign_torso_animation());
	torso().play			(m_skeleton_animated,torso_play_callback,&object());

	legs().animation		(assign_legs_animation());
	legs().play				(m_skeleton_animated,legs_play_callback,&object());

	VERIFY					(head().animation());

	if (!torso().animation()) {
		Msg					("TORSO ANIMATION BUG MINI DUMP");
		
		if (!object().inventory().ActiveItem())
			Msg				("no active item");
		else
			Msg				("active item [%s][%s]",*object().inventory().ActiveItem()->cName(),*object().inventory().ActiveItem()->cNameSect());
		
		if (m_weapon) {
			Msg				("active item is weapon");
			if (strapped())
				Msg			("weapon is strapped");
			else
				Msg			("weapon is not strapped");
		}
		else
			if (m_missile)
				Msg			("active item is missile");
			else
				Msg			("active item is not a weapon nor a missile");

		Msg					("object slot is %d",object_slot());

		switch (body_state()) {
			case eBodyStateStand : {
				Msg			("body state is STAND");
				break;
			}
			case eBodyStateCrouch : {
				Msg			("body state is CROUCH");
				break;
			}
			case eBodyStateStandDamaged : {
				Msg			("body state is STAND_DAMAGED");
				break;
			}
			default : {
				Msg			("body state is INVALID");
				break;
			}
		}

		Msg					("stalker name is %s",*m_object->cName());
		Msg					("stalker section is %s",*m_object->cNameSect());
		Msg					("stalker visual is %s",*m_object->cNameVisual());

		if (m_missile) {
			switch (m_missile->State()) {
				case MS_SHOWING	 :
					Msg				("norm_torso_%d_draw_0",object_slot());
					break;
				case MS_HIDING	 :
					Msg				("norm_torso_%d_holster_0",object_slot());
					break;
				case MS_THREATEN :
					Msg				("norm_torso_%d_attack_0",object_slot());
					break;
				case MS_READY	 :
					Msg				("norm_torso_%d_attack_1",object_slot());
					break;
				case MS_THROW	 :
					Msg				("norm_torso_%d_attack_2",object_slot());
					break;
				case MS_END		 :
					Msg				("norm_torso_%d_attack_2",object_slot());
					break;
				case MS_PLAYING	 :
					Msg				("norm_torso_%d_attack_2",object_slot());
					break;
				case MS_IDLE	 :
				case MS_HIDDEN	 :
				case MS_EMPTY	 :
				default			 :
					Msg				("norm_torso_%d_aim_0",object_slot());
			}
		}

		FlushLog			();
	}

	VERIFY					(torso().animation());
	VERIFY					(legs().animation());

	torso().synchronize		(m_legs);
}
