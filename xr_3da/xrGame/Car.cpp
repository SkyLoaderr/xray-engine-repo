#include "stdafx.h"
#include "car.h"
#include "..\camerabase.h"
#include "..\cameralook.h"
#include "..\xr_level_controller.h"

CCar::CCar(void)
{
	camera		= new CCameraLook		(this, pSettings, "actor_look_cam",		false);
}

CCar::~CCar(void)
{
	_DELETE		(camera);
}

void __stdcall CCar::cb_WheelFL	(CBoneInstance* B)
{
	CCar*	C			= dynamic_cast<CCar*>	(static_cast<CObject*>(B->Callback_Param));
	// B->mTransform.set(???);
}
void __stdcall CCar::cb_WheelFR	(CBoneInstance* B)
{
	CCar*	C			= dynamic_cast<CCar*>	(static_cast<CObject*>(B->Callback_Param));
	// B->mTransform.set(???);
}
void __stdcall CCar::cb_WheelBL	(CBoneInstance* B)
{
	CCar*	C			= dynamic_cast<CCar*>	(static_cast<CObject*>(B->Callback_Param));
	// B->mTransform.set(???);
}
void __stdcall CCar::cb_WheelBR	(CBoneInstance* B)
{
	CCar*	C			= dynamic_cast<CCar*>	(static_cast<CObject*>(B->Callback_Param));
	// B->mTransform.set(???);
}

void	CCar::cam_Update			(float dt)
{
	Fvector							P,Da;
	clCenter						(P);
	Da.set							(0,0,0);

	camera->Update					(P,Da);
	Level().Cameras.Update			(camera);
}

// Core events
void	CCar::Load					( CInifile* ini, const char *section )
{
	inherited::Load					(ini,section);

	CKinematics*	M				= PKinematics(pVisual);
	R_ASSERT						(M);
	M->PlayCycle					("init");
	M->LL_GetInstance				(M->LL_BoneID("phy_wheel_frontl")).set_callback	(cb_WheelFL,this);
	M->LL_GetInstance				(M->LL_BoneID("phy_wheel_frontr")).set_callback	(cb_WheelFR,this);
	M->LL_GetInstance				(M->LL_BoneID("phy_wheel_rearl")).set_callback	(cb_WheelBL,this);
	M->LL_GetInstance				(M->LL_BoneID("phy_wheel_rearr")).set_callback	(cb_WheelBR,this);
}

BOOL	CCar::Spawn					( BOOL bLocal, int server_id, Fvector& o_pos, Fvector& o_angle, NET_Packet& P, u16 flags )
{
	BOOL R = inherited::Spawn		(bLocal,server_id,o_pos,o_angle,P,flags);
	bVisible						= TRUE;
	return R;
}

void	CCar::Update				( DWORD T )
{
	inherited::Update				(T);

	float dt						= float(T)/1000.f;
	
	//	vPosition.set	(???)
	//  mRotate.set		(???)
	//	svTransform		(???)

	UpdateTransform					();
	if (IsMyCamera())				cam_Update(dt);
}

void	CCar::UpdateCL				( )
{
	//	clTransform		(???)
}

void	CCar::OnVisible				( )
{
	inherited::OnVisible			();
}

void	CCar::net_Export			(NET_Packet* P)
{
}

void	CCar::net_Import			(NET_Packet* P)
{
}

void	CCar::OnMouseMove			(int x, int y)
{
	if (Remote())					return;
}

void	CCar::OnKeyboardPress		(int cmd)
{
	if (Remote())					return;

	switch (cmd)	
	{
	case kACCEL:	break;
	case kR_STRAFE:	vPosition.x+=1; break;
	case kL_STRAFE:	vPosition.x-=1; break;
	case kFWD:		vPosition.z+=1; break;
	case kBACK:		vPosition.z-=1; break;
	};
}

void	CCar::OnKeyboardRelease		(int cmd)
{
	if (Remote())					return;
}

void	CCar::OnKeyboardHold		(int cmd)
{
	if (Remote())					return;

	switch(cmd)
	{
	case kUP:
	case kDOWN: 
	case kCAM_ZOOM_IN: 
	case kCAM_ZOOM_OUT: 
		camera->Move(cmd); break;
	case kLEFT:
	case kRIGHT:
		camera->Move(cmd); break;
	}
}

void	CCar::OnHUDDraw				(CCustomHUD* hud)
{
}
