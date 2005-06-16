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
#include "weapon.h"
#include "object_handler_planner.h"
#include "object_handler_space.h"
#include "game_object_space.h"
#include "script_callback_ex.h"

void CStalkerAnimationManager::reinit				()
{
	m_animation_switch_interval	= 500;
	m_direction_start_time		= 0;
	m_movement_direction		= eMovementDirectionForward;
	m_desirable_direction		= eMovementDirectionForward;
	
	m_script_animations.clear	();

	m_global.reset				();
	m_head.reset				();
	m_torso.reset				();
	m_legs.reset				();
	m_script.reset				();
	
	m_legs.step_dependence		(true);
	m_global.step_dependence	(true);
	m_script.step_dependence	(true);
	
	m_global.global_animation	(true);
	m_script.global_animation	(true);
	
	m_storage					= 0;

	m_call_script_callback		= false;
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

#ifdef USE_HEAD_BONE_PART_FAKE
	VERIFY						(!m_head_animations.A.empty());
	u16							bone_part = m_skeleton_animated->LL_GetMotionDef(m_head_animations.A.front())->bone_or_part;
	VERIFY						(bone_part != BI_NONE);
	m_script_bone_part_mask		= CStalkerAnimationPair::all_bone_parts ^ (1 << bone_part);
#endif

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
	m_skeleton_animated->PlayFX	(m_part_animations.A[object().movement().body_state()].m_global.A[0].A[fx_index],power_factor);
}

void CStalkerAnimationManager::update						()
{
	try {
		if (m_call_script_callback) {
			m_call_script_callback	= false;
			object().callback(GameObject::eScriptAnimation)();
		}

		if (!object().g_Alive())
			return;

		if (!script_animations().empty()) {
			global().reset		();
#ifndef USE_HEAD_BONE_PART_FAKE
			head().reset		();
#endif
			torso().reset		();
			legs().reset		();
			script().animation	(assign_script_animation());
#ifndef USE_HEAD_BONE_PART_FAKE
			script().play		(m_skeleton_animated,script_play_callback,&object());
#else
			script().play		(m_skeleton_animated,script_play_callback,&object(),m_script_bone_part_mask);
			head().animation	(assign_head_animation());
			head().play			(m_skeleton_animated,head_play_callback,&object());
#endif
			return;
		}
		
		script().reset			();

		const MotionID			&global_animation = assign_global_animation();
		if (global_animation) {
#ifndef USE_HEAD_BONE_PART_FAKE
			head().reset		();
#endif
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
		VERIFY					(torso().animation());
		VERIFY					(legs().animation());

		torso().synchronize		(m_skeleton_animated,m_legs);
	}
	catch(...) {
		Msg						("! error in stalker with visual %s",*object().cNameVisual());
		throw;
	}
}
