#include "stdafx.h"
#include "Actor.h"
#include "xr_weapon_list.h"
#include "..\xr_level_controller.h"
#include "..\xr_input.h"
#include "..\xr_creator.h"
#include "..\CameraBase.h"
void CActor::OnKeyboardPress(int cmd)
{
	if (Remote())												return;

	if (GAME_PHASE_PENDING == Game().phase || !g_Alive())
	{
		if (kWPN_FIRE == cmd)	
		{
			// Switch our "ready" status
			NET_Packet			P;
			u_EventGen			(P,GEG_PLAYER_READY,ID());
			u_EventSend			(P);
		}
		return;
	}

	if (!g_Alive())												return;

	if(m_vehicle)
	{
		m_vehicle->OnKeyboardPress(cmd);
		return;
	}

	if(m_inventory.Action(cmd, CMD_START))						return;


	switch(cmd){
	case kACCEL:	mstate_wishful |= mcAccel;					break;
	case kR_STRAFE:	mstate_wishful |= mcRStrafe;				break;
	case kL_STRAFE:	mstate_wishful |= mcLStrafe;				break;
	case kFWD:		mstate_wishful |= mcFwd;					break;
	case kBACK:		mstate_wishful |= mcBack;					break;
	case kJUMP:		mstate_wishful |= mcJump;					break;
	case kCROUCH:	mstate_wishful |= mcCrouch;					break;

	case kCAM_1:	cam_Set			(eacFirstEye);				break;
	case kCAM_2:	cam_Set			(eacLookAt);				break;
	case kCAM_3:	cam_Set			(eacFreeLook);				break;

	case kWPN_FIRE:	g_fireStart		(); 						break;
	case kWPN_ZOOM:	g_fire2Start	();							break;
//	case kWPN_ZOOM:	Weapons->Zoom	(TRUE);						break;
	case kWPN_1:	
	case kWPN_2:	
	case kWPN_3:	
	case kWPN_4:	
	case kWPN_5:	
	case kWPN_6:	
	case kWPN_7:	
	case kWPN_8:	
	case kWPN_9:	
		//Weapons->ActivateWeaponID	(cmd-kWPN_1);			
		break;
	case kBINOCULARS:
		//Weapons->ActivateWeaponID	(Weapons->WeaponCount()-1);
		break;
	case kWPN_RELOAD:
		//Weapons->Reload			();
		break;
	case kUSE:
		use_Vehicle();
		break;
	case kDROP:
		b_DropActivated			= TRUE;
		f_DropPower				= 0;
		break;
	}
}

void CActor::OnKeyboardRelease(int cmd)
{
	if (Remote())		return;

	if (g_Alive())	
	{
		if(m_inventory.Action(cmd, CMD_STOP)) return;

		if(m_vehicle)
		{
			m_vehicle->OnKeyboardRelease(cmd);
			return;
		}

		switch(cmd)
		{
		case kACCEL:	mstate_wishful &=~mcAccel;		break;
		case kR_STRAFE:	mstate_wishful &=~mcRStrafe;	break;
		case kL_STRAFE:	mstate_wishful &=~mcLStrafe;	break;
		case kFWD:		mstate_wishful &=~mcFwd;		break;
		case kBACK:		mstate_wishful &=~mcBack;		break;
		case kJUMP:		mstate_wishful &=~mcJump;		break;
		case kCROUCH:	mstate_wishful &=~mcCrouch;		break;

		case kWPN_FIRE:	g_fireEnd();					break;
		case kWPN_ZOOM:	g_fire2End();					break;
//		case kWPN_ZOOM:	Weapons->Zoom(FALSE);			break;

		case kDROP:		if(GAME_PHASE_INPROGRESS == Game().phase) g_PerformDrop();				break;
		}
	}
}

void CActor::OnKeyboardHold(int cmd)
{
	if (Remote() || !g_Alive())		return;

	if(m_vehicle)
	{
		m_vehicle->OnKeyboardHold(cmd);
		return;
	}

	switch(cmd)
	{
	case kUP:
	case kDOWN: 
	case kCAM_ZOOM_IN: 
	case kCAM_ZOOM_OUT: 
		cam_Active()->Move(cmd); break;
	case kLEFT:
	case kRIGHT:
		if (cam_active!=eacFreeLook) cam_Active()->Move(cmd); break;
	}
}

void CActor::OnMouseMove(int dx, int dy)
{
	if (Remote())	return;

	CCameraBase* C	= cameras	[cam_active];
	float scale		= (C->f_fov/DEFAULT_FOV)*psMouseSens * psMouseSensScale/50.f;
	if (dx){
		float d = float(dx)*scale;
		cam_Active()->Move((d<0)?kLEFT:kRIGHT, _abs(d));
	}
	if (dy){
		float d = ((psMouseInvert.test(1))?-1:1)*float(dy)*scale*3.f/4.f;
		cam_Active()->Move((d>0)?kUP:kDOWN, _abs(d));
	}
}
