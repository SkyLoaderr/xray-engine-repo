#include "stdafx.h"
#include "telewhirlwind.h"
#include "PhysicsShell.h"
#include "PhysicsShellHolder.h"
#include "../level.h"

		CTeleWhirlwindObject::		CTeleWhirlwindObject()
{
			m_telekinesis=0;
			
}
	

bool		CTeleWhirlwindObject::		init(CTelekinesis* tele,CPhysicsShellHolder *obj, float s, float h, u32 ttk)
{
			bool result			=inherited::init(tele,obj,s,h,ttk);
			m_telekinesis		=static_cast<CTeleWhirlwind*>(tele);
			if(obj->PPhysicsShell())
			{
				obj->PPhysicsShell()->SetAirResistance(0.f,0.f);
				//obj->m_pPhysicsShell->set_ApplyByGravity(TRUE);
			}
			return result;
}
void		CTeleWhirlwindObject::		raise_update			()
{
	
	//u32 time=Level().timeServer();
//	if (time_raise_started + 100000 < time) release();
	
}

void		CTeleWhirlwindObject::		release					()
{
	if (!object || !object->m_pPhysicsShell || !object->m_pPhysicsShell->bActive) return;
	
		
	Fvector dir_inv;
	dir_inv.set(0.f,-1.0f,0.f);

	// включить гравитацию
	Fvector zer;zer.set(0,0,0);
	object->m_pPhysicsShell->set_LinearVel(zer);
	object->m_pPhysicsShell->set_ApplyByGravity(TRUE);

	switch_state(TS_None);
}
void		CTeleWhirlwindObject::		raise					(float power)
{

		CPhysicsShell*	p					=	get_object()	->PPhysicsShell();
		u16				element_number		=	p				->get_ElementsNumber();
		Fvector			center				=	m_telekinesis	->Center();

		for(u16 element=0;element<element_number;++element)
		{
			float k=600.f;
			float predict_v_eps=0.1f;
			float mag_eps	   =2.0f;

			CPhysicsElement* E=	p->get_ElementByStoreOrder(element);
			if (!E->bActive) continue;
			Fvector pos=E->mass_Center();

			Fvector diff;
			diff.sub(center,pos);
			float mag=diff.magnitude();
			
			float accel=k/mag/mag/mag;//*E->getMass()
			if(mag<mag_eps)
			{
				accel=k/mag_eps/mag_eps/mag_eps;
				Fvector zer;zer.set(0,0,0);
				E->set_LinearVel(zer);
			}
	
			Fvector vel;
			E->get_LinearVel(vel);
			float delta_v=accel*fixed_step;
			Fvector dir;dir.set(diff);dir.mul(1.f/mag);
			Fvector delta_vel; delta_vel.set(dir);delta_vel.mul(delta_v);
			Fvector predict_vel;predict_vel.add(vel,delta_vel);
			Fvector delta_pos;delta_pos.set(predict_vel);delta_pos.mul(fixed_step);
			Fvector predict_pos;predict_pos.add(pos,delta_pos);
			
			Fvector predict_diff;predict_diff.sub(center,predict_pos);
			float predict_mag=predict_diff.magnitude();
			float predict_v=predict_vel.magnitude();

			Fvector force;force.set(dir);
			if(predict_mag>mag && predict_vel.dotproduct(dir)>0.f && predict_v>predict_v_eps)
			{
	
				Fvector motion_dir;motion_dir.set(predict_vel);motion_dir.mul(1.f/predict_v);
				float needed_d=diff.dotproduct(motion_dir);
				Fvector needed_diff;needed_diff.set(motion_dir);needed_diff.mul(needed_d);
				Fvector nearest_p;nearest_p.add(pos,needed_diff);//
				Fvector needed_vel;needed_vel.set(needed_diff);needed_vel.mul(1.f/fixed_step);
				force.sub(needed_vel,vel);
				force.mul(E->getMass()/fixed_step);
			}
			else
			{
				force.mul(accel*E->getMass());
			}


			//float cur_vel_to_center=vel.dotproduct(dir);
			//
			//Fvector v_to_c; v_to_c.set(dir);v_to_c.mul(cur_vel_to_center);
			//Fvector tangent_vel; tangent_vel.sub(vel,v_to_c);
			//float  t_vel=tangent_vel.magnitude();

			//float predict_vel_to_center=cur_vel_to_center+delta_v;
			//float max_vel_to_center=mag/fixed_step;
			//Fvector force;force.set(dir);
			//if(abs(predict_vel_to_center)>abs(max_vel_to_center))
			//{
			//	
			//	float	max_delta_v=cur_vel_to_center-max_vel_to_center;
			//	force.mul(E->getMass()*max_delta_v/fixed_step);////1.f/mag*E->getMass()*max_vel_to_center*fixed_step
			//}
			//else
			//{
			//	force.mul(accel*E->getMass());
			//}
	

		
			E->applyForce(force.x,force.y,force.z);
		}
}
void		CTeleWhirlwindObject::		keep					()
{
	//inherited::keep();
}
void		CTeleWhirlwindObject::		fire					(const Fvector &target)
{
	//inherited::fire(target);
}
void		CTeleWhirlwindObject::		fire					(const Fvector &target, float power)
{
	//inherited:: fire(target,power);
}

void		CTeleWhirlwindObject::switch_state(ETelekineticState new_state)
{
	inherited::switch_state(new_state);
}