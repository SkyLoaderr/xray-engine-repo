////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script_classes.cpp
//	Created 	: 25.09.2003
//  Modified 	: 25.09.2003
//	Author		: Dmitriy Iassenev
//	Description : XRay Script classes
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_script_classes.h"

void CLuaGameObject::Hit(float percent, Fvector &dir,	CLuaGameObject* who, s16 element, Fvector p_in_object_space, float impulse, ALife::EHitType hit_type)
{
	NET_Packet		P;
	m_tpGameObject->u_EventGen(P,GE_HIT,m_tpGameObject->ID());
	P.w_u16			(u16(who->ID()));
	P.w_dir			(dir);
	P.w_float		(impulse);
	P.w_s16			(element);
	P.w_vec3		(p_in_object_space);
	P.w_float		(impulse);
	P.w_u16			(u16(hit_type));
	m_tpGameObject->u_EventSend(P);
}

