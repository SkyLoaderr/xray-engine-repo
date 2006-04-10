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
#include "stalker_animation_data_storage.h"
#include "stalker_animation_data.h"
#include "profiler.h"
#include "stalker_animation_manager_impl.h"

void CStalkerAnimationManager::reinit				()
{
	m_direction_start			= 0;
	m_current_direction			= eMovementDirectionForward;
	m_target_direction			= eMovementDirectionForward;

	m_change_direction_time		= 0;
	m_looking_back				= 0;

	m_no_move_actual			= false;
	m_crouch_state				= 0;
	
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

	m_previous_speed			= 0.f;
	m_current_speed				= 0.f;
}

void CStalkerAnimationManager::reload				(CAI_Stalker *_object)
{
	m_object					= _object;
	m_visual					= object().Visual();

	if (object().already_dead())
		return;

	m_skeleton_animated			= smart_cast<CKinematicsAnimated*>(m_visual);
	VERIFY						(m_skeleton_animated);

	m_data_storage				= stalker_animation_data_storage().object(m_skeleton_animated);
	VERIFY						(m_data_storage);

	if (!object().g_Alive())
		return;

#ifdef USE_HEAD_BONE_PART_FAKE
	VERIFY						(!m_data_storage->m_head_animations.A.empty());
	u16							bone_part = m_skeleton_animated->LL_GetMotionDef(m_data_storage->m_head_animations.A.front())->bone_or_part;
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

void CStalkerAnimationManager::play_fx(float power_factor, int fx_index)
{
	VERIFY						(fx_index >= 0);
	VERIFY						(fx_index < (int)m_data_storage->m_part_animations.A[object().movement().body_state()].m_global.A[0].A.size());
#ifdef DEBUG
	if (psAI_Flags.is(aiAnimation)) {
		LPCSTR					name = m_skeleton_animated->LL_MotionDefName_dbg(m_data_storage->m_part_animations.A[object().movement().body_state()].m_global.A[0].A[fx_index]);
		Msg						("%6d [%s][%s][%s][%f]",Device.dwTimeGlobal,*object().cName(),"FX",name,power_factor);
	}
#endif
	m_skeleton_animated->PlayFX	(m_data_storage->m_part_animations.A[object().movement().body_state()].m_global.A[0].A[fx_index],power_factor);
}

void CStalkerAnimationManager::play_delayed_callbacks		()
{
	if (!m_call_script_callback)
		return;

	m_call_script_callback							= false;
	object().callback(GameObject::eScriptAnimation)	();
}

void CStalkerAnimationManager::update						()
{
	START_PROFILE("stalker/client_update/animations")
	try {
		play_delayed_callbacks	();

		if	(
				(
					!script_animations().empty() &&
					object().callback(GameObject::eScriptAnimation)
				) ||
				object().animation().setup_storage()
			)
				m_skeleton_animated->UpdateTracks	();

		if (!object().g_Alive())
			return;

		if (!script_animations().empty()) {
#if 0//def DEBUG
			if (setup_storage()) {
				Msg				("! Do not setup script animations while strapping/unstrapping");
				Msg				("! ERROR description :");
				Msg				("! object     %s",*object().cName());
				Msg				("! item       %s",object().inventory().ActiveItem() ? *object().inventory().ActiveItem()->object().cName() : "no active item");
				Msg				("! strapped   %s",object().weapon_strapped() ? "true" : "false");
				Msg				("! unstrapped %s",object().weapon_unstrapped() ? "true" : "false");
				Msg				("! animation  %s",m_skeleton_animated->LL_MotionDefName_dbg(script_animations().front().animation()));
				Msg				("! animations %d",script_animations().size());
			}
			VERIFY2				(!setup_storage(),"Do not setup script animations while strapping/unstrapping");
#endif

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
#ifndef USE_HEAD_BONE_PART_FAKE
			global().play		(m_skeleton_animated,global_play_callback,&object());
#else
			global().play		(m_skeleton_animated,global_play_callback,&object(),m_script_bone_part_mask);
			head().animation	(assign_head_animation());
			head().play			(m_skeleton_animated,head_play_callback,&object());
#endif
			return;
		}

		global().reset			();

		head().animation		(assign_head_animation());
		head().play				(m_skeleton_animated,head_play_callback,&object());

		torso().animation		(assign_torso_animation());
		torso().play			(m_skeleton_animated,torso_play_callback,&object());

		float					speed = 0.f;
		bool					first_time = !legs().animation();
		bool					result = legs().animation(assign_legs_animation());
		
		if (!first_time && !result && legs().blend()) {
			float				amount = legs().blend()->blendAmount;
			m_previous_speed	= (m_current_speed - m_previous_speed)*amount + m_previous_speed;
		}

		legs().play				(m_skeleton_animated,legs_play_callback,&object());
		
		if (result && legs().blend()) {
			float				amount = legs().blend()->blendAmount;
			speed				= (m_current_speed - m_previous_speed)*amount + m_previous_speed;
		}

		if (!fis_zero(speed) && legs().blend())
			object().movement().setup_speed_from_animation	(speed);

		VERIFY					(head().animation());
		VERIFY					(torso().animation());
		VERIFY					(legs().animation());

		torso().synchronize		(m_skeleton_animated,m_legs);
	}
	catch(...) {
		Msg						("! error in stalker with visual %s",*object().cNameVisual());
		throw;
	}
	STOP_PROFILE
}
