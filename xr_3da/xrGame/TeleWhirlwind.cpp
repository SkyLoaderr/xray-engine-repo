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
		float time=10.f;
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
			diff.mul(1.f/time/fixed_step);
			E->applyForce(diff.x,diff.y,diff.z);
		}
}
void		CTeleWhirlwindObject::		keep					()
{

}
void		CTeleWhirlwindObject::		fire					(const Fvector &target)
{

}
void		CTeleWhirlwindObject::		fire					(const Fvector &target, float power)
{

}