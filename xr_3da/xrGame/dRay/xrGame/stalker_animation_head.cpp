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
#include "ai/stalker/ai_stalker_space.h"
#include "sound_player.h"
#include "stalker_animation_data.h"

void CStalkerAnimationManager::head_play_callback		(CBlend *blend)
{
	CAI_Stalker		*object = (CAI_Stalker*)blend->CallbackParam;
	VERIFY			(object);
	object->animation().head().make_inactual();
}

MotionID CStalkerAnimationManager::assign_head_animation	()
{
	CSoundPlayer	&sound = object().sound();
	if (!sound.active_sound_count(true))
		return		(m_data_storage->m_head_animations.A[0]);

	if (!sound.active_sound_type((u32)StalkerSpace::eStalkerSoundMaskMovingInDanger))
		return		(m_data_storage->m_head_animations.A[1]);

	return			(m_data_storage->m_head_animations.A[0]);
}
