////////////////////////////////////////////////////////////////////////////
//	Module 		: object_property_evaluator_empty.cpp
//	Created 	: 13.03.2004
//  Modified 	: 13.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object empty property evaluator
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "object_property_evaluator_empty.h"
#include "weapon.h"

CObjectPropertyEvaluatorEmpty::CObjectPropertyEvaluatorEmpty(CWeapon *item, CAI_Stalker *owner, u32 ammo_type) :
	inherited		(item,owner),
	m_ammo_type		(ammo_type)
{
}

bool CObjectPropertyEvaluatorEmpty::evaluate	()
{
	if (!m_ammo_type)
		return		(!m_item->GetAmmoElapsed());
	else
		return		(false);
}
