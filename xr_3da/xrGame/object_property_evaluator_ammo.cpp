////////////////////////////////////////////////////////////////////////////
//	Module 		: object_property_evaluator_ammo.cpp
//	Created 	: 13.03.2004
//  Modified 	: 13.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object ammo property evaluator
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "object_property_evaluator_ammo.h"
#include "weapon.h"

CObjectPropertyEvaluatorAmmo::CObjectPropertyEvaluatorAmmo	(CWeapon *item, CAI_Stalker *owner, u32 ammo_type) :
	inherited		(item,owner),
	m_ammo_type		(ammo_type)
{
}

bool CObjectPropertyEvaluatorAmmo::evaluate	()
{
	if (!m_ammo_type)
		return		(!!(m_item->GetAmmoCurrent() - m_item->GetAmmoElapsed()));
	else
		return		(false);
}
