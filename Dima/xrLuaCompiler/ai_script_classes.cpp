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
#include "inventory_item.h"
#include "weapon.h"
#include "ParticlesObject.h"
#include "PDA.h"
#include "inventory.h"

void CLuaGameObject::Hit(CLuaHit &tLuaHit)
{
	NET_Packet		P;
	m_tpGameObject->u_EventGen(P,GE_HIT,m_tpGameObject->ID());
	P.w_u16			(u16(tLuaHit.m_tpDraftsman->ID()));
	P.w_dir			(tLuaHit.m_tDirection);
	P.w_float		(tLuaHit.m_fPower);
	CKinematics		*V = PKinematics(m_tpGameObject->Visual());
	R_ASSERT		(V);
	if (xr_strlen	(tLuaHit.m_caBoneName))
		P.w_s16		(V->LL_BoneID(*tLuaHit.m_caBoneName));
	else
		P.w_s16		(s16(0));
	P.w_vec3		(Fvector().set(0,0,0));
	P.w_float		(tLuaHit.m_fImpulse);
	P.w_u16			(u16(tLuaHit.m_tHitType));
	m_tpGameObject->u_EventSend(P);
}

bool CLuaGameObject::GiveInfoPortion(int info_index)
{
	CInventoryOwner* pInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	if(!pInventoryOwner) return false;

	if(!pInventoryOwner->GetPDA()) return false;

	//отправляем от нашему PDA пакет информации с номером
	NET_Packet		P;
	m_tpGameObject->u_EventGen(P,GE_INFO_TRANSFER,pInventoryOwner->GetPDA()->ID());
	P.w_u16			(u16(pInventoryOwner->GetPDA()->ID()));		//отправитель
	P.w_s32			(info_index);								//сообщение
	m_tpGameObject->u_EventSend(P);
	return			true;
}

bool CLuaGameObject::GiveInfoPortionViaPda(int info_index, CLuaGameObject* pFromWho)
{
	CInventoryOwner* pInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	if(!pInventoryOwner) return false;
	if(!pInventoryOwner->GetPDA()) return false;

	CInventoryOwner* pFromWhoInvOwner = dynamic_cast<CInventoryOwner*>(pFromWho->m_tpGameObject);
	if(!pFromWhoInvOwner) return false;
	if(!pFromWhoInvOwner->GetPDA()) return false;

	//отправляем от нашему PDA пакет информации с номером
	NET_Packet		P;
	m_tpGameObject->u_EventGen(P,GE_PDA,pInventoryOwner->GetPDA()->ID());
	P.w_u16			(u16(pFromWhoInvOwner->GetPDA()->ID()));		//отправитель
	P.w_s16			(ePdaMsgInfo);									
	P.w_s32			(info_index);									
	m_tpGameObject->u_EventSend(P);
	return			true;
}


CLuaGameObject *CLuaGameObject::GetCurrentWeapon() const
{
	CAI_Stalker		*l_tpStalker = dynamic_cast<CAI_Stalker*>(m_tpGameObject);
	if (!l_tpStalker) {
		LuaOut		(Lua::eLuaMessageTypeError,"CAI_Stalker : cannot access class member GetCurrentWeapon!");
		return		(0);
	}
	return			(xr_new<CLuaGameObject>(l_tpStalker->GetCurrentWeapon()));
}

CLuaGameObject *CLuaGameObject::GetCurrentEquipment() const
{
	CAI_Stalker		*l_tpStalker = dynamic_cast<CAI_Stalker*>(m_tpGameObject);
	if (!l_tpStalker) {
		LuaOut		(Lua::eLuaMessageTypeError,"CAI_Stalker : cannot access class member GetCurrentWeapon!");
		return		(0);
	}
	return			(xr_new<CLuaGameObject>(l_tpStalker->GetCurrentEquipment()));
}

CLuaGameObject *CLuaGameObject::GetFood() const
{
	CAI_Stalker		*l_tpStalker = dynamic_cast<CAI_Stalker*>(m_tpGameObject);
	if (!l_tpStalker) {
		LuaOut		(Lua::eLuaMessageTypeError,"CAI_Stalker : cannot access class member GetCurrentWeapon!");
		return		(0);
	}
	return			(xr_new<CLuaGameObject>(l_tpStalker->GetFood()));
}

CLuaGameObject *CLuaGameObject::GetMedikit() const
{
	CAI_Stalker		*l_tpStalker = dynamic_cast<CAI_Stalker*>(m_tpGameObject);
	if (!l_tpStalker) {
		LuaOut		(Lua::eLuaMessageTypeError,"CAI_Stalker : cannot access class member GetCurrentWeapon!");
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
	m_caParticleToRun	= caParticleToRun;
	m_tGoalType			= eGoalTypeParticleAttached;
	m_tpParticleSystem	= xr_new<CParticlesObject>(*m_caParticleToRun,m_bAutoRemove = bAutoRemove);
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

u32	CLuaGameObject::Cost			() const
{
	CInventoryItem		*inventory_item = dynamic_cast<CInventoryItem*>(m_tpGameObject);
	if (!inventory_item) {
		LuaOut			(Lua::eLuaMessageTypeError,"CSciptMonster : cannot access class member Cost!");
		return			(false);
	}
	return				(inventory_item->Cost());
}

float CLuaGameObject::GetCondition	() const
{
	CInventoryItem		*inventory_item = dynamic_cast<CInventoryItem*>(m_tpGameObject);
	if (!inventory_item) {
		LuaOut			(Lua::eLuaMessageTypeError,"CSciptMonster : cannot access class member Cost!");
		return			(false);
	}
	return				(inventory_item->GetCondition());
}

u32	CLuaGameObject::GetInventoryObjectCount() const
{
	CInventoryOwner		*l_tpInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	if (l_tpInventoryOwner)
		return			(l_tpInventoryOwner->inventory().dwfGetObjectCount());
	else {
		LuaOut			(Lua::eLuaMessageTypeError,"CLuaGameObject : cannot access class member obj_count!");
		return			(0);
	}
}

CLuaGameObject	*CLuaGameObject::GetActiveItem()
{
	CInventoryOwner		*l_tpInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	if (l_tpInventoryOwner)
		if (l_tpInventoryOwner->inventory().ActiveItem())
			return		(xr_new<CLuaGameObject>(l_tpInventoryOwner->inventory().ActiveItem()));
		else
			return		(0);
	else {
		LuaOut			(Lua::eLuaMessageTypeError,"CLuaGameObject : cannot access class member activge_item!");
		return			(0);
	}
}

CLuaGameObject	*CLuaGameObject::GetObjectByName	(LPCSTR caObjectName) const
{
	CInventoryOwner		*l_tpInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	if (l_tpInventoryOwner) {
		CInventoryItem	*l_tpInventoryItem = l_tpInventoryOwner->inventory().GetItemFromInventory(caObjectName);
		CGameObject		*l_tpGameObject = dynamic_cast<CGameObject*>(l_tpInventoryItem);
		if (!l_tpGameObject)
			return		(0);
		else
			return		(xr_new<CLuaGameObject>(l_tpGameObject));
	}
	else {
		LuaOut			(Lua::eLuaMessageTypeError,"CLuaGameObject : cannot access class member object!");
		return			(0);
	}
}

CLuaGameObject	*CLuaGameObject::GetObjectByIndex	(int iIndex) const
{
	CInventoryOwner		*l_tpInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	if (l_tpInventoryOwner) {
		CInventoryItem	*l_tpInventoryItem = l_tpInventoryOwner->inventory().tpfGetObjectByIndex(iIndex);
		CGameObject		*l_tpGameObject = dynamic_cast<CGameObject*>(l_tpInventoryItem);
		if (!l_tpGameObject)
			return		(0);
		else
			return		(xr_new<CLuaGameObject>(l_tpGameObject));
	}
	else {
		LuaOut			(Lua::eLuaMessageTypeError,"CLuaGameObject : cannot access class member object!");
		return			(0);	
	}
}