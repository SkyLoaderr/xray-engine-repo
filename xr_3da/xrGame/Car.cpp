#include "stdafx.h"
#include "car.h"
#include "hudmanager.h"
#include "..\camerabase.h"
#include "..\cameralook.h"
#include "..\camerafirsteye.h"
#include "..\xr_level_controller.h"
#include "Actor.h"
enum ECarCamType{
	ectFirst	= 0,
	ectChase,
	ectFree
};

const float drive_force		= 1000;

extern CPHWorld*	ph_world;

CCar::CCar(void)
{
	active_camera	= 0;
	m_vCamDeltaHP.set(0.f,0.f,0.f);
	camera[ectFirst]= xr_new<CCameraFirstEye>	(this, pSettings, "car_firsteye_cam",	false); camera[ectFirst]->tag	= ectFirst;
	camera[ectChase]= xr_new<CCameraLook>		(this, pSettings, "car_look_cam",		false);	camera[ectChase]->tag	= ectChase;
	camera[ectFree]	= xr_new<CCameraLook>		(this, pSettings, "car_free_cam",		false);	camera[ectFree]->tag	= ectFree;
	OnCameraChange(ectChase);

	m_repairing		=false;
	m_owner			=NULL;

	///////////////////////////////
	//////////////////////////////
	/////////////////////////////
	VelocityRate=3.f;
	DriveForce=0;
	DriveVelocity=12.f * M_PI;
	DriveDirection=0;
	Breaks=false;
	///////////////////////////////
	//////////////////////////////
	/////////////////////////////
}

CCar::~CCar(void)
{
	xr_delete			(camera[0]);
	xr_delete			(camera[1]);
	xr_delete			(camera[2]);
	snd_engine.destroy	();
	m_jeep.Destroy();

}

void __stdcall CCar::cb_WheelFL	(CBoneInstance* B)
{
	CCar*	C			= dynamic_cast<CCar*>	(static_cast<CObject*>(B->Callback_Param));

	Fmatrix		M,m,A,M2,m2,t;
	M.rotateX			(deg2rad(-90.f));
	m.rotateZ			(deg2rad(-90.f));	// ?	2
	t.mul				(M,m);		// ?			2
	/////////////
	Fmatrix b_t;
	C->m_jeep.DynamicData[2].InterpolateTransformVsParent(b_t);
	A.mul(b_t,t);
	/////////////////
	//A.mul				(C->m_jeep.DynamicData[2].BoneTransform,t);
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
	/////////////
	Fmatrix b_t;
	C->m_jeep.DynamicData[3].InterpolateTransformVsParent(b_t);
	A.mul(b_t,t);
	/////////////////
	//A.mul				(C->m_jeep.DynamicData[3].BoneTransform,t);
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
		/////////////
	Fmatrix b_t;
	C->m_jeep.DynamicData[0].InterpolateTransformVsParent(b_t);
	A.mul(b_t,t);
	/////////////////
	//A.mul				(C->m_jeep.DynamicData[0].BoneTransform,t);
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
		/////////////
	Fmatrix b_t;
	C->m_jeep.DynamicData[1].InterpolateTransformVsParent(b_t);
	A.mul(b_t,t);
	/////////////////
	//A.mul				(C->m_jeep.DynamicData[1].BoneTransform,t);
	M2.rotateX			(deg2rad(-90.f));
	m2.rotateZ			(deg2rad(90.f));	// ?	2
	t.mul				(m2,M2);	// ?			2
	A.mulA				(t);
	B->mTransform.mulB	(A);
}

void __stdcall  CCar::cb_Steer(CBoneInstance* B)
{
CCar*	C			= dynamic_cast<CCar*>	(static_cast<CObject*>(B->Callback_Param));
Fmatrix m;
m.rotateZ(C->m_jeep.GetSteerAngle());
B->mTransform.mulB	(m);
}

void	CCar::cam_Update			(float dt)
{
	Fvector							P,Da;
	Da.set							(0,0,0);
	if(m_owner)	m_owner->setEnabled(false);
	float yaw_dest,pitch_dest,bank_dest;
	clXFORM().getHPB				(yaw_dest,pitch_dest,bank_dest);

	switch(active_camera->tag) {
	case ectFirst:
		angle_lerp					(active_camera->yaw,	-yaw_dest+m_vCamDeltaHP.x,		PI_DIV_2,dt);
		angle_lerp					(active_camera->pitch,	-pitch_dest+m_vCamDeltaHP.y,	PI_DIV_2,dt);
		P.set						(-0.5f,1.5f,-.05f);
		clXFORM().transform_tiny	(P);
		m_vCamDeltaHP.x				= m_vCamDeltaHP.x*(1-PI*dt)+((active_camera->lim_yaw.y+active_camera->lim_yaw.x)/2.f)*PI*dt;
		break;
	case ectChase:
		angle_lerp					(active_camera->yaw,	-(yaw_dest+m_vCamDeltaHP.x),	PI_DIV_4,dt);
		angle_lerp					(active_camera->pitch,	-pitch_dest+m_vCamDeltaHP.y,	PI_DIV_4,dt);
		clCenter					(P);
		m_vCamDeltaHP.x				= m_vCamDeltaHP.x*(1-PI*dt)+((active_camera->lim_yaw.y+active_camera->lim_yaw.x)/2.f)*PI*dt;
		break;
	case ectFree:
		clCenter					(P);
		break;
	}

	active_camera->Update			(P,Da);
	Level().Cameras.Update			(active_camera);
	if(m_owner)	m_owner->setEnabled(true);
}

// Core events
void	CCar::Load					( LPCSTR section )
{
	inherited::Load					(section);

	snd_engine.create				(TRUE,"car\\car1");
}

BOOL	CCar::net_Spawn				(LPVOID DC)
{
	BOOL R = inherited::net_Spawn	(DC);



	setVisible						(TRUE);

	Sound->play_at_pos				(snd_engine,this,vPosition,true);
	ActivateJeep();
	//


	return R;
}

void	CCar::Update				( u32 T )
{
	inherited::Update				(T);

	//vPosition.set	(m_jeep.DynamicData.BoneTransform.c);
	
	Fmatrix mY;
	mY.rotateY		(deg2rad(90.f));
	/////////////
	Fmatrix b_t;
	m_jeep.DynamicData.InterpolateTransform(b_t);
	/////////////////
	//mRotate.mul		(m_jeep.DynamicData.BoneTransform,mY);

	//svTransform.mul	(m_jeep.DynamicData.BoneTransform,mY);
	svTransform.mul	(b_t,mY);
	mRotate.set		(svTransform);
	mRotate.c.set	(0,0,0);
	vPosition.set	(svTransform.c);
	UpdateTransform					();
	if(m_owner)
	{
		Fmatrix glob_driver_pos;
		glob_driver_pos.mul(svTransform,fmPosDriver);
		m_owner->Position().set(glob_driver_pos.c);
		m_owner->Rotation().set(glob_driver_pos);
		m_owner->Rotation().c.set(0,0,0);
		m_owner->UpdateTransform();
		//if(m_owner->IsMyCamera()) cam_Update	(Device.fTimeDelta);
	}
}

void	CCar::UpdateCL				( )
{
	// Transform
	Fmatrix mY;
	mY.rotateY			(deg2rad(90.f));
	/////////////
	Fmatrix b_t;
	m_jeep.DynamicData.InterpolateTransform(b_t);
	/////////////////
	//clTransform.mul		(m_jeep.DynamicData.BoneTransform,mY);
	clTransform.mul		(b_t,mY);

	// Sound
	Fvector		C,V;
	clCenter	(C);
	V.set		(m_jeep.GetVelocity	());
	float		velocity						= V.magnitude();
	float		scale							= 1.5f + 1.f*(velocity/10.f);

	snd_engine.set_position			(C);
	snd_engine.set_frequency		(scale);

	// Camera
	if (IsMyCamera())				
		cam_Update	(Device.fTimeDelta);
	if(m_owner)
	{
	Fmatrix glob_driver_pos;
	glob_driver_pos.mul(clTransform,fmPosDriver);
	m_owner->Position().set(glob_driver_pos.c);
	m_owner->Rotation().set(glob_driver_pos);
	m_owner->Rotation().c.set(0,0,0);
	m_owner->UpdateTransform();
	if(m_owner->IsMyCamera()) 
		cam_Update	(Device.fTimeDelta);
	}

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
	case kCAM_1:	OnCameraChange(ectFirst);	break;
	case kCAM_2:	OnCameraChange(ectChase);	break;
	case kCAM_3:	OnCameraChange(ectFree);	break;
	case kACCEL:	m_jeep.DriveVelocity=5*M_PI;
					m_jeep.Drive();
					break;
	case kRIGHT:	m_jeep.Steer(1);
					m_owner->steer_Vehicle(1);
					break;
	case kLEFT:		m_jeep.Steer(-1);
					m_owner->steer_Vehicle(-1);
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
	case kUSE:
					detach_Actor();
	
	};

}

void	CCar::OnKeyboardRelease		(int cmd)
{
	if (Remote())					return;
		switch (cmd)	
	{
	case kACCEL:	m_jeep.DriveVelocity=15*M_PI;
					m_jeep.Drive();
					break;
	case kLEFT:	
	case kRIGHT:	m_jeep.Steer(0);
					m_owner->steer_Vehicle(0);
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

void	CCar::OnCameraChange		(int type)
{
	if (!active_camera||active_camera->tag!=type){
		active_camera	= camera[type];
		float Y,P,R;
		clXFORM().getHPB			(Y,P,R);
		active_camera->yaw			= -Y;
		m_vCamDeltaHP.y	= active_camera->pitch;
	}
}
void	CCar::OnKeyboardHold		(int cmd)
{
	if (Remote())					return;

	switch(cmd)
	{
	case kCAM_ZOOM_IN: 
	case kCAM_ZOOM_OUT: 
		active_camera->Move(cmd); break;
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
	case kREPAIR:	m_repairing=true;		
		break;
	}
	clamp(m_vCamDeltaHP.x, -PI_DIV_2,	PI_DIV_2);
	clamp(m_vCamDeltaHP.y, active_camera->lim_pitch.x,	active_camera->lim_pitch.y);
}

void	CCar::OnHUDDraw				(CCustomHUD* hud)
{
#ifdef DEBUG
	HUD().pFontSmall->SetColor		(0xffffffff);
	HUD().pFontSmall->OutSet		(120,530);
	HUD().pFontSmall->OutNext		("Position:      [%3.2f, %3.2f, %3.2f]",VPUSH(vPosition));
	HUD().pFontSmall->OutNext		("Velocity:      [%3.2f]",m_jeep.GetVelocity().magnitude());
#endif
}


void CCar::PhDataUpdate(dReal step){
	//m_jeep.DynamicData.CalculateData();
	m_jeep.DynamicData.UpdateInterpolationRecursive();
	m_jeep.LimitWeels();
	if(m_repairing) m_jeep.Revert();

	
}
void CCar:: PhTune(dReal step){
	m_jeep.JointTune(step);
}

void CCar::Hit(float P,Fvector &dir,CObject *who,s16 element,Fvector p_in_object_space, float impulse){
	switch(element){
	case 0: m_jeep.applyImpulseTrace(0,p_in_object_space,dir,impulse); break;
	case 1: m_jeep.applyImpulseTrace(2,p_in_object_space,dir,impulse); break;
	case 2: m_jeep.applyImpulseTrace(1,p_in_object_space,dir,impulse); break;
	case 3: m_jeep.applyImpulseTrace(4,p_in_object_space,dir,impulse); break;
	case 4: m_jeep.applyImpulseTrace(3,p_in_object_space,dir,impulse); break;

	}
	inherited::Hit(P,dir,who,element,p_in_object_space, 0);
}

void CCar::detach_Actor()
{
if(!m_owner) return;
m_owner->detach_Vehicle();
m_owner=NULL;
}

bool CCar::attach_Actor(CActor* actor)
{
if(m_owner) return false;
m_owner=actor;



//////////////////////////////////////////////////////////////////////////
CKinematics* M		= PKinematics(pVisual);			VERIFY(M);
int id=M->LL_BoneID("pos_driver");
CBoneInstance& instance=M->LL_GetInstance				(id);
/////////////////////////////////////////////////////////////////////////
//instance.set_callback(m_phSkeleton->GetBonesCallback(),element);
fmPosDriver.set(instance.mTransform);
return true;
}

void CCar::ActivateJeep()
{
	m_jeep.Create	(ph_world->GetSpace(),phWorld);
	ph_world->AddObject((CPHObject*)this);
	m_jeep.SetPosition				(vPosition);
	dMatrix3						Rot;
	Fmatrix							ry,mr;
	ry.rotateY						(deg2rad(90.f));
	ry.invert						();
	Fmatrix33 m33;
	mr.mul							(ry,mRotate);
	m33.set							(mr);

	PHDynamicData::FMX33toDMX		(m33,Rot);
	m_jeep.SetRotation				(Rot);

	CKinematics*	M				= PKinematics(pVisual);
	R_ASSERT						(M);
	M->PlayCycle					("idle");
	M->LL_GetInstance				(M->LL_BoneID("phy_wheel_frontl")).set_callback	(cb_WheelFL,this);
	M->LL_GetInstance				(M->LL_BoneID("phy_wheel_frontr")).set_callback	(cb_WheelFR,this);
	M->LL_GetInstance				(M->LL_BoneID("phy_wheel_rearl")).set_callback	(cb_WheelBL,this);
	M->LL_GetInstance				(M->LL_BoneID("phy_wheel_rearr")).set_callback	(cb_WheelBR,this);
	M->LL_GetInstance				(M->LL_BoneID("steer")).set_callback			(cb_Steer,this);
	 ///clTransform.set					( m_jeep.DynamicData.BoneTransform	);
}

void CCar::CreateShell()
{
m_pPhysicsShell=P_create_Shell();

static const dReal scaleParam=1.f;
static const dVector3 scaleBox={scaleParam, scaleParam, scaleParam};
dVector3 jeepBox,cabinBox,startPosition;
//jeepBox={scaleBox[0],scaleBox[0],scaleBox[0]};
//jeepBox[0]=REAL(4.2)*scaleBox[0];jeepBox[1]=REAL(1.)*scaleBox[1];jeepBox[2]=REAL(2.08)*scaleBox[2];
jeepBox[0]=REAL(3.680)*scaleBox[0];jeepBox[1]=REAL(0.612)*scaleBox[1];jeepBox[2]=0.88f*2.f*scaleBox[2];
//cabinBox[0]=scaleBox[0]*1.9f;cabinBox[1]=scaleBox[1]*0.6f;cabinBox[2]=scaleBox[2]*2.08f;
cabinBox[0]=scaleBox[0]*1.7f;cabinBox[1]=scaleBox[1]*0.66f;cabinBox[2]=scaleBox[2]*2.f*0.76f;

static const dReal wheelRadius = 0.345f* scaleParam;
///////////////////////////////////////////////
VelocityRate=3.f;
DriveForce=0;
DriveVelocity=12.f * M_PI;
DriveDirection=0;
Breaks=false;
///////////////////////////////////
startPosition[0]=10.0f;startPosition[1]=1.f;startPosition[2]=0.f;
//static const dReal weelSepX=scaleBox[0]*2.74f/2.f,weelSepZ=scaleBox[2]*1.7f/2.f,weelSepY=scaleBox[1]*0.6f;
static const dReal weelSepXF=scaleBox[0]*1.32f,weelSepXB=scaleBox[0]*1.155f,weelSepZ=scaleBox[2]*1.53f/2.f,weelSepY=scaleBox[1]*0.463f;
static const dReal cabinSepX=scaleBox[0]*0.61f,cabinSepY=scaleBox[1]*0.55f;
dReal MassShift=0.25f;

// car body
//dMass m;
//dMassSetBox(&m, 1.f, jeepBox[0], jeepBox[1]/4.f, jeepBox[2]); // density,lx,ly,lz
//dMassAdjust(&m, 800.f); // mass
//dMassTranslate(&m,0.f,-1.f,0.f);

//dBodySetPosition(Bodies[0], startPosition[0], startPosition[1]-MassShift, startPosition[2]); // x,y,z



//dGeomGetUserData(Geoms[0])->material=GMLib.GetMaterialIdx("materials\\car_cabine");
//dGeomGetUserData(Geoms[6])->material=GMLib.GetMaterialIdx("materials\\car_cabine");

//dGeomSetPosition(Geoms[0], 0.f, MassShift, 0.f); // x,y,z
//dGeomSetPosition(Geoms[6], -jeepBox[0]/2.f+cabinBox[0]/2.f+0.55f, cabinBox[1]/2.f+jeepBox[1]/2.f+MassShift, 0.f); // x,y,z
//dGeomSetPosition(Geoms[6], -cabinSepX, cabinSepY+MassShift, 0.f); // x,y,z

//dGeomSetPosition(Geoms[0], 0,0/*-jeepBox[1]-wheelRadius*/, 0); // x,y,z





// wheel bodies
//dMassSetSphere(&m, 1, wheelRadius); // density, radius
//dMassAdjust(&m, 20); // mass

u32 i;
for(i = 1; i <= 4; ++i)
{
	
	//dBodySetMass(Bodies[i], &m);
		
//	Geoms[i] = dCreateCylinder(0, wheelRadius,0.19f);
	
//	dGeomGetUserData(Geoms[i])->material=GMLib.GetMaterialIdx("materials\\rubber");

}




//dBodySetPosition(Bodies[1], startPosition[0]-weelSepXB, startPosition[1]-weelSepY,  startPosition[2]+weelSepZ); // x,y,z
//dBodySetPosition(Bodies[2], startPosition[0]-weelSepXB, startPosition[1]-weelSepY,  startPosition[2]-weelSepZ); // x,y,z-0.9, 2.6,   9.3); // x,y,z
//dBodySetPosition(Bodies[3], startPosition[0]+weelSepXF, startPosition[1]-weelSepY,  startPosition[2]+weelSepZ); // x,y,z 0.9, 2.6,  10.7); // x,y,z
//dBodySetPosition(Bodies[4], startPosition[0]+weelSepXF, startPosition[1]-weelSepY,  startPosition[2]-weelSepZ); // x,y,z 0.9, 2.6,   9.3); // x,y,z



// wheel joints
for(i = 0; i < 4; ++i)
{
	
//	dJointAttach(Joints[i], Bodies[0], Bodies[i+1]);
//	const dReal* const wPos = dBodyGetPosition(Bodies[i+1]);
//	dJointSetHinge2Anchor(Joints[i], wPos[0], wPos[1], wPos[2]);
//	dJointSetHinge2Axis1(Joints[i], 0.f, 1.f, 0.f);
//	dJointSetHinge2Axis2(Joints[i], 0.f, 0.f, ((i % 2) == 0) ? -1.f : 1.f);

//	dJointSetHinge2Param(Joints[i], dParamLoStop, 0.f);
//	dJointSetHinge2Param(Joints[i], dParamHiStop, 0.f);
//	dJointSetHinge2Param(Joints[i], dParamFMax, 10000.f );
//	dJointSetHinge2Param(Joints[i], dParamFudgeFactor, 0.001f);

//	dJointSetHinge2Param(Joints[i], dParamVel2, 0.f);
//	dJointSetHinge2Param(Joints[i], dParamFMax2, 500.f);
	dReal k_p=20000000.f;//20000.f;
	dReal k_d=10.f;//1000.f;
	dReal h=0.02222f;


//	dJointSetHinge2Param(Joints[i], dParamSuspensionERP, h*k_p / (h*k_p + k_d));
//	dJointSetHinge2Param(Joints[i], dParamSuspensionCFM, 1.f / (h*k_p + k_d));

}



//m_pPhysicsShell->
}

//void CCar::S