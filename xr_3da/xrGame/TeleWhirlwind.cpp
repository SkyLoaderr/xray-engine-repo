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
			return result;
}
void		CTeleWhirlwindObject::		raise_update			()
{
	if (time_raise_started + 100000 > Level().timeServer()) release();
	
}

void		CTeleWhirlwindObject::		release					()
{
	if (!object || !object->m_pPhysicsShell || !object->m_pPhysicsShell->bActive) return;

		
	Fvector dir_inv;
	dir_inv.set(0.f,-1.0f,0.f);

	// включить гравитацию
	object->m_pPhysicsShell->set_ApplyByGravity(TRUE);

	state = TS_None;
}
void		CTeleWhirlwindObject::		raise					(float power)
{

		CPhysicsShell*	p					=	get_object()	->PPhysicsShell();
		u16				element_number		=	p				->get_ElementsNumber();
		Fvector			center				=	m_telekinesis	->Center();

		for(u16 element=0;element<element_number;++element)
		{
			float k=100000.f;
			CPhysicsElement* E=	p->get_ElementByStoreOrder(element);
			if (!E->bActive) continue;
			Fvector pos=E->mass_Center();
			Fvector vel;
			E->get_LinearVel(vel);
			Fvector diff;
			diff.sub(center,pos);
			float mag=diff.magnitude();
			float accel=k/mag/mag/mag;//*E->getMass()
			float delta_v=accel*fixed_step;
			Fvector dir;dir.set(diff);dir.mul(1.f/mag);
			float cur_vel_to_center=vel.dotproduct(dir);
			
			Fvector v_to_c; v_to_c.set(dir);v_to_c.mul(cur_vel_to_center);
			Fvector tangent_vel; tangent_vel.sub(vel,v_to_c);
			float  t_vel=tangent_vel.magnitude();

			float predict_vel_to_center=cur_vel_to_center+delta_v;
			float max_vel_to_center=mag/fixed_step;
			Fvector force;force.set(dir);
			if(abs(predict_vel_to_center)>abs(max_vel_to_center))
			{
				
				float	max_delta_v=cur_vel_to_center-max_vel_to_center;
				force.mul(E->getMass()*max_delta_v/fixed_step);////1.f/mag*E->getMass()*max_vel_to_center*fixed_step
			}
			else
			{
				force.mul(accel*E->getMass());
			}
	

		
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

