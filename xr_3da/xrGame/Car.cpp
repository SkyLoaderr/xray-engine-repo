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
#include "../skeletonanimated.h"
#include "level.h"
#include "ui/UIMainIngameWnd.h"

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
	m_power_increment_factor=0.5f;
	m_rpm_increment_factor=0.5f;
	b_breaks=false;
	m_break_start=0.f;
	m_break_time=1.;
	m_breaks_to_back_rate=1.f;
	m_death_time=u32(-1);
	m_time_to_explode=5000;
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
	CScriptEntity::reinit	();
}

void CCar::reload		(LPCSTR section)
{
	CEntity::reload			(section);
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

BOOL	CCar::net_Spawn				(CSE_Abstract* DC)
{
	CSE_Abstract					*e = (CSE_Abstract*)(DC);
	CSE_ALifeCar					*co=smart_cast<CSE_ALifeCar*>(e);
	BOOL							R = inherited::net_Spawn(DC);
	CPHSkeleton::Spawn(e);
	setEnabled						(TRUE);
	setVisible						(TRUE);
	m_fSaveMaxRPM					= m_max_rpm;
	fEntityHealth					=co->health;
	CDamagableItem::RestoreEffect();
	return							(CScriptEntity::net_Spawn(DC) && R);
}

void CCar::SpawnInitPhysics	(CSE_Abstract	*D)
{
	CSE_PHSkeleton		*so = smart_cast<CSE_PHSkeleton*>(D);
	R_ASSERT						(so);
	ParseDefinitions				();//parse ini filling in m_driving_wheels,m_steering_wheels,m_breaking_wheels
	CreateSkeleton					();//creates m_pPhysicsShell & fill in bone_map
	CKinematics *K					=smart_cast<CKinematics*>(Visual());
	K->CalculateBones_Invalidate();//this need to call callbacks
	K->CalculateBones	();
	Init							();//inits m_driving_wheels,m_steering_wheels,m_breaking_wheels values using recieved in ParceDefinitions & from bone_map
	SetDefaultNetState				(so);
	CPHUpdateObject::Activate       ();
}

void	CCar::net_Destroy()
{
	inherited::net_Destroy();
	CScriptEntity::net_Destroy();
	CExplosive::net_Destroy();
	if(m_pPhysicsShell)
	{
		m_pPhysicsShell->Deactivate();
		m_pPhysicsShell->ZeroCallbacks();
		xr_delete(m_pPhysicsShell);
	}
	CHolderCustom::detach_Actor();
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
	m_damage_particles.Clear();
	CPHCollisionDamageReceiver::Clear();
	m_death_time=u32(-1);
	b_breaks=false;
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
	P.w_vec3(Position());
	Fvector Angle;
	XFORM().getHPB(Angle);
	P.w_vec3(Angle);
	{
		xr_map<u16,SDoor>::iterator i,e;
		i=m_doors.begin();
		e=m_doors.end();
		P.w_u16(u16(m_doors.size()));
		for(;i!=e;++i)
			i->second.SaveNetState(P);
	}

	{
		xr_map<u16,SWheel>::iterator i,e;
		i=m_wheels_map.begin();
		e=m_wheels_map.end();
		P.w_u16(u16(m_wheels_map.size()));
		for(;i!=e;++i)
			i->second.SaveNetState(P);
	}
	P.w_float(fEntityHealth);
}

void CCar::RestoreNetState(CSE_PHSkeleton* po)
{
	if(!po->_flags.test(CSE_PHSkeleton::flSavedData))return;
	CPHSkeleton::RestoreNetState(po);
	
	CSE_ALifeCar* co=smart_cast<CSE_ALifeCar*>(po);

	{
		xr_map<u16,SDoor>::iterator i,e;
		xr_vector<CSE_ALifeCar::SDoorState>::iterator		ii=co->door_states.begin();
		i=m_doors.begin();
		e=m_doors.end();
		for(;i!=e;++i,++ii)
		{
			i->second.RestoreNetState(*ii);
		}
	}
	{
		xr_map<u16,SWheel>::iterator i,e;
		xr_vector<CSE_ALifeCar::SWheelState>::iterator		ii=co->wheel_states.begin();
		i=m_wheels_map.begin();
		e=m_wheels_map.end();
		for(;i!=e;++i,++ii)
		{
			i->second.RestoreNetState(*ii);
		}
	}
/////////////////////////////////////////////////////////////////////////
	Fmatrix restored_form;
	PPhysicsShell()->GetGlobalTransformDynamic(&restored_form);
	Fmatrix inv ,replace,sof;
	sof.setHPB(co->o_Angle.x,co->o_Angle.y,co->o_Angle.z);
	sof.c.set(co->o_Position);
	inv.set(restored_form);
	inv.invert();
	replace.mul(sof,inv);
	PPhysicsShell()->TransformPosition(replace);
	PPhysicsShell()->GetGlobalTransformDynamic(&XFORM());
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
	if(fEntityHealth<=0.f && m_death_time!=u32(-1)&& Device.dwTimeGlobal-m_death_time>m_time_to_explode)
	{
		CarExplode();
	}
}

void	CCar::UpdateCL				( )
{
	inherited::UpdateCL();
	CExplosive::UpdateCL();
	#ifdef DEBUG
	if(m_pPhysicsShell&&Owner() && bDebug)
	{
		Fvector v;
		m_pPhysicsShell->get_LinearVel(v);
		string32 s;
		sprintf(s,"speed, %f km/hour",v.magnitude()/1000.f*3600.f);
		HUD().Font().pFontSmall->SetColor(color_rgba(0xff,0xff,0xff,0xff));
		HUD().Font().pFontSmall->OutSet	(120,530);
		HUD().Font().pFontSmall->OutNext(s);
		HUD().Font().pFontSmall->OutNext("Transmission num:      [%d]",m_current_transmission_num);
		HUD().Font().pFontSmall->OutNext("gear ratio:			  [%3.2f]",m_current_gear_ratio);
		HUD().Font().pFontSmall->OutNext		("Power:      [%3.2f]",m_current_engine_power/(0.8f*1000.f));
		HUD().Font().pFontSmall->OutNext		("rpm:      [%3.2f]",m_current_rpm/(1.f/60.f*2.f*M_PI));
		HUD().Font().pFontSmall->OutNext		("wheel torque:      [%3.2f]",RefWheelCurTorque());
		HUD().Font().pFontSmall->OutNext		("engine torque:      [%3.2f]",EngineCurTorque());
		HUD().Font().pFontSmall->OutNext		("fuel:      [%3.2f]",m_fuel);
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
		
		if(m_pPhysicsShell->isEnabled())
		{
			Owner()->XFORM().set(XFORM());
			Owner()->XFORM().mulB	(m_sits_transforms[0]);
		}
		if(Owner()->IsMyCamera()) 
			cam_Update	(Device.fTimeDelta);
		HUD().GetUI()->UIMainIngameWnd->CarPanel().SetSpeed(lin_vel.magnitude()/1000.f*3600.f/100.f);
		HUD().GetUI()->UIMainIngameWnd->CarPanel().SetRPM(m_current_rpm/m_max_rpm/2.f);
	}

	UpdateExhausts	();
	m_lights.Update	();
}
void	CCar::renderable_Render				( )
{
	inherited::renderable_Render			();
}

void	CCar::net_Export			(NET_Packet& P)
{
	inherited::net_Export(P);
//	P.w_u32 (Level().timeServer());
//	P.w_u16 (0);
}

void	CCar::net_Import			(NET_Packet& P)
{
	inherited::net_Import(P);
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
	HUD().Font().pFontSmall->SetColor		(0xffffffff);
	HUD().Font().pFontSmall->OutSet		(120,530);
	HUD().Font().pFontSmall->OutNext		("Position:      [%3.2f, %3.2f, %3.2f]",VPUSH(Position()));
	HUD().Font().pFontSmall->OutNext		("Velocity:      [%3.2f]",velocity.magnitude());


#endif
}

void CCar::Hit(float P,Fvector &dir,CObject * who,s16 element,Fvector p_in_object_space, float impulse, ALife::EHitType hit_type)
{
	//if(||) P=0.f;
	WheelHit(P,element,hit_type);
	DoorHit(P,element,hit_type);
	float hitScale=1.f,woundScale=1.f;
	if(hit_type!=ALife::eHitTypeStrike) CDamageManager::HitScale(element, hitScale, woundScale);
	P *= m_HitTypeK[hit_type]*hitScale;
	
	inherited::Hit(P,dir,who,element,p_in_object_space,impulse,hit_type);
	if(fEntityHealth<=0.f)CExplosive::SetInitiator(who->ID());
	HitEffect();
	if(Owner()&&Owner()->ID()==Level().CurrentEntity()->ID())
		HUD().GetUI()->UIMainIngameWnd->CarPanel().SetCarHealth(fEntityHealth/100.f);
}
void CCar::PHHit(float P,Fvector &dir, CObject *who,s16 element,Fvector p_in_object_space, float impulse, ALife::EHitType hit_type)
{
	if(m_bone_steer==element) return;
	if(m_pPhysicsShell)		m_pPhysicsShell->applyHit(p_in_object_space,dir,impulse,element,hit_type);
}


void CCar::ApplyDamage(u16 level)
{
	CDamagableItem::ApplyDamage(level);
	switch(level)
	{
		case 1: m_damage_particles.Play1(this);break;
		case 2: 
			{
				m_damage_particles.Play2(this);
			}
											   break;
		case 3: m_fuel=0.f;
				m_death_time=Device.dwTimeGlobal; break;	
	}

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
	HUD().GetUI()->UIMainIngameWnd->CarPanel().Show(false);
	///Break();
	//H_SetParent(NULL);
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
	

	HUD().GetUI()->UIMainIngameWnd->CarPanel().Show(true);
	HUD().GetUI()->UIMainIngameWnd->CarPanel().SetCarHealth(fEntityHealth/100.f);
	//HUD().GetUI()->UIMainIngameWnd.ShowBattery(true);
	//CBoneData&	bone_data=K->LL_GetData(id);
	//Fmatrix driver_pos_tranform;
	//driver_pos_tranform.setHPB(bone_data.bind_hpb.x,bone_data.bind_hpb.y,bone_data.bind_hpb.z);
	//driver_pos_tranform.c.set(bone_data.bind_translate);
	//m_sits_transforms.push_back(driver_pos_tranform);
	//H_SetParent(actor);
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
	CExplosive::Load(ini,"explosion");
	CExplosive::SetInitiator(ID());
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

	if(ini->line_exist("car_definition","power_increment_factor"))
		m_power_increment_factor=	ini->r_float("car_definition","power_increment_factor");

	if(ini->line_exist("car_definition","rpm_increment_factor"))
		m_rpm_increment_factor=	ini->r_float("car_definition","rpm_increment_factor");

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

	if(ini->line_exist("car_definition","hand_break_torque"))
	{
		m_hand_break_torque=ini->r_float("car_definition","hand_break_torque");
	}
	if(ini->line_exist("car_definition","break_time"))
	{
		m_break_time=ini->r_float("car_definition","break_time");
	}
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

	m_damage_particles.Init(this);
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
	CDamagableItem::Init(fEntityHealth,3);

	{
		xr_map<u16,SWheel>::iterator i,e;
		i=m_wheels_map.begin();
		e=m_wheels_map.end();
		for(;i!=e;++i)
		{
			i->second.Init();
			i->second.CDamagableHealthItem::Init(100.f,2);
		}
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
		{
			i->second.Init();
			i->second.CDamagableHealthItem::Init(100,1);
		}
			
	}

	if(ini->section_exist("damage_items"))
	{
		CInifile::Sect& data		= ini->r_section("damage_items");
		for (CInifile::SectIt I=data.begin(); I!=data.end(); I++){
			CInifile::Item& item	= *I;
			u16 index				= pKinematics->LL_BoneID(*item.first); 
			R_ASSERT3(index != BI_NONE, "Wrong bone name", *item.first);
			xr_map   <u16,SWheel>::iterator i=m_wheels_map.find(index);
			
			if(i!=m_wheels_map.end())
					i->second.CDamagableHealthItem::Init(float(atof(*item.second)),2);
			else 
			{
				xr_map   <u16,SDoor>::iterator i=m_doors.find(index);
				R_ASSERT3(i!=m_doors.end(),"only wheel and doors bones allowed for damage defs",*item.first);
				i->second.CDamagableHealthItem::Init(float(atof(*item.second)),1);
			}

		}
	}
	CPHCollisionDamageReceiver::Init();

	if(ini->section_exist("immunities"))
	{
		InitImmunities("immunities",ini);
	}

	CDamageManager::reload("car_definition",ini);
	
	HandBreak();
	Transmission(1);
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
void CCar::ReleaseHandBreak()
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
	b_starting=true;
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
		VERIFY2(CurrentTransmission()<m_gear_ratious.size(),"wrong transmission");
		if(m_current_rpm<m_gear_ratious[CurrentTransmission()][1]) TransmissionDown();
		if(m_current_rpm>m_gear_ratious[CurrentTransmission()][2]) TransmissionUp();
	}

	xr_vector<SWheelDrive>::iterator i,e;
	i=m_driving_wheels.begin();
	e=m_driving_wheels.end();
	for(;i!=e;++i)
		i->UpdatePower();

	if(brp)
		HandBreak();
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
void CCar::HandBreak()
{
	xr_vector<SWheelBreak>::iterator i,e;
	i=m_breaking_wheels.begin();
	e=m_breaking_wheels.end();
	for(;i!=e;++i)
		i->HandBreak();
}

void CCar::StartBreaking()
{
	if(!b_breaks)
	{
		b_breaks=true;
		m_break_start=Device.fTimeGlobal;
	}
}
void CCar::StopBreaking()
{
	b_breaks=false;
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
		DriveForward();
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
		//DriveBack();
		Unclutch();
		NeutralDrive();
		StartBreaking();
	}
	bkp=true;
}
void CCar::PressBreaks()
{
	HandBreak();
	brp=true;
}

void CCar::DriveBack()
{
	Clutch();
	Transmission(0);
	if(1==CurrentTransmission()||0==CurrentTransmission())Starter();
	Drive();
}
void CCar::DriveForward()
{
	Clutch();
	if(0==CurrentTransmission()) Transmission(1);
	if(1==CurrentTransmission()||0==CurrentTransmission())Starter();
	Drive();
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
		Transmission(0);
		if(1==CurrentTransmission()||0==CurrentTransmission())Starter();
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
		if(0==CurrentTransmission()) Transmission(1);
		if(1==CurrentTransmission()||0==CurrentTransmission()) Starter();
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
	ReleaseHandBreak();
	brp=false;
}

void CCar::Transmission(size_t num)
{

	if(num<m_gear_ratious.size())
	{
		if(CurrentTransmission()!=num)m_car_sound->TransmissionSwitch();
		m_current_transmission_num=num;
		m_current_gear_ratio=m_gear_ratious[num][0];

	}
#ifdef DEBUG
	//Log("Transmission switch %d",(u32)num);
#endif
}
void CCar::CircleSwitchTransmission()
{
	if(0==CurrentTransmission())return;
	size_t transmission=1+CurrentTransmission();
	transmission=transmission%m_gear_ratious.size();
	0==transmission ? transmission++ : transmission;
	Transmission(transmission);
	Drive();
}

void CCar::TransmissionUp()
{
	if(0==CurrentTransmission())return;
	size_t transmission=1+CurrentTransmission();
	size_t max_transmition_num=m_gear_ratious.size()-1;
	transmission>max_transmition_num ? transmission=max_transmition_num :transmission;
	Transmission(transmission);
	Drive();
}

void CCar::TransmissionDown()
{
	if(0==CurrentTransmission())return;
	size_t transmission=CurrentTransmission()-1;
	transmission<1 ? transmission=1 : transmission;
	Transmission(transmission);
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
	m_c=_sqrt(2.f*m_power_rpm*(m_power_rpm - m_torque_rpm));


}
void CCar::PhTune(dReal step)
{

}
void CCar::UpdateBack()
{
	if(b_breaks)
	{
		float k=1.f;
		float time=(Device.fTimeGlobal-m_break_start);
		if(time<m_break_time)
		{
			k*=(time/m_break_time);
		}
		xr_vector<SWheelBreak>::iterator i,e;
		i=m_breaking_wheels.begin();
		e=m_breaking_wheels.end();
		for(;i!=e;++i)
				i->Break(k);
		if(DriveWheelsMeanAngleRate()<m_breaks_to_back_rate)
		{
			DriveBack();
			StopBreaking();
		}
	}
	//else
	//{
	//	UpdatePower();
	//	if(b_engine_on&&!b_starting && m_current_rpm<m_min_rpm)Stall();
	//}
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
	
	collide::ray_defs Q(pos, dir, 3.f, 0,collide::rqtObject);  // CDB::OPT_ONLYFIRST CDB::OPT_ONLYNEAREST
	if (g_pGameLevel->ObjectSpace.RayQuery(collidable.model,Q))
	{
		collide::rq_results& R = g_pGameLevel->ObjectSpace.r_results;
		int y=R.r_count();
		for (int k=0; k<y; ++k)
		{
			collide::rq_result* I = R.r_begin()+k;
			if(is_Door((u16)I->element,i)) 
			{
				bool front=i->second.IsFront(pos,dir);
				if((Owner()&&!front)||(!Owner()&&front))i->second.Use();
				if(i->second.state==SDoor::broken) break;
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

	float value;
	if(m_current_rpm<m_min_rpm)
	{
		if(b_starting) value= Parabola(m_min_rpm);

	}
	else
	{
		if(b_starting&&Device.dwTimeGlobal-m_dwStartTime>1000) 
														b_starting=false;
	}

	value = Parabola(m_current_rpm);
	return value * m_power_increment_factor+m_current_engine_power*(1.f-m_power_increment_factor);
}
float CCar::DriveWheelsMeanAngleRate()
{
	xr_vector<SWheelDrive>::iterator i,e;
	i=m_driving_wheels.begin();
	e=m_driving_wheels.end();
	float drive_speed=0.f;
	for(;i!=e;++i)
	{
		drive_speed+=i->ASpeed();
		//if(wheel_speed<drive_speed)drive_speed=wheel_speed;
	}
	return drive_speed/m_driving_wheels.size();
}
float CCar::EngineDriveSpeed()
{
	//float wheel_speed,drive_speed=dInfinity;
	float calc_rpm=dFabs(DriveWheelsMeanAngleRate()*m_current_gear_ratio);
	if(!b_clutch&&calc_rpm<m_min_rpm)
	{
		calc_rpm=m_min_rpm;
	}
	return		(1.f-m_rpm_increment_factor)*m_current_rpm+m_rpm_increment_factor*calc_rpm;
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
	CExplosive::OnEvent		(P,type);

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
	CScriptEntity::ResetScriptData(P);
	if (PPhysicsShell())
	{
		CScriptEntityAction	l_tpEntityAction;
		l_tpEntityAction.m_tMovementAction.SetInputKeys(CScriptMovementAction::eInputKeyEngineOff);
		bfAssignMovement(&l_tpEntityAction);
	}
	m_max_rpm		= m_fSaveMaxRPM;
}

void CCar::PhDataUpdate(dReal step)
{
	if (GetScriptControl())
			ProcessScripts();

		if(m_repairing)Revert();
		LimitWheels();
		UpdateFuel(step);
		//if(fwp)
		{	
			UpdatePower();
			if(b_engine_on&&!b_starting && m_current_rpm<m_min_rpm)Stall();
		}
		if(bkp)
		{
			UpdateBack();
		}
//////////////////////////////////////////////////////////
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

}

BOOL CCar::UsedAI_Locations()
{
	return					(TRUE);
}

u16 CCar::DriverAnimationType()
{
	return m_driver_anim_type;
}

void CCar::OnAfterExplosion()
{

}

void CCar::OnBeforeExplosion()
{

}
void CCar::CarExplode()
{

	m_death_time=u32(-1);
	CExplosive::GenExplodeEvent(Position(),Fvector().set(0.f,1.f,0.f));
	//m_damage_particles.PlayExplosion(this);
	//m_car_sound->Explosion();
	//
	//if (Owner()&&OnServer())
	//{
	//	NET_Packet	l_P;
	//	u_EventGen	(l_P,GE_HIT, Owner()->ID());
	//	l_P.w_u16	(u16(Owner()->ID()));
	//	l_P.w_u16	(ID());
	//	l_P.w_dir	(Fvector().set(0.f,-1.f,0.f));//dir
	//	l_P.w_float	(100.f);
	//	l_P.w_s16	(0/*(s16)BI_NONE*/);
	//	Fvector		position_in_bone_space={0.f,0.f,0.f};
	//	l_P.w_vec3	(position_in_bone_space);
	//	l_P.w_float	(0.f);
	//	l_P.w_u16	(ALife::eHitTypeExplosion);
	//	u_EventSend	(l_P);
	//	/////////////////////////////////////////////////////////
	//	return;
	//};
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

DLL_Pure *CCar::_construct			()
{
	inherited::_construct		();
	CScriptEntity::_construct	();
	return						(this);
}

u16 CCar::Initiator()
{
	if(g_Alive() && Owner())
	{
		return Owner()->ID();
	}
	else return CExplosive::Initiator();
}