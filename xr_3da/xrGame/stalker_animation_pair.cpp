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

void CStalkerAnimationPair::synchronize		(const CStalkerAnimationPair &stalker_animation) const
{
	if (!blend())
		return;

	if (!(animation()->flags & esmSyncPart))
		return;

	if (!stalker_animation.blend())
		return;

	if (!(stalker_animation.animation()->flags & esmSyncPart))
		return;

	blend()->timeCurrent	= stalker_animation.blend()->timeCurrent;
}

void CStalkerAnimationPair::play			(CSkeletonAnimated *skeleton_animated, PlayCallback callback, CAI_Stalker *object)
{
	VERIFY					(animation());
	if (actual())
		return;

	m_blend					= skeleton_animated->PlayCycle(animation(),TRUE,callback,object);
	m_actual				= true;

#ifdef DEBUG
	if (psAI_Flags.is(aiAnimation))
		Msg					("%6d [%s][%s][%s]",Device.dwTimeGlobal,m_object_name,m_animation_type_name,m_skeleton_animated->LL_MotionDefName_dbg(animation()));
#endif
}
