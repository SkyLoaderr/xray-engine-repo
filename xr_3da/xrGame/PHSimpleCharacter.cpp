

#include "stdafx.h"
#include "PHDynamicData.h"

#include "ExtendedGeom.h"
#include "../cl_intersect.h"
#include "tri-colliderKNoOPC\__aabb_tri.h"
#include "PHSimpleCharacter.h"
#include "PHContactBodyEffector.h"
#include "ui/uistatic.h"
#include "SpaceUtils.h"
#include "PhysicsGamePars.h"
#include "MathUtils.h"
#include "level.h"
#include "gamemtllib.h"
#include "gameobject.h"
#include "physicsshellholder.h"
const float LOSE_CONTROL_DISTANCE=0.5f; //fly distance to lose control
const float CLAMB_DISTANCE=0.5f;
const float CLIMB_GETUP_HEIGHT=0.3f;
static u16 lastMaterial;

////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////class//CPHSimpleCharacter////////////////////
CPHSimpleCharacter::CPHSimpleCharacter()
{

	m_object_contact_callback=NULL;

	m_geom_shell=NULL;
	m_wheel=NULL;

	m_space=NULL;
	m_wheel_transform=NULL;
	m_shell_transform=NULL;

	m_hat=NULL;
	m_hat_transform=NULL;

	m_acceleration.set(0,0,0);
	b_external_impulse=false;
	m_phys_ref_object=NULL;
	p_lastMaterial=&lastMaterial;
	b_on_object=false;
	b_climb=false;
	b_pure_climb=true;
	m_friction_factor=1.f;

	m_control_force[0]=0.f;
	m_control_force[1]=0.f;
	m_control_force[2]=0.f;

	m_depart_position[0]=0.f;
	m_depart_position[1]=0.f;
	m_depart_position[2]=0.f;
	is_contact=false;
	was_contact=false;
	is_control=false;
	b_depart=false;
	b_meet=false;
	b_lose_control=true;
	b_lose_ground=true;
	b_jump=false;
	b_side_contact=false;
	b_clamb_jump=false;
	b_any_contacts=false;
	b_valide_ground_contact=false;
	b_valide_wall_contact=false;
	b_jump=false;
	b_exist=false;
	m_mass=70.f;
	m_max_velocity=5.f;
	//m_update_time=0.f;
	b_meet_control=false;
	b_jumping=false;
	b_death_pos=false;
	jump_up_velocity=6.f;
	m_air_control_factor=0;

	m_capture_joint=NULL;


}


void __stdcall TestPathCallback(bool& do_colide,dContact& c,SGameMtl * /*material_1*/,SGameMtl * /*material_2*/)
{
	do_colide=false;
}


void CPHSimpleCharacter::Create(dVector3 sizes){

	if(b_exist) return;
	b_air_contact_state=false;
	b_climb_getup=false;
	b_near_leader=false;
	////////////////////////////////////////////////////////
	/*
	m_control_force[0]=0.f;
	m_control_force[1]=0.f;
	m_control_force[2]=0.f;

	m_depart_position[0]=0.f;
	m_depart_position[1]=0.f;
	m_depart_position[2]=0.f;
	b_external_impulse=false;
	b_on_object=false;
	b_was_on_object=true;
	b_jumping=false;
	is_contact=false;
	was_contact=false;
	is_control=false;
	b_depart=false;
	b_meet=false;
	b_lose_control=true;
	b_jump=false;
	b_side_contact=false;
	b_clamb_jump=false;
	b_any_contacts=false;
	b_valide_ground_contact=false;
	b_valide_wall_contact=false;
	b_jump=false;
	b_depart_control=false;
	b_saved_contact_velocity=false;
	m_update_time=0.f;
	b_meet_control=false;
	m_contact_velocity=0.f;
	//////////////////////////////////////
	previous_p[0]=dInfinity;
	dis_count_f=0;
	//////////////////////////
	*/
	m_radius=_min(sizes[0],sizes[2])/2.f;
	m_cyl_hight=sizes[1]-2.f*m_radius;
	if (m_cyl_hight<0.f) m_cyl_hight=0.01f;

	b_exist=true;
	const dReal k=1.20f;
	dReal doun=m_radius*_sqrt(1.f-1.f/k/k)/2.f;
	m_geom_shell=dCreateCylinder(0,m_radius/k,m_cyl_hight+doun);

	m_wheel=dCreateSphere(0,m_radius);
	m_hat=dCreateSphere(0,m_radius/k);




	m_shell_transform=dCreateGeomTransform(0);
	dGeomTransformSetCleanup(m_shell_transform,0);
	m_hat_transform=dCreateGeomTransform(0);
	dGeomTransformSetCleanup(m_hat_transform,0);
	//m_wheel_transform=dCreateGeomTransform(0);

	dGeomTransformSetInfo(m_shell_transform,1);
	dGeomTransformSetInfo(m_hat_transform,1);
	//dGeomTransformSetInfo(m_wheel_transform,1);
	////////////////////////////////////////////////////////////////////////


	dGeomSetPosition(m_hat,0.f,m_cyl_hight,0.f);
	////////////////////////////////////////////////////////////////////////

	//dGeomSetPosition(chSphera2,0.f,-0.7f,0.f);

	////////////////////////////////////////////////////////////////////////
	dGeomSetPosition(m_geom_shell,0.f,m_cyl_hight/2.f-doun,0.f);

	dGeomTransformSetGeom(m_hat_transform,m_hat);
	//dGeomTransformSetGeom(m_wheel_transform,m_wheel);
	dGeomTransformSetGeom(m_shell_transform,m_geom_shell);
	m_body=dBodyCreate(0);
	Island().AddBody(m_body);
	dGeomSetBody(m_shell_transform,m_body);
	dGeomSetBody(m_hat_transform,m_body);
	//dGeomSetBody(m_wheel_transform,m_body);
	dGeomSetBody(m_wheel,m_body);
	//dBodySetPosition(m_body,-10,3,0);
	//dGeomCreateUserData(m_wheel_transform);
	//dGeomCreateUserData(m_wheel);
	//dGeomCreateUserData(m_cap_transform);
	//dGeomCreateUserData(m_shell_transform);
	dGeomCreateUserData(m_geom_shell);

	dGeomCreateUserData(m_wheel);
	dGeomCreateUserData(m_hat);


	//dGeomUserDataSetPhObject(m_wheel_transform,(CPHUpdateObject*)this);
	dGeomUserDataSetPhObject(m_wheel,(CPHObject*)this);
	//dGeomUserDataSetPhObject(m_shell_transform,(CPHUpdateObject*)this);
	dGeomUserDataSetPhObject(m_geom_shell,(CPHObject*)this);
	//dGeomUserDataSetPhObject(m_cap_transform,(CPHUpdateObject*)this);

	dGeomUserDataSetPhObject(m_hat,(CPHObject*)this);
	//dGeomGetUserData(m_cap_transform)->friction=0.f;
	//dGeomGetUserData(m_shell_transform)->friction=0.f;
	/////////////////////////////////////////////////////////////////////////
	dMass m;
	dMassSetBox(&m,1,1000000.f,1000000.f,1000000.f);
	dMassAdjust(&m,m_mass);
	dBodySetMass(m_body,&m);

	m_space=dSimpleSpaceCreate(0);
	//dGeomGroupAdd(m_geom_group,m_wheel_transform);
	dSpaceAdd(m_space,m_wheel);

	dSpaceAdd(m_space,m_shell_transform);
	dSpaceAdd(m_space,m_hat_transform);
	//dGeomGroupAdd(chRGeomGroup,chRCylinder);
	m_body_interpolation.SetBody(m_body);



	float test_radius=m_radius*2.f;
	float test_height=test_radius+m_radius/2.f;
	m_cap=dCreateSphere(0,test_radius);
	dGeomSetPosition(m_cap,0.f,test_height,0.f);
	m_cap_transform=dCreateGeomTransform(0);
	dGeomTransformSetCleanup(m_cap_transform,0);
	dGeomTransformSetInfo(m_cap_transform,1);
	dGeomTransformSetGeom(m_cap_transform,m_cap);
	dGeomCreateUserData(m_cap);


	dGeomUserDataSetPhObject(m_cap,(CPHObject*)this);
	dSpaceAdd(m_space,m_cap_transform);
	dGeomSetBody(m_cap_transform,m_body);
	dGeomUserDataSetObjectContactCallback(m_cap,TestPathCallback);
	if(m_phys_ref_object)
	{
		dGeomUserDataSetPhysicsRefObject(m_geom_shell,m_phys_ref_object);
		dGeomUserDataSetPhysicsRefObject(m_wheel,m_phys_ref_object);
		dGeomUserDataSetPhysicsRefObject(m_hat,m_phys_ref_object);
		dGeomUserDataSetPhysicsRefObject(m_cap,m_phys_ref_object);
	}
	if(m_object_contact_callback)
	{
		dGeomUserDataSetObjectContactCallback(m_hat,m_object_contact_callback);
		dGeomUserDataSetObjectContactCallback(m_geom_shell,m_object_contact_callback);
		dGeomUserDataSetObjectContactCallback(m_wheel,m_object_contact_callback);
		//dGeomUserDataSetObjectContactCallback(m_cap,m_object_contact_callback);
	}
	CPHObject::activate();
	spatial_register();
}
void CPHSimpleCharacter::Destroy(){
	if(!b_exist) return;
	b_exist=false;
	//if(ph_world)
	CPHObject::deactivate();
	spatial_unregister();


	if(m_geom_shell){
		dGeomDestroyUserData(m_geom_shell);
		dGeomDestroy(m_geom_shell);
		m_geom_shell=NULL;
	}

	if(m_wheel) {
		dGeomDestroyUserData(m_wheel);
		dGeomDestroy(m_wheel);
		m_wheel=NULL;
	}

	if(m_shell_transform){
		dGeomDestroyUserData(m_shell_transform);
		dGeomDestroy(m_shell_transform);
		m_shell_transform=NULL;
	}




	if(m_wheel_transform){
		dGeomDestroyUserData(m_wheel_transform);
		dGeomDestroy(m_wheel_transform);
		m_wheel_transform=NULL;
	}


	if(m_hat){
		dGeomDestroyUserData(m_hat);
		dGeomDestroy(m_hat);
		m_hat=NULL;
	}
	if(m_hat_transform){
		dGeomDestroyUserData(m_hat_transform);
		dGeomDestroy(m_hat_transform);
		m_hat_transform=NULL;
	}
	if(m_space){
		dSpaceDestroy(m_space);
		m_space=NULL;
	}

	if(m_body) {
		Island().RemoveBody(m_body);
		dBodyDestroy(m_body);
		m_body=NULL;
	}


}

void		CPHSimpleCharacter::ApplyImpulse(const Fvector& dir,const dReal P)
{
	if(!b_exist) return;
	//if(!dBodyIsEnabled(m_body)) dBodyEnable(m_body);
	Enable();
	b_lose_control=true;
	b_external_impulse=true;
	dBodyAddForce(m_body,dir.x*P/fixed_step,dir.y*P/fixed_step,dir.z*P/fixed_step);
}

void		CPHSimpleCharacter::ApplyForce(const Fvector& force)
{
	ApplyForce(force.x,force.y,force.z);
}

void		CPHSimpleCharacter::ApplyForce(float x, float y, float z)
{
	if(!b_exist) return;
	Enable();
	dBodyAddForce(m_body,x,y,z);
}

void		CPHSimpleCharacter::ApplyForce(const Fvector& dir,float force)
{
	ApplyForce(dir.x*force,dir.y*force,dir.z*force);
}

void CPHSimpleCharacter::PhDataUpdate(dReal /**step/**/){
	///////////////////


	SafeAndLimitVelocity();

	if( !dBodyIsEnabled(m_body)) {
		if(!ph_world->IsFreezed())b_lose_control=false;
		return;
	}
	if(is_contact&&!is_control&&!b_lose_ground)
		Disabling();

	if( !dBodyIsEnabled(m_body)) {

		return;
	}
	///////////////////////
	b_external_impulse=false;
	was_contact=is_contact;
	was_control=is_control;
	is_contact=false;
	b_side_contact=false;
	b_any_contacts=false;
	b_valide_ground_contact=false;
	b_valide_wall_contact=false;

	b_climb=false;
	b_pure_climb=true;
	b_was_on_object=b_on_object;
	b_on_object=false;
	b_death_pos=false;
	m_contact_count=0;


	dMatrix3 R;
	dRSetIdentity (R);
	dBodySetAngularVel(m_body,0.f,0.f,0.f);
	dBodySetRotation(m_body,R);

	dMass mass;
	const float		*linear_velocity		=dBodyGetLinearVel(m_body);
	dReal			linear_velocity_mag		=_sqrt(dDOT(linear_velocity,linear_velocity));
	dBodyGetMass(m_body,&mass);
	dReal l_air=linear_velocity_mag*default_k_l;//force/velocity !!!
	if(l_air>mass.mass/fixed_step) l_air=mass.mass/fixed_step;//validate

	if(!fis_zero(l_air))
		dBodyAddForce(
		m_body,
		-linear_velocity[0]*l_air,
		-linear_velocity[1]*l_air,
		-linear_velocity[2]*l_air
		);

	m_body_interpolation.UpdatePositions();
}

void CPHSimpleCharacter::PhTune(dReal /**step/**/){

	//if(b_jumping)
	//{
	//	Log("vel" ,*((const Fvector*)(dBodyGetLinearVel(m_body))));
	//	Log("pos" ,*((const Fvector*)(dBodyGetPosition(m_body))));
	//	Log("time %f",Device.fTimeGlobal);
	//}
	//if(!b_exist)return;
	b_air_contact_state=!is_contact;
	bool b_good_graund=b_valide_ground_contact&&m_ground_contact_normal[1]>M_SQRT1_2;
	if(!b_death_pos)dVectorSet(m_death_position,dGeomGetPosition(m_wheel));
	CPHContactBodyEffector* contact_effector=
		(CPHContactBodyEffector*) dBodyGetData(m_body);
	if(contact_effector)contact_effector->Apply();

	if(!dBodyIsEnabled(m_body)) 
	{	
		if(!ph_world->IsFreezed())b_lose_control=false;
		return;
	}

	if(m_acceleration.magnitude()>0.f) is_control=true;
	else							   is_control=false;

	b_pure_climb=b_pure_climb&&b_any_contacts&&(!b_lose_control);
	if(b_pure_climb) 
			b_at_wall=true;

	if(b_lose_control || (!b_climb&&is_contact)||
		(!b_pure_climb)//b_good_graund=b_valide_ground_contact&&m_ground_contact_normal[1]>M_SQRT1_2//&&b_good_graund
		) 
		b_at_wall=false;

	b_depart=was_contact&&(!is_contact);
	b_stop_control=was_control&&(!is_control);
	b_meet=(!was_contact)&&(is_contact);
	if(b_lose_control&&is_contact)b_meet_control=true;
	b_on_ground=b_valide_ground_contact ||(b_meet&&(!b_depart));


	if(b_at_wall ) 
	{
		// b_clamb_jump=true;
		b_side_contact=false;
		m_friction_factor=1.f;
		if(b_stop_control) 
			dBodySetLinearVel(m_body,0.f,0.f,0.f);
	}

	//save depart position
	if(b_depart) 
		dVectorSet(m_depart_position,dBodyGetPosition(m_body));

	const dReal* velocity=dBodyGetLinearVel(m_body);
	dReal linear_vel_smag=dDOT(velocity,velocity);
	if(b_lose_control											 &&
		b_on_ground												 &&
		m_ground_contact_normal[1]>M_SQRT1_2/2.f				 && 
		!b_external_impulse										 && 
		dSqrt(velocity[0]*velocity[0]+velocity[2]*velocity[2])<5.||
		fis_zero(linear_vel_smag)								 ||
		b_climb
		) 
		b_lose_control=false;
	
	
	if(b_jumping&&b_good_graund ||(b_at_wall&&b_valide_wall_contact)) //b_good_graund=b_valide_ground_contact&&m_ground_contact_normal[1]>M_SQRT1_2
		b_jumping=false;

	//deside if control lost
	if(!b_on_ground)
	{
		const dReal* current_pos=dBodyGetPosition(m_body);
		dVector3 dif={current_pos[0]-m_depart_position[0],
			current_pos[1]-m_depart_position[1],
			current_pos[2]-m_depart_position[2]};
		if(dDOT(dif,dif)>
			LOSE_CONTROL_DISTANCE*
			LOSE_CONTROL_DISTANCE&&dFabs(dif[1])>0.1)
		{
			b_lose_control=true;
			b_depart_control=true;
		}
	}

	if(b_on_object)
	{
		//deside to stop clamb
		if(b_clamb_jump){
			const dReal* current_pos=dBodyGetPosition(m_body);
			dVector3 dif={current_pos[0]-m_clamb_depart_position[0],
				current_pos[1]-m_clamb_depart_position[1],
				current_pos[2]-m_clamb_depart_position[2]};
			if( //!b_valide_wall_contact||
				//(dDOT(dif,dif)>CLAMB_DISTANCE*CLAMB_DISTANCE))	//	(m_wall_contact_normal[1]> M_SQRT1_2) ||							]
				dFabs(dif[1])>CLAMB_DISTANCE){
					b_clamb_jump=false;
					//	dBodySetLinearVel(m_body,0.f,0.f,0.f);
				}
		}

		//decide to clamb
		if(!b_climb&&b_valide_wall_contact && (m_contact_count>1)&&(m_wall_contact_normal[1]<M_SQRT1_2 )&& !b_side_contact ) //&& dDOT(m_wall_contact_normal,m_ground_contact_normal)<.9f
		{
			//if( dDOT(m_wall_contact_normal,m_ground_contact_normal)<.999999f)
			//dVector3 diff={m_wall_contact_normal[0]-m_ground_contact_normal[0],m_wall_contact_normal[1]-m_ground_contact_normal[1],m_wall_contact_normal[2]-m_ground_contact_normal[2]};
			//if( dDOT(diff,diff)>0.001f)
			if(((m_wall_contact_position[0]-m_ground_contact_position[0])*m_control_force[0]+
				(m_wall_contact_position[2]-m_ground_contact_position[2])*m_control_force[2])>0.05f &&
				m_wall_contact_position[1]-m_ground_contact_position[1]>0.01f)
				b_clamb_jump=true;


		}

		if(b_valide_wall_contact && (m_contact_count>1)&& b_clamb_jump)
			if(
				dFabs((m_wall_contact_position[0]-m_ground_contact_position[0])+		//*m_control_force[0]
				(m_wall_contact_position[2]-m_ground_contact_position[2]))>0.05f &&//0.01f//*m_control_force[2]
				m_wall_contact_position[1]-m_ground_contact_position[1]>0.01f)
				dVectorSet(m_clamb_depart_position,dBodyGetPosition(m_body));
	}

	else b_clamb_jump=ValidateWalkOn();

	//jump	
	if(b_jump)
	{
		b_lose_control=true;
		dBodySetLinearVel(m_body,m_jump_accel.x,m_jump_accel.y,m_jump_accel.z);//vel[1]+
		//Log("jmp",m_jump_accel);
		dVectorSet(m_jump_depart_position,dBodyGetPosition(m_body));
		//m_jump_accel=m_acceleration;
		b_jump=false;
		b_jumping=true;
	}

	b_lose_ground=!(b_good_graund||b_at_wall)||b_lose_control;



	ApplyAcceleration();
	


	dReal* chVel=const_cast<dReal*>(dBodyGetLinearVel(m_body));
	//if(b_jump)
	//	dBodyAddForce(m_body,0.f,m_control_force[1],0.f);//+2.f*9.8f*70.f
	//else
	dMass m;
	dBodyGetMass(m_body,&m);
	if(is_control){
		dVector3 sidedir;
		dVector3 y={0.,1.,0.};
		dCROSS(sidedir,=,m_control_force,y);
		dNormalize3(sidedir);
		dReal vProj=dDOT(sidedir,chVel);

		dBodyAddForce(m_body,m_control_force[0],m_control_force[1],m_control_force[2]);//+2.f*9.8f*70.f
		if(!b_lose_control||b_clamb_jump)
			dBodyAddForce(m_body,
				-sidedir[0]*vProj*(500.f+200.f*b_clamb_jump)*m_friction_factor,
				-m.mass*(50.f+90.f*b_at_wall)*(!b_lose_control&&!(is_contact||(b_climb&&b_any_contacts))),//&&!b_climb
				-sidedir[2]*vProj*(500.f+200.f*b_clamb_jump)*m_friction_factor
				);
		//if(b_clamb_jump){
		//dNormalize3(m_control_force);
		//dReal proj=dDOT(m_control_force,chVel);
		//if(proj<0.f)
		//		dBodyAddForce(m_body,-chVel[0]*500.f,-chVel[1]*500.f,-chVel[2]*500.f);
		//		}
	}
	if(!b_jumping&&b_at_wall)//!is_control&&
		dBodyAddForce(m_body,0.f,m.mass*world_gravity,0.f);

	if(b_jumping)
	{

		dReal proj=m_acceleration.x*chVel[0]+m_acceleration.z*chVel[2];

		const dReal* current_pos=dBodyGetPosition(m_body);
		dVector3 dif={current_pos[0]-m_jump_depart_position[0],
			current_pos[1]-m_jump_depart_position[1],
			current_pos[2]-m_jump_depart_position[2]};
		dReal amag =_sqrt(m_acceleration.x*m_acceleration.x+m_acceleration.z*m_acceleration.z);
		if(amag>0.f)
			if(dif[0]*m_acceleration.x/amag+dif[2]*m_acceleration.z/amag<0.3f)
			{
				dBodyAddForce(m_body,m_acceleration.x/amag*1000.f,0,m_acceleration.z/amag*1000.f);
			}
			if(proj<0.f){

				dReal vmag=chVel[0]*chVel[0]+chVel[2]*chVel[2];
				dBodyAddForce(m_body,chVel[0]/vmag/amag*proj*3000.f,0,chVel[2]/vmag/amag*proj*3000.f);
			}
	}
	//else
	//dBodyAddForce(m_body,-chVel[0]*10.f,-20.f*70.f*(!is_contact),-chVel[2]*10.f);

	//	if(b_depart&&b_clamb_jump&&(chVel[1]>0.f)){
	//chVel[1]=0.f;
	//	}


}



const float CHWON_ACCLEL_SHIFT=0.4f;
const float CHWON_AABB_FACTOR =1.f;
const float CHWON_ANG_COS	  =M_SQRT1_2;
const float CHWON_CALL_UP_SHIFT=0.05f;
const float CHWON_CALL_FB_HIGHT=1.5f;
const float CHWON_AABB_FB_FACTOR =1.f;


bool CPHSimpleCharacter::ValidateWalkOn()
{
	Fvector AABB,AABB_forbid,center,center_forbid,accel_add,accel;
	dVector3 norm,side0,side1;
	AABB.x=m_radius;
	AABB.y=m_radius;
	AABB.z=m_radius;

	AABB_forbid.set(AABB);
	AABB_forbid.x*=0.7f;
	AABB_forbid.z*=0.7f;
	AABB_forbid.y+=m_radius;
	AABB_forbid.mul(CHWON_AABB_FB_FACTOR);

	accel_add.set(m_acceleration);
	float mag=accel_add.magnitude();
	if(!(mag>0.f)) return 
		true;
	accel_add.mul(CHWON_ACCLEL_SHIFT/mag);
	accel.set(accel_add);
	accel.div(CHWON_ACCLEL_SHIFT);
	AABB.mul(CHWON_AABB_FACTOR);
	GetPosition(center);
	center.add(accel_add);
	center_forbid.set(center);
	center_forbid.y+=CHWON_CALL_FB_HIGHT;
	center.y+=m_radius+CHWON_CALL_UP_SHIFT;
#ifdef DRAW_BOXES
	m_bcenter.set(center);
	m_bcenter_forbid.set(center_forbid);
	m_AABB.set(AABB);
	m_AABB_forbid.set(AABB_forbid);

#endif
	CDB::RESULT*    R_begin;
	CDB::RESULT*    R_end  ;
	CDB::TRI*       T_array ;
	XRC.box_options                (0);
	XRC.box_query                  (Level().ObjectSpace.GetStaticModel(),center_forbid,AABB_forbid);
	R_begin                         = XRC.r_begin();
	R_end                           = XRC.r_end();
	T_array                         = Level().ObjectSpace.GetStaticTris();
	b_near_leader=false;
	for (CDB::RESULT* Res=R_begin; Res!=R_end; ++Res)
	{
		//CDB::TRI* T = T_array + Res->id;
		Point vertices[3]={Point((dReal*)&Res->verts[0]),Point((dReal*)&Res->verts[1]),Point((dReal*)&Res->verts[2])};
		if(__aabb_tri(Point((float*)&center_forbid),Point((float*)&AABB_forbid),vertices)){
			side0[0]=Res->verts[1].x-Res->verts[0].x;
			side0[1]=Res->verts[1].y-Res->verts[0].y;
			side0[2]=Res->verts[1].z-Res->verts[0].z;
			
			side1[0]=Res->verts[2].x-Res->verts[1].x;
			side1[1]=Res->verts[2].y-Res->verts[1].y;
			side1[2]=Res->verts[2].z-Res->verts[1].z;

			CDB::TRI* T = T_array + Res->id;
			SGameMtl* material=GMLib.GetMaterialByIdx(T->material);

			b_near_leader=b_near_leader||(!!material->Flags.is(SGameMtl::flClimable));
			dCROSS(norm,=,side0,side1);//optimize it !!!
			dNormalize3(norm);
			//if(b_leader)dVectorSet((dReal*)&leader_norm,norm);
			if(dDOT(norm,(float*)&accel)<-CHWON_ANG_COS) 
				return 
				false;
		}
	}


	XRC.box_options                (0);
	XRC.box_query                  (Level().ObjectSpace.GetStaticModel(),center,AABB);
	R_begin                         = XRC.r_begin();
	R_end                           = XRC.r_end();
	T_array                         = Level().ObjectSpace.GetStaticTris();
	for (CDB::RESULT* Res=R_begin; Res!=R_end; ++Res)
	{
		//CDB::TRI* T = T_array + Res->id;
		Point vertices[3]={Point((dReal*)&Res->verts[0]),Point((dReal*)&Res->verts[1]),Point((dReal*)&Res->verts[2])};
		if(__aabb_tri(Point((float*)&center),Point((float*)&AABB),vertices)){
			side0[0]=Res->verts[1].x-Res->verts[0].x;
			side0[1]=Res->verts[1].y-Res->verts[0].y;
			side0[2]=Res->verts[1].z-Res->verts[0].z;

			side1[0]=Res->verts[2].x-Res->verts[1].x;
			side1[1]=Res->verts[2].y-Res->verts[1].y;
			side1[2]=Res->verts[2].z-Res->verts[1].z;

			dCROSS(norm,=,side0,side1);//optimize it !!!
			dNormalize3(norm);
			if(norm[1]>CHWON_ANG_COS) return 
				true;
		}
	}
	return 
		false;
}
void CPHSimpleCharacter::SetAcceleration(Fvector accel){
	if(!b_exist) return;

	if(!dBodyIsEnabled(m_body))
		if(!fsimilar(0.f,accel.magnitude()))
			Enable();
	m_acceleration=accel;
}

void CPHSimpleCharacter::ApplyAcceleration() 
{
	//	if (Level().iGetKeyState(DIK_RSHIFT))
	//	{
	//	__asm int 3
	//	int i;
	//	i=5;
	//	}



	m_control_force[0]=0.f;
	m_control_force[1]=0.f;
	m_control_force[2]=0.f;
	

	//if(m_max_velocity<EPS) return;
	dMass m;
	dBodyGetMass(m_body,&m);

	//if(b_jump) 
	//	m_control_force[1]=60.f*m.mass*2.f;

	if(b_lose_control) 
	{
		m_control_force[0]=m_acceleration.x*m.mass*m_air_control_factor;
		m_control_force[1]=m_acceleration.y*m.mass*m_air_control_factor;
		m_control_force[2]=m_acceleration.x*m.mass*m_air_control_factor;
		return;
	}


	dVector3 accel={m_acceleration.x,0.f,m_acceleration.z};
	dVector3 fvdir,sidedir;
	////////////////////////////////////////////////
	//deside which force direction use  
	dVector3 y={0.f,1.f,0.f};
	dCROSS(sidedir,=,y,accel);

	if(b_clamb_jump&&b_valide_wall_contact&&!b_at_wall){
		dCROSS(fvdir,=,sidedir,m_wall_contact_normal);
		dNormalize3(fvdir);
		m_control_force[0]+=fvdir[0]*m.mass*20.f;
		m_control_force[1]+=fvdir[1]*m.mass*20.f;
		m_control_force[2]+=fvdir[2]*m.mass*20.f;
	}
	else{/*
		 if(b_valide_ground_contact&&(m_ground_contact_normal[1]>M_SQRT1_2)){//M_SQRT1_2//0.8660f
		 dCROSS(fvdir,=,sidedir,m_ground_contact_normal);
		 dNormalize3(fvdir);
		 m_control_force[0]+=fvdir[0]*m.mass*20.f;
		 m_control_force[1]+=fvdir[1]*m.mass*20.f;
		 m_control_force[2]+=fvdir[2]*m.mass*20.f;

		 }

		 else
		 */
		if(b_at_wall&&b_valide_wall_contact){

			dReal press_to_ladder=world_gravity*m_mass*0.05f;
			dReal proj=dDOT(accel,m_wall_contact_normal);
			fvdir[0]=accel[0]-m_wall_contact_normal[0]*(proj+press_to_ladder);//proj+press_to_ladder
			//fvdir[1]=-proj;
			fvdir[1]=m_wall_contact_normal[1]<0.1f ? -proj : -m_wall_contact_normal[1]*(proj+press_to_ladder);//accel[1]-m_wall_contact_normal[1]*proj,//+press_to_ladder
			fvdir[2]=accel[2]-m_wall_contact_normal[2]*(proj+press_to_ladder);//+press_to_ladder
			dNormalize3(fvdir);
			m_control_force[0]+=fvdir[0]*m.mass*30.f;
			m_control_force[1]+=fvdir[1]*m.mass*30.f;
			m_control_force[2]+=fvdir[2]*m.mass*30.f;

		}
		else{
			//fvdir[0]=0.f;fvdir[1]=0.f;fvdir[2]=0.f;//
			dVectorSet(fvdir,accel);
			dNormalize3(fvdir);
			m_control_force[0]+=fvdir[0]*m.mass*30.f;
			m_control_force[1]+=fvdir[1]*m.mass*30.f;
			m_control_force[2]+=fvdir[2]*m.mass*30.f;
		}


	}







	if(!b_at_wall&&b_clamb_jump){//&&m_wall_contact_normal[1]<M_SQRT1_2
		m_control_force[0]*=4.f;
		m_control_force[1]*=4.f;//*8.f
		m_control_force[2]*=4.f;
		m_control_force[1]=dFabs(m_control_force[1]);
		m_control_force[0]=m_control_force[0]*accel[0]>0.f ? m_control_force[0] : -m_control_force[0];
		m_control_force[2]=m_control_force[2]*accel[2]>0.f ? m_control_force[2] : -m_control_force[2];
	}

	//M->m_Flags.is(GameMtl::flClimable);
	if(!b_at_wall&&b_climb)
	{
		if(!b_block_climb_getup)
		{
			if(!b_climb_getup)
			{
				b_climb_getup=true;
				m_start_climb_getup_height=dBodyGetPosition(m_body)[1];
			}

				if(dFabs(dBodyGetPosition(m_body)[1]-m_start_climb_getup_height)<CLIMB_GETUP_HEIGHT)
				{
					//m_control_force[0]*=4.f;
					if(b_side_contact)m_control_force[1]+=m.mass*40.f;
					else 
					{
						if(b_valide_wall_contact)
						{
							dReal k=m.mass*20.f;
							//dReal k2=m.mass*10.f;
							m_control_force[0]+=m_wall_contact_normal[0]*k;
							m_control_force[1]+=m_wall_contact_normal[1]*k;
							m_control_force[2]+=m_wall_contact_normal[2]*k;
						}
					}
					//m_control_force[2]*=4.f;
				}
				else
				{
					b_climb_getup=false;
					b_block_climb_getup=true;
				}
			
		}
		else
		{
				if((dBodyGetPosition(m_body)[1]-m_start_climb_getup_height)<CLIMB_GETUP_HEIGHT)
				{
					b_block_climb_getup=false;
				}
		}
	}
	else 
	{
		b_block_climb_getup=false;
	}

	m_control_force[0]*=m_friction_factor;
	m_control_force[1]*=m_friction_factor;
	m_control_force[2]*=m_friction_factor;

}


void	CPHSimpleCharacter::IPosition(Fvector& pos) {

	if(!b_exist){
		pos.set(m_safe_position[0],
			m_safe_position[1],
			m_safe_position[2]);
		return;
	}

	m_body_interpolation.InterpolatePosition(pos);
	pos.y-=m_radius;
	return;
}

void CPHSimpleCharacter::SetPosition(Fvector pos){
	if(!b_exist) return;
	m_death_position[0]=pos.x;
	m_death_position[1]=pos.y+m_radius;
	m_death_position[2]=pos.z;
	m_safe_position[0]=pos.x;
	m_safe_position[1]=pos.y+m_radius;
	m_safe_position[2]=pos.z;
	b_death_pos=true;

	dGeomGetUserData(m_wheel)->pushing_b_neg=false;
	dGeomGetUserData(m_hat)->pushing_b_neg=false;
	dGeomGetUserData(m_geom_shell)->pushing_b_neg=false;
	dGeomGetUserData(m_hat)->pushing_b_neg=false;
	dGeomGetUserData(m_wheel)->pushing_neg=false;
	dGeomGetUserData(m_hat)->pushing_neg=false;
	dGeomGetUserData(m_geom_shell)->pushing_neg=false;
	dGeomGetUserData(m_hat)->pushing_neg=false;

	dBodySetPosition(m_body,pos.x,pos.y+m_radius,pos.z);
	m_body_interpolation.ResetPositions();
	CPHObject::spatial_move();
}



void CPHSimpleCharacter::GetPosition(Fvector& vpos){
	if(!b_exist){
		vpos.set(m_safe_position[0],m_safe_position[1]-m_radius,m_safe_position[2]);
	}
	const dReal* pos=dBodyGetPosition(m_body);


	dVectorSet((dReal*)&vpos,pos);
	vpos.y-=m_radius;
}

void CPHSimpleCharacter::GetVelocity(Fvector& vvel){
	if(!b_exist){

		vvel.set(m_safe_velocity[0],m_safe_velocity[1],m_safe_velocity[2]);
		return ;
	}
	const dReal* vel=dBodyGetLinearVel(m_body);
	dVectorSet((dReal*)&vvel,vel);
	return;
}


void CPHSimpleCharacter::SetVelocity(Fvector vel){
	if(!b_exist) return;
	dBodySetLinearVel(m_body,vel.x,vel.y,vel.z);
}


void CPHSimpleCharacter::SetMas(dReal mass){
	m_mass=mass;
	if(!b_exist) return;
	dMass m;
	dMassSetBox(&m,1,1000000.f,1000000.f,1000000.f);
	dMassAdjust(&m,mass);
	dBodySetMass(m_body,&m);
}
#ifdef DEBUG
void CPHSimpleCharacter::OnRender(){
	if(!b_exist) return;
	Fmatrix m;
	m.identity();
	Fvector n=*(Fvector*)m_ground_contact_normal;
	n.mul(100.f);
	Fvector pos;
	GetPosition(pos);
	pos.y+=m_radius;
	RCache.dbg_DrawLINE(m,pos,*(Fvector*)m_control_force, color_rgba(256,0,0,1));
	RCache.dbg_DrawLINE(m,pos,n, 0xefffffff);


	Fvector scale;
	scale.set(0.35f,0.35f,0.35f);
	Fmatrix M;
	M.identity();
	M.scale(scale);
	M.c.set(pos);


	RCache.dbg_DrawEllipse(M, 0xffffffff);

#ifdef DRAW_BOXES
	RCache.dbg_DrawAABB			(m_bcenter,m_AABB.x,m_AABB.y,m_AABB.z,D3DCOLOR_XRGB(0,0,255));
	RCache.dbg_DrawAABB			(m_bcenter_forbid,m_AABB_forbid.x,m_AABB_forbid.y,m_AABB_forbid.z,D3DCOLOR_XRGB(255,0,0));
#endif
	///M.c.set(0.f,1.f,0.f);
	//RCache.dbg_DrawEllipse(M, 0xffffffff);
}
#endif


EEnvironment	 CPHSimpleCharacter::CheckInvironment(){
	//if(b_on_ground||(is_control&&!b_lose_control))
	if(b_lose_control)
		return	peInAir;	
	//else									 return peAtWall;
	else if(b_at_wall) 
		return peAtWall;

	return peOnGround;	 								

}



void CPHSimpleCharacter::SetPhysicsRefObject					(CPhysicsShellHolder* ref_object)
{
	m_phys_ref_object=ref_object;
	if(b_exist)
	{
		dGeomUserDataSetPhysicsRefObject(m_geom_shell,ref_object);
		dGeomUserDataSetPhysicsRefObject(m_wheel,ref_object);
		dGeomUserDataSetPhysicsRefObject(m_hat,ref_object);
	}
}



void CPHSimpleCharacter::CaptureObject(dBodyID body,const dReal* anchor)
{
	m_capture_joint=dJointCreateBall(0,0);
	Island().AddJoint(m_capture_joint);
	dJointAttach(m_capture_joint,m_body,body);
	dJointSetBallAnchor(m_capture_joint,anchor[0],anchor[1],anchor[2]);
	dJointSetFeedback(m_capture_joint,&m_capture_joint_feedback);
}

void CPHSimpleCharacter::CapturedSetPosition(const dReal* position)
{
	if(!m_capture_joint) return;
	dJointSetBallAnchor(m_capture_joint,position[0],position[1],position[2]);
}

void CPHSimpleCharacter::CheckCaptureJoint()
{
	//m_capture_joint_feedback
}

void CPHSimpleCharacter::doCaptureExist(bool& do_exist)
{
	do_exist=!!m_capture_joint;
}

void CPHSimpleCharacter::SafeAndLimitVelocity()
{

	const float		*linear_velocity		=dBodyGetLinearVel(m_body);
	//limit velocity
	dReal l_limit;
	if(is_control&&!b_lose_control) 
		l_limit = m_max_velocity/phTimefactor;
	else			
		l_limit=10.f/fixed_step;

	dReal mag;

	if(dV_valid(linear_velocity))
	{	
		mag=_sqrt(linear_velocity[0]*linear_velocity[0]+linear_velocity[1]*linear_velocity[1]+linear_velocity[2]*linear_velocity[2]);//
		if(mag>l_limit)
		{
			dReal f=mag/l_limit;
			if(b_lose_ground&&linear_velocity[1]<0.f)
				dBodySetLinearVel(m_body,linear_velocity[0]/f,linear_velocity[1],linear_velocity[2]/f);///f
			else			 
				dBodySetLinearVel(m_body,linear_velocity[0]/f,linear_velocity[1]/f,linear_velocity[2]/f);///f
			if(is_control&&!b_lose_control)
				dBodySetPosition(m_body,
				m_safe_position[0]+linear_velocity[0]*fixed_step,
				m_safe_position[1]+linear_velocity[1]*fixed_step,
				m_safe_position[2]+linear_velocity[2]*fixed_step);
		}
	}
	else
	{
		dBodySetLinearVel(m_body,m_safe_velocity[0],m_safe_velocity[1],m_safe_velocity[2]);
	}

	if(!dV_valid(dBodyGetPosition(m_body)))
		dBodySetPosition(m_body,m_safe_position[0]-m_safe_velocity[0]*fixed_step,
		m_safe_position[1]-m_safe_velocity[1]*fixed_step,
		m_safe_position[2]-m_safe_velocity[2]*fixed_step);


	dVectorSet(m_safe_position,dBodyGetPosition(m_body));
	dVectorSet(m_safe_velocity,linear_velocity);
}

void CPHSimpleCharacter::SetObjectContactCallback(ObjectContactCallbackFun* callback)
{
	m_object_contact_callback=callback;
	if(!b_exist) return;

	dGeomUserDataSetObjectContactCallback(m_hat,callback);
	dGeomUserDataSetObjectContactCallback(m_geom_shell,callback);
	dGeomUserDataSetObjectContactCallback(m_wheel,callback);
}

u16 CPHSimpleCharacter::RetriveContactBone()

{
	Fvector dir;
	dir.set(m_damege_contact.geom.normal[0]*m_dmc_signum,m_damege_contact.geom.normal[1]*m_dmc_signum,m_damege_contact.geom.normal[2]*m_dmc_signum);
	Collide::ray_defs Q(*(((Fvector*)(m_damege_contact.geom.pos))), dir, m_radius, CDB::OPT_ONLYNEAREST,Collide::rqtBoth);  // CDB::OPT_ONLYFIRST CDB::OPT_ONLYNEAREST
	u16 contact_bone=0;
	CObject* object=smart_cast<CObject*>(m_phys_ref_object);
	if (g_pGameLevel->ObjectSpace.RayQuery(object->collidable.model,Q)){

		Collide::rq_result* R = g_pGameLevel->ObjectSpace.r_results.r_begin();
		contact_bone=(u16)R->element;
		//int y=result.r_count();
		//for (int k=0; k<y; ++k)
		//{
		//	ICollisionForm::RayQuery::Result* R = result.r_begin()+k;
		//	if(is_Door(R->element,i)) 
		//	{
		//		i->second.Use();
		//		return false;

		//	}
		//}

	}
	else 
	{


		CKinematics* K=smart_cast<CKinematics*>(object->Visual());
		u16 count=K->LL_BoneCount();
		CBoneInstance* bone_instances=&K->LL_GetBoneInstance(0);
		Fvector pos_in_object;
		pos_in_object.sub(*(Fvector*)m_damege_contact.geom.pos,object->Position());//vector from object center to contact position currently in global frame
		Fmatrix object_form;
		object_form.set(object->XFORM());
		object_form.transpose();
		object_form.transform_dir(pos_in_object); //project pos_in_object on object axes now it is position of contact in object frame
		float sq_dist=dInfinity;
		for(u16 i=0;i<count;++i)
		{
			Fvector c_to_bone;
			c_to_bone.sub(bone_instances[i].mTransform.c,pos_in_object);
			float temp_sq_dist=c_to_bone.square_magnitude();
			if(temp_sq_dist<sq_dist)
			{
				sq_dist=temp_sq_dist;
				contact_bone=i;
			}
		}
		VERIFY(dInfinity!=sq_dist);
	}
	return contact_bone;
}

///////////////////////////////////////////////////////////////////
const dReal def_spring_rate=0.5f;
const dReal def_dumping_rate=20.1f;
/////////////////////////////////////////////////////////////////
void CPHSimpleCharacter::InitContact(dContact* c,bool	&do_collide,SGameMtl * material_1,SGameMtl * material_2){

	bool bo1=(c->geom.g1==m_wheel)||c->geom.g1==m_cap_transform||c->geom.g1==m_shell_transform||c->geom.g1==m_hat_transform;

	//SGameMtl* tri_material=GMLib.GetMaterialByIdx((u16)c->surface.mode);
	SGameMtl* tri_material=0;
	if(bo1)
		tri_material=material_2;
	else
		tri_material=material_1;

	bool bClimable=!!tri_material->Flags.is(SGameMtl::flClimable);
	if(is_control&&b_at_wall)
	{
		c->surface.mu=0.f;
		c->surface.soft_cfm=world_cfm*2.f;
		c->surface.soft_erp=world_erp;
	}
	b_climb=b_climb || bClimable;
	b_pure_climb=b_pure_climb && (bClimable||c->geom.g1==m_cap_transform||c->geom.g2==m_cap_transform);
	if(tri_material->Flags.is(SGameMtl::flPassable))return;

	dReal spring_rate=def_spring_rate;
	dReal dumping_rate=def_dumping_rate;
	bool object=(dGeomGetBody(c->geom.g1)&&dGeomGetBody(c->geom.g2));
	b_on_object=b_on_object||object;
	*p_lastMaterial=((dxGeomUserData*)dGeomGetData(m_wheel))->tri_material;

	if((c->geom.g1==m_cap_transform||c->geom.g2==m_cap_transform||c->geom.g1==m_shell_transform||c->geom.g2==m_shell_transform))
	{
		
		//Fvector2 f={m_control_force[0],m_control_force[2]},cn={c->geom.normal[0],c->geom.normal[2]};
		//f.normalize(),cn.normalize();
		//float product=f.x*cn.x+f.y*cn.y;
		//if(is_control&& (bo1	?	product<-M_SQRT1_2	:	product>M_SQRT1_2))
		b_side_contact=true;

		//c->surface.soft_cfm*=spring_rate;//0.01f;
		//c->surface.soft_erp*=dumping_rate;//10.f;
		MulSprDmp(c->surface.soft_cfm,c->surface.soft_erp,spring_rate,dumping_rate);
		c->surface.mu		=0.00f;
	}
	
	if(bo1)
	{
		if(c->geom.g1==m_wheel)
		{
			if(c->geom.normal[1]<0.f)
			{
				do_collide=false;
				return;
			}
		}
	}
	else
	{
		if(c->geom.g2==m_wheel)
		{
			if(c->geom.normal[1]>0.f)
			{
				do_collide=false;
				return;
			}
		}
	}

	if(!do_collide) return;
	
	if(object){
		spring_rate*=10.f;
		const dReal* vel=dBodyGetLinearVel(m_body);
		dReal c_vel;
		dBodyID b;

		SGameMtl* obj_material=NULL;
		if(bo1)
		{
			b=dGeomGetBody(c->geom.g2);
			//obj_material=GMLib.GetMaterialByIdx(retrieveGeomUserData(c->geom.g2)->material);
			obj_material=material_2;
		}
		else
		{
			b=dGeomGetBody(c->geom.g1);
			//obj_material=GMLib.GetMaterialByIdx(retrieveGeomUserData(c->geom.g1)->material);
			obj_material=material_1;
		}


		dMass m;
		dBodyGetMass(b,&m);
		const dReal* obj_vel=dBodyGetLinearVel(b);
		dVector3 obj_vel_effective={obj_vel[0]*object_demage_factor,obj_vel[1]*object_demage_factor,obj_vel[2]*object_demage_factor};
		dVector3 obj_impuls={obj_vel_effective[0]*m.mass,obj_vel_effective[1]*m.mass,obj_vel_effective[2]*m.mass};
		dVector3 impuls={vel[0]*m_mass,vel[1]*m_mass,vel[2]*m_mass};
		dVector3 c_mas_impuls={obj_impuls[0]+impuls[0],obj_impuls[1]+impuls[1],obj_impuls[2]+impuls[2]};
		dReal cmass=m_mass+m.mass;
		dVector3 c_mass_vel={c_mas_impuls[0]/cmass,c_mas_impuls[1]/cmass,c_mas_impuls[2]/cmass};
		//dVector3 rel_impuls={obj_impuls[0]-impuls[0],obj_impuls[1]-impuls[1],obj_impuls[2]-impuls[2]};
		//c_vel=dFabs(dDOT(obj_vel,c->geom.normal)*_sqrt(m.mass/m_mass));
		dReal vel_prg=dDOT(vel,c->geom.normal);
		dReal obj_vel_prg=dDOT(obj_vel_effective,c->geom.normal);
		dReal c_mass_vel_prg=dDOT(c_mass_vel,c->geom.normal);

		//dReal kin_energy_start=dDOT(vel,vel)*m_mass/2.f+dDOT(obj_vel,obj_vel)*m.mass/2.f*object_demage_factor;
		//dReal kin_energy_end=dDOT(c_mass_vel,c_mass_vel)*cmass/2.f;

		dReal kin_energy_start=vel_prg*vel_prg*m_mass/2.f+obj_vel_prg*obj_vel_prg*m.mass/2.f;
		dReal kin_energy_end=c_mass_vel_prg*c_mass_vel_prg*cmass/2.f;

		dReal accepted_energy=(kin_energy_start-kin_energy_end);
		if(accepted_energy>0.f)
				c_vel=dSqrt(accepted_energy/m_mass*2.f)*obj_material->fBounceDamageFactor;
		else c_vel=0.f;
		//	}
		//	else
		//	{
		//		dBodyID b=dGeomGetBody(c->geom.g1);
		//		dMass m;
		//		dBodyGetMass(b,&m);
		//		const dReal* obj_vel=dBodyGetLinearVel(b);
		//	dVector3 rel_vel={obj_vel[0]-vel[0],obj_vel[1]-vel[1],obj_vel[2]-vel[2]};
		//		c_vel=dDOT(obj_vel,c->geom.normal)/m_mass*m.mass;
		//	}
		if(c_vel>m_contact_velocity) 
		{
			m_contact_velocity=c_vel;
			m_dmc_signum=bo1 ? 1.f : -1.f;
			m_dmc_type=ctStatic;
			m_damege_contact=*c;
		}
	}





	//.	if(material->Flags.is(SGameMtl::flWalkOn))
	//.		b_clamb_jump=true;


	//bool bo1=(c->geom.g1==m_wheel_transform);
	//if(!(bo1 || (c->geom.g2==m_wheel_transform))) 
	//	return;
	b_any_contacts=true;
	is_contact=true;

	//if(b_at_wall&&!bClimable) return;
	if(!((c->geom.g1==m_wheel) || (c->geom.g2==m_wheel)||(b_at_wall)  ))//
		return;



	///m_friction_factor=(c->surface.mu<1.f&&!object) ? c->surface.mu : 1.f;
	if(c->surface.mu<1.f&&!object)m_friction_factor=c->surface.mu; else m_friction_factor=1.f;


	++m_contact_count;

	//if( !b_saved_contact_velocity)
	//		{//b_lose_control &&
	//			b_saved_contact_velocity=true;
	//			const dReal* v=dBodyGetLinearVel(m_body);
	//			m_contact_velocity=dFabs(dDOT(v,c->geom.normal));
	//		}

	//if(b_lose_control) 
	{
		const dReal* v=dBodyGetLinearVel(m_body);
		dReal mag=dFabs(dDOT(v,c->geom.normal))*tri_material->fBounceDamageFactor;
		if(mag>m_contact_velocity)
		{
			m_contact_velocity=mag;
			m_dmc_signum=bo1 ? 1.f : -1.f;
			m_dmc_type=ctStatic;
			m_damege_contact=*c;
		}
	}




	if(bo1){
		////////////////////////////
		//if(c->geom.normal[1]<0.1f){
		//	c->geom.normal[1]=-c->geom.normal[1];
		//	c->geom.depth=0.f;
		//}


		///////////////////////////////////
		if(c->geom.g1==m_wheel){
			dVectorSet(m_death_position,dGeomGetPosition(c->geom.g1));
			m_death_position[1]+=c->geom.depth;
			if(dGeomGetUserData(c->geom.g1)->pushing_neg)
				m_death_position[1]=dGeomGetUserData(c->geom.g1)->neg_tri.pos;

			if(dGeomGetUserData(c->geom.g1)->pushing_b_neg)
				m_death_position[1]=dGeomGetUserData(c->geom.g1)->b_neg_tri.pos;
			b_death_pos=true;
		}
		/////////////////////////////////////////////////////////////
		if(c->geom.normal[1]>m_ground_contact_normal[1]||!b_valide_ground_contact)//
		{
			m_ground_contact_normal[0]=c->geom.normal[0];
			m_ground_contact_normal[1]=c->geom.normal[1];
			m_ground_contact_normal[2]=c->geom.normal[2];
			dVectorSet(m_ground_contact_position,c->geom.pos);
			b_valide_ground_contact=true;
		}

		if((c->geom.normal[0]*m_acceleration.x+
			c->geom.normal[2]*m_acceleration.z)<
			(m_wall_contact_normal[0]*m_acceleration.x+
			m_wall_contact_normal[2]*m_acceleration.z)
			||!b_valide_wall_contact)//
		{
			m_wall_contact_normal[0]=c->geom.normal[0];
			m_wall_contact_normal[1]=c->geom.normal[1];
			m_wall_contact_normal[2]=c->geom.normal[2];
			dVectorSet(m_wall_contact_position,c->geom.pos);
			b_valide_wall_contact=true;
		}

	}
	else{
		if(c->geom.g2==m_wheel){
			dVectorSet(m_death_position,dGeomGetPosition(c->geom.g2));
			m_death_position[1]+=c->geom.depth;
			if(dGeomGetUserData(c->geom.g2)->pushing_neg)
				m_death_position[1]=dGeomGetUserData(c->geom.g2)->neg_tri.pos;

			if(dGeomGetUserData(c->geom.g2)->pushing_b_neg)
				m_death_position[1]=dGeomGetUserData(c->geom.g2)->b_neg_tri.pos;
			b_death_pos=true;
		}
		if(c->geom.normal[1]<-m_ground_contact_normal[1]||!b_valide_ground_contact)//
		{
			m_ground_contact_normal[0]=-c->geom.normal[0];
			m_ground_contact_normal[1]=-c->geom.normal[1];
			m_ground_contact_normal[2]=-c->geom.normal[2];
			dVectorSet(m_ground_contact_position,c->geom.pos);
			b_valide_ground_contact=true;
		}
		if(c->geom.normal[0]*m_acceleration.x+
			c->geom.normal[2]*m_acceleration.z>
			-m_wall_contact_normal[0]*m_acceleration.x-
			m_wall_contact_normal[2]*m_acceleration.z
			||!b_valide_wall_contact)//
		{
			m_wall_contact_normal[0]=-c->geom.normal[0];
			m_wall_contact_normal[1]=-c->geom.normal[1];
			m_wall_contact_normal[2]=-c->geom.normal[2];
			dVectorSet(m_wall_contact_position,c->geom.pos);
			b_valide_wall_contact=true;
		}
	}



	if(is_control&&!b_lose_control||b_jumping){
		if(c->geom.g1==m_wheel||c->geom.g2==m_wheel)
		{
			c->surface.mu = 0.f;//0.00f;
			c->surface.mode|=dContactFDir1|dContactMotion1;
			dReal mag=m_acceleration.magnitude();
			c->surface.motion1=mag/10.f;
			c->fdir1[0]=m_acceleration[0]/mag;
			c->fdir1[1]=m_acceleration[1]/mag;
			c->fdir1[2]=m_acceleration[2]/mag;
		}
		else{
				c->surface.mu = 0.00f;
			}
		//dNormalize3(c->fdir1);
		//c->surface.soft_cfm*=spring_rate;//0.01f;
		//c->surface.soft_erp*=dumping_rate;//10.f;
		MulSprDmp(c->surface.soft_cfm,c->surface.soft_erp,spring_rate,dumping_rate);
		
	}
	else
	{
		//c->surface.soft_cfm*=spring_rate;//0.01f;
		//c->surface.soft_erp*=dumping_rate;//10.f;
		MulSprDmp(c->surface.soft_cfm,c->surface.soft_erp,spring_rate,dumping_rate);
		c->surface.mu *= (1.f+b_clamb_jump*3.f+b_climb*20.f);
	}


}

const Fvector& CPHSimpleCharacter::GroundNormal()
{
	if(b_at_wall)
	{
		return *((Fvector*)m_wall_contact_normal);

	}
	else
	{
		return *((Fvector*)m_ground_contact_normal);
	}
}
u16 CPHSimpleCharacter::ContactBone()
{
	return RetriveContactBone();
}
void CPHSimpleCharacter::SetMaterial	(u16 material)
{
	if(!b_exist) return;
	dGeomGetUserData(m_geom_shell)->material=material;
	dGeomGetUserData(m_wheel)->material		=material;
	dGeomGetUserData(m_cap)->material		=material;
	dGeomGetUserData(m_hat)->material		=material;
}
void CPHSimpleCharacter::get_State(SPHNetState& state)
{
	CPHCharacter::get_State(state);
	
	state.previous_position.y-=m_radius;
}
void CPHSimpleCharacter::set_State(const SPHNetState& state)
{
	CPHCharacter::set_State (state);
}

void CPHSimpleCharacter::get_spatial_params()
{
	spatialParsFromDGeom((dGeomID)m_space,spatial.center,AABB,spatial.radius);
}

float CPHSimpleCharacter::FootRadius()
{
	if(b_exist) return m_radius;
	else		return 0.f;

}
void CPHSimpleCharacter::DeathPosition(Fvector& deathPos)
{ 
	if(!b_exist) return;
	deathPos.set(m_death_position);
	deathPos.y=m_death_position[1]-m_radius;
}
void	CPHSimpleCharacter::	AddControlVel						(const Fvector& vel)
{
	m_acceleration.add(vel);
	 m_max_velocity+=vel.magnitude();
}

