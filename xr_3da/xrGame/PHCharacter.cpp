#include "stdafx.h"
#include "phcharacter.h"
#include "PHDynamicData.h"
#include "Physics.h"
#include "ExtendedGeom.h"
#include "..\cl_intersect.h"
#include "tri-colliderKNoOPC\__aabb_tri.h"

const float LOSE_CONTROL_DISTANCE=0.5f; //fly distance to lose control
const float CLAMB_DISTANCE=0.5f;
//const float JUMP_HIGHT=0.5;
const float JUMP_UP_VELOCITY=6.0f;//5.6f;
const float JUMP_INCREASE_VELOCITY_RATE=1.2f;
static u32 lastMaterial;
void dBodyAngAccelFromTorqu(const dBodyID body, dReal* ang_accel, const dReal* torque){
      dMass m;
      dMatrix3 invI;
      dBodyGetMass(body,&m);
      dInvertPDMatrix (m.I, invI, 3);
      dMULTIPLY1_333(ang_accel,invI, torque);
      }


CPHCharacter::CPHCharacter(void)
{
p_lastMaterial=&lastMaterial;
b_on_object=false;
b_climb=false;
b_pure_climb=true;
m_friction_factor=1.f;
m_body=NULL;
m_wheel_body=NULL;
m_geom_shell=NULL;
m_wheel=NULL;
m_cap=NULL;
m_geom_group=NULL;
m_wheel_transform=NULL;
m_shell_transform=NULL;
m_cap_transform=NULL;
m_wheel_joint=NULL;
m_hat=NULL;
m_hat_transform=NULL;
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

}

CPHCharacter::~CPHCharacter(void)
{
}

void		CPHCharacter::ApplyImpulse(const Fvector& dir,const dReal P){
if(!b_exist) return;
if(!dBodyIsEnabled(m_body)) dBodyEnable(m_body);
dBodyAddForce(m_body,dir.x*P/fixed_step,dir.y*P/fixed_step,dir.z*P/fixed_step);
}
////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////class//CPHSimpleCharacter////////////////////

void CPHSimpleCharacter::Create(dVector3 sizes){

if(b_exist) return;

////////////////////////////////////////////////////////
m_control_force[0]=0.f;
m_control_force[1]=0.f;
m_control_force[2]=0.f;

m_depart_position[0]=0.f;
m_depart_position[1]=0.f;
m_depart_position[2]=0.f;

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
m_radius=_min(sizes[0],sizes[2])/2.f;
m_cyl_hight=sizes[1]-2.f*m_radius;
if (m_cyl_hight<0.f) m_cyl_hight=0.01f;

m_ident=ph_world->AddObject(this);
b_exist=true;
const dReal k=1.20f;
dReal doun=m_radius*_sqrt(1.f-1.f/k/k)/2.f;
m_geom_shell=dCreateCylinder(0,m_radius/k,m_cyl_hight+doun);
m_cap=dCreateSphere(0,m_radius+m_radius/30.f);
m_wheel=dCreateSphere(0,m_radius);
m_hat=dCreateSphere(0,m_radius/k);
m_cap_transform=dCreateGeomTransform(0);
m_shell_transform=dCreateGeomTransform(0);
m_hat_transform=dCreateGeomTransform(0);
//m_wheel_transform=dCreateGeomTransform(0);
dGeomTransformSetInfo(m_cap_transform,1);
dGeomTransformSetInfo(m_shell_transform,1);
dGeomTransformSetInfo(m_hat_transform,1);
//dGeomTransformSetInfo(m_wheel_transform,1);
////////////////////////////////////////////////////////////////////////

dGeomSetPosition(m_cap,0.f,m_radius,0.f);
dGeomSetPosition(m_hat,0.f,m_cyl_hight,0.f);
////////////////////////////////////////////////////////////////////////

//dGeomSetPosition(chSphera2,0.f,-0.7f,0.f);

////////////////////////////////////////////////////////////////////////
dGeomSetPosition(m_geom_shell,0.f,m_cyl_hight/2.f-doun,0.f);
dGeomTransformSetGeom(m_cap_transform,m_cap);
dGeomTransformSetGeom(m_hat_transform,m_hat);
//dGeomTransformSetGeom(m_wheel_transform,m_wheel);
dGeomTransformSetGeom(m_shell_transform,m_geom_shell);
m_body=dBodyCreate(phWorld);
dGeomSetBody(m_cap_transform,m_body);
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
dGeomCreateUserData(m_cap);
dGeomCreateUserData(m_wheel);
dGeomCreateUserData(m_hat);
//dGeomUserDataSetPhObject(m_wheel_transform,(CPHObject*)this);
dGeomUserDataSetPhObject(m_wheel,(CPHObject*)this);
//dGeomUserDataSetPhObject(m_shell_transform,(CPHObject*)this);
dGeomUserDataSetPhObject(m_geom_shell,(CPHObject*)this);
//dGeomUserDataSetPhObject(m_cap_transform,(CPHObject*)this);
dGeomUserDataSetPhObject(m_cap,(CPHObject*)this);
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
dGeomGroupAdd(m_geom_group,m_cap_transform);
dGeomGroupAdd(m_geom_group,m_shell_transform);
dGeomGroupAdd(m_geom_group,m_hat_transform);
//dGeomGroupAdd(chRGeomGroup,chRCylinder);
m_body_interpolation.SetBody(m_body);
}


void CPHSimpleCharacter::Destroy(){
	if(!b_exist) return;
	b_exist=false;
	ph_world->RemoveObject(m_ident);
	if(m_wheel_joint){
		dJointDestroy(m_wheel_joint);
		m_wheel_joint=NULL;
	}


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

	if(m_cap) {
		dGeomDestroyUserData(m_cap);
		dGeomDestroy(m_cap);
		m_cap=NULL;
	}


	if(m_wheel_transform){
		dGeomDestroyUserData(m_wheel_transform);
		dGeomDestroy(m_wheel_transform);
			m_wheel_transform=NULL;
	}

	if(m_cap_transform){
		dGeomDestroyUserData(m_cap_transform);
		dGeomDestroy(m_cap_transform);
		m_cap_transform=NULL;
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

	if(m_wheel_body){
		dBodyDestroy(m_wheel_body);
		m_wheel_body=NULL;
	}
}



void CPHSimpleCharacter::PhDataUpdate(dReal step){
///////////////////
	if( !dBodyIsEnabled(m_body)) {
					if(previous_p[0]!=dInfinity) previous_p[0]=dInfinity;
					return;
				}
	if(is_contact&&!is_control)
							Disable();
///////////////////////

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
		mag=_sqrt(vel[0]*vel[0]+vel[1]*vel[1]+vel[2]*vel[2]);//
		if(mag>l_limit){
						dReal f=mag/l_limit;
						dBodySetLinearVel(m_body,vel[0]/f,vel[1]/f,vel[2]/f);///f
						}
		else if(!(mag>-dInfinity)||!(mag<dInfinity))
			dBodySetLinearVel(m_body,0.f,0.f,0.f);

		
		
		const dReal* pos=dBodyGetPosition(m_body);
		dReal smag=dDOT(pos,pos);
		if(!((smag>-dInfinity)&&(smag<dInfinity)))
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
	else					  is_control=false;

	b_pure_climb=b_pure_climb&&b_any_contacts;
	if(b_pure_climb) b_at_wall=true;
	if(b_lose_control || (!b_climb&&is_contact)||
		(!b_pure_climb&&b_valide_ground_contact&&m_ground_contact_normal[1]>M_SQRT1_2)
	) b_at_wall=false;
	b_depart=was_contact&&(!is_contact);
	b_stop_control=was_control&&(!is_control);
	b_meet=(!was_contact)&&(is_contact);
	if(b_lose_control&&is_contact)b_meet_control=true;
	b_on_ground=is_contact||(b_meet&&(!b_depart));


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

	if(is_contact) b_lose_control=false;
	if(b_valide_ground_contact&&m_ground_contact_normal[1]>M_SQRT1_2 ||(b_at_wall&&b_valide_wall_contact)) b_jumping=false;

//deside if control lost
	if(!b_on_ground){
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
	if(b_on_object){
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
				dBodySetLinearVel(m_body,vel[0]*JUMP_INCREASE_VELOCITY_RATE+m_acceleration.x/amag*0.2f,vel[1]+jump_up_velocity,vel[2]*JUMP_INCREASE_VELOCITY_RATE +m_acceleration.z/amag*0.2f);
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

		if(b_jumping){

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

///////////////////////////////////////////////////////////////////
const dReal spring_rate=0.5f;
const dReal dumping_rate=20.1f;
/////////////////////////////////////////////////////////////////
void CPHSimpleCharacter::InitContact(dContact* c){
	bool object=(dGeomGetBody(c->geom.g1)&&dGeomGetBody(c->geom.g2));
	b_on_object=b_on_object||object;
	*p_lastMaterial=((dxGeomUserData*)dGeomGetData(m_wheel))->tri_material;
	bool bo1=(c->geom.g1==m_wheel)||c->geom.g1==m_cap_transform||c->geom.g1==m_shell_transform||c->geom.g1==m_hat_transform;
	if(c->geom.g1==m_cap_transform||c->geom.g2==m_cap_transform||c->geom.g1==m_shell_transform||c->geom.g2==m_shell_transform){//
		dNormalize3(m_control_force);

		if(is_control&& (dDOT(m_control_force,c->geom.normal))<-M_SQRT1_2)
			b_side_contact=true;


	//c->surface.mode =dContactApprox1|dContactSoftCFM|dContactSoftERP;
	//c->surface.soft_cfm=0.0001f;
	//c->surface.soft_erp=0.2f;
	c->surface.soft_cfm*=spring_rate;//0.01f;
	c->surface.soft_erp*=dumping_rate;//10.f;
	//c->surface.mode =dContactApprox0;
	c->surface.mu = 0.00f;
		}
	

		if(object){
		//const dReal* vel=dBodyGetLinearVel(m_body);
		dReal c_vel;
	//	if(bo1)
		{
			dBodyID b=dGeomGetBody(c->geom.g2);
			dMass m;
			dBodyGetMass(b,&m);
			const dReal* obj_vel=dBodyGetLinearVel(b);
		//	dVector3 rel_vel={obj_vel[0]-vel[0],obj_vel[1]-vel[1],obj_vel[2]-vel[2]};
			c_vel=dFabs(dDOT(obj_vel,c->geom.normal)*dSqrt(m.mass/m_mass));
		}
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
								m_contact_velocity=c_vel;
		}

		
		bool bClimable=!!(((DWORD)c->surface.mode)& SGameMtl::flClimbable);
		b_climb=b_climb || bClimable;
		b_pure_climb=b_pure_climb && bClimable;

		if((c->surface.mode&SGameMtl::flWalkOn))
									b_clamb_jump=true;


	//bool bo1=(c->geom.g1==m_wheel_transform);
	//if(!(bo1 || (c->geom.g2==m_wheel_transform))) 
	//	return;
	b_any_contacts=true;
	is_contact=true;
	
	//if(b_at_wall&&!bClimable) return;
	if(!((c->geom.g1==m_wheel) || (c->geom.g2==m_wheel)||(b_at_wall)  ))//
		return;


	
	m_friction_factor=c->surface.mu<1.f ? c->surface.mu : 1.f;
		

	
	m_contact_count++;

//if( !b_saved_contact_velocity)
//		{//b_lose_control &&
//			b_saved_contact_velocity=true;
//			const dReal* v=dBodyGetLinearVel(m_body);
//			m_contact_velocity=dFabs(dDOT(v,c->geom.normal));
//		}

	//if(b_lose_control) 
	{
		const dReal* v=dBodyGetLinearVel(m_body);
		dReal mag=dFabs(dDOT(v,c->geom.normal));
		if(mag>m_contact_velocity)					//!b_saved_contact_velocity||
						m_contact_velocity=mag;
	//					b_saved_contact_velocity=false;
	}




	if(bo1){
////////////////////////////
		//if(c->geom.normal[1]<0.1f){
		//	c->geom.normal[1]=-c->geom.normal[1];
		//	c->geom.depth=0.f;
		//}


///////////////////////////////////
		if(c->geom.g1==m_wheel){
		Memory.mem_copy(m_death_position,dGeomGetPosition(c->geom.g1),sizeof(dVector3));
		m_death_position[1]+=c->geom.depth;
		if(dGeomGetUserData(c->geom.g1)->pushing_neg)
			m_death_position[1]=dGeomGetUserData(c->geom.g1)->neg_tri.pos;

		if(dGeomGetUserData(c->geom.g1)->pushing_b_neg)
			m_death_position[1]=dGeomGetUserData(c->geom.g1)->b_neg_tri.pos;
		}
/////////////////////////////////////////////////////////////
		if(c->geom.normal[1]>m_ground_contact_normal[1]||!b_valide_ground_contact)//
		{
		m_ground_contact_normal[0]=c->geom.normal[0];
		m_ground_contact_normal[1]=c->geom.normal[1];
		m_ground_contact_normal[2]=c->geom.normal[2];
		Memory.mem_copy(m_ground_contact_position,c->geom.pos,sizeof(dVector3));
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
		Memory.mem_copy(m_wall_contact_position,c->geom.pos,sizeof(dVector3));
		b_valide_wall_contact=true;
		}

	}
	else{
	
		if(c->geom.normal[1]<m_ground_contact_normal[1]||!b_valide_ground_contact)//
		{
		m_ground_contact_normal[0]=-c->geom.normal[0];
		m_ground_contact_normal[1]=-c->geom.normal[1];
		m_ground_contact_normal[2]=-c->geom.normal[2];
		Memory.mem_copy(m_ground_contact_position,c->geom.pos,sizeof(dVector3));
		b_valide_ground_contact=true;
		}
		if(c->geom.normal[0]*m_acceleration.x+
			c->geom.normal[2]*m_acceleration.z<
			m_wall_contact_normal[0]*m_acceleration.x+
			m_wall_contact_normal[2]*m_acceleration.z
			||!b_valide_wall_contact)//
		{
		m_wall_contact_normal[0]=-c->geom.normal[0];
		m_wall_contact_normal[1]=-c->geom.normal[1];
		m_wall_contact_normal[2]=-c->geom.normal[2];
		Memory.mem_copy(m_wall_contact_position,c->geom.pos,sizeof(dVector3));
		b_valide_wall_contact=true;
		}
	}



	if(is_control&&!b_lose_control||b_jumping){
					//c->surface.mode =dContactApprox1|dContactSoftCFM|dContactSoftERP;// dContactBounce;|dContactFDir1
					c->surface.mu = 0.00f;
					//c->surface.mu2 = dInfinity;
					//c->surface.soft_cfm=0.0001f;
					//c->surface.soft_erp=0.2f;
					c->surface.soft_cfm*=spring_rate;//0.01f;
					c->surface.soft_erp*=dumping_rate;//10.f;
					//Memory.mem_copy(c->fdir1,m_control_force,sizeof(dVector3));
					//dNormalize3(c->fdir1);
					
					}
		else
		{
		//c->surface.mode =dContactApprox1|dContactSoftCFM|dContactSoftERP;
		//c->surface.soft_cfm=0.0001f;
		//c->surface.soft_erp=0.2f;
		c->surface.soft_cfm*=spring_rate;//0.01f;
		c->surface.soft_erp*=dumping_rate;//10.f;
		c->surface.mu *= (1.f+b_clamb_jump*3.f+b_climb*20.f);

		}
	
	
}

void CPHSimpleCharacter::StepFrameUpdate(dReal step)
{
	
//m_body_interpolation.UpdatePositions();
m_position=GetPosition();
m_velocity=GetVelocity();
m_update_time=Device.fTimeGlobal;

}
const float CHWON_ACCLEL_SHIFT=0.1f;
const float CHWON_AABB_FACTOR =1.f;
const float CHWON_ANG_COS	  =M_SQRT1_2;
const float CHWON_CALL_UP_SHIFT=0.5f;
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
	if(!(mag>0.f)) return false;
	accel_add.mul(CHWON_ACCLEL_SHIFT/mag);
	accel.set(accel_add);
	accel.div(CHWON_ACCLEL_SHIFT);
	AABB.mul(CHWON_AABB_FACTOR);
	center=GetPosition();
	center.add(accel_add);
	center_forbid.set(center);
	center_forbid.y+=CHWON_CALL_FB_HIGHT;
	center.y+=m_radius*(1.f+CHWON_CALL_UP_SHIFT);
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
		return false;
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
	return false;
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


dMass m;
dBodyGetMass(m_body,&m);
m_control_force[0]=0.f;
m_control_force[1]=0.f;
m_control_force[2]=0.f;
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

void CPHSimpleCharacter::SetPosition(Fvector pos){
if(!b_exist) return;
	m_death_position[0]=pos.x;
	m_death_position[1]=pos.y+m_radius;
	m_death_position[2]=pos.z;
	m_safe_position[0]=pos.x;
	m_safe_position[1]=pos.y+m_radius;
	m_safe_position[2]=pos.z;
	dBodySetPosition(m_body,pos.x,pos.y+m_radius,pos.z);
}

bool CPHSimpleCharacter::TryPosition(Fvector pos){
if(!b_exist) return false;
if(b_on_object) return false;
SetPosition(pos);
m_body_interpolation.UpdatePositions();
m_body_interpolation.UpdatePositions();
dBodyDisable(m_body);
return true;
}

Fvector CPHSimpleCharacter::GetPosition(){
	if(!b_exist){
		 Fvector ret;
		ret.set(m_safe_position[0],m_safe_position[1]-m_radius,m_safe_position[2]);
		return ret;
	}
	const dReal* pos=dBodyGetPosition(m_body);
	Fvector vpos;
	
	Memory.mem_copy(&vpos,pos,sizeof(Fvector));
	vpos.y-=m_radius;
	return vpos;
}

Fvector CPHSimpleCharacter::GetVelocity(){
	if(!b_exist){
		 Fvector ret;
		ret.set(m_safe_velocity[0],m_safe_velocity[1],m_safe_velocity[2]);
		return ret;
	}
	const dReal* vel=dBodyGetLinearVel(m_body);
	Fvector vvel;
	Memory.mem_copy(&vvel,vel,sizeof(Fvector));
	return vvel;
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
	Fvector pos=GetPosition();
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

	M.c.set(0.f,1.f,0.f);
	RCache.dbg_DrawEllipse(M, 0xffffffff);
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





//////////////////////////////////////////////////////////////////////////
/////////////////////CPHWheeledCharacter//////////////////////////////////
//////////////////////////////////////////////////////////////////////////




void CPHWheeledCharacter::Create(dVector3 sizes){

//chRCylinder=dCreateBox(0,0.8f,2.f,0.8f);
m_radius=_min(sizes[0],sizes[2])/2.f;
m_cap=dCreateSphere(0,0.5f);
m_wheel=dCreateSphere(0,0.5f);
m_geom_shell=dCreateCylinder(0,0.4f,2.f);


//dGeomTransformSetInfo(chTransform1,1);
//dGeomTransformSetInfo(chTransform2,1);
dMatrix3 RZ;
dRFromAxisAndAngle (RZ,0.f, 0.f, 1.f, M_PI/1800.f);
//dGeomSetRotation(m_cap,RZ);
dGeomSetRotation(m_geom_shell,RZ);
dGeomSetPosition(m_cap,0.f,1.f,0.f);
m_cap_transform=dCreateGeomTransform(0);
m_shell_transform=dCreateGeomTransform(0);
dGeomTransformSetInfo(m_cap_transform,1);
dGeomTransformSetInfo(m_shell_transform,1);

//dGeomSetPosition(chSphera2,0.f,-0.7f,0.f);
//dGeomSetPosition(chRCylinder,0.f,0.f,0.f);
dGeomTransformSetGeom(m_cap_transform,m_cap);
dGeomTransformSetGeom(m_shell_transform,m_geom_shell);
m_body=dBodyCreate(phWorld);
m_wheel_body=dBodyCreate(phWorld);




dGeomSetBody(m_cap_transform,m_body);
dGeomSetBody(m_shell_transform,m_body);
//dGeomSetBody(chRCylinder,chRBody);
dGeomSetBody(m_wheel,m_wheel_body);
dBodySetPosition(m_body,-9,4,0);
dBodySetPosition(m_wheel_body,-9,2.5,0);
dMass m;

dMassSetBox(&m,1,1000000.f,1000000.f,1000000.f);
dMassAdjust(&m,70.f);
dBodySetMass(m_body,&m);

dMassSetSphere(&m,1,0.5);
dMassAdjust(&m,1.f);
dBodySetMass(m_wheel_body,&m);

m_wheel_joint=dJointCreateHinge2(phWorld,0);
dJointAttach(m_wheel_joint,m_body,m_wheel_body);

dJointSetHinge2Anchor (m_wheel_joint,-9 , 2.5, 0);
dJointSetHinge2Axis1 (m_wheel_joint, 0, 1, 0);
dJointSetHinge2Axis2 (m_wheel_joint, 0, 0, 1);


//dJointSetHinge2Param(chRJoint, dParamLoStop, 0);
//dJointSetHinge2Param(chRJoint, dParamHiStop, 0);
dJointSetHinge2Param(m_wheel_joint, dParamFMax, 5000);

dJointSetHinge2Param(m_wheel_joint, dParamVel2, 0);
dJointSetHinge2Param(m_wheel_joint, dParamFMax2, 1000);

		dReal k_p=4000.f;
		dReal k_d=500.f;
		dReal h=0.025f;
			


//	dWorldSetCFM(world,  1.f / (h*k_p + k_d));
//	dWorldSetERP(world,  h*k_p / (h*k_p + k_d));

dJointSetHinge2Param(m_wheel_joint, dParamSuspensionERP, h*k_p / (h*k_p + k_d));
dJointSetHinge2Param(m_wheel_joint, dParamSuspensionCFM, 1.f / (h*k_p + k_d));


m_geom_group=dCreateGeomGroup(ph_world->GetSpace());


dGeomGroupAdd(m_geom_group,m_wheel);
dGeomGroupAdd(m_geom_group,m_cap_transform);
dGeomGroupAdd(m_geom_group,m_shell_transform);
//dGeomGroupAdd(chRGeomGroup,chRCylinder);



}




void CPHWheeledCharacter::Destroy(){

	if(m_body) {
		dBodyDestroy(m_body);
		m_body=NULL;
	}

	if(m_wheel_body){
		dBodyDestroy(m_wheel_body);
		m_wheel_body=NULL;
	}

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


	if(m_cap) {
		dGeomDestroyUserData(m_cap);
		dGeomDestroy(m_cap);
		m_cap=NULL;
	}

	if(m_wheel_transform){
		dGeomDestroyUserData(m_wheel_transform);
		dGeomDestroy(m_wheel_transform);
			m_wheel_transform=NULL;
	}
	if(m_shell_transform){
		dGeomDestroyUserData(m_shell_transform);
		dGeomDestroy(m_shell_transform);
		m_shell_transform=NULL;
	}
	if(m_cap_transform){
		dGeomDestroyUserData(m_cap_transform);
		dGeomDestroy(m_cap_transform);
		m_cap_transform=NULL;
	}
	if(m_geom_group){
		dGeomDestroy(m_geom_group);
		m_geom_group=NULL;
	}


}

void CPHWheeledCharacter::SetMas(dReal mass){
dMass m;
dMassSetBox(&m,1,1000000.f,1000000.f,1000000.f);
dMassAdjust(&m,mass);
dBodySetMass(m_body,&m);
}
EEnvironment	 CPHWheeledCharacter::CheckInvironment(){

	return peOnGround;

}

void	CPHCharacter::Disable(){

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
