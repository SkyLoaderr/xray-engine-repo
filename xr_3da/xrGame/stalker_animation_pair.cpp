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

void CStalkerAnimationPair::synchronize		(CKinematicsAnimated *skeleton_animated, const CStalkerAnimationPair &stalker_animation) const
{
	if (!blend())
		return;

	CMotionDef				*motion0 = skeleton_animated->LL_GetMotionDef(animation());
	VERIFY					(motion0);
	if (!(motion0->flags & esmSyncPart))
		return;

	if (!stalker_animation.blend())
		return;

	CMotionDef				*motion1 = skeleton_animated->LL_GetMotionDef(stalker_animation.animation());
	VERIFY					(motion1);
	if (!(motion1->flags & esmSyncPart))
		return;

	blend()->timeCurrent	= stalker_animation.blend()->timeCurrent;
}

#ifndef USE_HEAD_BONE_PART_FAKE
void CStalkerAnimationPair::play_global_animation	(CKinematicsAnimated *skeleton_animated, PlayCallback callback, CAI_Stalker *object)
#else
void CStalkerAnimationPair::play_global_animation	(CKinematicsAnimated *skeleton_animated, PlayCallback callback, CAI_Stalker *object, const u32 &bone_part)
#endif
{
	m_blend				= 0;
	for (u16 i=0; i<MAX_PARTS; ++i) {
#ifdef USE_HEAD_BONE_PART_FAKE
		if (!(bone_part & (1 << i)))
			continue;
#endif

		CBlend			*blend = 0;
		if (!m_blend)
			blend		= skeleton_animated->LL_PlayCycle(i,animation(),TRUE,callback,object);
		else
			skeleton_animated->LL_PlayCycle(i,animation(),TRUE,0,0);

		if (blend && !m_blend)
			m_blend		= blend;
	}
}

#ifndef USE_HEAD_BONE_PART_FAKE
void CStalkerAnimationPair::play			(CKinematicsAnimated *skeleton_animated, PlayCallback callback, CAI_Stalker *object)
#else
void CStalkerAnimationPair::play			(CKinematicsAnimated *skeleton_animated, PlayCallback callback, CAI_Stalker *object, const u32 &bone_part)
#endif
{
	VERIFY					(animation());
	if (actual()) {
#if 0
#	ifdef DEBUG
		if (psAI_Flags.is(aiAnimation) && blend())
			Msg				("%6d [%s][%s][%s][%f]",Device.dwTimeGlobal,m_object_name,m_animation_type_name,*animation()->name(),blend()->timeCurrent);
#	endif
#endif
		return;
	}

	if (!global_animation()) {
		float				pos = 0.f;
		VERIFY				(!m_blend || !fis_zero(m_blend->timeTotal));
		if (m_step_dependence && m_blend)
			pos				= fmod(m_blend->timeCurrent,m_blend->timeTotal)/m_blend->timeTotal;
		m_blend				= skeleton_animated->PlayCycle(animation(),TRUE,callback,object);
		if (m_step_dependence) {
//			if (we were standing and now we are moving)
//				pos						= 0.5f*Random.randI(2);
			if (m_blend)
				m_blend->timeCurrent = m_blend->timeTotal*pos;
		}
	}
	else
#ifndef USE_HEAD_BONE_PART_FAKE
		play_global_animation	(skeleton_animated,callback,object);
#else
		play_global_animation	(skeleton_animated,callback,object,bone_part);
#endif
	m_actual				= true;

	if (m_step_dependence)
		object->CStepManager::on_animation_start(animation(),blend());

#ifdef DEBUG
	if (psAI_Flags.is(aiAnimation)) {
		CMotionDef			*motion = skeleton_animated->LL_GetMotionDef(animation());
		VERIFY				(motion);
		LPCSTR				name = skeleton_animated->LL_MotionDefName_dbg(animation());
		Msg					("%6d [%s][%s][%s][%d] - LOOPED: %d",Device.dwTimeGlobal,m_object_name,m_animation_type_name,name,motion->bone_or_part,! (motion->flags & esmStopAtEnd));
	}
#endif
}
