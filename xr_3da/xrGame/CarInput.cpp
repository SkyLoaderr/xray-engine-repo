#include "stdafx.h"
#pragma hdrstop

#include "car.h"
#include "actor.h"
#include "cameralook.h"
#include "camerafirsteye.h"
#include "ai_script_actions.h"

void	CCar::IR_OnMouseMove(int dx, int dy)
{
	if (Remote())					return;

	CCameraBase* C	= active_camera;
	float scale		= (C->f_fov/DEFAULT_FOV)*psMouseSens * psMouseSensScale/50.f;
	if (dx){
		float d		= float(dx)*scale;
		C->Move		((d<0)?kLEFT:kRIGHT, _abs(d));
	}
	if (dy){
		float d		= ((psMouseInvert.test(1))?-1:1)*float(dy)*scale*3.f/4.f;
		C->Move		((d>0)?kUP:kDOWN, _abs(d));
	}
}

bool CCar::bfAssignMovement(CEntityAction *tpEntityAction)
{
	if (tpEntityAction->m_tMovementAction.m_bCompleted)
		return(true);

	u32		l_tInput = tpEntityAction->m_tMovementAction.m_tInputKeys;

	vfProcessInputKey(kFWD		,	!!(l_tInput & CMovementAction::eInputKeyForward		));
	vfProcessInputKey(kBACK		,	!!(l_tInput & CMovementAction::eInputKeyBack		));
	vfProcessInputKey(kL_STRAFE	,	!!(l_tInput & CMovementAction::eInputKeyLeft		));
	vfProcessInputKey(kR_STRAFE	,	!!(l_tInput & CMovementAction::eInputKeyRight		));
	vfProcessInputKey(kACCEL	,	!!(l_tInput & CMovementAction::eInputKeyShiftUp		));
	vfProcessInputKey(kCROUCH	,	!!(l_tInput & CMovementAction::eInputKeyShiftDown	));
	vfProcessInputKey(kJUMP		,	!!(l_tInput & CMovementAction::eInputKeyBreaks		));
	if (!!(l_tInput & CMovementAction::eInputKeyEngineOn))	StartEngine();
	if (!!(l_tInput & CMovementAction::eInputKeyEngineOff)) StopEngine();

	return	(false);
}

void CCar::vfProcessInputKey	(int iCommand, bool bPressed)
{
	if (bPressed)
		IR_OnKeyboardPress	(iCommand);
	else
		IR_OnKeyboardRelease(iCommand);
}

void CCar::IR_OnKeyboardPress(int cmd)
{
	if (Remote())								return;

	switch (cmd)	
	{
	case kCAM_1:	OnCameraChange(ectFirst);	break;
	case kCAM_2:	OnCameraChange(ectChase);	break;
	case kCAM_3:	OnCameraChange(ectFree);	break;
	case kACCEL:	TransmisionUp();			break;
	case kCROUCH:	TransmisionDown();			break;
	case kFWD:		PressForward();				break;
	case kBACK:		PressBack();				break;
	case kR_STRAFE:	PressRight();	m_owner->steer_Vehicle(1);	break;
	case kL_STRAFE:	PressLeft();	m_owner->steer_Vehicle(-1);	break;
	case kJUMP:		PressBreaks();				break;
	case kWPN_FIRE: SwitchEngine();				break;
	case kUSE:									break;
	};

}

void	CCar::IR_OnKeyboardRelease(int cmd)
{
	if (Remote())								return;
	switch (cmd)	
	{
	case kACCEL:break;
	case kFWD:		ReleaseForward();			break;
	case kBACK:		ReleaseBack();				break;
	case kL_STRAFE:	ReleaseLeft();	m_owner->steer_Vehicle(0);	break;
	case kR_STRAFE:	ReleaseRight();	m_owner->steer_Vehicle(0);	break;
	case kJUMP:		ReleaseBreaks();			break;
	case kREPAIR:	m_repairing=false;			break;
	};
}

void	CCar::IR_OnKeyboardHold(int cmd)
{
	if (Remote())								return;

	switch(cmd)
	{
	case kCAM_ZOOM_IN: 
	case kCAM_ZOOM_OUT: 
	case kUP:
	case kDOWN:
	case kLEFT:
	case kRIGHT:	active_camera->Move(cmd);	break;
	case kREPAIR:	m_repairing=true;			break;
/*
	case kFWD:		
		if (ectFree==active_camera->tag)	active_camera->Move(kUP);
		else								m_vCamDeltaHP.y += active_camera->rot_speed.y*Device.fTimeDelta;
		break;
	case kBACK:		
		if (ectFree==active_camera->tag)	active_camera->Move(kDOWN);
		else								m_vCamDeltaHP.y -= active_camera->rot_speed.y*Device.fTimeDelta;
		break;
	case kL_STRAFE: 
		if (ectFree==active_camera->tag)	active_camera->Move(kLEFT);
		else								m_vCamDeltaHP.x -= active_camera->rot_speed.x*Device.fTimeDelta;
		break;
	case kR_STRAFE: 
		if (ectFree==active_camera->tag)	active_camera->Move(kRIGHT);
		else								m_vCamDeltaHP.x += active_camera->rot_speed.x*Device.fTimeDelta;
		break;
*/
	}
//	clamp(m_vCamDeltaHP.x, -PI_DIV_2,	PI_DIV_2);
//	clamp(m_vCamDeltaHP.y, active_camera->lim_pitch.x,	active_camera->lim_pitch.y);
}

