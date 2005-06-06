#include "stdafx.h"
#include "alife_space.h"
#include "hit.h"
#include "ode_include.h"
#include "../bone.h"
SHit::SHit(float aP,Fvector &adir,CObject *awho,s16 aelement,Fvector ap_in_object_space, float aimpulse,  ALife::EHitType ahit_type)
{
		P						=aP										;
		dir						.set(adir)								;
		who						=awho									;
		element					=aelement								;
		p_in_object_space		.set(ap_in_object_space)				;
		impulse					=impulse								;
		hit_type				=ahit_type								;
}

SHit::SHit()
{
	P						=-dInfinity								;
	dir						.set(-dInfinity,-dInfinity,-dInfinity)	;
	who						=NULL									;
	element					=BI_NONE								;
	p_in_object_space		.set(-dInfinity,-dInfinity,-dInfinity)	;
	impulse					=-dInfinity								;
	hit_type				=ALife::eHitTypeMax						;
}