#include "stdafx.h"
#include "alife_space.h"
#include "hit.h"
#include "ode_include.h"
#include "../bone.h"
SHit::SHit(float aP,Fvector &adir,CObject *awho,s16 aelement,Fvector ap_in_bone_space, float aimpulse,  ALife::EHitType ahit_type, float aAP)
{
		P						=aP										;
		dir						.set(adir)								;
		who						=awho									;
		element					=aelement								;
		p_in_bone_space			.set(ap_in_bone_space)					;
		impulse					=aimpulse								;
		hit_type				=ahit_type								;
		ap						= aAP									;
}

SHit::SHit()
{
	invalidate();
}
void SHit::invalidate()
{
	P						=-dInfinity								;
	dir						.set(-dInfinity,-dInfinity,-dInfinity)	;
	who						=NULL									;
	element					=BI_NONE								;
	p_in_bone_space		.set(-dInfinity,-dInfinity,-dInfinity)	;
	impulse					=-dInfinity								;
	hit_type				=ALife::eHitTypeMax						;
	ap						= 0.0f;
}
bool SHit::is_valide() const
{
	return hit_type!=ALife::eHitTypeMax;
}