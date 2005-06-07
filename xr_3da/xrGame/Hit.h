#pragma once

struct SHit
{
	SHit										(float P,Fvector &dir,CObject *who,s16 element,Fvector p_in_object_space, float impulse,  ALife::EHitType hit_type)				;
	SHit										()																																;
	bool				is_valide				()																														const	;	
	void				invalidate				()																																;
IC	float				damage					()																														const	{VERIFY(is_valide());return P;}
IC	const Fvector		&direction				()																														const	{VERIFY(is_valide());return dir;}
IC	const CObject		*initiator				()																														const	{VERIFY(is_valide());return who;}
IC			s16			bone					()																														const	{VERIFY(is_valide());return element;}
IC	const Fvector		&bone_space_position	()																														const	{VERIFY(is_valide());return p_in_bone_space;}
IC			float		phys_impulse			()																														const	{VERIFY(is_valide());return impulse;}
IC	ALife::EHitType		type					()																														const	{VERIFY(is_valide());return hit_type;}								
private:
	float				P																																						;
	Fvector				dir																																						;
	CObject				*who																																					;
	s16					element																																					;
	Fvector				p_in_bone_space																																			;
	float				impulse																																					;
	ALife::EHitType		hit_type																																				;
};