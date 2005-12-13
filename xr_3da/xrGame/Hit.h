#pragma once

struct SHit
{
	SHit										(float Power,Fvector &dir,CObject *who,s16 element,Fvector p_in_object_space, float impulse,  ALife::EHitType hit_type, float ap = 0.0f);
	SHit										();	
	bool				is_valide				()																														const	;	
	void				invalidate				()																																;
IC	float				damage					()																														const	{VERIFY(is_valide());return power;}
IC	const Fvector		&direction				()																														const	{VERIFY(is_valide());return dir;}
IC	const CObject		*initiator				()																														const	{VERIFY(is_valide());return who;}
IC			s16			bone					()																														const	{VERIFY(is_valide());return element;}
IC	const Fvector		&bone_space_position	()																														const	{VERIFY(is_valide());return p_in_bone_space;}
IC			float		phys_impulse			()																														const	{VERIFY(is_valide());return impulse;}
IC	ALife::EHitType		type					()																														const	{VERIFY(is_valide());return hit_type;}								
	void				Read_Packet				(NET_Packet	P);
	void				Write_Packet			(NET_Packet	&P);
//private:
	//GE_HIT
	u32					Time;
	u16					HIT_TYPE;
	u16					DestID;
	
	float				power																																						;
	Fvector				dir																																						;
	CObject				*who																																					;
	u16					whoID;
	u16					weaponID;
	s16					element																																					;
	Fvector				p_in_bone_space																																			;
	float				impulse																																					;
	ALife::EHitType		hit_type																																				;
	float				ap;
	//---------------------------------------------------
	//GE_HIT_STATISTIC
	u32					BulletID;
	u32					SenderID;
};