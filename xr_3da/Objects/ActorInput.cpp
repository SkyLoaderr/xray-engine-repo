#include "stdafx.h"
#include "Actor.h"
#include "xr_weapon_list.h"
#include "..\xr_level_controller.h"
#include "..\xr_input.h"
#include "..\xr_creator.h"
#include "..\CameraBase.h"

void CActor::OnKeyboardPress(int cmd)
{
	if (Remote() || !bAlive)	return;

	switch(cmd){
	case kACCEL:	mstate_wishful |= mcAccel;		break;
	case kR_STRAFE:	mstate_wishful |= mcRStrafe;	break;
	case kL_STRAFE:	mstate_wishful |= mcLStrafe;	break;
	case kFWD:		mstate_wishful |= mcFwd;		break;
	case kBACK:		mstate_wishful |= mcBack;		break;
	case kJUMP:		mstate_wishful |= mcJump;		break;
	case kCROUCH:	mstate_wishful |= mcCrouch;		break;

	case kCAM_1:	cam_Set			(eacFirstEye);	break;
	case kCAM_2:	cam_Set			(eacLookAt);	break;
	case kCAM_3:	cam_Set			(eacFreeLook);	break;

	case kWPN_FIRE:	g_cl_fireStart();				break;
	case kWPN_ZOOM:	Weapons->Zoom(TRUE);			break;
	case kWPN_1:	
	case kWPN_2:	
	case kWPN_3:	
	case kWPN_4:	
	case kWPN_5:	
	case kWPN_6:	
	case kWPN_7:	
	case kWPN_8:	
	case kWPN_9:	
		Weapons->ActivateWeaponID(cmd-kWPN_1);			
		break;
	}
}

void CActor::OnKeyboardRelease(int cmd){
	if (Remote() || !bAlive)	return;

	switch(cmd){
	case kACCEL:	mstate_wishful &=~mcAccel;		break;
	case kR_STRAFE:	mstate_wishful &=~mcRStrafe;	break;
	case kL_STRAFE:	mstate_wishful &=~mcLStrafe;	break;
	case kFWD:		mstate_wishful &=~mcFwd;		break;
	case kBACK:		mstate_wishful &=~mcBack;		break;
	case kJUMP:		mstate_wishful &=~mcJump;		break;
	case kCROUCH:	mstate_wishful &=~mcCrouch;		break;

	case kWPN_FIRE:	g_fireEnd();					break;
	case kWPN_ZOOM:	Weapons->Zoom(FALSE);			break;
	}
}

void CActor::OnKeyboardHold(int cmd)
{
	if (Remote() || !bAlive)	return;

	switch(cmd)
	{
	case kUP:
	case kDOWN: 
	case kCAM_ZOOM_IN: 
	case kCAM_ZOOM_OUT: 
		cameras[cam_active]->Move(cmd); break;
	case kLEFT:
	case kRIGHT:
		if (cam_active!=eacFreeLook) cameras[cam_active]->Move(cmd); break;
	}
}

void CActor::OnMouseMove(int dx, int dy)
{
	if (Remote())	return;
	float scale		= psMouseSens * psMouseSensScale/50.f;
	if (dx){
		float d = float(dx)*scale;
		cameras[cam_active]->Move((d<0)?kLEFT:kRIGHT, fabsf(d));
	}
	if (dy){
		float d = ((psMouseInvert)?-1:1)*float(dy)*scale*MouseHWScale;
		cameras[cam_active]->Move((d>0)?kUP:kDOWN, fabsf(d));
	}
}
