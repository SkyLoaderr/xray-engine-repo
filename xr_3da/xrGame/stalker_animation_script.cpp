////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_animation_script.cpp
//	Created 	: 25.02.2003
//  Modified 	: 19.11.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker animation manager : script animations
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_animation_manager.h"
#include "ai/stalker/ai_stalker.h"
#include "script_engine.h"
#include "game_object_space.h"
#include "script_callback_ex.h"

void	CStalkerAnimationManager::script_play_callback(CBlend *blend)
{
	CAI_Stalker					*object = (CAI_Stalker*)blend->CallbackParam;
	VERIFY						(object);
	CStalkerAnimationManager	&animation_manager = object->animation();
	if	(
			animation_manager.script().animation() && 
			!animation_manager.script_animations().empty() && 
			(
				animation_manager.script().animation() == 
				animation_manager.script_animations().front().animation()
			)
		)
		animation_manager.pop_script_animation();
	object->callback(GameObject::eScriptAnimation)();
}

void CStalkerAnimationManager::add_script_animation	(LPCSTR animation, bool hand_usage)
{
	CMotionDef						*motion = m_skeleton_animated->ID_Cycle_Safe(animation);
	if (!motion) {
		ai().script_engine().script_log(eLuaMessageTypeError,"There is no animation %s (object %s)!",animation,*object().cName());
		return;
	}
	m_script_animations.push_back	(CStalkerAnimationScript(hand_usage,CAnimationPair(motion,shared_str(animation))));
}

const CAnimationPair *CStalkerAnimationManager::assign_script_animation	()
{
	VERIFY				(!script_animations().empty());
	return				(script_animations().front().animation());
}
