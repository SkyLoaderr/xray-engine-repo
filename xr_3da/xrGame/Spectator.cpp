// Actor.cpp: implementation of the CSpectator class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "spectator.h"
#include "..\effectorfall.h"
#include "..\CameraLook.h"
#include "..\CameraFirstEye.h"
#include "..\xr_level_controller.h"

//--------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CSpectator::CSpectator() : CGameObject()
{
	// Cameras
	cameras[eacFirstEye]	= new CCameraFirstEye	(this, pSettings, "actor_firsteye_cam", false);
	cameras[eacLookAt]		= new CCameraLook		(this, pSettings, "actor_look_cam",		false);
	cameras[eacFreeLook]	= new CCameraLook		(this, pSettings, "actor_free_cam",	false);

	cam_active				= eacFirstEye;
}

CSpectator::~CSpectator()
{
	for (int i=0; i<eacMaxCam; i++) _DELETE(cameras[i]);
}

void CSpectator::UpdateCL()
{
	inherited::UpdateCL();
	if (pCreator->CurrentViewEntity()==this){
		cam_Update								();
	}

//	setVisible				(!HUDview	());
}

void CSpectator::Update	(u32 DT)
{
	if (!getEnabled())	return;
	if (!Ready())		return;
}

void CSpectator::OnKeyboardPress(int cmd)
{
/*
	if (Remote())												return;

	if (GAME_PHASE_PENDING	== Game().phase || !g_Alive())
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
	case kWPN_ZOOM:	Weapons->Zoom	(TRUE);						break;
	case kWPN_1:	
	case kWPN_2:	
	case kWPN_3:	
	case kWPN_4:	
	case kWPN_5:	
	case kWPN_6:	
	case kWPN_7:	
	case kWPN_8:	
	case kWPN_9:	
		Weapons->ActivateWeaponID	(cmd-kWPN_1);			
		break;
	case kBINOCULARS:
		Weapons->ActivateWeaponID	(Weapons->WeaponCount()-1);
		break;
	case kWPN_RELOAD:
		Weapons->Reload			();
		break;
	case kUSE:
		break;
	case kDROP:
		b_DropActivated			= TRUE;
		f_DropPower				= 0;
		break;
	}
*/
}

void CSpectator::OnKeyboardRelease(int cmd)
{
/*	if (Remote())		return;

	if (g_Alive())	
	{
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
		case kWPN_ZOOM:	Weapons->Zoom(FALSE);			break;

		case kDROP:		g_PerformDrop();				break;
		}
	}
*/
}

void CSpectator::OnKeyboardHold(int cmd)
{
//	if (Remote())		return;

	switch(cmd)
	{
	case kFWD:			
		Fvector vmove;
		vmove.set( clTransform.k );
		vmove.mul( 5.f*Device.fTimeDelta );
		svTransform.c.add( vmove );
		Log("T",svTransform.c);
		break;
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

void CSpectator::OnMouseMove(int dx, int dy)
{
	if (Remote())	return;

	CCameraBase* C	= cameras	[cam_active];
	float scale		= (C->f_fov/DEFAULT_FOV)*psMouseSens * psMouseSensScale/50.f;
	if (dx){
		float d = float(dx)*scale;
		cameras[cam_active]->Move((d<0)?kLEFT:kRIGHT, _abs(d));
	}
	if (dy){
		float d = ((psMouseInvert)?-1:1)*float(dy)*scale*3.f/4.f;
		cameras[cam_active]->Move((d>0)?kUP:kDOWN, _abs(d));
	}
}

void CSpectator::cam_Set	(EActorCameras style)
{
	CCameraBase* old_cam = cameras[cam_active];
	cam_active = style;
	old_cam->OnDeactivate();
	cameras[cam_active]->OnActivate(old_cam);
}

void CSpectator::cam_Update()
{
	Fvector point, dangle;
	point.set					(0.f,1.f,0.f);
	svTransform.transform_tiny	(point);

	// apply shift
	dangle.set					(0,0,0);
	CCameraBase* C				= cameras	[cam_active];
	C->Update					(point,dangle);
	pCreator->Cameras.Update	(C);
}
