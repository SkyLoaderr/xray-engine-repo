#include "stdafx.h"

#include "car.h"
#include "..\camerabase.h"
#include "..\cameralook.h"
#include "..\xr_level_controller.h"
#include "PHDynamicData.h"
#include "Physics.h"

extern CPHWorld*	ph_world;

CCar::CCar(void)
{
	camera		= new CCameraLook		(this, pSettings, "actor_look_cam",		false);
}

CCar::~CCar(void)
{
	_DELETE				(camera);
	pSounds->Delete		(snd_engine);
}

void __stdcall CCar::cb_WheelFL	(CBoneInstance* B)
{
	CCar*	C			= dynamic_cast<CCar*>	(static_cast<CObject*>(B->Callback_Param));

	Fmatrix		M,m,A,M2,m2,t;
	M.rotateX			(deg2rad(-90.f));
	m.rotateZ			(deg2rad(-90.f));	// ?	2
	t.mul				(M,m);		// ?			2
	A.mul				(ph_world->Jeep.DynamicData[2].BoneTransform,t);
	M2.rotateX			(deg2rad(90.f));
	m2.rotateZ			(deg2rad(90.f));	// ?	2
	t.mul				(m2,M2);	// ?			2
	A.mulA				(t);
	B->mTransform.mulB	(A);
}

void __stdcall CCar::cb_WheelFR	(CBoneInstance* B)
{
	CCar*	C			= dynamic_cast<CCar*>	(static_cast<CObject*>(B->Callback_Param));
	Fmatrix		M,m,A,M2,m2,t;
	M.rotateX			(deg2rad(90.f));
	m.rotateZ			(deg2rad(-90.f));	
	t.mul				(M,m);		
	A.mul				(ph_world->Jeep.DynamicData[3].BoneTransform,t);
	M2.rotateX			(deg2rad(-90.f));
	m2.rotateZ			(deg2rad(90.f));	
	t.mul				(m2,M2);	
	A.mulA				(t);
	B->mTransform.mulB	(A);
}
void __stdcall CCar::cb_WheelBL	(CBoneInstance* B)
{
	CCar*	C			= dynamic_cast<CCar*>	(static_cast<CObject*>(B->Callback_Param));
	Fmatrix		M,m,A,M2,m2,t;
	M.rotateX			(deg2rad(-90.f));
	m.rotateZ			(deg2rad(-90.f));	// ?	2
	t.mul				(M,m);		// ?			2
	A.mul				(ph_world->Jeep.DynamicData[0].BoneTransform,t);
	M2.rotateX			(deg2rad(90.f));
	m2.rotateZ			(deg2rad(90.f));	// ?	2
	t.mul				(m2,M2);	// ?			2
	A.mulA				(t);
	B->mTransform.mulB	(A);
}
void __stdcall CCar::cb_WheelBR	(CBoneInstance* B)
{
	CCar*	C			= dynamic_cast<CCar*>	(static_cast<CObject*>(B->Callback_Param));
	Fmatrix		M,m,A,M2,m2,t;
	M.rotateX			(deg2rad(90.f));
	m.rotateZ			(deg2rad(-90.f));	// ?	2
	t.mul				(M,m);		// ?			2
	A.mul				(ph_world->Jeep.DynamicData[1].BoneTransform,t);
	M2.rotateX			(deg2rad(-90.f));
	m2.rotateZ			(deg2rad(90.f));	// ?	2
	t.mul				(m2,M2);	// ?			2
	A.mulA				(t);
	B->mTransform.mulB	(A);
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
void	CCar::Load					( LPCSTR section )
{
	inherited::Load					(section);

	CKinematics*	M				= PKinematics(pVisual);
	R_ASSERT						(M);
	M->PlayCycle					("init");
	M->LL_GetInstance				(M->LL_BoneID("phy_wheel_frontl")).set_callback	(cb_WheelFL,this);
	M->LL_GetInstance				(M->LL_BoneID("phy_wheel_frontr")).set_callback	(cb_WheelFR,this);
	M->LL_GetInstance				(M->LL_BoneID("phy_wheel_rearl")).set_callback	(cb_WheelBL,this);
	M->LL_GetInstance				(M->LL_BoneID("phy_wheel_rearr")).set_callback	(cb_WheelBR,this);
	clTransform.set( ph_world->Jeep.DynamicData.BoneTransform	);

	pSounds->Create					(snd_engine,TRUE,"car\\car1",TRUE);
}

BOOL	CCar::Spawn					( BOOL bLocal, int server_id, Fvector& o_pos, Fvector& o_angle, NET_Packet& P, u16 flags )
{
	BOOL R = inherited::Spawn		(bLocal,server_id,o_pos,o_angle,P,flags);
	bVisible						= TRUE;
	//o_pos.y=1;
	//o_pos.z=-10;
	//o_pos.x=0;
	ph_world->Jeep.SetPosition		(o_pos);
	//ph_world->Jeep.Create			(Space,phWorld);
	pSounds->PlayAtPos				(snd_engine,this,o_pos,true);
	return R;
}

void	CCar::Update				( DWORD T )
{
	inherited::Update				(T);

	float dt						= float(T)/1000.f;
	vPosition.set	(ph_world->Jeep.DynamicData.BoneTransform.c);
	
	Fmatrix mY;
	mY.rotateY		(deg2rad(90.f));
	mRotate.mul		(ph_world->Jeep.DynamicData.BoneTransform,mY);
	mRotate.c.set	(0,0,0);
	svTransform.mul	(ph_world->Jeep.DynamicData.BoneTransform,mY);
	
	UpdateTransform					();
}

void	CCar::UpdateCL				( )
{
	// Transform
	Fmatrix mY,Tr;
	Tr.translate		(0,-1.f,0);
	mY.rotateY			(deg2rad(90.f));
	clTransform.mul		(ph_world->Jeep.DynamicData.BoneTransform,mY);
	clTransform.mulB	(Tr);

	// Sound
	Fvector		C,V;
	clCenter	(C);
	V.set		(ph_world->Jeep.GetVelocity	());
	float		velocity						= V.magnitude();
	snd_engine.feedback->SetPosition			(C);
	snd_engine.feedback->SetFrequencyScale		(velocity/10.f);

	// Camera
	if (IsMyCamera())				cam_Update	(Device.fTimeDelta);
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
	case kACCEL:	ph_world->Jeep.VelocityRate=2;
					break;
	case kR_STRAFE:	ph_world->Jeep.Steer(1);//vPosition.x+=1; 
					break;
	case kL_STRAFE:	ph_world->Jeep.Steer(-1);//vPosition.x-=1;
					break;
	case kFWD:		ph_world->Jeep.Drive(1);//vPosition.z+=1; 
					break;
	case kBACK:		ph_world->Jeep.Drive(-1);//vPosition.z-=1; 
					break;
	//default:        ph_world->Jeep.Steer(0,0);
	};

}

void	CCar::OnKeyboardRelease		(int cmd)
{
	if (Remote())					return;
		switch (cmd)	
	{
	case kACCEL:	ph_world->Jeep.VelocityRate=1;
					break;
	case kR_STRAFE:	ph_world->Jeep.Steer(0);//vPosition.x+=1; 
					break;
	case kL_STRAFE:	ph_world->Jeep.Steer(0);//vPosition.x-=1;
					break;
	case kFWD:		ph_world->Jeep.Drive(0);//vPosition.z+=1; 
					break;
	case kBACK:		ph_world->Jeep.Drive(0);//vPosition.z-=1; 
					break;
	//default:        ph_world->Jeep.Steer(0,0);
	};
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
