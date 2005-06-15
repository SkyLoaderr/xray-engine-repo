#pragma once

#include "control_combase.h"
#include "../../../SkeletonAnimated.h"

struct SControlAnimationData : public ControlCom::IComData {
	MotionID			motion;
	bool				start_animation;
	float				speed;
};

class CControlAnimation : public CControl_ComPure<SControlAnimationData> {
	typedef CControl_ComPure<SControlAnimationData> inherited;

	CBlend					*blend;

	u32						time_started;
	
	struct {
		float				current;
		float				target;
	} speed;

	float					acceleration;				// todo: find out if needed

	u16						default_bone_part;

#ifdef DEBUG
	shared_str				name;	
#endif

public:
	virtual void	reinit				();
	virtual void	update_frame		();

			void	on_animation_end	();	
			CBlend	*current_blend		() {return blend;}

private:
	void	update				();
	void	play				();
};


