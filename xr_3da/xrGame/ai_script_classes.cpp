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
#include "Inventory.h"
#include "weapon.h"
#include "ParticlesObject.h"

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

CLuaGameObject *CLuaGameObject::GetCurrentWeapon() const
{
	CAI_Stalker		*l_tpStalker = dynamic_cast<CAI_Stalker*>(m_tpGameObject);
	if (!l_tpStalker) {
		Log		("* [LUA] CAI_Stalker : cannot access class member GetCurrentWeapon!");
		return		(0);
	}
	return			(xr_new<CLuaGameObject>(l_tpStalker->GetCurrentWeapon()));
}

CLuaGameObject *CLuaGameObject::GetCurrentEquipment() const
{
	CAI_Stalker		*l_tpStalker = dynamic_cast<CAI_Stalker*>(m_tpGameObject);
	if (!l_tpStalker) {
		Log		("* [LUA] CAI_Stalker : cannot access class member GetCurrentWeapon!");
		return		(0);
	}
	return			(xr_new<CLuaGameObject>(l_tpStalker->GetCurrentEquipment()));
}

CLuaGameObject *CLuaGameObject::GetFood() const
{
	CAI_Stalker		*l_tpStalker = dynamic_cast<CAI_Stalker*>(m_tpGameObject);
	if (!l_tpStalker) {
		Log		("* [LUA] CAI_Stalker : cannot access class member GetCurrentWeapon!");
		return		(0);
	}
	return			(xr_new<CLuaGameObject>(l_tpStalker->GetFood()));
}

CLuaGameObject *CLuaGameObject::GetMedikit() const
{
	CAI_Stalker		*l_tpStalker = dynamic_cast<CAI_Stalker*>(m_tpGameObject);
	if (!l_tpStalker) {
		Log		("* [LUA] CAI_Stalker : cannot access class member GetCurrentWeapon!");
		return		(0);
	}
	return			(xr_new<CLuaGameObject>(l_tpStalker->GetMedikit()));
}

void CMovementAction::SetObjectToGo(CLuaGameObject *tpObjectToGo)
{
	if (tpObjectToGo)
		m_tpObjectToGo	= tpObjectToGo->operator CObject*();
	else
		m_tpObjectToGo	= 0;
	m_tGoalType			= eGoalTypeObject;
	m_bCompleted		= false;
}

void CWatchAction::SetWatchObject(CLuaGameObject *tpObjectToWatch)
{
	m_tpObjectToWatch	= tpObjectToWatch->operator CObject*();
	m_tGoalType			= eGoalTypeObject;
	m_bCompleted		= false;
}

CParticleAction::~CParticleAction()
{
	//xr_delete			(m_tpParticleSystem);
}

void CParticleAction::SetParticle(LPCSTR caParticleToRun, bool bAutoRemove)
{
	strcpy				(m_caParticleToRun,caParticleToRun);
	m_tGoalType			= eGoalTypeParticleAttached;
	m_tpParticleSystem	= xr_new<CParticlesObject>(m_caParticleToRun,bAutoRemove);
	m_bStartedToPlay	= false;
	m_bCompleted		= false;
}

void CObjectAction::SetObject(CLuaGameObject *tpLuaGameObject)
{
	m_tpObject			= tpLuaGameObject->operator CObject*();
	m_bCompleted		= false;
}

#pragma todo("Dima to Dima : find out why user defined conversion operators work incorrect")

CLuaGameObject::operator CObject*()
{
	return			(dynamic_cast<CObject*>(m_tpGameObject));
}
