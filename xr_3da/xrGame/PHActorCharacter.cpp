#include "stdafx.h"
#include "phactorcharacter.h"
#include "Extendedgeom.h"



//const float JUMP_HIGHT=0.5;
const float JUMP_UP_VELOCITY=6.0f;//5.6f;
const float JUMP_INCREASE_VELOCITY_RATE=1.2f;

CPHActorCharacter::CPHActorCharacter(void)
{
	m_cap=NULL;
	m_cap_transform=NULL;
}

CPHActorCharacter::~CPHActorCharacter(void)
{
}

void CPHActorCharacter::Create(dVector3 sizes)
{
	if(b_exist) return;
	inherited::Create(sizes);
	m_cap=dCreateSphere(0,m_radius+m_radius/30.f);
	dGeomSetPosition(m_cap,0.f,m_radius,0.f);
	m_cap_transform=dCreateGeomTransform(0);
	dGeomTransformSetInfo(m_cap_transform,1);
	dGeomTransformSetGeom(m_cap_transform,m_cap);
	dGeomCreateUserData(m_cap);

	if(m_phys_ref_object)
	{

		dGeomUserDataSetPhysicsRefObject(m_cap,m_phys_ref_object);

	}
	dGeomUserDataSetPhObject(m_cap,(CPHObject*)this);
	dGeomGroupAdd(m_geom_group,m_cap_transform);
	dGeomSetBody(m_cap_transform,m_body);
	if(m_object_contact_callback)dGeomUserDataSetObjectContactCallback(m_cap,m_object_contact_callback);

}

void CPHActorCharacter::Destroy()
{
	if(!b_exist) return;
	if(m_cap) {
		dGeomDestroyUserData(m_cap);
		dGeomDestroy(m_cap);
		m_cap=NULL;
	}

	if(m_cap_transform){
		dGeomDestroyUserData(m_cap_transform);
		dGeomDestroy(m_cap_transform);
		m_cap_transform=NULL;
	}
	inherited::Destroy();
}

void CPHActorCharacter::SetPhysicsRefObject(CPhysicsRefObject* ref_object)
{
	inherited::SetPhysicsRefObject(ref_object);

	if(b_exist)
	{

		dGeomUserDataSetPhysicsRefObject(m_cap,ref_object);

	}
}



///////////////////////////////////////////////////////////////////
const dReal spring_rate=0.5f;
const dReal dumping_rate=20.1f;
/////////////////////////////////////////////////////////////////
void CPHActorCharacter::InitContact(dContact* c){
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
		const dReal* vel=dBodyGetLinearVel(m_body);
		dReal c_vel;
		dBodyID b;

		if(bo1)
			b=dGeomGetBody(c->geom.g2);
		else
			b=dGeomGetBody(c->geom.g1);

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
			c_vel=dSqrt(accepted_energy/m_mass*2.f);
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

void CPHActorCharacter::SetAcceleration(Fvector accel)
{

	inherited::SetAcceleration(accel);
	if(!b_exist) return;

	if( m_acceleration.y>0.f&&!b_lose_control && (m_ground_contact_normal[1]>0.5f||b_at_wall))
	{
		b_jump=true;
		const dReal* vel=dBodyGetLinearVel(m_body);
		dReal amag =m_acceleration.magnitude();
		if(amag<1.f)amag=1.f;
		m_jump_accel.set(vel[0]*JUMP_INCREASE_VELOCITY_RATE+m_acceleration.x/amag*0.2f,jump_up_velocity,vel[2]*JUMP_INCREASE_VELOCITY_RATE +m_acceleration.z/amag*0.2f);
	}

}

void CPHActorCharacter::SetObjectContactCallback(ObjectContactCallbackFun* callback)
{
	inherited::SetObjectContactCallback(callback);
	if(!b_exist) return;
	dGeomUserDataSetObjectContactCallback(m_cap,callback);
}