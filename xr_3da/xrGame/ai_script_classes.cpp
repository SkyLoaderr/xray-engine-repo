////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script_classes.cpp
//	Created 	: 25.09.2003
//  Modified 	: 25.09.2003
//	Author		: Dmitriy Iassenev
//	Description : XRay Script classes
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_script_classes.h"
#include "ai_script_actions.h"

void CLuaGameObject::Hit(CLuaHit &tLuaHit)
{
	NET_Packet		P;
	m_tpGameObject->u_EventGen(P,GE_HIT,m_tpGameObject->ID());
	P.w_u16			(u16(tLuaHit.m_tpDraftsman->ID()));
	P.w_dir			(tLuaHit.m_tDirection);
	P.w_float		(tLuaHit.m_fPower);
	CKinematics		*V = PKinematics(m_tpGameObject->Visual());
	R_ASSERT		(V);
	P.w_s16			(V->LL_BoneID(tLuaHit.m_caBoneName));
	P.w_vec3		(Fvector().set(0,0,0));
	P.w_float		(tLuaHit.m_fImpulse);
	P.w_u16			(u16(tLuaHit.m_tHitType));
	m_tpGameObject->u_EventSend(P);
}

CMovementAction::CMovementAction(StalkerSpace::EBodyState tBodyState, StalkerSpace::EMovementType tMovementType, StalkerSpace::EPathType tPathType, CLuaGameObject *tpObjectToGo)
{
	SetBodyState		(tBodyState);
	SetMovementType		(tMovementType);
	SetPathType			(tPathType);
	SetObjectToGo		(tpObjectToGo);
	m_bCompleted		= false;
}

void CMovementAction::SetObjectToGo(CLuaGameObject *tpObjectToGo)
{
	m_tpObjectToGo		= tpObjectToGo->operator CObject*();
	m_tGoalType			= eGoalTypeObject;
}

void CWatchAction::SetWatchObject(CLuaGameObject *tpObjectToWatch)
{
	m_tpObjectToWatch	= tpObjectToWatch->operator CObject*();
	m_tGoalType			= eGoalTypeObject;
}

void CObjectAction::SetObject(CLuaGameObject *tpLuaGameObject)
{
	m_tpObject			= tpLuaGameObject->operator CObject*();
}

#pragma todo("Dima to Dima : find out why user defined conversion operators work incorrect")

CLuaGameObject::operator CObject*()
{
	return			(dynamic_cast<CObject*>(m_tpGameObject));
}
