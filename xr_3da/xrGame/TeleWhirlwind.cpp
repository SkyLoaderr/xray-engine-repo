#include "stdafx.h"
#include "telewhirlwind.h"
#include "PhysicsShell.h"
#include "PhysicsShellHolder.h"


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

void		CTeleWhirlwindObject::		raise					(float power)
{

		CPhysicsShell*	p			=	get_object()	->PPhysicsShell();
		u16				element		=	p				->get_ElementsNumber()-1;
		Fvector			center		=	m_telekinesis	->Center();
		for(;element;element--)
		{
			CPhysicsElement* E=	p->get_ElementByStoreOrder(element);
			if (!E->bActive) continue;
			Fvector pos=E->mass_Center();
			Fvector vel;
			E->get_LinearVel(vel);
			Fvector diff;
			diff.sub(center,pos);
			float mag=diff.magnitude();
			diff.mul(1000000.f*E->getMass()/mag/mag/mag);
			E->applyForce(diff.x,diff.y,diff.z);
		}
}
void		CTeleWhirlwindObject::		keep					()
{
	inherited::keep();
}
void		CTeleWhirlwindObject::		fire					(const Fvector &target)
{
	inherited::fire(target);
}
void		CTeleWhirlwindObject::		fire					(const Fvector &target, float power)
{
	inherited:: fire(target,power);
}