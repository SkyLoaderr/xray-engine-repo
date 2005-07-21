#pragma once
#include "control_combase.h"
#include "../../../SkeletonAnimated.h"

class CControlRotationJump : public CControl_ComCustom<> {
	u32				m_time_next_rotation_jump;

	float			m_target_velocity;
	float			m_start_velocity;
	float			m_accel;
	float			m_dist;
	float			m_time;

	MotionID		m_anim_stop_ls,m_anim_run_ls;
	MotionID		m_anim_stop_rs,m_anim_run_rs;

	bool			m_right_side;

	enum EStage {
		eStop,
		eRun, 
		eNone
	} m_stage;

	CSkeletonAnimated	*m_skeleton_animated;

public:
	virtual void	reinit					();

	virtual void	on_event				(ControlCom::EEventType, ControlCom::IEventData*);
	virtual void	activate				();
	virtual void	on_release				();
	virtual bool	check_start_conditions	();

private:	
			void	build_line_first		();
			void	build_line_second		();
};

