////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_animation_pair.h
//	Created 	: 25.02.2003
//  Modified 	: 19.11.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker animation pair
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../skeletonanimated.h"

class CMotionDef;
class CBlend;
class CAI_Stalker;

class CStalkerAnimationPair {
private:
	CMotionDef			*m_animation;
	CBlend				*m_blend;
	bool				m_actual;
	bool				m_step_dependence;

private:
#ifdef DEBUG
	CSkeletonAnimated	*m_skeleton_animated;
	LPCSTR				m_object_name;
	LPCSTR				m_animation_type_name;
#endif

public:
	IC					CStalkerAnimationPair	();
	IC		void		reset					();
			void		synchronize				(const CStalkerAnimationPair &stalker_animation_pair) const;
	IC		bool		actual					() const;
	IC		void		animation				(CMotionDef	*animation);
	IC		CMotionDef	*animation				() const;
	IC		CBlend		*blend					() const;
	IC		void		step_dependence			(bool value);
	IC		bool		step_dependence			() const;
			void		play					(CSkeletonAnimated *skeleton_animated, PlayCallback callback, CAI_Stalker *object);
#ifdef DEBUG
	IC		void		set_dbg_info			(CSkeletonAnimated *skeleton_animated, LPCSTR object_name, LPCSTR animation_type_name);
#endif
};

#include "stalker_animation_pair_inline.h"