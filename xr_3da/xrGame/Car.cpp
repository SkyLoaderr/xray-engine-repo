#include "stdafx.h"
#include "car.h"
#include "..\\camerabase.h"
#include "..\\bodyinstance.h"

CCar::CCar(void)
{
}

CCar::~CCar(void)
{
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
	return inherited::Spawn			(bLocal,server_id,o_pos,o_angle,P,flags);
}

void	CCar::Update				( DWORD T )
{
	inherited::Update				(T);

	float dt						= float(T)/1000.f;
	
	vPosition.y						+= sinf(Device.fTimeGlobal);

	UpdateTransform					();
	if (IsMyCamera())				cam_Update(dt);
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
}

void	CCar::OnKeyboardPress		(int dik)
{
}

void	CCar::OnKeyboardRelease		(int dik)
{
}

void	CCar::OnKeyboardHold		(int dik)
{
}

void	CCar::OnHUDDraw				(CCustomHUD* hud)
{
}
