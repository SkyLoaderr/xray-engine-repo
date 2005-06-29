#pragma once

#include "control_combase.h"
#include "../../../SkeletonAnimated.h"

struct SAnimationPart {
	MotionID		motion;
	CBlend			*blend;
	bool			actual;
	u32				time_started;
	
	void	init	() {
		motion.invalidate	();
		blend				= 0;
		actual				= true;
		time_started		= 0;
	}
};

struct SControlAnimationData : public ControlCom::IComData {
	float				speed;
	
	SAnimationPart		global;
	SAnimationPart		legs;
	SAnimationPart		torso;
};

class CControlAnimation : public CControl_ComPure<SControlAnimationData> {
	typedef CControl_ComPure<SControlAnimationData> inherited;

	CSkeletonAnimated		*m_skeleton_animated;

#ifdef DEBUG
	shared_str				name;	
#endif

public:
	virtual void	reinit					();
	virtual void	update_frame			();

			void	on_global_animation_end	();	
			void	on_torso_animation_end	();
			void	on_legs_animation_end	();

			CBlend	*current_blend			() {return m_data.global.blend;}

private:
	void	play				();
	void	play_part			(SAnimationPart &part, PlayCallback callback);
};


