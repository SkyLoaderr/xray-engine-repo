#pragma once
#include "control_combase.h"
#include "../../../SkeletonAnimated.h"

struct SControlJumpData : public ControlCom::IComData {
	CObject					*target_object;
	u32						velocity_mask;
	Fvector					target_position;
	bool					skip_prepare;
	bool					play_glide_once;
	MotionID				pool[3];
};

class CControlJump : public CControl_ComCustom<SControlJumpData> {
	typedef CControl_ComCustom<SControlJumpData> inherited;

	enum EStateAnimJump {
		eStatePrepare,
		eStateGlide,
		eStateGround,
		eStateNone
	};

	// loadable parameters
	u32				m_delay_after_jump;
	float			m_jump_factor;
	float			m_trace_ground_range;
	float			m_hit_trace_range;
	float			m_build_line_distance;
	float			m_min_distance;
	float			m_max_distance;
	float			m_max_angle;

	// run-time params
	u32				m_time_next_allowed;
	u32				m_time_started;
	float			m_jump_time;
	float			m_blend_speed;
	Fvector			m_target_position;


	// state flags
	bool			m_object_hitted;
	bool			m_velocity_bounced;
	bool			m_use_prediction;
	bool			m_enable_bounce;

	// animation
	EStateAnimJump	m_anim_state_prev;
	EStateAnimJump	m_anim_state_current;

public:
	virtual void	load					(LPCSTR section);
	virtual void	reinit					();
	virtual bool	check_start_conditions	();
	virtual void	activate				();
	virtual void	on_release				();
	virtual void	on_event				(ControlCom::EEventType, ControlCom::IEventData*);


	// process jump
	virtual void	update_frame			();

	// check for distance and angle difference
	virtual	bool	can_jump				(CObject *target);

	// stop/break jump and all of jumping states
	virtual void	stop					();

			void	disable_bounce			() {m_enable_bounce = false;}

SControlJumpData	&setup_data				() {return m_data;}

private:	
			// service routines		
			// build path after jump 
			void	build_line			();
			// get target position according to object center point
			Fvector get_target			(CObject *obj);
			// finalize jump
			void	pointbreak			();
			// check for hit object
			void	hit_test			();

			// check current jump state		
			bool	is_landing			();	
			bool	is_on_the_ground	();

			// position prediction
			Fvector	predict_position	(CObject *obj, const Fvector &pos);

			void	start_jump			(const Fvector &point);

			// animation control method
			void	select_next_anim_state	();
			void	play_selected			();
			void	on_start_jump			();
};
