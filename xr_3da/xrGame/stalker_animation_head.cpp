////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_animation_head.cpp
//	Created 	: 25.02.2003
//  Modified 	: 19.11.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker animation manager : head animations
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_animation_manager.h"
#include "ai/stalker/ai_stalker.h"
#include "sound_player.h"

void CStalkerAnimationManager::head_play_callback		(CBlend *blend)
{
	CAI_Stalker	*object = (CAI_Stalker*)blend->CallbackParam;
	VERIFY		(object);
	object->animation().head().make_inactual();
}

MotionID CStalkerAnimationManager::assign_head_animation	()
{
	if (object().sound().active_sound_count(true))
		return	(m_head_animations.A[1]);
	else
		return	(m_head_animations.A[0]);
}
