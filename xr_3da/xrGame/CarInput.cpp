#include "stdafx.h"
#pragma hdrstop

#include "car.h"
#include "actor.h"
#include "cameralook.h"
#include "camerafirsteye.h"
#include "script_entity_action.h"
#include "xr_level_controller.h"

void	CCar::OnMouseMove(int dx, int dy)
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

bool CCar::bfAssignMovement(CScriptEntityAction *tpEntityAction)
{
	if (tpEntityAction->m_tMovementAction.m_bCompleted)
		return(false);

	u32		l_tInput = tpEntityAction->m_tMovementAction.m_tInputKeys;

	vfProcessInputKey(kFWD		,	!!(l_tInput & CScriptMovementAction::eInputKeyForward		));
	vfProcessInputKey(kBACK		,	!!(l_tInput & CScriptMovementAction::eInputKeyBack		));
	vfProcessInputKey(kL_STRAFE	,	!!(l_tInput & CScriptMovementAction::eInputKeyLeft		));
	vfProcessInputKey(kR_STRAFE	,	!!(l_tInput & CScriptMovementAction::eInputKeyRight		));
	vfProcessInputKey(kACCEL	,	!!(l_tInput & CScriptMovementAction::eInputKeyShiftUp		));
	vfProcessInputKey(kCROUCH	,	!!(l_tInput & CScriptMovementAction::eInputKeyShiftDown	));
	vfProcessInputKey(kJUMP		,	!!(l_tInput & CScriptMovementAction::eInputKeyBreaks		));
	if (!!(l_tInput & CScriptMovementAction::eInputKeyEngineOn))	StartEngine();
	if (!!(l_tInput & CScriptMovementAction::eInputKeyEngineOff)) StopEngine();

	if (_abs(tpEntityAction->m_tMovementAction.m_fSpeed) > EPS_L)
		m_max_rpm = _abs(tpEntityAction->m_tMovementAction.m_fSpeed*m_current_gear_ratio);

	return	(true);
}

bool CCar::bfAssignObject(CScriptEntityAction *tpEntityAction)
{
	CScriptObjectAction	&l_tObjectAction = tpEntityAction->m_tObjectAction;
	if (l_tObjectAction.m_bCompleted || !xr_strlen(l_tObjectAction.m_caBoneName))
		return((l_tObjectAction.m_bCompleted = true) == false);

	s16	l_sBoneID = smart_cast<CKinematics*>(Visual())->LL_BoneID(l_tObjectAction.m_caBoneName);
	if (is_Door(l_sBoneID)) {
		switch(l_tObjectAction.m_tGoalType) {
			case MonsterSpace::eObjectActionActivate : {
				if (!DoorOpen(l_sBoneID))
					return((l_tObjectAction.m_bCompleted = true) == false);
				break;
			}
			case MonsterSpace::eObjectActionDeactivate : {
				if (!DoorClose(l_sBoneID))
					return((l_tObjectAction.m_bCompleted = true) == false);
				break;
			}
			case MonsterSpace::eObjectActionUse : {
				if (!DoorSwitch(l_sBoneID))
					return((l_tObjectAction.m_bCompleted = true) == false);
				break;
			}
			default : 
				return	((l_tObjectAction.m_bCompleted = true) == false);
		}
		return		(false);
	}
	SCarLight* light=NULL;
	if (m_lights.findLight(l_sBoneID,light)) {
		switch(l_tObjectAction.m_tGoalType) {
			case MonsterSpace::eObjectActionActivate : {
				light->TurnOn();
				return		((l_tObjectAction.m_bCompleted = true) == false);
			}
			case MonsterSpace::eObjectActionDeactivate : {
				light->TurnOff();
				return		((l_tObjectAction.m_bCompleted = true) == false);
			}
			case MonsterSpace::eObjectActionUse : {
				light->Switch();
				return		((l_tObjectAction.m_bCompleted = true) == false);
			}
			default : 
				return	((l_tObjectAction.m_bCompleted = true) == false);
		}
	
	}
	
	return			(false);
}

void CCar::vfProcessInputKey	(int iCommand, bool bPressed)
{
	if (bPressed)
		OnKeyboardPress			(iCommand);
	else
		OnKeyboardRelease		(iCommand);
}

void CCar::OnKeyboardPress(int cmd)
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
	case kR_STRAFE:	PressRight();				if (Owner()) Owner()->steer_Vehicle(1);	break;
	case kL_STRAFE:	PressLeft();				if (Owner()) Owner()->steer_Vehicle(-1);break;
	case kJUMP:		PressBreaks();				break;
	case kWPN_FIRE: SwitchEngine();				break;
	case kTORCH:	m_lights.SwitchHeadLights();break;
	case kUSE:									break;
	};

}

void	CCar::OnKeyboardRelease(int cmd)
{
	if (Remote())								return;
	switch (cmd)	
	{
	case kACCEL:break;
	case kFWD:		ReleaseForward();			break;
	case kBACK:		ReleaseBack();				break;
	case kL_STRAFE:	ReleaseLeft();				if (Owner()) Owner()->steer_Vehicle(0);	break;
	case kR_STRAFE:	ReleaseRight();				if (Owner()) Owner()->steer_Vehicle(0);	break;
	case kJUMP:		ReleaseBreaks();			break;
	case kREPAIR:	m_repairing=false;			break;
	};
}

void	CCar::OnKeyboardHold(int cmd)
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