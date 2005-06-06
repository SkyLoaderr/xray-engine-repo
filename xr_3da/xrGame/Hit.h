#pragma once

struct SHit
{
	SHit										(float P,Fvector &dir,CObject *who,s16 element,Fvector p_in_object_space, float impulse,  ALife::EHitType hit_type)	;
	SHit										()																													;
	float				P																																			;
	Fvector				dir																																			;
	CObject				*who																																		;
	s16					element																																		;
	Fvector				p_in_object_space																															;
	float				impulse																																		;
	ALife::EHitType		hit_type																																	;
};