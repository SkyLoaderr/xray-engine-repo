////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_animation_pair.cpp
//	Created 	: 25.02.2003
//  Modified 	: 19.11.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker animation pair
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_animation_pair.h"
#include "../motion.h"
#include "ai_debug.h"
#include "ai/stalker/ai_stalker.h"
#include "ai/ai_monsters_anims.h"

void CStalkerAnimationPair::synchronize		(const CStalkerAnimationPair &stalker_animation) const
{
	if (!blend())
		return;

	if (!(animation()->animation()->flags & esmSyncPart))
		return;

	if (!stalker_animation.blend())
		return;

	if (!(stalker_animation.animation()->animation()->flags & esmSyncPart))
		return;

	blend()->timeCurrent	= stalker_animation.blend()->timeCurrent;
}

void CStalkerAnimationPair::play			(CSkeletonAnimated *skeleton_animated, PlayCallback callback, CAI_Stalker *object)
{
	VERIFY					(animation());
	if (actual())
		return;

	m_blend					= skeleton_animated->PlayCycle(animation()->animation(),TRUE,callback,object);
	m_actual				= true;

	if (m_step_dependence)
		object->CStepManager::on_animation_start(animation()->name());

#ifdef DEBUG
	if (psAI_Flags.is(aiAnimation))
		Msg					("%6d [%s][%s][%s]",Device.dwTimeGlobal,m_object_name,m_animation_type_name,*animation()->name());
#endif
}
