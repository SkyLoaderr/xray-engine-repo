////////////////////////////////////////////////////////////////////////////
//	Module 		: object_state_fire_primary.h
//	Created 	: 17.01.2004
//  Modified 	: 17.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Object state fire primary
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "object_state_base.h"
#include "ai/stalker/ai_stalker.h"

class CObjectStateFirePrimary : public CObjectStateBase {
public:
						CObjectStateFirePrimary	(CInventoryItem *inventory_item, const CWeapon::EWeaponStates weapon_state, bool equality = false) :
							CObjectStateBase(inventory_item,weapon_state,equality)
	{
	}

	virtual	void		initialize			()
	{
//		CWeapon			*weapon = dynamic_cast<CWeapon*>(m_inventory_item);
//		if (weapon && (weapon->STATE != CWeapon::eFire) && !weapon->IsWorking())
			m_object->inventory().Action(kWPN_FIRE,	CMD_START);
	}

	virtual	void		execute			()
	{
//		CWeapon			*weapon = dynamic_cast<CWeapon*>(m_inventory_item);
//		if (weapon && (weapon->STATE != CWeapon::eFire) && !weapon->IsWorking())
			m_object->inventory().Action(kWPN_FIRE,	CMD_START);
	}
};