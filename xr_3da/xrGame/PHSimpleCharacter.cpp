

#include "stdafx.h"
#include "PHDynamicData.h"
#include "Physics.h"
#include "ExtendedGeom.h"
#include "..\cl_intersect.h"
#include "tri-colliderKNoOPC\__aabb_tri.h"
#include "PHSimpleCharacter.h"

const float LOSE_CONTROL_DISTANCE=0.5f; //fly distance to lose control
const float CLAMB_DISTANCE=0.5f;
//const float JUMP_HIGHT=0.5;
const float JUMP_UP_VELOCITY=6.0f;//5.6f;
const float JUMP_INCREASE_VELOCITY_RATE=1.2f;
static u32 lastMaterial;
float object_demage_factor=4.f;
////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////class//CPHSimpleCharacter////////////////////
CPHSimpleCharacter::CPHSimpleCharacter()
{

	m_geom_shell=NULL;
	m_wheel=NULL;

	m_geom_group=NULL;
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
	m_update_time=0.f;
	b_meet_control=false;
	b_jumping=false;
	m_contact_velocity=0.f;
	jump_up_velocity=6.f;

	previous_p[0]=dInfinity;
	dis_count_f=0;
	
	m_capture_joint=NULL;


}





void CPHSimpleCharacter::Create(dVector3 sizes){

	if(b_exist) return;

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
	CPHObject::Activate();
	b_exist=true;
	const dReal k=1.20f;
	dReal doun=m_radius*_sqrt(1.f-1.f/k/k)/2.f;
	m_geom_shell=dCreateCylinder(0,m_radius/k,m_cyl_hight+doun);

	m_wheel=dCreateSphere(0,m_radius);
	m_hat=dCreateSphere(0,m_radius/k);




	m_shell_transform=dCreateGeomTransform(0);
	m_hat_transform=dCreateGeomTransform(0);
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
	m_body=dBodyCreate(phWorld);

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

	if(m_phys_ref_object)
	{
		dGeomUserDataSetPhysicsRefObject(m_geom_shell,m_phys_ref_object);

		dGeomUserDataSetPhysicsRefObject(m_wheel,m_phys_ref_object);
		dGeomUserDataSetPhysicsRefObject(m_hat,m_phys_ref_object);
	}
	//dGeomUserDataSetPhObject(m_wheel_transform,(CPHObject*)this);
	dGeomUserDataSetPhObject(m_wheel,(CPHObject*)this);
	//dGeomUserDataSetPhObject(m_shell_transform,(CPHObject*)this);
	dGeomUserDataSetPhObject(m_geom_shell,(CPHObject*)this);
	//dGeomUserDataSetPhObject(m_cap_transform,(CPHObject*)this);

	dGeomUserDataSetPhObject(m_hat,(CPHObject*)this);
	//dGeomGetUserData(m_cap_transform)->friction=0.f;
	//dGeomGetUserData(m_shell_transform)->friction=0.f;
	/////////////////////////////////////////////////////////////////////////
	dMass m;
	dMassSetBox(&m,1,1000000.f,1000000.f,1000000.f);
	dMassAdjust(&m,m_mass);
	dBodySetMass(m_body,&m);

	m_geom_group=dCreateGeomGroup(ph_world->GetSpace());
	//dGeomGroupAdd(m_geom_group,m_wheel_transform);
	dGeomGroupAdd(m_geom_group,m_wheel);

	dGeomGroupAdd(m_geom_group,m_shell_transform);
	dGeomGroupAdd(m_geom_group,m_hat_transform);
	//dGeomGroupAdd(chRGeomGroup,chRCylinder);
	m_body_interpolation.SetBody(m_body);
}


void CPHSimpleCharacter::Destroy(){
	if(!b_exist) return;
	b_exist=false;
	//if(ph_world)
	CPHObject::Deactivate();



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
	if(m_geom_group){
		dGeomDestroy(m_geom_group);
		m_geom_group=NULL;
	}

	if(m_body) {
		dBodyDestroy(m_body);
		m_body=NULL;
	}


}

void		CPHSimpleCharacter::ApplyImpulse(const Fvector& dir,const dReal P){
	if(!b_exist) return;
	if(!dBodyIsEnabled(m_body)) dBodyEnable(m_body);
	b_lose_control=true;
	b_external_impulse=true;
	dBodyAddForce(m_body,dir.x*P/fixed_step,dir.y*P/fixed_step,dir.z*P/fixed_step);
}

void CPHSimpleCharacter::PhDataUpdate(dReal step){
	///////////////////
	if( !dBodyIsEnabled(m_body)) {
		if(previous_p[0]!=dInfinity) previous_p[0]=dInfinity;
		return;
	}
	if(is_contact&&!is_control)
		Disabling();

	///////////////////////
	b_external_impulse=false;
	was_contact=is_contact;
	was_control=is_control;
	is_contact=false;
	b_side_contact=false;
	b_any_contacts=false;
	b_valide_ground_contact=false;
	b_valide_wall_contact=false;
	if(!dBodyIsEnabled(m_body)) return;
	b_climb=false;
	b_pure_climb=true;
	b_was_on_object=b_on_object;
	b_on_object=false;

	m_contact_count=0;
	//limit velocity
	dReal l_limit;
	if(is_control&&!b_lose_control) 
		l_limit = m_max_velocity;
	else			
		l_limit=10.f/fixed_step;

	dReal mag;
	const dReal* vel = dBodyGetLinearVel(m_body);
	if(!dV_valid(vel))
		dBodySetLinearVel(m_body,0.f,0.f,0.f);
	mag=_sqrt(vel[0]*vel[0]+vel[1]*vel[1]+vel[2]*vel[2]);//
	if(mag>l_limit){
		dReal f=mag/l_limit;
		dBodySetLinearVel(m_body,vel[0]/f,vel[1]/f,vel[2]/f);///f
	}





	const dReal* pos=dBodyGetPosition(m_body);
	if(!dV_valid(pos))
		dBodySetPosition(m_body,m_safe_position[0]-m_safe_velocity[0]*fixed_step,
		m_safe_position[1]-m_safe_velocity[1]*fixed_step,
		m_safe_position[2]-m_safe_velocity[2]*fixed_step);

	Memory.mem_copy(m_safe_position,dBodyGetPosition(m_body),sizeof(dVector3));
	Memory.mem_copy(m_safe_velocity,dBodyGetPosition(m_body),sizeof(dVector3));



	dMatrix3 R;
	dRSetIdentity (R);
	dBodySetAngularVel(m_body,0.f,0.f,0.f);
	dBodySetRotation(m_body,R);

	m_body_interpolation.UpdatePositions();
}

void CPHSimpleCharacter::PhTune(dReal step){
	if(!dBodyIsEnabled(m_body)) return;

	if(m_acceleration.magnitude()>0.f) is_control=true;
	else							   is_control=false;

	b_pure_climb=b_pure_climb&&b_any_contacts&&(!b_lose_control);
	if(b_pure_climb) b_at_wall=true;

	if(b_lose_control || (!b_climb&&is_contact)||
		(!b_pure_climb&&b_valide_ground_contact&&m_ground_contact_normal[1]>M_SQRT1_2)
		) b_at_wall=false;

	b_depart=was_contact&&(!is_contact);
	b_stop_control=was_control&&(!is_control);
	b_meet=(!was_contact)&&(is_contact);
	if(b_lose_control&&is_contact)b_meet_control=true;
	b_on_ground=b_valide_ground_contact||(b_meet&&(!b_depart));


	if(b_at_wall ) {

		// b_clamb_jump=true;
		b_side_contact=false;
		m_friction_factor=1.f;
		m_max_velocity=0.4f;
		if(b_stop_control) 
			dBodySetLinearVel(m_body,0.f,0.f,0.f);
	}

	//save depart position
	if(b_depart) 
		Memory.mem_copy(m_depart_position,dBodyGetPosition(m_body),sizeof(dVector3));

	const dReal* velocity=dBodyGetLinearVel(m_body);
	if(b_lose_control&&b_on_ground&&m_ground_contact_normal[1]>M_SQRT1_2/2.f&& !b_external_impulse && dSqrt(velocity[0]*velocity[0]+velocity[2]*velocity[2])<5.) 
		b_lose_control=false;

	if(b_jumping&&b_valide_ground_contact&&m_ground_contact_normal[1]>M_SQRT1_2 ||(b_at_wall&&b_valide_wall_contact)) 
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
				Memory.mem_copy(m_clamb_depart_position,dBodyGetPosition(m_body),sizeof(dVector3));
	}

	else b_clamb_jump=ValidateWalkOn();

	//jump	
	if(b_jump){
		b_lose_control=true;
		const dReal* vel=dBodyGetLinearVel(m_body);
		dReal amag =m_acceleration.magnitude();
		if(amag<1.f)amag=1.f;
		//dBodySetLinearVel(m_body,vel[0]*JUMP_INCREASE_VELOCITY_RATE+m_acceleration.x/amag*0.2f,vel[1]+JUMP_UP_VELOCITY,vel[2]*JUMP_INCREASE_VELOCITY_RATE +m_acceleration.z/amag*0.2f);
		dBodySetLinearVel(m_body,vel[0]*JUMP_INCREASE_VELOCITY_RATE+m_acceleration.x/amag*0.2f,jump_up_velocity,vel[2]*JUMP_INCREASE_VELOCITY_RATE +m_acceleration.z/amag*0.2f);//vel[1]+
		Memory.mem_copy(m_jump_depart_position,dBodyGetPosition(m_body),sizeof(dVector3));
		m_jump_accel=m_acceleration;
		b_jump=false;
		b_jumping=true;

	}





	ApplyAcceleration();

	dReal* chVel=const_cast<dReal*>(dBodyGetLinearVel(m_body));
	//if(b_jump)
	//	dBodyAddForce(m_body,0.f,m_control_force[1],0.f);//+2.f*9.8f*70.f
	//else
	dMass m;
	dBodyGetMass(m_body,&m);
	if((!b_lose_control||b_clamb_jump)&&is_control){
		dVector3 sidedir;
		dVector3 y={0.,1.,0.};
		dCROSS(sidedir,=,m_control_force,y);
		dNormalize3(sidedir);
		dReal vProj=dDOT(sidedir,chVel);

		dBodyAddForce(m_body,m_control_force[0],m_control_force[1],m_control_force[2]);//+2.f*9.8f*70.f
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
		dBodyAddForce(m_body,0.f,m.mass*9.8f*2.f,0.f);

	if(b_jumping)
	{

		dReal proj=m_jump_accel.x*chVel[0]+m_jump_accel.z*chVel[2];

		const dReal* current_pos=dBodyGetPosition(m_body);
		dVector3 dif={current_pos[0]-m_jump_depart_position[0],
			current_pos[1]-m_jump_depart_position[1],
			current_pos[2]-m_jump_depart_position[2]};
		dReal amag =_sqrt(m_jump_accel.x*m_jump_accel.x+m_jump_accel.z*m_jump_accel.z);
		if(amag>0.f)
			if(dif[0]*m_jump_accel.x/amag+dif[2]*m_jump_accel.z/amag<0.3f)
			{
				dBodyAddForce(m_body,m_jump_accel.x/amag*1000.f,0,m_jump_accel.z/amag*1000.f);
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



const float CHWON_ACCLEL_SHIFT=0.1f;
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
	AABB_forbid.y+=m_radius;
	AABB_forbid.mul(CHWON_AABB_FB_FACTOR);

	accel_add.set(m_acceleration);
	float mag=accel_add.magnitude();
	if(!(mag>0.f)) return 
		false;
	accel_add.mul(CHWON_ACCLEL_SHIFT/mag);
	accel.set(accel_add);
	accel.div(CHWON_ACCLEL_SHIFT);
	AABB.mul(CHWON_AABB_FACTOR);
	GetPosition(center);
	center.add(accel_add);
	center_forbid.set(center);
	center_forbid.y+=CHWON_CALL_FB_HIGHT;
	center.y+=m_radius+CHWON_CALL_UP_SHIFT;
	CDB::RESULT*    R_begin;
	CDB::RESULT*    R_end  ;
	CDB::TRI*       T_array ;
	XRC.box_options                (0);
	XRC.box_query                  (Level().ObjectSpace.GetStaticModel(),center_forbid,AABB_forbid);
	R_begin                         = XRC.r_begin();
	R_end                           = XRC.r_end();
	T_array                         = Level().ObjectSpace.GetStaticTris();
	for (CDB::RESULT* Res=R_begin; Res!=R_end; Res++)
	{
		CDB::TRI* T = T_array + Res->id;
		Point vertices[3]={Point((dReal*)T->verts[0]),Point((dReal*)T->verts[1]),Point((dReal*)T->verts[2])};
		if(__aabb_tri(Point((float*)&center_forbid),Point((float*)&AABB_forbid),vertices)){
			side0[0]=T->verts[1]->x-T->verts[0]->x;
			side0[1]=T->verts[1]->y-T->verts[0]->y;
			side0[2]=T->verts[1]->z-T->verts[0]->z;

			side1[0]=T->verts[2]->x-T->verts[1]->x;
			side1[1]=T->verts[2]->y-T->verts[1]->y;
			side1[2]=T->verts[2]->z-T->verts[1]->z;

			dCROSS(norm,=,side0,side1);//optimize it !!!
			dNormalize3(norm);
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
	for (CDB::RESULT* Res=R_begin; Res!=R_end; Res++)
	{
		CDB::TRI* T = T_array + Res->id;
		Point vertices[3]={Point((dReal*)T->verts[0]),Point((dReal*)T->verts[1]),Point((dReal*)T->verts[2])};
		if(__aabb_tri(Point((float*)&center),Point((float*)&AABB),vertices)){
			side0[0]=T->verts[1]->x-T->verts[0]->x;
			side0[1]=T->verts[1]->y-T->verts[0]->y;
			side0[2]=T->verts[1]->z-T->verts[0]->z;

			side1[0]=T->verts[2]->x-T->verts[1]->x;
			side1[1]=T->verts[2]->y-T->verts[1]->y;
			side1[2]=T->verts[2]->z-T->verts[1]->z;

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
		if(accel.magnitude()!=0.f)
			dBodyEnable(m_body);
	m_acceleration=accel;
	if( m_acceleration.y>0.f&&!b_lose_control && (m_ground_contact_normal[1]>0.5f||b_at_wall)) 
		b_jump=true;

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

	if(m_max_velocity<EPS) return;
	dMass m;
	dBodyGetMass(m_body,&m);

	//if(b_jump) 
	//	m_control_force[1]=60.f*m.mass*2.f;

	if(b_lose_control) return;


	dVector3 accel={m_acceleration.x,0.f,m_acceleration.z};
	dVector3 fvdir,sidedir;
	////////////////////////////////////////////////
	//deside which force direction use  
	dVector3 y={0.f,1.f,0.f};
	dCROSS(sidedir,=,y,accel);

	if(b_clamb_jump&&b_valide_wall_contact){
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


			dReal proj=dDOT(accel,m_wall_contact_normal);
			fvdir[0]=accel[0]-m_wall_contact_normal[0]*proj;
			//fvdir[1]=-proj;
			fvdir[1]=m_wall_contact_normal[1]<0.1f ? -proj : -m_wall_contact_normal[1]*proj;//accel[1]-m_wall_contact_normal[1]*proj,
			fvdir[2]=accel[2]-m_wall_contact_normal[2]*proj;
			dNormalize3(fvdir);
			m_control_force[0]+=fvdir[0]*m.mass*30.f;
			m_control_force[1]+=fvdir[1]*m.mass*30.f;
			m_control_force[2]+=fvdir[2]*m.mass*30.f;

		}
		else{
			//fvdir[0]=0.f;fvdir[1]=0.f;fvdir[2]=0.f;//
			Memory.mem_copy(fvdir,accel,sizeof(dVector3));
			dNormalize3(fvdir);
			m_control_force[0]+=fvdir[0]*m.mass*30.f;
			m_control_force[1]+=fvdir[1]*m.mass*30.f;
			m_control_force[2]+=fvdir[2]*m.mass*30.f;
		}


	}







	if(b_clamb_jump){//&&m_wall_contact_normal[1]<M_SQRT1_2
		m_control_force[0]*=4.f;
		m_control_force[1]*=4.f;//*8.f
		m_control_force[2]*=4.f;
		m_control_force[1]=dFabs(m_control_force[1]);
		m_control_force[0]=m_control_force[0]*accel[0]>0.f ? m_control_force[0] : -m_control_force[0];
		m_control_force[2]=m_control_force[2]*accel[2]>0.f ? m_control_force[2] : -m_control_force[2];
	}

	//M->m_Flags.is(GameMtl::flClimbable);
	if(!b_at_wall&&b_climb){
		//m_control_force[0]*=4.f;
		m_control_force[1]+=m.mass*40.f;
		//m_control_force[2]*=4.f;
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

	dBodySetPosition(m_body,pos.x,pos.y+m_radius,pos.z);
	m_body_interpolation.ResetPositions();
}



void CPHSimpleCharacter::GetPosition(Fvector& vpos){
	if(!b_exist){
		vpos.set(m_safe_position[0],m_safe_position[1]-m_radius,m_safe_position[2]);
	}
	const dReal* pos=dBodyGetPosition(m_body);


	Memory.mem_copy(&vpos,pos,sizeof(Fvector));
	vpos.y-=m_radius;
}

void CPHSimpleCharacter::GetVelocity(Fvector& vvel){
	if(!b_exist){

		vvel.set(m_safe_velocity[0],m_safe_velocity[1],m_safe_velocity[2]);
		return ;
	}
	const dReal* vel=dBodyGetLinearVel(m_body);
	Memory.mem_copy(&vvel,vel,sizeof(Fvector));
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
	RCache.dbg_DrawLINE(m,pos,*(Fvector*)m_control_force, 0xffffffff);
	RCache.dbg_DrawLINE(m,pos,n, 0xefffffff);


	Fvector scale;
	scale.set(0.35f,0.35f,0.35f);
	Fmatrix M;
	M.identity();
	M.scale(scale);
	M.c.set(pos);


	RCache.dbg_DrawEllipse(M, 0xffffffff);

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



void CPHSimpleCharacter::SetPhysicsRefObject					(CPhysicsRefObject* ref_object)
{
	m_phys_ref_object=ref_object;
	if(b_exist)
	{
		dGeomUserDataSetPhysicsRefObject(m_geom_shell,ref_object);
		dGeomUserDataSetPhysicsRefObject(m_wheel,ref_object);
		dGeomUserDataSetPhysicsRefObject(m_hat,ref_object);
	}
}


void	CPHSimpleCharacter::Disabling(){
	if(b_lose_control) return;
	/////////////////////////////////////////////////////////////////////////////////////
	////////disabling main body//////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////
	{
		const dReal* torqu=dBodyGetTorque(m_body);
		const dReal* force=dBodyGetForce(m_body);
		dReal t_m =_sqrt(	torqu[0]*torqu[0]+
			torqu[1]*torqu[1]+
			torqu[2]*torqu[2]);
		dReal f_m=_sqrt(	force[0]*force[0]+
			force[1]*force[1]+
			force[2]*force[2]);
		if(t_m+f_m>0.000000){
			previous_p[0]=dInfinity;
			previous_v=0;
			dis_count_f=1;
			dis_count_f1=0;
			return;
		}
	}
	if(previous_p[0]==dInfinity&&ph_world->disable_count==0){
		const dReal* position=dBodyGetPosition(m_body);
		Memory.mem_copy(previous_p,position,sizeof(dVector3));
		Memory.mem_copy(previous_p1,position,sizeof(dVector3));	
		previous_v=0;
		dis_count_f=1;
		dis_count_f1=0;
	}


	if(ph_world->disable_count==dis_frames){	
		if(previous_p[0]!=dInfinity){
			const dReal* current_p=dBodyGetPosition(m_body);
			dVector3 velocity={current_p[0]-previous_p[0],
				current_p[1]-previous_p[1],
				current_p[2]-previous_p[2]};
			dReal mag_v=_sqrt(
				velocity[0]*velocity[0]+
				velocity[1]*velocity[1]+
				velocity[2]*velocity[2]);
			mag_v/=dis_count_f;
			if(mag_v<0.001f* dis_frames )
				dBodyDisable(m_body);
			if(previous_v>mag_v)

			{
				dis_count_f++;
				previous_v=mag_v;
				//return;
			}
			else{
				previous_v=0;
				dis_count_f=1;
				Memory.mem_copy(previous_p,current_p,sizeof(dVector3));
			}

			{

				dVector3 velocity={current_p[0]-previous_p1[0],
					current_p[1]-previous_p1[1],
					current_p[2]-previous_p1[2]};
				dReal mag_v=_sqrt(
					velocity[0]*velocity[0]+
					velocity[1]*velocity[1]+
					velocity[2]*velocity[2]);
				mag_v/=dis_count_f;

				if(mag_v<0.04* dis_frames )
					dis_count_f1++;
				else{
					Memory.mem_copy(previous_p1,current_p,sizeof(dVector3));
				}

				if(dis_count_f1>10) dis_count_f*=10;

			}


		}

	}
	/////////////////////////////////////////////////////////////////

}


void CPHSimpleCharacter::CaptureObject(dBodyID body,const dReal* anchor)
{
m_capture_joint=dJointCreateBall(phWorld,0);
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
