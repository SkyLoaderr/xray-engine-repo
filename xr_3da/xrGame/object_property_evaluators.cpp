////////////////////////////////////////////////////////////////////////////
//	Module 		: object_property_evaluators.cpp
//	Created 	: 12.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object property evaluators
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "object_property_evaluators.h"
#include "weapon.h"
#include "ai/stalker/ai_stalker.h"
#include "inventory.h"
#include "missile.h"
#include "fooditem.h"

//////////////////////////////////////////////////////////////////////////
// CObjectPropertyEvaluatorState
//////////////////////////////////////////////////////////////////////////

CObjectPropertyEvaluatorState::CObjectPropertyEvaluatorState	(CWeapon *item, CAI_Stalker *owner, u32 state, bool equality) :
	inherited		(item,owner),
	m_state			(state),
	m_equality		(equality)
{
}

CObjectPropertyEvaluatorState::_value_type CObjectPropertyEvaluatorState::evaluate	()
{
	VERIFY			(m_item);
	return			(_value_type((m_item->STATE == m_state) == m_equality));
}

//////////////////////////////////////////////////////////////////////////
// CObjectPropertyEvaluatorAmmo
//////////////////////////////////////////////////////////////////////////

CObjectPropertyEvaluatorAmmo::CObjectPropertyEvaluatorAmmo	(CWeapon *item, CAI_Stalker *owner, u32 ammo_type) :
	inherited		(item,owner),
	m_ammo_type		(ammo_type)
{
}

CObjectPropertyEvaluatorAmmo::_value_type CObjectPropertyEvaluatorAmmo::evaluate	()
{
	if (!m_ammo_type)
		return		(_value_type(!!(m_item->GetAmmoCurrent() - m_item->GetAmmoElapsed())));
	else
		return		(_value_type(false));
}

//////////////////////////////////////////////////////////////////////////
// CObjectPropertyEvaluatorEmpty
//////////////////////////////////////////////////////////////////////////

CObjectPropertyEvaluatorEmpty::CObjectPropertyEvaluatorEmpty(CWeapon *item, CAI_Stalker *owner, u32 ammo_type) :
	inherited		(item,owner),
	m_ammo_type		(ammo_type)
{
}

CObjectPropertyEvaluatorEmpty::_value_type CObjectPropertyEvaluatorEmpty::evaluate	()
{
	if (!m_ammo_type)
		return		(_value_type(!m_item->GetAmmoElapsed()));
	else
		return		(_value_type(false));
}

//////////////////////////////////////////////////////////////////////////
// CObjectPropertyEvaluatorReady
//////////////////////////////////////////////////////////////////////////

CObjectPropertyEvaluatorReady::CObjectPropertyEvaluatorReady(CWeapon *item, CAI_Stalker *owner, u32 ammo_type) :
	inherited		(item,owner),
	m_ammo_type		(ammo_type)
{
}

CObjectPropertyEvaluatorReady::_value_type CObjectPropertyEvaluatorReady::evaluate	()
{
	if (!m_ammo_type)
		return		(_value_type(!m_item->IsMisfire() && m_item->GetAmmoElapsed()));
	else
		return		(_value_type(false));
}

//////////////////////////////////////////////////////////////////////////
// CObjectPropertyEvaluatorNoItems
//////////////////////////////////////////////////////////////////////////

CObjectPropertyEvaluatorNoItems::CObjectPropertyEvaluatorNoItems(CAI_Stalker *owner)
{
	m_object		= owner;
}

CObjectPropertyEvaluatorNoItems::_value_type CObjectPropertyEvaluatorNoItems::evaluate	()
{
	return			(!m_object->inventory().ActiveItem() || m_object->inventory().ActiveItem()->IsHidden());
}

//////////////////////////////////////////////////////////////////////////
// CObjectPropertyEvaluatorMissile
//////////////////////////////////////////////////////////////////////////

CObjectPropertyEvaluatorMissile::CObjectPropertyEvaluatorMissile	(CMissile *item, CAI_Stalker *owner, u32 state, bool equality) :
	inherited		(item,owner),
	m_state			(state),
	m_equality		(equality)
{
}

CObjectPropertyEvaluatorMissile::_value_type CObjectPropertyEvaluatorMissile::evaluate	()
{
	VERIFY			(m_item);
	return			(_value_type((m_item->State() == m_state) == m_equality));
}

//////////////////////////////////////////////////////////////////////////
// CObjectPropertyEvaluatorFood
//////////////////////////////////////////////////////////////////////////

CObjectPropertyEvaluatorFood::CObjectPropertyEvaluatorFood	(CFoodItem *item, CAI_Stalker *owner, u32 state, bool equality) :
	inherited		(item,owner),
	m_state			(state),
	m_equality		(equality)
{
}

CObjectPropertyEvaluatorFood::_value_type CObjectPropertyEvaluatorFood::evaluate	()
{
	VERIFY			(m_item);
	return			(_value_type((m_item->State() == m_state) == m_equality));
}

//////////////////////////////////////////////////////////////////////////
// CObjectPropertyEvaluatorPrepared
//////////////////////////////////////////////////////////////////////////

CObjectPropertyEvaluatorPrepared::CObjectPropertyEvaluatorPrepared	(CFoodItem *item, CAI_Stalker *owner) :
	inherited		(item,owner)
{
}

CObjectPropertyEvaluatorPrepared::_value_type CObjectPropertyEvaluatorPrepared::evaluate	()
{
	VERIFY			(m_item);
	return			(_value_type(m_item->ready_to_eat()));
}

