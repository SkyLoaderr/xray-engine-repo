////////////////////////////////////////////////////////////////////////////
//	Module 		: object_state.cpp
//	Created 	: 16.01.2004
//  Modified 	: 16.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Object base state
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stdafx.h"
#include "object_state_base.h"
#include "ai/stalker/ai_stalker.h"

CObjectStateBase::CObjectStateBase	(CInventoryItem *inventory_item, const CWeapon::EWeaponStates weapon_state, bool equality)
{
	Init							(inventory_item,weapon_state,equality);
}

CObjectStateBase::~CObjectStateBase	()
{
}

void CObjectStateBase::Init			(CInventoryItem *inventory_item, const CWeapon::EWeaponStates weapon_state, bool equality)
{
	m_inventory_item		= inventory_item;
	m_weapon_state			= weapon_state;
	m_equality				= equality;
}

void CObjectStateBase::Load			(LPCSTR section)
{
	inherited::Load					(section);
}

void CObjectStateBase::reinit		(CAI_Stalker *object)
{
	inherited::reinit				(object);
}

void CObjectStateBase::reload		(LPCSTR section)
{
	inherited::reload				(section);
}

void CObjectStateBase::initialize	()
{
	inherited::initialize			();
	m_started						= false;
	CWeapon							*weapon = dynamic_cast<CWeapon*>(m_inventory_item);
	if (weapon && (CWeapon::EWeaponStates(weapon->STATE) == m_weapon_state) && !m_equality)
		m_started					= true;
}

void CObjectStateBase::execute		()
{
	CWeapon			*weapon = dynamic_cast<CWeapon*>(m_inventory_item);
	if (weapon && (CWeapon::EWeaponStates(weapon->STATE) == m_weapon_state) && !m_equality)
		m_started					= true;
}

void CObjectStateBase::finalize		()
{
	inherited::finalize				();
}

bool CObjectStateBase::completed	() const
{
	CWeapon			*weapon = dynamic_cast<CWeapon*>(m_inventory_item);
	if (!weapon)
		return		(inherited::completed());

	return					(
		inherited::completed() &&
		(
		((CWeapon::EWeaponStates(weapon->STATE) == m_weapon_state) && m_equality) ||
		((CWeapon::EWeaponStates(weapon->STATE) != m_weapon_state) && !m_equality && m_started)
		)
	);
}
