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
void		CTeleWhirlwindObject::		raise					(float power)
{

		CPhysicsShell*	p			=	get_object()	->PPhysicsShell();
		u16				element		=	p				->get_ElementsNumber()-1;
		Fvector			center		=	m_telekinesis	->Center();

		for(;element;element--)
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
			float predict_vel_to_center=cur_vel_to_center+delta_v;
			float max_vel_to_center=mag/fixed_step;
			Fvector force;force.set(dir);
			if(predict_vel_to_center>max_vel_to_center)
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

