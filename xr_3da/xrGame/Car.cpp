#include "stdafx.h"
#include "ParticlesObject.h"
#include "Physics.h"
#include "car.h"
#include "hudmanager.h"
#include "cameralook.h"
#include "camerafirsteye.h"
#include "Actor.h"
#define   _USE_MATH_DEFINES
#include "math.h"
#include "script_entity_action.h"
#include "inventory.h"
#include "xrserver_objects_alife_items.h"

BONE_P_MAP CCar::bone_map=BONE_P_MAP();

extern CPHWorld*	ph_world;

CCar::CCar(void)

{

	m_driver_anim_type = 0;
	m_bone_steer	= BI_NONE;
	active_camera	= 0;
	camera[ectFirst]= xr_new<CCameraFirstEye>	(this, pSettings, "car_firsteye_cam",	CCameraBase::flRelativeLink|CCameraBase::flPositionRigid); 
	camera[ectFirst]->tag	= ectFirst;
	camera[ectChase]= xr_new<CCameraLook>		(this, pSettings, "car_look_cam",		CCameraBase::flRelativeLink); 
	camera[ectChase]->tag	= ectChase;
	camera[ectFree]	= xr_new<CCameraLook>		(this, pSettings, "car_free_cam",		0); 
	camera[ectFree]->tag	= ectFree;
	OnCameraChange(ectFirst);

	m_repairing		=false;

	///////////////////////////////
	//////////////////////////////
	/////////////////////////////
	b_wheels_limited=false;
	b_engine_on=false;
	e_state_steer=idle;
	e_state_drive=neutral;
	m_current_gear_ratio=dInfinity;
	rsp=false;lsp=false;fwp=false;bkp=false;brp=false;
	///////////////////////////////
	//////////////////////////////
	/////////////////////////////
	m_exhaust_particles	="vehiclefx\\exhaust_1";
	m_car_sound			=xr_new<SCarSound>	(this);

	//у машины слотов в инвентаре нет
	inventory			= xr_new<CInventory>();
	inventory->SetSlotsUseful(false);
	m_doors_torque_factor = 2.f;
}

CCar::~CCar(void)
{
	xr_delete			(camera[0]);
	xr_delete			(camera[1]);
	xr_delete			(camera[2]);
	m_car_sound->Destroy();
	xr_delete			(m_car_sound);
	ClearExhausts();
	xr_delete			(inventory);
 //	xr_delete			(l_tpEntityAction);
}

void CCar::reinit		()
{
	CEntity::reinit			();
	CScriptMonster::reinit	();
}

void CCar::reload		(LPCSTR section)
{
	CEntity::reload			(section);
	CScriptMonster::reload	(section);
}


void __stdcall  CCar::cb_Steer(CBoneInstance* B)
{
	CCar*	C			= static_cast<CCar*>(B->Callback_Param);
	Fmatrix m;

	C->m_steer_angle=C->m_steering_wheels.begin()->GetSteerAngle()*0.1f+C->m_steer_angle*0.9f;
	m.rotateZ(C->m_steer_angle);

	B->mTransform.mulB	(m);
}

// Core events
void	CCar::Load					( LPCSTR section )
{
	inherited::Load					(section);
	//CPHSkeleton::Load(section);
	ISpatial*		self				=	smart_cast<ISpatial*> (this);
	if (self)		self->spatial.type	|=	STYPE_VISIBLEFORAI;	
}

BOOL	CCar::net_Spawn				(LPVOID DC)
{
#ifdef DEBUG
	Log("car spawn");
#endif
	CSE_Abstract					*e = (CSE_Abstract*)(DC);
	BOOL							R = inherited::net_Spawn(DC);
	CPHSkeleton::Spawn(e);
	setEnabled						(TRUE);
	setVisible						(TRUE);
	m_fSaveMaxRPM					= m_max_rpm;
	return							(CScriptMonster::net_Spawn(DC) && R);
}

void CCar::SpawnInitPhysics	(CSE_Abstract	*D)
{
	CSE_PHSkeleton		*so = smart_cast<CSE_PHSkeleton*>(D);
	R_ASSERT						(so);
	ParseDefinitions				();//parse ini filling in m_driving_wheels,m_steering_wheels,m_breaking_wheels
	CreateSkeleton					();//creates m_pPhysicsShell & fill in bone_map
	smart_cast<CKinematics*>(Visual())->CalculateBones	();
	Init							();//inits m_driving_wheels,m_steering_wheels,m_breaking_wheels values using recieved in ParceDefinitions & from bone_map
	SetDefaultNetState				(so);
	CPHUpdateObject::Activate       ();
}

void	CCar::net_Destroy()
{
	inherited::net_Destroy();
	CScriptMonster::net_Destroy();
	if(m_pPhysicsShell)
	{
		m_pPhysicsShell->Deactivate();
		m_pPhysicsShell->ZeroCallbacks();
		xr_delete(m_pPhysicsShell);
	}
	ClearExhausts();
	m_wheels_map.clear();
	m_steering_wheels.clear();
	m_driving_wheels.clear();
	m_exhausts.clear();
	m_breaking_wheels.clear();
	m_doors.clear();
	m_gear_ratious.clear();
	CPHUpdateObject::Deactivate();
	CKinematics* pKinematics=smart_cast<CKinematics*>(Visual());

	if(m_bone_steer!=BI_NONE)
	{
		
		pKinematics->LL_GetBoneInstance(m_bone_steer).set_callback(0,0);
		
	}
	CPHSkeleton::RespawnInit();
}

void CCar::net_Save(NET_Packet& P)
{
	inherited::net_Save(P);
	SaveNetState(P);

	
}



BOOL CCar::net_SaveRelevant()
{
	return TRUE;
}

void CCar::SaveNetState(NET_Packet& P)
{
	CPHSkeleton::SaveNetState	   (P);
	xr_map<u16,SDoor>::iterator i,e;
	i=m_doors.begin();
	e=m_doors.end();
	P.w_u16(u16(m_doors.size()));
	for(;i!=e;++i)
		i->second.SaveNetState(P);
}

void CCar::RestoreNetState(CSE_PHSkeleton* po)
{
	if(!po->_flags.test(CSE_PHSkeleton::flSavedData))return;
	CPHSkeleton::RestoreNetState(po);
	CSE_ALifeCar* co=smart_cast<CSE_ALifeCar*>(po);
	xr_map<u16,SDoor>::iterator i,e;
	xr_vector<u8>::iterator		ii=co->door_states.begin();
	i=m_doors.begin();
	e=m_doors.end();
	for(;i!=e;++i,++ii)
	{
		i->second.RestoreNetState(*ii);
	}
}
void CCar::SetDefaultNetState(CSE_PHSkeleton* po)
{
	if(po->_flags.test(CSE_PHSkeleton::flSavedData))return;
	xr_map<u16,SDoor>::iterator i,e;
	i=m_doors.begin();
	e=m_doors.end();
	for(;i!=e;++i)
	{
		i->second.SetDefaultNetState();
	}
}
void CCar::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);
	CPHSkeleton::Update(dt);
}

void	CCar::UpdateCL				( )
{
	inherited::UpdateCL();

	#ifdef DEBUG
	if(m_pPhysicsShell&&Owner())
	{
		Fvector v;
		m_pPhysicsShell->get_LinearVel(v);
		string32 s;
		sprintf(s,"speed, %f km/hour",v.magnitude()/1000.f*3600.f);
		HUD().pFontSmall->SetColor(color_rgba(0xff,0xff,0xff,0xff));
		HUD().pFontSmall->OutSet	(120,530);
		HUD().pFontSmall->OutNext(s);
		HUD().pFontSmall->OutNext("Transmission num:      [%d]",m_current_transmission_num);
		HUD().pFontSmall->OutNext("gear ratio:			  [%3.2f]",m_current_gear_ratio);
		HUD().pFontSmall->OutNext		("Power:      [%3.2f]",m_current_engine_power/(0.8f*1000.f));
		HUD().pFontSmall->OutNext		("rpm:      [%3.2f]",m_current_rpm/(1.f/60.f*2.f*M_PI));
		HUD().pFontSmall->OutNext		("wheel torque:      [%3.2f]",RefWheelCurTorque());
		HUD().pFontSmall->OutNext		("engine torque:      [%3.2f]",EngineCurTorque());
		HUD().pFontSmall->OutNext		("fuel:      [%3.2f]",m_fuel);
		//HUD().pFontSmall->OutNext("Vel Magnitude: [%3.2f]",m_PhysicMovementControl->GetVelocityMagnitude());
		//HUD().pFontSmall->OutNext("Vel Actual:    [%3.2f]",m_PhysicMovementControl->GetVelocityActual());
	}
	#endif
	//	Log("UpdateCL",Device.dwFrame);
	//XFORM().set(m_pPhysicsShell->mXFORM);
	VisualUpdate();
}

 void CCar::VisualUpdate()
{
	m_pPhysicsShell->InterpolateGlobalTransform(&XFORM());

	Fvector lin_vel;
	m_pPhysicsShell->get_LinearVel(lin_vel);
	// Sound
	Fvector		C,V;
	Center		(C);
	V.set		(lin_vel);

	m_car_sound->Update();
	if(Owner())
	{
		Owner()->XFORM().set(XFORM());
		if(m_pPhysicsShell->isEnabled())Owner()->XFORM().mulB	(m_sits_transforms[0]);

		if(Owner()->IsMyCamera()) 
			cam_Update	(Device.fTimeDelta);
	}

	UpdateExhausts();
	m_lights.Update();
}
void	CCar::renderable_Render				( )
{
	inherited::renderable_Render			();
}

void	CCar::net_Export			(NET_Packet& P)
{
//	P.w_u32 (Level().timeServer());
//	P.w_u16 (0);
}

void	CCar::net_Import			(NET_Packet& P)
{
//	u32 TimeStamp = 0;
//	P.w_u32 (TimeStamp);
//	u16 NumItems = 0;
//	P.w_u32 (NumItems);
}

void	CCar::OnHUDDraw				(CCustomHUD* /**hud/**/)
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

void CCar::Hit(float /**P/**/,Fvector &dir,CObject * /**who/**/,s16 element,Fvector p_in_object_space, float impulse, ALife::EHitType hit_type)
{
	if(m_bone_steer==element) return;
	if(m_pPhysicsShell)		m_pPhysicsShell->applyHit(p_in_object_space,dir,impulse,element,hit_type);
}

void CCar::detach_Actor()
{
	if(!Owner()) return;
	Owner()->setVisible(1);
	CHolderCustom::detach_Actor();
	NeutralDrive();
	Unclutch();
	ResetKeys();
	m_current_rpm=m_min_rpm;

	///Break();
}

bool CCar::attach_Actor(CActor* actor)
{
	if(Owner()) return false;
	CHolderCustom::attach_Actor(actor);

	CKinematics* K	= smart_cast<CKinematics*>(Visual());
	CInifile* ini	= K->LL_UserData();
	int id;
	if(ini->line_exist("car_definition","driver_place"))
		id=K->LL_BoneID(ini->r_string("car_definition","driver_place"));
	else
	{	
		Owner()->setVisible(0);
		id=K->LL_GetBoneRoot();
	}
	CBoneInstance& instance=K->LL_GetBoneInstance				(u16(id));
	m_sits_transforms.push_back(instance.mTransform);
	OnCameraChange(ectFirst);
	PPhysicsShell()->Enable();
	VisualUpdate();
	


	//CBoneData&	bone_data=K->LL_GetData(id);
	//Fmatrix driver_pos_tranform;
	//driver_pos_tranform.setHPB(bone_data.bind_hpb.x,bone_data.bind_hpb.y,bone_data.bind_hpb.z);
	//driver_pos_tranform.c.set(bone_data.bind_translate);
	//m_sits_transforms.push_back(driver_pos_tranform);
	return true;
}


bool CCar::is_Door(u16 id,xr_map<u16,SDoor>::iterator& i)
{
	i	= m_doors.find(id);
	if (i == m_doors.end()) 
	{
		return false;
	}
	else
	{
		if(i->second.joint)//temp for fake doors
			return true;
		else
			return false;
	}
}
bool CCar::is_Door(u16 id)
{
	xr_map<u16,SDoor>::iterator i;
	i	= m_doors.find(id);
	if (i == m_doors.end()) 
	{
		return false;
	}
	return true;
}

bool CCar::Enter(const Fvector& pos,const Fvector& dir,const Fvector& foot_pos)
{
	xr_map<u16,SDoor>::iterator i,e;

	i=m_doors.begin();e=m_doors.end();
	Fvector enter_pos;
	enter_pos.add(pos,foot_pos);
	enter_pos.mul(0.5f);
	for(;i!=e;++i)
	{
		if(i->second.CanEnter(pos,dir,enter_pos)) return true;
	}
	return false;
}

bool CCar::Exit(const Fvector& pos,const Fvector& dir)
{
	xr_map<u16,SDoor>::iterator i,e;

	i=m_doors.begin();e=m_doors.end();
	for(;i!=e;++i)
	{
		if(i->second.CanExit(pos,dir)) 
		{	
			i->second.GetExitPosition(m_exit_position);
			return true;
		}
	}
	return false;

}

void CCar::ParseDefinitions()
{
	bone_map.clear();

	CKinematics* pKinematics=smart_cast<CKinematics*>(Visual());
	bone_map.insert(mk_pair(pKinematics->LL_GetBoneRoot(),physicsBone()));
	CInifile* ini = pKinematics->LL_UserData();
	R_ASSERT2(ini,"Car has no description !!! See ActorEditor Object - UserData");

	m_camera_position			= ini->r_fvector3("car_definition","camera_pos");
	///////////////////////////car definition///////////////////////////////////////////////////
	fill_wheel_vector			(ini->r_string	("car_definition","driving_wheels"),m_driving_wheels);
	fill_wheel_vector			(ini->r_string	("car_definition","steering_wheels"),m_steering_wheels);
	fill_wheel_vector			(ini->r_string	("car_definition","breaking_wheels"),m_breaking_wheels);
	fill_exhaust_vector			(ini->r_string	("car_definition","exhausts"),m_exhausts);
	fill_doors_map				(ini->r_string	("car_definition","doors"),m_doors);

	///////////////////////////car properties///////////////////////////////


	m_max_power			=		ini->r_float("car_definition","engine_power");
	m_max_power			*=		(0.8f*1000.f);

	m_max_rpm			=		ini->r_float("car_definition","max_engine_rpm");
	m_max_rpm			*=		(1.f/60.f*2.f*M_PI);


	m_min_rpm			=		ini->r_float("car_definition","idling_engine_rpm");
	m_min_rpm			*=		(1.f/60.f*2.f*M_PI);

	m_power_rpm			=		ini->r_float("car_definition","max_power_rpm");
	m_power_rpm			*=		(1.f/60.f*2.f*M_PI);//

	m_torque_rpm		=		ini->r_float("car_definition","max_torque_rpm");
	m_torque_rpm		*=		(1.f/60.f*2.f*M_PI);//
	if(ini->line_exist("car_definition","exhaust_particles"))
	{
		m_exhaust_particles =ini->r_string("car_definition","exhaust_particles");
	}
			
	b_auto_switch_transmission= !!ini->r_bool("car_definition","auto_transmission");

	InitParabola		();

	m_axle_friction		=		ini->r_float("car_definition","axle_friction");
	m_steering_speed	=		ini->r_float("car_definition","steering_speed");
	m_break_torque		=		ini->r_float("car_definition","break_torque");
	m_hand_break_torque	=		ini->r_float("car_definition","break_torque");

	/////////////////////////transmission////////////////////////////////////////////////////////////////////////
	float main_gear_ratio=ini->r_float("car_definition","main_gear_ratio");

	R_ASSERT2(ini->section_exist("transmission_gear_ratio"),"no section transmission_gear_ratio");
	m_gear_ratious.push_back(ini->r_fvector3("transmission_gear_ratio","R"));
	m_gear_ratious[0][0]=-m_gear_ratious[0][0]*main_gear_ratio;
	string32 rat_num;
	for(int i=1;true;++i)
	{
		sprintf(rat_num,"N%d",i);
		if(!ini->line_exist("transmission_gear_ratio",rat_num)) break;
		Fvector gear_rat=ini->r_fvector3("transmission_gear_ratio",rat_num);
		gear_rat[0]*=main_gear_ratio;
		gear_rat[1]*=(1.f/60.f*2.f*M_PI);
		gear_rat[2]*=(1.f/60.f*2.f*M_PI);
		m_gear_ratious.push_back(gear_rat);
	}

	///////////////////////////////sound///////////////////////////////////////////////////////
	m_car_sound->Init();
	///////////////////////////////fuel///////////////////////////////////////////////////
	m_fuel_tank=ini->r_float("car_definition","fuel_tank");
	m_fuel=m_fuel_tank;
	m_fuel_consumption=ini->r_float("car_definition","fuel_consumption");
	m_fuel_consumption/=100000.f;
	if(ini->line_exist("car_definition","exhaust_particles"))
		m_exhaust_particles = ini->r_string("car_definition","exhaust_particles");
	///////////////////////////////lights///////////////////////////////////////////////////
	m_lights.Init(this);
	m_lights.ParseDefinitions();


	
	if(ini->section_exist("animations"))
	{
		m_driver_anim_type=ini->r_u16("animations","driver_animation_type");
	}

	
	if(ini->section_exist("doors"))
	{
		m_doors_torque_factor=ini->r_u16("doors","open_torque_factor");
	}

}

void CCar::CreateSkeleton()
{

	if (!Visual()) return;
	CSkeletonAnimated* K = smart_cast<CSkeletonAnimated*>(Visual());
	if(K)
	{
		K->PlayCycle		("idle");
		K->CalculateBones	();
	}


	m_pPhysicsShell		= P_create_Shell();
	m_pPhysicsShell->build_FromKinematics(smart_cast<CKinematics*>(Visual()),&bone_map);
	m_pPhysicsShell->set_PhysicsRefObject(this);
	m_pPhysicsShell->mXFORM.set(XFORM());
	m_pPhysicsShell->Activate(true);
	m_pPhysicsShell->SetAirResistance(0.f,0.f);
	m_pPhysicsShell->SetPrefereExactIntegration();

}

void CCar::Init()
{
#ifdef DEBUG
	Log("car spawn");
#endif
	//get reference wheel radius
	CKinematics* pKinematics=smart_cast<CKinematics*>(Visual());
	CInifile* ini = pKinematics->LL_UserData();
	R_ASSERT2(ini,"Car has no description !!! See ActorEditor Object - UserData");
	///SWheel& ref_wheel=m_wheels_map.find(pKinematics->LL_BoneID(ini->r_string("car_definition","reference_wheel")))->second;
	if(ini->line_exist("car_definition","steer"))
	{
		m_bone_steer=pKinematics->LL_BoneID(ini->r_string("car_definition","steer"));
		pKinematics->LL_GetBoneInstance(m_bone_steer).set_callback(cb_Steer,this);
	}
	//ref_wheel.Init();
	m_ref_radius=ini->r_float("car_definition","reference_radius");//ref_wheel.radius;

	b_engine_on=false;
	b_clutch   =false;
	b_starting =false;
	b_stalling =false;
	m_root_transform.set(bone_map.find(pKinematics->LL_GetBoneRoot())->second.element->mXFORM);
	m_current_transmission_num=0;
	m_pPhysicsShell->set_DynamicScales(1.f,1.f);

	{
		xr_map<u16,SWheel>::iterator i,e;
		i=m_wheels_map.begin();
		e=m_wheels_map.end();
		for(;i!=e;++i)
			i->second.Init();
	}

	{
		xr_vector<SWheelDrive>::iterator i,e;
		i=m_driving_wheels.begin();
		e=m_driving_wheels.end();
		for(;i!=e;++i)
			i->Init();
	}

	{
		xr_vector<SWheelBreak>::iterator i,e;
		i=m_breaking_wheels.begin();
		e=m_breaking_wheels.end();
		for(;i!=e;++i)
			i->Init();
	}

	{
		xr_vector<SWheelSteer>::iterator i,e;
		i=m_steering_wheels.begin();
		e=m_steering_wheels.end();
		for(;i!=e;++i)
			i->Init();
	}

	{
		xr_vector<SExhaust>::iterator i,e;
		i=m_exhausts.begin();
		e=m_exhausts.end();
		for(;i!=e;++i)
			i->Init();
	}

	{
		xr_map<u16,SDoor>::iterator i,e;
		i=m_doors.begin();
		e=m_doors.end();
		for(;i!=e;++i)
			i->second.Init();
	}
	Break();
	Transmision(1);
}

void CCar::Revert()
{
	//if(!bActive) return;
	//dBodyAddForce(Bodies[0], 0, 2*9000, 0);
	//dBodyAddRelTorque(Bodies[0], 300, 0, 0);

	m_pPhysicsShell->applyForce(0,40.f*m_pPhysicsShell->getMass(),0);
}

void CCar::NeutralDrive()
{

	xr_vector<SWheelDrive>::iterator i,e;
	i=m_driving_wheels.begin();
	e=m_driving_wheels.end();
	for(;i!=e;++i)
		i->Neutral();
	e_state_drive=neutral;
}
void CCar::Unbreak()
{
	xr_vector<SWheelBreak>::iterator i,e;
	i=m_breaking_wheels.begin();
	e=m_breaking_wheels.end();
	for(;i!=e;++i)
		i->Neutral();
	if(e_state_drive==drive) 
		Drive();
}
void CCar::Drive()
{

	if(!b_clutch||!b_engine_on) return;
	m_pPhysicsShell->Enable();
	m_current_rpm=EngineDriveSpeed();
	m_current_engine_power=EnginePower();
	xr_vector<SWheelDrive>::iterator i,e;
	i=m_driving_wheels.begin();
	e=m_driving_wheels.end();
	for(;i!=e;++i)
		i->Drive();
	e_state_drive=drive;

}

void CCar::StartEngine()
{
	if(m_fuel<EPS) return;
	PlayExhausts();
	//m_car_sound.Start();
	m_car_sound->Drive();
	b_engine_on=true;
	m_current_rpm=0.f;
}
void CCar::StopEngine()
{
	m_car_sound->Stop();
	StopExhausts();
	NeutralDrive();//set zero speed
	b_engine_on=false;
	UpdatePower();//set engine friction;
	m_current_rpm=0.f;
}

void CCar::Stall()
{
	m_car_sound->Stall();
	StopExhausts();
	NeutralDrive();//set zero speed
	b_engine_on=false;
	UpdatePower();//set engine friction;
	m_current_rpm=0.f;

}
void CCar::ReleasePedals()
{
	Clutch();
	NeutralDrive();//set zero speed
	UpdatePower();//set engine friction;
}

void CCar::SwitchEngine()
{
	if(b_engine_on) StopEngine();
	else			StartEngine();
}
void CCar::Clutch()
{
	b_clutch=true;
}

void CCar::Unclutch()
{
	b_clutch=false;
}

void CCar::Starter()
{
	b_starting=true;
	m_dwStartTime=Device.dwTimeGlobal;
}
void CCar::UpdatePower()
{
	m_current_rpm=EngineDriveSpeed();
	m_current_engine_power=EnginePower();

	if(b_auto_switch_transmission) 
	{
		VERIFY2(m_current_transmission_num<m_gear_ratious.size(),"wrong transmission");
		if(m_current_rpm<m_gear_ratious[m_current_transmission_num][1]) TransmisionDown();
		if(m_current_rpm>m_gear_ratious[m_current_transmission_num][2]) TransmisionUp();
	}

	xr_vector<SWheelDrive>::iterator i,e;
	i=m_driving_wheels.begin();
	e=m_driving_wheels.end();
	for(;i!=e;++i)
		i->UpdatePower();

	if(brp)
		Break();
}

void CCar::SteerRight()
{
	b_wheels_limited=true;  //no need to limit wheels when stiring
	m_pPhysicsShell->Enable();
	xr_vector<SWheelSteer>::iterator i,e;
	i=m_steering_wheels.begin();
	e=m_steering_wheels.end();
	for(;i!=e;++i)
		i->SteerRight();
	e_state_steer=right;

}
void CCar::SteerLeft()
{
	b_wheels_limited=true; //no need to limit wheels when stiring
	m_pPhysicsShell->Enable();
	xr_vector<SWheelSteer>::iterator i,e;
	i=m_steering_wheels.begin();
	e=m_steering_wheels.end();
	for(;i!=e;++i)
		i->SteerLeft();
	e_state_steer=left;
}

void CCar::SteerIdle()
{
	b_wheels_limited=false;
	m_pPhysicsShell->Enable();
	xr_vector<SWheelSteer>::iterator i,e;
	i=m_steering_wheels.begin();
	e=m_steering_wheels.end();
	for(;i!=e;++i)
		i->SteerIdle();
	e_state_steer=idle;
}

void CCar::LimitWheels()
{
	if(b_wheels_limited) return;
	b_wheels_limited=true;
	xr_vector<SWheelSteer>::iterator i,e;
	i=m_steering_wheels.begin();
	e=m_steering_wheels.end();
	for(;i!=e;++i)
		i->Limit();
}
void CCar::Break()
{
	xr_vector<SWheelBreak>::iterator i,e;
	i=m_breaking_wheels.begin();
	e=m_breaking_wheels.end();
	for(;i!=e;++i)
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
	if(bkp) 
	{	
		Unclutch();
		NeutralDrive();
	}
	else 
	{
		Clutch();
		if(0==m_current_transmission_num) Transmision(1);
		if(1==m_current_transmission_num||0==m_current_transmission_num)Starter();
		Drive();
	}
	fwp=true;
}
void CCar::PressBack()
{
	if(fwp) 
	{
		Unclutch();
		NeutralDrive();
	}
	else 
	{
		Clutch();
		Transmision(0);
		if(1==m_current_transmission_num||0==m_current_transmission_num)Starter();
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
		Clutch();
		Transmision(0);
		if(1==m_current_transmission_num||0==m_current_transmission_num)Starter();
		Drive();
	}
	else
	{
		Unclutch();
		NeutralDrive();
	}

	fwp=false;
}
void CCar::ReleaseBack()
{
	if(fwp)
	{
		Clutch();
		if(0==m_current_transmission_num) Transmision(1);
		if(1==m_current_transmission_num||0==m_current_transmission_num) Starter();
		Drive();
	}
	else
	{
		Unclutch();
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

	if(0==num||1==num)Starter();
	if(num<m_gear_ratious.size())
	{
		m_current_transmission_num=num;
		m_current_gear_ratio=m_gear_ratious[num][0];
		//	m_current_rpm=m_torque_rpm;
	}
#ifdef DEBUG
	//Log("Transmission switch %d",(u32)num);
#endif
}
void CCar::CircleSwitchTransmission()
{
	if(0==m_current_transmission_num)return;
	++m_current_transmission_num;
	m_current_transmission_num=m_current_transmission_num%m_gear_ratious.size();
	0==m_current_transmission_num ? m_current_transmission_num + 1 : m_current_transmission_num;
	Transmision(m_current_transmission_num);
	Drive();
}

void CCar::TransmisionUp()
{
	if(0==m_current_transmission_num)return;
	++m_current_transmission_num;
	size_t max_transmition_num=m_gear_ratious.size()-1;
	m_current_transmission_num>max_transmition_num ? m_current_transmission_num=max_transmition_num :m_current_transmission_num;
	Transmision(m_current_transmission_num);
	Drive();
}

void CCar::TransmisionDown()
{
	if(0==m_current_transmission_num)return;
	--m_current_transmission_num;
	m_current_transmission_num<1 ? m_current_transmission_num=1 : m_current_transmission_num;
	Transmision(m_current_transmission_num);
	Drive();
}
void CCar::InitParabola()
{
	//float t1=(m_power_rpm-m_torque_rpm);
	//float t2=m_max_power/m_power_rpm;
	//m_c = t2* (3.f*m_power_rpm - 4.f*m_torque_rpm)/t1/2.f;
	//t2/=m_power_rpm;
	//m_a = -t2/t1/2.f;
	//m_b = t2*m_torque_rpm/t1;


	//m_c = m_max_power* (3.f*m_power_rpm - 4.f*m_torque_rpm)/(m_power_rpm-m_torque_rpm)/2.f/m_power_rpm;
	//m_a = -m_max_power/(m_power_rpm-m_torque_rpm)/m_power_rpm/m_power_rpm/2.f;
	//m_b = m_max_power*m_torque_rpm/(m_power_rpm-m_torque_rpm)/m_power_rpm/m_power_rpm;




	m_a=expf((m_power_rpm - m_torque_rpm)/(2.f*m_power_rpm))*m_max_power/m_power_rpm;
	m_b=m_torque_rpm;
	m_c=-(1.41421356237309504880f*_sqrt(m_power_rpm*(m_power_rpm - m_torque_rpm)));


}
void CCar::PhTune(dReal step)
{
	if(m_repairing)Revert();
	LimitWheels();
	UpdateFuel(step);
	if(fwp||bkp)
	{	
		UpdatePower();
		if(b_engine_on&&!b_starting && m_current_rpm<m_min_rpm)Stall();
	}


}


void CCar::PlayExhausts()
{
	if(b_engine_on) return;
	xr_vector<SExhaust>::iterator i,e;
	i=m_exhausts.begin();
	e=m_exhausts.end();
	for(;i!=e;++i)
		i->Play();

}

void CCar::StopExhausts()
{
	if(!b_engine_on) return;
	xr_vector<SExhaust>::iterator i,e;
	i=m_exhausts.begin();
	e=m_exhausts.end();
	for(;i!=e;++i)
		i->Stop();
}

void CCar::UpdateExhausts()
{
	if(!b_engine_on) return;
	xr_vector<SExhaust>::iterator i,e;
	i=m_exhausts.begin();
	e=m_exhausts.end();
	for(;i!=e;++i)
		i->Update();
}

void CCar::ClearExhausts()
{
	xr_vector<SExhaust>::iterator i,e;
	i=m_exhausts.begin();
	e=m_exhausts.end();
	for(;i!=e;++i)
		i->Clear();
}

bool CCar::Use(const Fvector& pos,const Fvector& dir,const Fvector& foot_pos)
{
	xr_map<u16,SDoor>::iterator i;

	if(!Owner())
	{
		if(Enter(pos,dir,foot_pos)) return true;
	}
	
	Collide::ray_defs Q(pos, dir, 3.f, 0,Collide::rqtObject);  // CDB::OPT_ONLYFIRST CDB::OPT_ONLYNEAREST
	if (g_pGameLevel->ObjectSpace.RayQuery(collidable.model,Q))
	{
		Collide::rq_results& R = g_pGameLevel->ObjectSpace.r_results;
		int y=R.r_count();
		for (int k=0; k<y; ++k)
		{
			Collide::rq_result* I = R.r_begin()+k;
			if(is_Door((u16)I->element,i)) 
			{
				i->second.Use();
				return false;

			}
		}
	}

	if(Owner())return Exit(pos,dir);

	return false;

}
bool CCar::DoorUse(u16 id)
{

	xr_map<u16,SDoor>::iterator i;
	if(is_Door(id,i)) 
	{
		i->second.Use();
		return true;
	}
	else
	{
		return false;
	}

}

bool CCar::DoorSwitch(u16 id)
{
	xr_map<u16,SDoor>::iterator i;
	if(is_Door(id,i)) 
	{
		i->second.Switch();
		return true;
	}
	else
	{
		return false;
	}
}
bool CCar::DoorClose(u16 id)
{

	xr_map<u16,SDoor>::iterator i;
	if(is_Door(id,i)) 
	{
		i->second.Close();
		return true;
	}
	else
	{
		return false;
	}
}

bool CCar::DoorOpen(u16 id)
{

	xr_map<u16,SDoor>::iterator i;
	if(is_Door(id,i)) 
	{
		i->second.Open();
		return true;
	}
	else
	{
		return false;
	}
}

float CCar::Parabola(float rpm)
{
	//float rpm_2=rpm*rpm;
	//float value=(m_a*rpm_2*rpm_2*rpm_2+m_b*rpm_2+m_c)*rpm_2;
	float ex=(rpm-m_b)/m_c;
	float value=m_a*expf(-ex*ex)*rpm;
	if(value<0.f) return 0.f;
	return value;
}

float CCar::EnginePower()
{

	if(m_current_rpm<m_min_rpm)
	{
		if(b_starting) return Parabola(m_min_rpm);

	}
	else
	{
		if(b_starting&&Device.dwTimeGlobal-m_dwStartTime>1000) b_starting=false;
	}

	return Parabola(m_current_rpm);



}

float CCar::EngineDriveSpeed()
{
	//float wheel_speed,drive_speed=dInfinity;
	float drive_speed=0.f;
	xr_vector<SWheelDrive>::iterator i,e;
	i=m_driving_wheels.begin();
	e=m_driving_wheels.end();
	for(;i!=e;++i)
	{
		drive_speed+=i->ASpeed();
		//if(wheel_speed<drive_speed)drive_speed=wheel_speed;
	}
	return (0.5f*m_current_rpm+0.5f*dFabs(drive_speed*m_current_gear_ratio)/m_driving_wheels.size());
	//if(drive_speed<dInfinity) return dFabs(drive_speed*m_current_gear_ratio);
	//else					  return 0.f;
}



void CCar::UpdateFuel(float time_delta)
{
	if(!b_engine_on) return;
	if(m_current_rpm>m_min_rpm)
		m_fuel-=time_delta*(m_current_rpm-m_min_rpm)*m_fuel_consumption;
	else
		m_fuel-=time_delta*m_min_rpm*m_fuel_consumption;
	if(m_fuel<EPS) StopEngine();
}

float CCar::AddFuel(float ammount)
{
	float free_space=m_fuel_tank-m_fuel;
	if(ammount < free_space)
	{
		m_fuel+=ammount;
		return ammount;
	}
	else
	{
		m_fuel=m_fuel_tank;
		return free_space;
	}
}

void CCar::ResetKeys()
{
	bkp=false;
	fwp=false;
	lsp=false;
	rsp=false;
}


#undef   _USE_MATH_DEFINES

void CCar::OnEvent(NET_Packet& P, u16 type)
{
	inherited::OnEvent		(P,type);


	//обработка сообщений, нужных для работы с багажником машины
	u16 id;
	switch (type)
	{
	case GE_OWNERSHIP_TAKE:
		{
			P.r_u16		(id);
			CObject* O	= Level().Objects.net_Find	(id);

			if(GetInventory()->Take(smart_cast<CGameObject*>(O), false, false)) 
			{
				O->H_SetParent(this);
			}
			else 
			{
				if (!O || !O->H_Parent() || (this != O->H_Parent())) return;
				NET_Packet P;
				u_EventGen(P,GE_OWNERSHIP_REJECT,ID());
				P.w_u16(u16(O->ID()));
				u_EventSend(P);
			}
		}
		break;
	case GE_OWNERSHIP_REJECT:
		{
			P.r_u16		(id);
			CObject* O	= Level().Objects.net_Find	(id);

			if(GetInventory()->Drop(smart_cast<CGameObject*>(O))) 
			{
				O->H_SetParent(0);
			}
		}
		break;
	}

}

void CCar::ResetScriptData(void	*P)
{
	CScriptMonster::ResetScriptData(P);
	{
		CScriptEntityAction	l_tpEntityAction;
		l_tpEntityAction.m_tMovementAction.SetInputKeys(CScriptMovementAction::eInputKeyEngineOff);
		bfAssignMovement(&l_tpEntityAction);
	}
	m_max_rpm		= m_fSaveMaxRPM;
}

void CCar::PhDataUpdate(dReal /**step/**/)
{
	for (int k=0; k<(int)m_doors_update.size(); ++k){
		SDoor* D = m_doors_update[k];
		if (!D->update)
		{
			m_doors_update.erase(m_doors_update.begin()+k);
			--k;
		}
		else
		{
			D->Update();
		}
	}
	if (GetScriptControl())
		ProcessScripts();
}

BOOL CCar::UsedAI_Locations()
{
	return					(TRUE);
}

u16 CCar::DriverAnimationType()
{
	return m_driver_anim_type;
}
//void CCar::object_contactCallbackFun(bool& do_colide,dContact& c,SGameMtl * /*material_1*/,SGameMtl * /*material_2*/)
//{
//
//	dxGeomUserData *l_pUD1 = NULL;
//	dxGeomUserData *l_pUD2 = NULL;
//	l_pUD1 = retrieveGeomUserData(c.geom.g1);
//	l_pUD2 = retrieveGeomUserData(c.geom.g2);
//
//	if(! l_pUD1) return;
//	if(!l_pUD2) return;
//
//	CEntityAlive* capturer=smart_cast<CEntityAlive*>(l_pUD1->ph_ref_object);
//	if(capturer)
//	{
//		CPHCapture* capture=capturer->m_PhysicMovementControl->PHCapture();
//		if(capture)
//		{
//			if(capture->m_taget_element->PhysicsRefObject()==l_pUD2->ph_ref_object)
//			{
//				do_colide = false;
//				capture->m_taget_element->Enable();
//				if(capture->e_state==CPHCapture::cstReleased) capture->ReleaseInCallBack();
//			}
//
//		}
//
//
//	}
//
//	capturer=smart_cast<CEntityAlive*>(l_pUD2->ph_ref_object);
//	if(capturer)
//	{
//		CPHCapture* capture=capturer->m_PhysicMovementControl->PHCapture();
//		if(capture)
//		{
//			if(capture->m_taget_element->PhysicsRefObject()==l_pUD1->ph_ref_object)
//			{
//				do_colide = false;
//				capture->m_taget_element->Enable();
//				if(capture->e_state==CPHCapture::cstReleased) capture->ReleaseInCallBack();
//			}
//
//		}
//
//	}
//}

template <class T> IC void CCar::fill_wheel_vector(LPCSTR S,xr_vector<T>& type_wheels)
{
	CKinematics* pKinematics	=smart_cast<CKinematics*>(Visual());
	string64					S1;
	int count =					_GetItemCount(S);
	for (int i=0 ;i<count; ++i) 
	{
		_GetItem					(S,i,S1);

		u16 bone_id	=				pKinematics->LL_BoneID(S1);

		type_wheels.push_back		(T());
		T& twheel				= type_wheels.back();


		BONE_P_PAIR_IT J		= bone_map.find(bone_id);
		if (J == bone_map.end()) 
		{
			bone_map.insert(mk_pair(bone_id,physicsBone()));


			SWheel& wheel			=	(m_wheels_map.insert(mk_pair(bone_id,SWheel(this)))).first->second;
			wheel.bone_id			=	bone_id;
			twheel.pwheel			=	&wheel;
		}
		else
		{
			twheel.pwheel			=	&(m_wheels_map.find(bone_id))->second;
		}
	}
}

IC void CCar::fill_exhaust_vector(LPCSTR S,xr_vector<SExhaust>& exhausts)
{
	CKinematics* pKinematics	=smart_cast<CKinematics*>(Visual());
	string64					S1;
	int count =					_GetItemCount(S);
	for (int i=0 ;i<count; ++i) 
	{
		_GetItem					(S,i,S1);

		u16 bone_id	=				pKinematics->LL_BoneID(S1);

		exhausts.push_back		(SExhaust(this));
		SExhaust& exhaust				= exhausts.back();
		exhaust.bone_id						= bone_id;

		BONE_P_PAIR_IT J		= bone_map.find(bone_id);
		if (J == bone_map.end()) 
		{
			bone_map.insert(mk_pair(bone_id,physicsBone()));
		}

	}
}

IC void CCar::fill_doors_map(LPCSTR S,xr_map<u16,SDoor>& doors)
{
	CKinematics* pKinematics	=smart_cast<CKinematics*>(Visual());
	string64					S1;
	int count =					_GetItemCount(S);
	for (int i=0 ;i<count; ++i) 
	{
		_GetItem					(S,i,S1);

		u16 bone_id	=				pKinematics->LL_BoneID(S1);
		SDoor						door(this);
		door.bone_id=				bone_id;
		doors.insert				(mk_pair(bone_id,door));
		BONE_P_PAIR_IT J		= bone_map.find(bone_id);
		if (J == bone_map.end()) 
		{
			bone_map.insert(mk_pair(bone_id,physicsBone()));
		}

	}
}
