////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_animation_manager_update.cpp
//	Created 	: 25.02.2003
//  Modified 	: 13.12.2006
//	Author		: Dmitriy Iassenev
//	Description : Stalker animation manager update cycle
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_animation_manager.h"
#include "ai/stalker/ai_stalker.h"
#include "game_object_space.h"
#include "script_callback_ex.h"
#include "profiler.h"
#include "stalker_movement_manager.h"

IC	void CStalkerAnimationManager::play_delayed_callbacks	()
{
	if (!m_call_script_callback)
		return;

	m_call_script_callback	= false;
	object().callback		(GameObject::eScriptAnimation)	();
}

IC	bool CStalkerAnimationManager::script_callback			() const
{
	if (script_animations().empty())
		return				(false);
	
	return					(object().callback(GameObject::eScriptAnimation));
}

IC	bool CStalkerAnimationManager::need_update				() const
{
	if (script_callback())
		return				(true);

	return					(non_script_need_update());
}

IC	void CStalkerAnimationManager::update_tracks			()
{
	if (!need_update())
		return;

	m_skeleton_animated->UpdateTracks	();
}

#ifdef USE_HEAD_BONE_PART_FAKE
IC	void CStalkerAnimationManager::play_script_impl			()
{
	global().reset			();
	torso().reset			();
	legs().reset			();

	script().animation		(assign_script_animation());
	script().play			(m_skeleton_animated,script_play_callback,&object(),false,m_script_bone_part_mask);

	head().animation		(assign_head_animation());
	head().play				(m_skeleton_animated,head_play_callback,&object());
}
#else // USE_HEAD_BONE_PART_FAKE
IC	void CStalkerAnimationManager::play_script_impl			()
{
	global().reset			();
	head().reset			();
	torso().reset			();
	legs().reset			();

	script().animation		(assign_script_animation());
	script().play			(m_skeleton_animated,script_play_callback,&object());
}
#endif // USE_HEAD_BONE_PART_FAKE

bool CStalkerAnimationManager::play_script					()
{
	if (script_animations().empty()) {
		script().reset		();
		return				(false);
	}

	play_script_impl		();

	return					(true);
}

#ifdef USE_HEAD_BONE_PART_FAKE
IC	void CStalkerAnimationManager::play_global_impl			(const MotionID &animation)
{
	torso().reset			();
	legs().reset			();

	global().animation		(animation);
	global().play			(m_skeleton_animated,global_play_callback,&object(),false,m_script_bone_part_mask);

	head().animation		(assign_head_animation());
	head().play				(m_skeleton_animated,head_play_callback,&object());
}
#else // USE_HEAD_BONE_PART_FAKE
IC	void CStalkerAnimationManager::play_global_impl			(const MotionID &animation)
{
	head().reset			();
	torso().reset			();
	legs().reset			();

	global().animation		(animation);
	global().play			(m_skeleton_animated,global_play_callback,&object());
}
#endif // USE_HEAD_BONE_PART_FAKE

bool CStalkerAnimationManager::play_global					()
{
	const MotionID			&global_animation = assign_global_animation();
	if (!global_animation) {
		global().reset		();
		return				(false);
	}

	play_global_impl		(global_animation);

	return					(true);
}

IC	void CStalkerAnimationManager::play_head				()
{
	head().animation		(assign_head_animation());
	head().play				(m_skeleton_animated,head_play_callback,&object());
}

IC	void CStalkerAnimationManager::play_torso				()
{
	torso().animation		(assign_torso_animation());
	torso().play			(m_skeleton_animated,torso_play_callback,&object());
}

void CStalkerAnimationManager::play_legs					()
{
	float					speed = 0.f;
	bool					first_time = !legs().animation();
	bool					result = legs().animation(assign_legs_animation());
	
	if (!first_time && !result && legs().blend()) {
		float				amount = legs().blend()->blendAmount;
		m_previous_speed	= (m_current_speed - m_previous_speed)*amount + m_previous_speed;
	}

	legs().play				(m_skeleton_animated,legs_play_callback,&object(),!fis_zero(m_current_speed));
	
	if (result && legs().blend()) {
		float				amount = legs().blend()->blendAmount;
		speed				= (m_current_speed - m_previous_speed)*amount + m_previous_speed;
	}

	if (fis_zero(speed))
		return;
	
	if (!legs().blend())
		return;

	object().movement().setup_speed_from_animation	(speed);
}

void CStalkerAnimationManager::update_impl					()
{
	if (!object().g_Alive())
		return;

	play_delayed_callbacks	();
	update_tracks			();

	if (play_script())
		return;

	if (play_global())
		return;

	play_head				();
	play_torso				();
	play_legs				();

	torso().synchronize		(m_skeleton_animated,m_legs);
}

void CStalkerAnimationManager::update						()
{
	START_PROFILE("stalker/client_update/animations")
	try {
		update_impl			();
	}
	catch(...) {
		Msg					("! error in stalker with visual %s",*object().cNameVisual());
		throw;
	}
	STOP_PROFILE
}