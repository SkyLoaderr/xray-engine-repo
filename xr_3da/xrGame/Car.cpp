#include "stdafx.h"
#include "car.h"
#include "..\camerabase.h"
#include "..\cameralook.h"
#include "..\xr_level_controller.h"


const float drive_force		= 1000;

extern CPHWorld*	ph_world;

CCar::CCar(void)
{
	camera			= new CCameraLook		(this, pSettings, "car_look_cam",		false);
	m_jeep.Create	(ph_world->GetSpace(),phWorld);
	ph_world->AddObject((CPHObject*)this);
	m_repairing		=false;
	m_fCamDPitch	= 0.5f;
}

CCar::~CCar(void)
{
	_DELETE				(camera);
	pSounds->Delete		(snd_engine);
	m_jeep.Destroy();

}

void __stdcall CCar::cb_WheelFL	(CBoneInstance* B)
{
	CCar*	C			= dynamic_cast<CCar*>	(static_cast<CObject*>(B->Callback_Param));

	Fmatrix		M,m,A,M2,m2,t;
	M.rotateX			(deg2rad(-90.f));
	m.rotateZ			(deg2rad(-90.f));	// ?	2
	t.mul				(M,m);		// ?			2
	A.mul				(C->m_jeep.DynamicData[2].BoneTransform,t);
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
	A.mul				(C->m_jeep.DynamicData[3].BoneTransform,t);
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
	A.mul				(C->m_jeep.DynamicData[0].BoneTransform,t);
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
	A.mul				(C->m_jeep.DynamicData[1].BoneTransform,t);
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

	float yaw_dest,pitch_dest;
	clXFORM().k.getHP				(yaw_dest,pitch_dest);

	angle_lerp						(camera->yaw,-yaw_dest,PI_DIV_4,dt);
	angle_lerp						(camera->pitch,pitch_dest+m_fCamDPitch,PI_DIV_4,dt);
	camera->Update					(P,Da);
	Level().Cameras.Update			(camera);
}

// Core events
void	CCar::Load					( LPCSTR section )
{
	inherited::Load					(section);

	pSounds->Create					(snd_engine,TRUE,"car\\car1",TRUE);
}

BOOL	CCar::net_Spawn				(LPVOID DC)
{
	BOOL R = inherited::net_Spawn	(DC);
	setVisible						(TRUE);
	m_jeep.SetPosition				(vPosition);
	dMatrix3 Rot;
	Fmatrix ry,mr;
	ry.rotateY		(deg2rad(90.f));
	ry.invert();
	Fmatrix33 m33;
	mr.mul(ry,mRotate);
	m33.set(mr);
	
	PHDynamicData::FMX33toDMX(m33,Rot);
	m_jeep.SetRotation(Rot);
	pSounds->PlayAtPos				(snd_engine,this,vPosition,true);
	return R;
}

void	CCar::Update				( DWORD T )
{
	inherited::Update				(T);

	vPosition.set	(m_jeep.DynamicData.BoneTransform.c);
	
	Fmatrix mY;
	mY.rotateY		(deg2rad(90.f));
	mRotate.mul		(m_jeep.DynamicData.BoneTransform,mY);
	mRotate.c.set	(0,0,0);
	svTransform.mul	(m_jeep.DynamicData.BoneTransform,mY);
	
	UpdateTransform					();
}

void	CCar::UpdateCL				( )
{
	// Transform
	Fmatrix mY;//,Tr;
	//Tr.translate		(0,-1.f,0);
	mY.rotateY			(deg2rad(90.f));
	clTransform.mul		(m_jeep.DynamicData.BoneTransform,mY);
	//clTransform.mulB	(Tr);

	// Sound
	Fvector		C,V;
	clCenter	(C);
	V.set		(m_jeep.GetVelocity	());
	float		velocity						= V.magnitude();
	float		scale							= .5f + .4f*(velocity/10.f);
//	Log			("carS",scale);
	snd_engine.feedback->SetPosition			(C);
	snd_engine.feedback->SetFrequencyScale		(scale);

	// Camera
	if (IsMyCamera())				cam_Update	(Device.fTimeDelta);
}

void	CCar::OnVisible				( )
{
	inherited::OnVisible			();
}

void	CCar::net_Export			(NET_Packet& P)
{
}

void	CCar::net_Import			(NET_Packet& P)
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
	case kACCEL:	m_jeep.DriveVelocity=1*M_PI;
					m_jeep.Drive();
					break;
	case kRIGHT:	m_jeep.Steer(1);		
					break;
	case kLEFT:		m_jeep.Steer(-1);
					break;
	case kUP:		m_jeep.DriveDirection=1;
					m_jeep.DriveForce=drive_force;
					m_jeep.Drive();
					break;
	case kDOWN:		m_jeep.DriveDirection=-1;
					m_jeep.DriveForce=drive_force;
					m_jeep.Drive();
					break;
	case kJUMP:		m_jeep.Breaks=true;
					m_jeep.Drive();
					break;
	
	};

}

void	CCar::OnKeyboardRelease		(int cmd)
{
	if (Remote())					return;
		switch (cmd)	
	{
	case kACCEL:	m_jeep.DriveVelocity=12*M_PI;
					m_jeep.Drive();
					break;
	case kLEFT:	
	case kRIGHT:	m_jeep.Steer(0);
					break;
	case kUP:	
	case kDOWN:		m_jeep.DriveDirection=0;
					m_jeep.DriveForce=0;
					m_jeep.Drive();
					break;
	case kREPAIR:	m_repairing=false; break;
	case kJUMP:		m_jeep.Breaks=false;
					m_jeep.Drive();
					break;
	};
}

void	CCar::OnKeyboardHold		(int cmd)
{
	if (Remote())					return;

	switch(cmd)
	{
	case kCAM_ZOOM_IN: 
	case kCAM_ZOOM_OUT: 
		camera->Move(cmd); break;
	case kFWD:		m_fCamDPitch += PI_DIV_4*Device.fTimeDelta; //camera->Move(kUP);		
		break;
	case kBACK:		m_fCamDPitch -= PI_DIV_4*Device.fTimeDelta; //camera->Move(kDOWN);	
		break;
	case kL_STRAFE: camera->Move(kLEFT);	
		break;
	case kR_STRAFE: camera->Move(kRIGHT);	
		break;
	case kREPAIR:	m_repairing=true;		
		break;
	}
	clamp(m_fCamDPitch, 0.f, 1.f);
}

void	CCar::OnHUDDraw				(CCustomHUD* hud)
{
}


void CCar::PhDataUpdate(dReal step){
	m_jeep.DynamicData.CalculateData();

}
void CCar:: PhTune(dReal step){
	m_jeep.JointTune(step);
	if(m_repairing) m_jeep.Revert();
}

void CCar::OnDeviceCreate()
{
	inherited::OnDeviceCreate();

	//
	CKinematics*	M				= PKinematics(pVisual);
	R_ASSERT						(M);
	M->PlayCycle					("init");
	M->LL_GetInstance				(M->LL_BoneID("phy_wheel_frontl")).set_callback	(cb_WheelFL,this);
	M->LL_GetInstance				(M->LL_BoneID("phy_wheel_frontr")).set_callback	(cb_WheelFR,this);
	M->LL_GetInstance				(M->LL_BoneID("phy_wheel_rearl")).set_callback	(cb_WheelBL,this);
	M->LL_GetInstance				(M->LL_BoneID("phy_wheel_rearr")).set_callback	(cb_WheelBR,this);
	// clTransform.set					( m_jeep.DynamicData.BoneTransform	);
}
