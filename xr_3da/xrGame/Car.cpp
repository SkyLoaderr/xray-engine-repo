#include "stdafx.h"
#include "..\PGObject.h"
#include "Physics.h"
#include "car.h"
#include "hudmanager.h"
#include "cameralook.h"
#include "camerafirsteye.h"
#include "Actor.h"


static float car_snd_volume=1.f;

enum ECarCamType{
	ectFirst	= 0,
	ectChase,
	ectFree
};

BONE_P_MAP CCar::bone_map=BONE_P_MAP();

extern CPHWorld*	ph_world;

CCar::CCar(void)
{
	m_pExhaustPG1	=NULL;
	m_pExhaustPG2	=NULL;
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
	b_wheels_limited=false;
	e_state_steer=idle;
	e_state_drive=neutral;
	m_current_gear_ratio=dInfinity;
	rsp=false;lsp=false;fwp=false;bkp=false;brp=false;
	///////////////////////////////
	//////////////////////////////
	/////////////////////////////
}

CCar::~CCar(void)
{
	xr_delete			(camera[0]);
	xr_delete			(camera[1]);
	xr_delete			(camera[2]);
	xr_delete			(m_pExhaustPG1);
	xr_delete			(m_pExhaustPG2);
	snd_engine.destroy	();

}



void __stdcall  CCar::cb_Steer(CBoneInstance* B)
{
	CCar*	C			= dynamic_cast<CCar*>	(static_cast<CObject*>(B->Callback_Param));
	Fmatrix m;
	m.rotateZ(C->m_steering_wheels.begin()->GetSteerAngle());
	
	B->mTransform.mulB	(m);
}

void	CCar::cam_Update			(float dt)
{
	Fvector							P,Da;
	Da.set							(0,0,0);
	if(m_owner)	m_owner->setEnabled(false);
	float yaw_dest,pitch_dest,bank_dest;
	XFORM().getHPB				(yaw_dest,pitch_dest,bank_dest);

	switch(active_camera->tag) {
	case ectFirst:
		angle_lerp					(active_camera->yaw,	-yaw_dest+m_vCamDeltaHP.x,		PI_DIV_2,dt);
		angle_lerp					(active_camera->pitch,	-pitch_dest+m_vCamDeltaHP.y,	PI_DIV_2,dt);
		P.set						(-0.5f,1.5f,-.05f);
		XFORM().transform_tiny	(P);
		m_vCamDeltaHP.x				= m_vCamDeltaHP.x*(1-PI*dt)+((active_camera->lim_yaw.y+active_camera->lim_yaw.x)/2.f)*PI*dt;
		break;
	case ectChase:
		angle_lerp					(active_camera->yaw,	-(yaw_dest+m_vCamDeltaHP.x),	PI_DIV_4,dt);
		angle_lerp					(active_camera->pitch,	-pitch_dest+m_vCamDeltaHP.y,	PI_DIV_4,dt);
		Center					(P);
		m_vCamDeltaHP.x				= m_vCamDeltaHP.x*(1-PI*dt)+((active_camera->lim_yaw.y+active_camera->lim_yaw.x)/2.f)*PI*dt;
		break;
	case ectFree:
		Center					(P);
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
	car_snd_volume  				= pSettings->r_float(section,"snd_volume");
	snd_engine.create				(TRUE,"car\\car1");


}

BOOL	CCar::net_Spawn				(LPVOID DC)
{
	CSE_Abstract			*e	= (CSE_Abstract*)(DC);
	CSE_ALifeItemCar		*po	= dynamic_cast<CSE_ALifeItemCar*>(e);
	R_ASSERT(po);
	cNameVisual_set			(po->get_visual());
	BOOL R					= inherited::net_Spawn	(DC);

	setEnabled				(TRUE);
	setVisible				(TRUE);

	Sound->play_at_pos				(snd_engine,this,Position(),true);
	//ActivateJeep();
	//
	ParseDefinitions				();//parse ini filling in m_driving_wheels,m_steering_wheels,m_breaking_wheels
	CreateSkeleton					();//creates m_pPhysicsShell & fill in bone_map
	InitWheels						();//inits m_driving_wheels,m_steering_wheels,m_breaking_wheels values using recieved in ParceDefinitions & from bone_map
	m_ident=ph_world->AddObject(dynamic_cast<CPHObject*>(this));
	


	//m_pExhaustPG1					= xr_new<CPGObject>			("vehiclefx\\exhaust_1",Sector(),false);
	//m_pExhaustPG2					= xr_new<CPGObject>			("vehiclefx\\exhaust_1",Sector(),false);
	//m_pExhaustPG1->SetTransform		(XFORM());
	//m_pExhaustPG2->SetTransform		(XFORM());
	m_pPhysicsShell->set_PhysicsRefObject(this);
	
	return R;
}

void	CCar::net_Destroy()
{
	inherited::net_Destroy();
	if(m_pPhysicsShell)m_pPhysicsShell->Deactivate();
	ph_world->RemoveObject(m_ident);
	xr_delete(m_pExhaustPG1);
	xr_delete(m_pExhaustPG2);
}

void	CCar::Update				( u32 T )
{
	inherited::shedule_Update		(T);

	XFORM().set(m_pPhysicsShell->mXFORM);
	if	(m_owner)
		m_owner->XFORM().mul	(XFORM(),fmPosDriver);
}

void	CCar::UpdateCL				( )
{
	inherited::UpdateCL();
	XFORM().set(m_pPhysicsShell->mXFORM);

	Fvector lin_vel;
	m_pPhysicsShell->get_LinearVel(lin_vel);
	// Sound
	Fvector		C,V;
	Center	(C);
	V.set		(lin_vel);
	float		velocity						= V.magnitude();
	float		scale							= 0.5f+velocity/20.f; clamp(scale,0.5f,1.f);

	snd_engine.set_position			(C);
	snd_engine.set_frequency		(scale);
	snd_engine.set_volume			(car_snd_volume);

	// Camera
	if (IsMyCamera())				
		cam_Update	(Device.fTimeDelta);
	if(m_owner)
	{
		m_owner->XFORM().mul	(XFORM(),fmPosDriver);
		if(m_owner->IsMyCamera()) 
			cam_Update	(Device.fTimeDelta);
	}

// 	Fvector ang_vel,res_vel;
//	Fmatrix exhast,exhast_local;
	//m_pPhysicsShell->get_AngularVel(ang_vel);

/*
	exhast_local.set	(PKinematics(Visual())->LL_GetTransform(m_exhaust_ids[0]));
	exhast.mul			(XFORM(),exhast_local);
	res_vel.crossproduct(ang_vel,exhast_local.c);

	res_vel.add(lin_vel);
	m_pExhaustPG1->UpdateParent(exhast,res_vel);


	exhast_local.set(PKinematics(Visual())->LL_GetTransform(m_exhaust_ids[1]));
	exhast.mul(XFORM(),exhast_local);
	res_vel.crossproduct(ang_vel,exhast_local.c);
	res_vel.add(lin_vel);

	m_pExhaustPG2->UpdateParent(exhast,res_vel);
*/
}

void	CCar::renderable_Render				( )
{
	inherited::renderable_Render			();
}

void	CCar::net_Export			(NET_Packet& P)
{
}

void	CCar::net_Import			(NET_Packet& P)
{
}

void	CCar::IR_OnMouseMove			(int x, int y)
{
	if (Remote())					return;
}

void	CCar::IR_OnKeyboardPress		(int cmd)
{
	if (Remote())					return;

	switch (cmd)	
	{
	case kCAM_1:	OnCameraChange(ectFirst);	break;
	case kCAM_2:	OnCameraChange(ectChase);	break;
	case kCAM_3:	OnCameraChange(ectFree);	break;
	case kACCEL:	break;
	case kRIGHT:	PressRight();
		m_owner->steer_Vehicle(1);
		break;
	case kLEFT:		PressLeft();
		m_owner->steer_Vehicle(-1);
		break;
	case kUP:
		PressForward();
		//m_pExhaustPG2->Play				();
		//m_pExhaustPG1->Play				();
		break;
	case kDOWN:		;
		PressBack();
		//m_pExhaustPG2->Play				();
		//m_pExhaustPG1->Play				();
		break;
	case kJUMP:		
		PressBreaks();
		break;
	case kUSE:
		detach_Actor();

	};

}

void	CCar::IR_OnKeyboardRelease		(int cmd)
{
	if (Remote())					return;
	switch (cmd)	
	{
	case kACCEL:break;
	case kLEFT:		
		ReleaseLeft			  ();
		m_owner->steer_Vehicle(0);
		break;
	case kRIGHT:	ReleaseRight();
		m_owner->steer_Vehicle  (0);
		break;
	case kUP:		
		ReleaseForward			();
		//m_pExhaustPG1->Stop		();
		//m_pExhaustPG2->Stop		();
		break;
	case kDOWN:		;
		ReleaseBack				();
		//m_pExhaustPG1->Stop		();
		//m_pExhaustPG2->Stop		();
		break;
	case kREPAIR:	m_repairing=false; break;
	case kJUMP:		;
		ReleaseBreaks();
		break;
	};
}

void	CCar::OnCameraChange		(int type)
{
	if (!active_camera||active_camera->tag!=type){
		active_camera	= camera[type];
		float Y,P,R;
		XFORM().getHPB			(Y,P,R);
		active_camera->yaw			= -Y;
		m_vCamDeltaHP.y	= active_camera->pitch;
	}
}
void	CCar::IR_OnKeyboardHold		(int cmd)
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
	Fvector velocity;
	m_pPhysicsShell->get_LinearVel(velocity);
	HUD().pFontSmall->SetColor		(0xffffffff);
	HUD().pFontSmall->OutSet		(120,530);
	HUD().pFontSmall->OutNext		("Position:      [%3.2f, %3.2f, %3.2f]",VPUSH(Position()));
	HUD().pFontSmall->OutNext		("Velocity:      [%3.2f]",velocity.magnitude());
#endif
}





void CCar::Hit(float P,Fvector &dir,CObject *who,s16 element,Fvector p_in_object_space, float impulse)
{
	if(m_pPhysicsShell)		m_pPhysicsShell->applyImpulseTrace(p_in_object_space,dir,impulse,element);
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
	CKinematics* M		= PKinematics(Visual());			VERIFY(M);
	int id=M->LL_BoneID("pos_driver");
	CBoneInstance& instance=M->LL_GetInstance				(id);
	/////////////////////////////////////////////////////////////////////////
	//instance.set_callback(m_phSkeleton->GetBonesCallback(),element);
	fmPosDriver.set(instance.mTransform);
	return true;
}


bool CCar::is_Door(int id)
{
	if(id==m_doors_ids[0]) return true;
	if(id==m_doors_ids[1]) return true;
	return false;
}



void CCar::ParseDefinitions()
{
	bone_map.clear();
	bone_map.insert(mk_pair(0,physicsBone()));
	CKinematics* pKinematics=PKinematics(Visual());
	CInifile* ini = pKinematics->LL_UserData();
	if(! ini) return;
	feel_wheel_vector			(ini->r_string	("car_definition","driving_wheels"),m_driving_wheels);
	feel_wheel_vector			(ini->r_string	("car_definition","steering_wheels"),m_steering_wheels);
	feel_wheel_vector			(ini->r_string	("car_definition","breaking_wheels"),m_breaking_wheels);
	
	m_power				=		ini->r_float("car_definition","engine_power");
	m_power				*=		(0.8f*1000.f);
	m_max_rpm			=		ini->r_float("car_definition","max_engine_rpm");
	m_max_rpm			*=		(1.f/60.f*2.f*M_PI);
	//m_min_rpm			=		ini->r_float("car_definition","min_engine_rpm");
	m_idling_rpm		=		ini->r_float("car_definition","idling_engine_rpm");
	m_axle_friction		=		ini->r_float("car_definition","axle_friction");
	m_steering_speed	=		ini->r_float("car_definition","steering_speed");
	m_break_torque		=		ini->r_float("car_definition","break_torque");
	R_ASSERT2(ini->section_exist("transmission_gear_ratio"),"no section transmission_gear_ratio");
	m_gear_ratious.push_back(-ini->r_float("transmission_gear_ratio","R"));
	string32 rat_num;
	for(int i=1;true;i++)
	{
	sprintf(rat_num,"N%d",i);
	if(!ini->line_exist("transmission_gear_ratio",rat_num)) break;
	m_gear_ratious.push_back(ini->r_float("transmission_gear_ratio",rat_num));
	}
	
	
}

void CCar::CreateSkeleton()
{

	if (!Visual()) return;
	CKinematics* K= PKinematics(Visual());
	K->PlayCycle("idle");
	K->Calculate();

	K->LL_GetInstance				(K->LL_BoneID("steer")).set_callback			(cb_Steer,this);
	m_doors_ids[1]					=K->LL_BoneID("phy_door_left");
	m_doors_ids[0]					=K->LL_BoneID("phy_door_right");
	//m_exhaust_ids[0]				=K->LL_BoneID("pos_exhaust_1");
	//m_exhaust_ids[1]				=K->LL_BoneID("pos_exhaust_2");


	m_pPhysicsShell		= P_create_Shell();
	m_pPhysicsShell->build_FromKinematics(K,&bone_map);
	m_pPhysicsShell->set_PhysicsRefObject(this);
	m_pPhysicsShell->mXFORM.set(XFORM());
	m_pPhysicsShell->Activate(true);
	m_pPhysicsShell->SetAirResistance(0.f,0.f);
}

void CCar::InitWheels()
{
//get reference wheel radius
CKinematics* pKinematics=PKinematics(Visual());
CInifile* ini = pKinematics->LL_UserData();
SWheel& ref_wheel=m_wheels_map.find(pKinematics->LL_BoneID(ini->r_string("car_definition","reference_wheel")))->second;
ref_wheel.Init();
m_ref_radius=ref_wheel.radius;
m_power/=m_driving_wheels.size();
m_root_transform.set(bone_map.find(0)->second.element->mXFORM);

m_pPhysicsShell->set_DynamicScales(1.f,1.f);

{
	xr_map<int,SWheel>::iterator i,e;
	i=m_wheels_map.begin();
	e=m_wheels_map.end();
	for(;i!=e;i++)
		i->second.Init();
}

{
xr_vector<SWheelDrive>::iterator i,e;
i=m_driving_wheels.begin();
e=m_driving_wheels.end();
for(;i!=e;i++)
i->Init();
}

{
	xr_vector<SWheelBreak>::iterator i,e;
	i=m_breaking_wheels.begin();
	e=m_breaking_wheels.end();
	for(;i!=e;i++)
		i->Init();
}

{
	xr_vector<SWheelSteer>::iterator i,e;
	i=m_steering_wheels.begin();
	e=m_steering_wheels.end();
	for(;i!=e;i++)
		i->Init();
}

}

void CCar::Revert()
{
	//if(!bActive) return;
	//dBodyAddForce(Bodies[0], 0, 2*9000, 0);
	//dBodyAddRelTorque(Bodies[0], 300, 0, 0);
	m_pPhysicsShell->applyForce(0,2*9000,0);
}

void CCar::NeutralDrive()
{
//	for(u32 i = 0; i < 4; ++i){
//		dJointSetHinge2Param(Joints[i], dParamFMax2, 10.f);
//		dJointSetHinge2Param(Joints[i], dParamVel2, 0);
//	}
	
	xr_vector<SWheelDrive>::iterator i,e;
	i=m_driving_wheels.begin();
	e=m_driving_wheels.end();
	for(;i!=e;i++)
		i->Neutral();
		e_state_drive=neutral;
}
void CCar::Unbreak()
{
	xr_vector<SWheelBreak>::iterator i,e;
	i=m_breaking_wheels.begin();
	e=m_breaking_wheels.end();
	for(;i!=e;i++)
		i->Neutral();
	if(e_state_drive==drive) 
		Drive();
}
void CCar::Drive()
{
	m_pPhysicsShell->Enable();
	xr_vector<SWheelDrive>::iterator i,e;
	i=m_driving_wheels.begin();
	e=m_driving_wheels.end();
	for(;i!=e;i++)
		i->Drive();
	e_state_drive=drive;
	//	static const dReal wheelVelocity = 12.f * M_PI;//3*18.f * M_PI;
	/*
	ULONG i;

	if(!Breaks)
		switch(DriveDirection)
	{
		case 1:
			for(i = 0; i < 4; ++i)
				dJointSetHinge2Param(Joints[i], dParamVel2, ((i % 2) == 0) ? -DriveVelocity : DriveVelocity);
			break;

		case -1:
			for(i = 0; i < 4; ++i)
				dJointSetHinge2Param(Joints[i], dParamVel2, ((i % 2) == 0) ? DriveVelocity : -DriveVelocity);
			break;
		case 0:
			for(i = 0; i < 4; ++i){
				dJointSetHinge2Param(Joints[i], dParamVel2, 0.f);
				dJointSetHinge2Param(Joints[i], dParamFMax2, car_neutral_drive_resistance);
			}
			return;
	}
	else {
		for(i = 0; i < 2; ++i){

			dJointSetHinge2Param(Joints[i], dParamFMax2,car_breaks_resistance);
			dJointSetHinge2Param(Joints[i], dParamVel2, 0);
		}
		/////////////
		switch(DriveDirection)
		{
		case 1:
			for(i = 2; i < 4; ++i)
				dJointSetHinge2Param(Joints[i], dParamVel2, ((i % 2) == 0) ? -DriveVelocity : DriveVelocity);
			break;

		case -1:
			for(i = 2; i < 4; ++i)
				dJointSetHinge2Param(Joints[i], dParamVel2, ((i % 2) == 0) ? DriveVelocity : -DriveVelocity);
			break;
		case 0:
			for(i = 2; i < 4; ++i){
				dJointSetHinge2Param(Joints[i], dParamVel2, 0.f);
				dJointSetHinge2Param(Joints[i], dParamFMax2, 100);
			}
			return;
		}
		/////////////
		for(i=2;i<4;i++)
			dJointSetHinge2Param(Joints[i], dParamFMax2, DriveForce);
		return;
	}

	for(i=0;i<4;i++)
		dJointSetHinge2Param(Joints[i], dParamFMax2, DriveForce);
		*/
}

void CCar::SteerRight()
{
	m_pPhysicsShell->Enable();
	xr_vector<SWheelSteer>::iterator i,e;
	i=m_steering_wheels.begin();
	e=m_steering_wheels.end();
	for(;i!=e;i++)
		i->SteerRight();
	e_state_steer=right;
/*
	ULONG i;
	switch(steering)
	{
	case 1:
	case -1:
		weels_limited=true;
		for(i = 2; i < 4; ++i)
		{
			dJointSetHinge2Param(Joints[i], dParamHiStop, steeringLimit);
			dJointSetHinge2Param(Joints[i], dParamLoStop, -steeringLimit);
			dJointSetHinge2Param(Joints[i], dParamVel, ((i < 2) ? steering : -steering) * steeringRate);
		}
		break;

	default: // case 0
		weels_limited=false;
		for(i = 2; i < 4; ++i)
		{
			dReal angle = dJointGetHinge2Angle1(Joints[i]);


			if(angle < 0)
			{
				dJointSetHinge2Param(Joints[i], dParamHiStop, 0);

				dJointSetHinge2Param(Joints[i], dParamVel, steeringRate);
			}
			else
			{	

				dJointSetHinge2Param(Joints[i], dParamLoStop, 0);
				dJointSetHinge2Param(Joints[i], dParamVel, -steeringRate);
			}

		}
		break;
	}
*/
}
void CCar::SteerLeft()
{
	m_pPhysicsShell->Enable();
	xr_vector<SWheelSteer>::iterator i,e;
	i=m_steering_wheels.begin();
	e=m_steering_wheels.end();
	for(;i!=e;i++)
		i->SteerLeft();
	e_state_steer=left;
}

void CCar::SteerIdle()
{
	m_pPhysicsShell->Enable();
	xr_vector<SWheelSteer>::iterator i,e;
	i=m_steering_wheels.begin();
	e=m_steering_wheels.end();
	for(;i!=e;i++)
		i->SteerIdle();
	e_state_steer=idle;
}

void CCar::LimitWeels()
{
	/*
	if(!bActive) return;
	if(weels_limited) return;

	for(int i = 2; i < 4; ++i)
	{
		dReal angle = dJointGetHinge2Angle1(Joints[i]);
		if(dFabs(angle)<M_PI/180.f)
		{

			dJointSetHinge2Param(Joints[i], dParamHiStop, 0);
			dJointSetHinge2Param(Joints[i], dParamLoStop, 0);///
			dJointSetHinge2Param(Joints[i], dParamVel, 0);
			weels_limited=true;
		}
	}
*/
}
void CCar::Break()
{
	xr_vector<SWheelBreak>::iterator i,e;
	i=m_breaking_wheels.begin();
	e=m_breaking_wheels.end();
	for(;i!=e;i++)
		i->Break();
}

void CCar::PressRight()
{
	if(lsp)
	{
		if(!fwp)SteerIdle();
	}
	else
		SteerRight();
	rsp=true;
}
void CCar::PressLeft()
{
	if(rsp)
	{
		if(!fwp)SteerIdle();
	}
	else
		SteerLeft();
	lsp=true;
}
void CCar::PressForward()
{
	if(bkp) NeutralDrive();
	else 
	{
		Transmision(1);
		Drive();
	}
	fwp=true;
}
void CCar::PressBack()
{
	if(fwp) NeutralDrive();
	else 
	{
		Transmision(0);
		Drive();
	}
	bkp=true;
}
void CCar::PressBreaks()
{
	Break();
	brp=true;
}

void CCar::ReleaseRight()
{
	if(lsp)
		SteerLeft();
	else
		SteerIdle();
	rsp=false;
}
void CCar::ReleaseLeft()
{
	if(rsp)
		SteerRight();
	else
		SteerIdle();
	lsp=false;
}
void CCar::ReleaseForward()
{
if(bkp)
{
	Transmision(0);
	Drive();
}
else
	NeutralDrive();

fwp=false;
}
void CCar::ReleaseBack()
{
	if(fwp)
	{
		Transmision(1);
		Drive();
	}
	else
	{
		NeutralDrive();
	}
	bkp=false;
}
void CCar::ReleaseBreaks()
{
	Unbreak();
	brp=false;
}

void CCar::Transmision(size_t num)
{
if(num<m_gear_ratious.size())
m_current_gear_ratio=m_gear_ratious[num];
}

void CCar::SWheel::Init()
{
if(inited) return;
(bone_map.find(bone_id))->second.element->set_DynamicLimits(default_l_limit,default_w_limit*100.f);
radius=(bone_map.find(bone_id))->second.element->getRadius();
joint=(bone_map.find(bone_id))->second.joint->GetDJoint();
dJointSetHinge2Param(joint, dParamFMax2,0.f);//car->m_axle_friction
dJointSetHinge2Param(joint, dParamVel2, 0.f);
}



void CCar::SWheelDrive::Init()
{
pwheel->Init();
gear_factor=pwheel->radius/pwheel->car->m_ref_radius;
pos_fvd=bone_map.find(pwheel->bone_id)->second.element->mXFORM.k.dotproduct(pwheel->car->m_root_transform.i);
pos_fvd=pos_fvd>0.f ? -1.f : 1.f;


}
void CCar::SWheelDrive::Drive()
{
float cur_speed=pwheel->car->m_max_rpm/gear_factor/pwheel->car->m_current_gear_ratio;
dJointSetHinge2Param(pwheel->joint, dParamVel2, pos_fvd*cur_speed);
(cur_speed<0.f) ? (cur_speed=-cur_speed) :cur_speed;
dJointSetHinge2Param(pwheel->joint, dParamFMax2, pwheel->car->m_power/cur_speed);

}

void CCar::SWheelDrive::Neutral()
{
dJointSetHinge2Param(pwheel->joint, dParamFMax2, pwheel->car->m_axle_friction);
dJointSetHinge2Param(pwheel->joint, dParamVel2, 0.f);
}

void CCar::SWheelSteer::Init()
{
pwheel->Init();
(bone_map.find(pwheel->bone_id))->second.joint->GetLimits(lo_limit,hi_limit,0);
pos_right=bone_map.find(pwheel->bone_id)->second.element->mXFORM.i.dotproduct(pwheel->car->m_root_transform.j);
pos_right=pos_right>0.f ? -1.f : 1.f;
dJointSetHinge2Param(pwheel->joint, dParamFMax, 1000.f);
dJointSetHinge2Param(pwheel->joint, dParamVel, 0.f);
}

void CCar::SWheelSteer::SteerRight()
{
	if(pos_right>0)
	{
	
	dJointSetHinge2Param(pwheel->joint, dParamHiStop, hi_limit);
	dJointSetHinge2Param(pwheel->joint, dParamVel, pwheel->car->m_steering_speed);

	}
	else
	{
	dJointSetHinge2Param(pwheel->joint, dParamLoStop, lo_limit);
	dJointSetHinge2Param(pwheel->joint, dParamVel, -pwheel->car->m_steering_speed);
	}
}
void CCar::SWheelSteer::SteerLeft()
{
	if(pos_right<0)
	{

		dJointSetHinge2Param(pwheel->joint, dParamHiStop, hi_limit);
		dJointSetHinge2Param(pwheel->joint, dParamVel, pwheel->car->m_steering_speed);

	}
	else
	{
		dJointSetHinge2Param(pwheel->joint, dParamLoStop, lo_limit);
		dJointSetHinge2Param(pwheel->joint, dParamVel, -pwheel->car->m_steering_speed);
	}
}
void CCar::SWheelSteer::SteerIdle()
{
	if(pwheel->car->e_state_steer==right)
	{
		if(pos_right<0)
		{

			dJointSetHinge2Param(pwheel->joint, dParamHiStop, 0.f);
			dJointSetHinge2Param(pwheel->joint, dParamVel, pwheel->car->m_steering_speed);

		}
		else
		{
			dJointSetHinge2Param(pwheel->joint, dParamLoStop, 0.f);
			dJointSetHinge2Param(pwheel->joint, dParamVel, -pwheel->car->m_steering_speed);
		}
	}
	else
	{
		if(pos_right>0)
		{

			dJointSetHinge2Param(pwheel->joint, dParamHiStop,0.f);
			dJointSetHinge2Param(pwheel->joint, dParamVel, pwheel->car->m_steering_speed);

		}
		else
		{
			dJointSetHinge2Param(pwheel->joint, dParamLoStop, 0.f);
			dJointSetHinge2Param(pwheel->joint, dParamVel, -pwheel->car->m_steering_speed);
		}
	}
	
}

void CCar::SWheelSteer::Limit()
{
}
void CCar::SWheelBreak::Init()
{
pwheel->Init();
break_torque=pwheel->car->m_break_torque*pwheel->radius/pwheel->car->m_ref_radius;
}

void CCar::SWheelBreak::Break()
{
	dJointSetHinge2Param(pwheel->joint, dParamFMax2, break_torque);
	dJointSetHinge2Param(pwheel->joint, dParamVel2, 0.f);
}

void CCar::SWheelBreak::Neutral()
{
	dJointSetHinge2Param(pwheel->joint, dParamFMax2, pwheel->car->m_axle_friction);
	dJointSetHinge2Param(pwheel->joint, dParamVel2, 0.f);
}