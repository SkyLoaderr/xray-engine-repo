////////////////////////////////////////////////////////////////////////////
//	Module 		: object_state_show.h
//	Created 	: 17.01.2004
//  Modified 	: 17.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Object state show
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "object_state_base.h"
#include "ai/stalker/ai_stalker.h"

class CObjectStateShow : public CObjectStateBase {
public:
						CObjectStateShow	(CInventoryItem *inventory_item, const CWeapon::EWeaponStates weapon_state, bool equality = false) :
							CObjectStateBase(inventory_item,weapon_state,equality)
	{
	}

	virtual	void		execute			()
	{
		VERIFY			(m_inventory_item);
		m_object->inventory().Slot(m_inventory_item);
		m_object->inventory().Activate(m_inventory_item->GetSlot());
	}

	virtual bool		completed		() const
	{
		return			(m_object->inventory().ActiveItem()->ID() == m_inventory_item->ID());
	}
};