////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_animation_pair.h
//	Created 	: 25.02.2003
//  Modified 	: 19.11.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker animation pair
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../skeletonanimated.h"

class CBlend;
class CAI_Stalker;

class CStalkerAnimationPair {
private:
	MotionID				m_animation;
	CBlend					*m_blend;
	bool					m_actual;
	bool					m_step_dependence;
	bool					m_global_animation;

private:
#ifdef DEBUG
	LPCSTR					m_object_name;
	LPCSTR					m_animation_type_name;
#endif

protected:
			void			play_global_animation	(CSkeletonAnimated *skeleton_animated, PlayCallback callback, CAI_Stalker *object);

public:
	IC						CStalkerAnimationPair	();
	IC		void			reset					();
			void			synchronize				(CSkeletonAnimated *skeleton_animated, const CStalkerAnimationPair &stalker_animation_pair) const;
	IC		bool			actual					() const;
	IC		void			animation				(const MotionID &animation);
	IC		const MotionID	&animation				() const;
	IC		CBlend			*blend					() const;
	IC		void			step_dependence			(bool value);
	IC		bool			step_dependence			() const;
	IC		void			global_animation		(bool global_animation);
	IC		bool			global_animation		() const;
	IC		void			make_inactual			();
			void			play					(CSkeletonAnimated *skeleton_animated, PlayCallback callback, CAI_Stalker *object);
#ifdef DEBUG
	IC		void			set_dbg_info			(LPCSTR object_name, LPCSTR animation_type_name);
#endif
};

#include "stalker_animation_pair_inline.h"