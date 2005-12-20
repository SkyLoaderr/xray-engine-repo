#include "stdafx.h"
#include "alife_space.h"
#include "hit.h"
#include "ode_include.h"
#include "../bone.h"
#include "NET_utils.h"
#include "xrMessages.h"

SHit::SHit(float aPower,Fvector &adir,CObject *awho,s16 aelement,Fvector ap_in_bone_space, float aimpulse,  ALife::EHitType ahit_type, float aAP)
{
		power					=aPower									;
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
	power					=-dInfinity								;
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

void SHit::Read_Packet				(NET_Packet	Packet)
{
	u16 type_dummy;	
	Packet.r_begin			(type_dummy);
	Packet.r_u32			(Time);
	Packet.r_u16			(PACKET_TYPE);
	Packet.r_u16			(DestID);
	Read_Packet_Cont		(Packet);
};

void SHit::Read_Packet_Cont		(NET_Packet	Packet)
{

	Packet.r_u16			(whoID);
	Packet.r_u16			(weaponID);
	Packet.r_dir			(dir);
	Packet.r_float			(power);power/=100.0f;
	Packet.r_s16			(element);
	Packet.r_vec3			(p_in_bone_space);
	Packet.r_float			(impulse);
	hit_type				= (ALife::EHitType)Packet.r_u16();	//hit type

	if (hit_type == ALife::eHitTypeFireWound)
	{
		Packet.r_float	(ap);
	}
	if (PACKET_TYPE == GE_HIT_STATISTIC)
	{
		Packet.r_u32(BulletID);
		Packet.r_u32(SenderID);
	}
}

void SHit::Write_Packet			(NET_Packet	&Packet)
{
	Packet.w_begin	(M_EVENT);
	Packet.w_u32		(Time);
	Packet.w_u16		(u16(PACKET_TYPE&0xffff));
	Packet.w_u16		(u16(DestID&0xffff));

	Packet.w_u16		(whoID);
	Packet.w_u16		(weaponID);
	Packet.w_dir		(dir);
	Packet.w_float		(power);
	Packet.w_s16		(element);
	Packet.w_vec3		(p_in_bone_space);
	Packet.w_float		(impulse);
	Packet.w_u16		(u16(hit_type));	
	if (hit_type == ALife::eHitTypeFireWound)
	{
		Packet.w_float	(ap);
	}
	if (PACKET_TYPE == GE_HIT_STATISTIC)
	{
		Packet.w_u32(BulletID);
		Packet.w_u32(SenderID);
	}
};