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

struct SAnimationSignalEventData : public ControlCom::IEventData {
	MotionID	motion;
	float		time_perc;
	u32			event_id;
	IC			SAnimationSignalEventData(MotionID m, float perc, u32 id) : time_perc(perc), event_id(id), motion(m) {}
};


class CControlAnimation : public CControl_ComPure<SControlAnimationData> {
	typedef CControl_ComPure<SControlAnimationData> inherited;

	CSkeletonAnimated		*m_skeleton_animated;
	
	// animation events
	struct SAnimationEvent{
		float	time_perc;
		u32		event_id;
		bool	handled;
	};

	DEFINE_VECTOR			(SAnimationEvent, ANIMATION_EVENT_VEC, ANIMATION_EVENT_VEC_IT);
	DEFINE_MAP				(MotionID, ANIMATION_EVENT_VEC, ANIMATION_EVENT_MAP, ANIMATION_EVENT_MAP_IT);
	ANIMATION_EVENT_MAP		m_anim_events;

public:
	
	bool					m_global_animation_end;
	bool					m_legs_animation_end;
	bool					m_torso_animation_end;

public:
	virtual void	reinit					();
	virtual void	update_frame			();
	virtual	void	reset_data				();
				
			void	add_anim_event			(MotionID, float, u32);

			CBlend	*current_blend			() {return m_data.global.blend;}

private:
	void	play				();
	void	play_part			(SAnimationPart &part, PlayCallback callback);
	void	check_events		(SAnimationPart &part);
	void	check_callbacks		();

public:
	enum EAnimationEventType {
		eAnimationHit		= u32(0),
		eAnimationCustom
	};
};


