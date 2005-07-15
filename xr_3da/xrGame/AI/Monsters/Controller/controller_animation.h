#pragma once

#include "../control_animation_base.h"
#include "../ai_monster_defs.h"
#include "../../../../SkeletonAnimated.h"

class CController;

class CControllerAnimation : public CControlAnimationBase {
	typedef CControlAnimationBase inherited;

	CController	*m_controller;
	
	enum ELegsActionType {
		eLegsStand	= u32(0),
		eLegsRun,
		eLegsWalk,
		eLegsBackRun,
		eLegsRunFwdLeft,
		eLegsRunFwdRight,
		eLegsRunBkwdLeft,
		eLegsRunBkwdRight,
		eLegsSteal,
		
		eLegsStandDamaged,
		eLegsRunDamaged,
		eLegsWalkDamaged,
		eLegsBackRunDamaged,
		eLegsRunStrafeLeftDamaged,
		eLegsRunStrafeRightDamaged,

		eLegsUndefined = u32(-1),
	};

	enum ETorsoActionType {
		eTorsoIdle,
		eTorsoDanger,
		eTorsoPsyAttack,
		eTorsoPanic
	};

	ELegsActionType		m_current_legs_action;
	ELegsActionType		m_current_torso_action;

	DEFINE_MAP			(ELegsActionType,	MotionID, LEGS_MOTION_MAP,	LEGS_MOTION_MAP_IT);
	DEFINE_MAP			(ETorsoActionType,	MotionID, TORSO_MOTION_MAP, TORSO_MOTION_MAP_IT);
	
	LEGS_MOTION_MAP		m_legs;
	TORSO_MOTION_MAP	m_torso;

	struct SPathRotations{
		float			angle;
		ELegsActionType	legs_motion;
	};
	
	DEFINE_VECTOR		(SPathRotations, PATH_ROTATIONS_VEC, PATH_ROTATIONS_VEC_IT);
	DEFINE_MAP			(EAction, PATH_ROTATIONS_VEC, PATH_ROTATIONS_MAP, PATH_ROTATIONS_MAP_IT);
	PATH_ROTATIONS_MAP	m_path_rotations;


public:	
	virtual void		reinit				();
	virtual void		on_event			(ControlCom::EEventType, ControlCom::IEventData*);	
	virtual void		on_start_control	(ControlCom::EControlType type);
	virtual void		on_stop_control		(ControlCom::EControlType type);
	virtual void		update_frame		();

			// load
			void		load				();
			void		add_path_rotation	(EAction action, float angle, ELegsActionType type);
			
			
			void		select_velocity		();
			void		set_direction		();

			void		select_torso_animation	();
			void		select_legs_animation	();

			SPathRotations	get_path_rotation	(float cur_yaw);
};
