////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_script.cpp
//	Created 	: 29.09.2003
//  Modified 	: 29.09.2003
//	Author		: Dmitriy Iassenev
//	Description : Stalker script functions
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"

void CAI_Stalker::UseObject(const CObject *tpObject)
{
#pragma todo("Dima to Dima : Use object specified by script")
}

ALife::EStalkerRank	CAI_Stalker::GetRank() const
{
#pragma todo("Dima to Dima : Return correct stalker rank")
	return			(eStalkerRankNovice);
}

CWeapon	*CAI_Stalker::GetCurrentWeapon() const
{
	return			(dynamic_cast<CWeapon*>(m_inventory.ActiveItem()));
}

u32 CAI_Stalker::GetWeaponAmmo() const
{
	if (!GetCurrentWeapon())
		return		(0);
	return			(GetCurrentWeapon()->GetAmmoCurrent());	
}

CInventoryItem *CAI_Stalker::GetCurrentEquipment() const
{
#pragma todo("Dima to Dima : Return correct equipment")
	return			(0);
}

CInventoryItem *CAI_Stalker::GetMedikit() const
{
#pragma todo("Dima to Dima : Return correct medikit")
	return			(0);
}

CInventoryItem *CAI_Stalker::GetFood() const
{
#pragma todo("Dima to Dima : Return correct food")
	return			(0);
}

void CAI_Stalker::SetBodyState(const EBodyState tBodyState)
{
	m_tScriptBodyState		= tBodyState;
}

void CAI_Stalker::SetMovementType(const EMovementType tMovementType)
{
	m_tScriptMovementType	= tMovementType;
}

void CAI_Stalker::SetDestination(CObject *tpObject)
{
	m_tScriptDestinationObject = tpObject;
}

void CAI_Stalker::SetPathType(const EPathType tPathType)
{
	m_tScriptPathType		= tPathType;
}

void CAI_Stalker::SetPath(LPCSTR caPatrolPath)
{
	strcpy					(m_caScriptPatrolPath,caPatrolPath);
}

void CAI_Stalker::SetWatchObject(CObject *tpObject)
{
	m_tpScriptWatchObject	= tpObject;
}

void CAI_Stalker::SetWatchDirection(const Fvector &tDirection)
{
	m_tScriptWatchDirection	= tDirection;
}

void CAI_Stalker::SetWatchType(const ELookType	tWatchType)
{
	m_tScriptWatchType		= tWatchType;
}

void CAI_Stalker::SetMentalState(const EMentalState	tMentalState)
{
	m_tScriptMentalState	= tMentalState;
}

void CAI_Stalker::SetWeaponState(const EWeaponState	tWeaponState)
{
	m_tScriptWeaponState	= tWeaponState;
}

void CAI_Stalker::SetWeapon(CWeapon *tpWeapon)
{
	m_tpScriptWeapon		= tpWeapon;
}